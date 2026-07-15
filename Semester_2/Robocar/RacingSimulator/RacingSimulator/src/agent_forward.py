#!/usr/bin/env python3
import argparse
import json
import os
import signal
import threading
import time
from typing import Any

import numpy as np
from mlagents_envs.base_env import ActionTuple
from mlagents_envs.environment import UnityEnvironment

try:
    import pygame
except ImportError:
    pygame = None

try:
    from pynput import keyboard as pynput_keyboard
except ImportError:
    pynput_keyboard = None


RUNNING = True
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(SCRIPT_DIR)


def _handle_signal(_sig, _frame):
    global RUNNING
    RUNNING = False


def _load_config(config_path: str) -> None:
    with open(config_path, "r", encoding="utf-8") as f:
        payload = json.load(f)

    if "agents" not in payload or not isinstance(payload["agents"], list):
        raise ValueError("Invalid config: expected a top-level 'agents' list")

    for i, agent in enumerate(payload["agents"]):
        if not isinstance(agent, dict):
            raise ValueError(f"Invalid config: agents[{i}] must be an object")
        fov = int(agent.get("fov", 0))
        nb_ray = int(agent.get("nbRay", 0))
        if not (1 <= fov <= 180):
            raise ValueError(f"Invalid config: agents[{i}].fov must be in [1, 180]")
        if not (1 <= nb_ray <= 50):
            raise ValueError(f"Invalid config: agents[{i}].nbRay must be in [1, 50]")


def _build_continuous_action(
    n_agents: int,
    size: int,
    throttle_value: float,
    throttle_index: int,
    invert_throttle: bool,
) -> ActionTuple:
    actions = np.zeros((n_agents, size), dtype=np.float32)
    idx = max(0, min(size - 1, throttle_index))
    value = -throttle_value if invert_throttle else throttle_value
    actions[:, idx] = np.clip(value, -1.0, 1.0)
    return ActionTuple(continuous=actions)


def _build_discrete_action(n_agents: int, branches: tuple[int, ...]) -> ActionTuple:
    actions = np.zeros((n_agents, len(branches)), dtype=np.int32)
    for branch_idx, branch_size in enumerate(branches):
        actions[:, branch_idx] = min(1, branch_size - 1)
    return ActionTuple(discrete=actions)


def _build_random_continuous_action(n_agents: int, size: int) -> ActionTuple:
    actions = np.random.uniform(-1.0, 1.0, size=(n_agents, size)).astype(np.float32)
    return ActionTuple(continuous=actions)


def _build_random_discrete_action(n_agents: int, branches: tuple[int, ...]) -> ActionTuple:
    actions = np.zeros((n_agents, len(branches)), dtype=np.int32)
    for branch_idx, branch_size in enumerate(branches):
        actions[:, branch_idx] = np.random.randint(0, branch_size, size=n_agents, dtype=np.int32)
    return ActionTuple(discrete=actions)


def _manual_axes_from_keyboard() -> tuple[float, float]:
    pygame.event.pump()
    keys = pygame.key.get_pressed()

    # Also support ZQSD for AZERTY keyboards.
    left_pressed = keys[pygame.K_LEFT] or keys[pygame.K_q]
    right_pressed = keys[pygame.K_RIGHT] or keys[pygame.K_d]
    throttle_pressed = keys[pygame.K_UP] or keys[pygame.K_z] or keys[pygame.K_w]
    brake_pressed = keys[pygame.K_DOWN] or keys[pygame.K_s]

    steer = 0.0
    if left_pressed and not right_pressed:
        steer = -1.0
    elif right_pressed and not left_pressed:
        steer = 1.0

    throttle = 0.0
    if throttle_pressed and not brake_pressed:
        throttle = 1.0
    elif brake_pressed and not throttle_pressed:
        throttle = -1.0

    return steer, throttle


def _apply_deadzone(value: float, deadzone: float) -> float:
    if abs(value) < deadzone:
        return 0.0
    return float(np.clip(value, -1.0, 1.0))


class GamepadState:
    def __init__(self, deadzone: float = 0.2) -> None:
        self.deadzone = max(0.0, min(deadzone, 0.95))
        self._joysticks: list[Any] = []
        self._ready = False
        self._owns_pygame = False

    def start(self) -> bool:
        if pygame is None:
            return False

        if not pygame.get_init():
            pygame.init()
            self._owns_pygame = True

        if not pygame.joystick.get_init():
            pygame.joystick.init()

        self._scan_joysticks()
        self._ready = True
        return True

    def stop(self) -> None:
        if not self._ready or pygame is None:
            return

        for joystick in self._joysticks:
            if joystick.get_init():
                joystick.quit()
        self._joysticks = []

        if pygame.joystick.get_init():
            pygame.joystick.quit()

        if self._owns_pygame and pygame.get_init():
            pygame.quit()

        self._ready = False

    def names(self) -> list[str]:
        return [str(joystick.get_name()) for joystick in self._joysticks]

    def _scan_joysticks(self) -> None:
        if pygame is None:
            return
        self._joysticks = []
        for idx in range(pygame.joystick.get_count()):
            joystick = pygame.joystick.Joystick(idx)
            if not joystick.get_init():
                joystick.init()
            self._joysticks.append(joystick)

    def _axis(self, joystick: Any, idx: int) -> float:
        if idx < 0 or idx >= joystick.get_numaxes():
            return 0.0
        return _apply_deadzone(float(joystick.get_axis(idx)), self.deadzone)

    def _trigger_axis(self, joystick: Any, idx: int) -> float:
        raw = self._axis(joystick, idx)
        if raw <= -0.2:
            # SDL backends often expose triggers in [-1, 1] with rest = -1.
            return (raw + 1.0) / 2.0
        # Some mappings expose triggers in [0, 1] with rest = 0.
        return max(0.0, raw)

    def _hat_axes(self, joystick: Any) -> tuple[float, float]:
        if joystick.get_numhats() <= 0:
            return 0.0, 0.0
        hat_x, hat_y = joystick.get_hat(0)
        return float(hat_x), float(hat_y)

    def axes(self) -> tuple[float, float]:
        if not self._ready or pygame is None:
            return 0.0, 0.0

        pygame.event.pump()

        if len(self._joysticks) != pygame.joystick.get_count():
            self._scan_joysticks()

        best_steer = 0.0
        best_throttle = 0.0
        best_magnitude = 0.0

        for joystick in self._joysticks:
            # Steering is bound to the left stick horizontal axis.
            steer = self._axis(joystick, 0)

            # Throttle mapping requested: L2 forward (+), R2 reverse (-).
            trigger_pairs = [(4, 5), (3, 4), (2, 5), (5, 2)]
            trigger_throttle = 0.0
            trigger_activity = 0.0
            for lt_idx, rt_idx in trigger_pairs:
                lt = self._trigger_axis(joystick, lt_idx)
                rt = self._trigger_axis(joystick, rt_idx)
                candidate = np.clip(lt - rt, -1.0, 1.0)
                if abs(candidate) > abs(trigger_throttle):
                    trigger_throttle = float(candidate)
                trigger_activity = max(trigger_activity, lt, rt)

            stick_throttle = -self._axis(joystick, 1)
            hat_x, hat_y = self._hat_axes(joystick)

            if abs(hat_x) > abs(steer):
                steer = hat_x

            # Use triggers when pressed, otherwise fallback to left stick vertical axis.
            throttle = trigger_throttle if trigger_activity > self.deadzone else stick_throttle
            if abs(hat_y) > abs(throttle):
                throttle = hat_y

            magnitude = abs(steer) + abs(throttle)
            if magnitude > best_magnitude:
                best_magnitude = magnitude
                best_steer = float(np.clip(steer, -1.0, 1.0))
                best_throttle = float(np.clip(throttle, -1.0, 1.0))

        return best_steer, best_throttle


class GlobalKeyboardState:
    def __init__(self) -> None:
        self._pressed: set[str] = set()
        self._lock = threading.Lock()
        self._listener = None

    def _normalize_key(self, key: Any) -> str | None:
        if pynput_keyboard is None:
            return None

        if key == pynput_keyboard.Key.left:
            return "left"
        if key == pynput_keyboard.Key.right:
            return "right"
        if key == pynput_keyboard.Key.up:
            return "up"
        if key == pynput_keyboard.Key.down:
            return "down"

        char = getattr(key, "char", None)
        if isinstance(char, str) and char:
            return char.lower()
        return None

    def _on_press(self, key: Any) -> None:
        k = self._normalize_key(key)
        if k is None:
            return
        with self._lock:
            self._pressed.add(k)

    def _on_release(self, key: Any) -> None:
        k = self._normalize_key(key)
        if k is None:
            return
        with self._lock:
            self._pressed.discard(k)

    def start(self) -> None:
        if pynput_keyboard is None:
            raise RuntimeError("pynput is not available")
        self._listener = pynput_keyboard.Listener(on_press=self._on_press, on_release=self._on_release)
        self._listener.start()

    def stop(self) -> None:
        if self._listener is not None:
            self._listener.stop()
            self._listener = None

    def axes(self) -> tuple[float, float]:
        with self._lock:
            pressed = set(self._pressed)

        left_pressed = ("left" in pressed) or ("q" in pressed) or ("a" in pressed)
        right_pressed = ("right" in pressed) or ("d" in pressed)
        throttle_pressed = ("up" in pressed) or ("z" in pressed) or ("w" in pressed)
        brake_pressed = ("down" in pressed) or ("s" in pressed)

        steer = 0.0
        if left_pressed and not right_pressed:
            steer = -1.0
        elif right_pressed and not left_pressed:
            steer = 1.0

        throttle = 0.0
        if throttle_pressed and not brake_pressed:
            throttle = 1.0
        elif brake_pressed and not throttle_pressed:
            throttle = -1.0

        return steer, throttle


def _build_manual_continuous_action(
    n_agents: int,
    size: int,
    steer: float,
    throttle: float,
    steer_index: int,
    throttle_index: int,
    invert_steer: bool,
    invert_throttle: bool,
    steer_scale: float,
    throttle_scale: float,
) -> ActionTuple:
    actions = np.zeros((n_agents, size), dtype=np.float32)
    if size <= 0:
        return ActionTuple(continuous=actions)

    s_idx = max(0, min(size - 1, steer_index))
    t_idx = max(0, min(size - 1, throttle_index))

    steer_value = -steer if invert_steer else steer
    throttle_value = -throttle if invert_throttle else throttle

    actions[:, s_idx] = np.clip(steer_value * steer_scale, -1.0, 1.0)
    actions[:, t_idx] = np.clip(throttle_value * throttle_scale, -1.0, 1.0)
    return ActionTuple(continuous=actions)


def _build_manual_discrete_action(
    n_agents: int,
    branches: tuple[int, ...],
    steer: float,
    throttle: float,
) -> ActionTuple:
    actions = np.zeros((n_agents, len(branches)), dtype=np.int32)

    # Best-effort mapping for common layouts.
    for branch_idx, branch_size in enumerate(branches):
        value = 0
        if branch_idx == 0 and branch_size >= 3:
            if steer < -0.1:
                value = 1
            elif steer > 0.1:
                value = 2
        elif branch_idx == 1 and branch_size >= 3:
            if throttle > 0.1:
                value = 1
            elif throttle < -0.1:
                value = 2
        elif branch_size >= 2:
            if throttle > 0.1:
                value = 1

        actions[:, branch_idx] = min(value, branch_size - 1)

    return ActionTuple(discrete=actions)


def _serialize_behavior_specs(env: UnityEnvironment) -> dict[str, Any]:
    payload: dict[str, Any] = {}
    for behavior_name, behavior_spec in env.behavior_specs.items():
        action_spec = behavior_spec.action_spec
        payload[behavior_name] = {
            "observation_shapes": [list(obs_spec.shape) for obs_spec in behavior_spec.observation_specs],
            "action_type": "continuous" if action_spec.is_continuous() else "discrete",
            "continuous_size": int(action_spec.continuous_size),
            "discrete_branches": [int(v) for v in action_spec.discrete_branches],
        }
    return payload


class JsonlCollector:
    def __init__(self, dataset_path: str, overwrite: bool, append: bool) -> None:
        self.dataset_path = os.path.abspath(dataset_path)
        self.meta_path = self.dataset_path + ".meta.json"
        self.samples_written = 0
        self.existed_before = os.path.exists(self.dataset_path)

        if overwrite and append:
            raise ValueError("Use either overwrite or append, not both")

        os.makedirs(os.path.dirname(self.dataset_path), exist_ok=True)
        if (not overwrite) and (not append) and self.existed_before:
            raise FileExistsError(
                f"Dataset file already exists: {self.dataset_path}. Use --dataset-append to continue or --dataset-overwrite to replace it."
            )

        mode = "w" if overwrite else "a"
        self._f = open(self.dataset_path, mode, encoding="utf-8")

    def write_metadata(self, content: dict[str, Any]) -> None:
        with open(self.meta_path, "w", encoding="utf-8") as meta_file:
            json.dump(content, meta_file, indent=2)

    def write_sample(self, sample: dict[str, Any], flush_every: int) -> None:
        self._f.write(json.dumps(sample, separators=(",", ":")) + "\n")
        self.samples_written += 1
        if flush_every > 0 and self.samples_written % flush_every == 0:
            self._f.flush()

    def close(self) -> None:
        self._f.flush()
        self._f.close()


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Minimal Robocar client: drive forward and optionally collect supervised data"
    )
    parser.add_argument(
        "--sim-path",
        default=os.path.abspath(os.path.join(PROJECT_ROOT, "..", "RacingSimulator.x86_64")),
        help="Path to the simulator binary (ignored when --connect-only is used)",
    )
    parser.add_argument(
        "--config-path",
        default=os.path.join(PROJECT_ROOT, "configs", "agents_config.json"),
        help="Path to JSON config passed to simulator via --config-path",
    )
    parser.add_argument(
        "--connect-only",
        action="store_true",
        help="Connect to an already-running simulator instead of launching it",
    )
    parser.add_argument(
        "--base-port",
        type=int,
        default=5004,
        help="Base port used by UnityEnvironment",
    )
    parser.add_argument(
        "--steps",
        type=int,
        default=5000,
        help="Number of simulation steps to execute",
    )
    parser.add_argument(
        "--throttle",
        type=float,
        default=1.0,
        help="Throttle value for continuous actions",
    )
    parser.add_argument(
        "--sleep",
        type=float,
        default=0.0,
        help="Optional sleep in seconds after each env.step()",
    )
    parser.add_argument(
        "--speed-limit",
        nargs="?",
        const="none",
        default=None,
        help=(
            "Optional throttle cap in [0,1]. "
            "No flag: no cap. Flag without value: no cap. "
            "Example: --speed-limit 0.6"
        ),
    )
    parser.add_argument(
        "--mode",
        choices=["forward", "collect-forward", "collect-random", "manual", "manual-collect"],
        default="forward",
        help="Driving/collection mode",
    )
    parser.add_argument(
        "--dataset-path",
        default=os.path.join(PROJECT_ROOT, "data", "datasets", "samples.jsonl"),
        help="Output dataset path for collection modes",
    )
    parser.add_argument(
        "--dataset-overwrite",
        action="store_true",
        help="Overwrite output dataset file if it already exists",
    )
    parser.add_argument(
        "--dataset-append",
        action="store_true",
        help="Append to output dataset file if it already exists",
    )
    parser.add_argument(
        "--flush-every",
        type=int,
        default=200,
        help="Flush JSONL file every N samples (0 disables periodic flush)",
    )
    parser.add_argument(
        "--manual-steer-scale",
        type=float,
        default=1.0,
        help="Scale applied to manual steering axis for continuous actions",
    )
    parser.add_argument(
        "--manual-throttle-scale",
        type=float,
        default=1.0,
        help="Scale applied to manual throttle axis for continuous actions",
    )
    parser.add_argument(
        "--continuous-steer-index",
        type=int,
        default=1,
        help="Continuous action index used for steering (default tuned for this simulator)",
    )
    parser.add_argument(
        "--continuous-throttle-index",
        type=int,
        default=0,
        help="Continuous action index used for throttle/brake (default tuned for this simulator)",
    )
    parser.add_argument(
        "--invert-steer",
        action="store_true",
        help="Invert steering sign for continuous manual control",
    )
    parser.add_argument(
        "--invert-throttle",
        action="store_true",
        help="Invert throttle/brake sign for continuous control",
    )
    parser.add_argument(
        "--manual-capture-backend",
        choices=["auto", "pynput", "pygame"],
        default="auto",
        help="Keyboard capture backend for manual modes",
    )
    parser.add_argument(
        "--manual-controller",
        action=argparse.BooleanOptionalAction,
        default=True,
        help="Enable PS/Xbox controller input in manual modes",
    )
    parser.add_argument(
        "--manual-controller-deadzone",
        type=float,
        default=0.2,
        help="Deadzone applied to controller axes (0.0 to 0.95)",
    )
    args = parser.parse_args()

    speed_limit: float | None = None
    if args.speed_limit not in (None, "none"):
        speed_limit = float(args.speed_limit)
        if speed_limit <= 0.0:
            speed_limit = None
        else:
            speed_limit = float(min(1.0, speed_limit))

    _load_config(args.config_path)

    file_name = None if args.connect_only else args.sim_path
    additional_args = ["--config-path", os.path.abspath(args.config_path)]

    signal.signal(signal.SIGINT, _handle_signal)
    signal.signal(signal.SIGTERM, _handle_signal)

    env = UnityEnvironment(
        file_name=file_name,
        base_port=args.base_port,
        worker_id=0,
        additional_args=additional_args,
        no_graphics=False,
    )

    collector: JsonlCollector | None = None
    collection_enabled = args.mode in {"collect-forward", "collect-random", "manual-collect"}
    manual_mode = args.mode in {"manual", "manual-collect"}
    keyboard_state: GlobalKeyboardState | None = None
    controller_state: GamepadState | None = None
    pygame_keyboard_enabled = False

    if manual_mode:
        backend = args.manual_capture_backend
        if backend in {"auto", "pynput"} and pynput_keyboard is not None:
            keyboard_state = GlobalKeyboardState()
            keyboard_state.start()
            print("Manual mode enabled with global keyboard capture (pynput). Keep RacingSimulator focused.")
        elif backend == "pynput":
            raise RuntimeError(
                "manual-capture-backend=pynput selected but pynput is unavailable. Install dependencies with: python -m pip install -r requirements.txt"
            )
        else:
            if pygame is None:
                raise RuntimeError(
                    "Manual mode requires pygame fallback, but pygame is unavailable. Install dependencies with: python -m pip install -r requirements.txt"
                )
            pygame.init()
            pygame.display.set_caption("Robocar Keyboard Input")
            pygame.display.set_mode((420, 90))
            pygame_keyboard_enabled = True
            print("Manual mode enabled with pygame fallback: focus 'Robocar Keyboard Input' window.")

        if args.manual_controller:
            if pygame is None:
                print("Controller input disabled: pygame is unavailable.")
            else:
                controller_state = GamepadState(deadzone=args.manual_controller_deadzone)
                controller_state.start()
                controller_names = controller_state.names()
                if controller_names:
                    print(f"Controller input enabled: {', '.join(controller_names)}")
                else:
                    print("Controller input enabled, but no gamepad detected yet (connect one and move it).")

    try:
        env.reset()
        behavior_names = list(env.behavior_specs.keys())
        if not behavior_names:
            print("No behaviors found. Check simulator state and config.")
            return 1

        print("Connected. Behaviors:")
        for name in behavior_names:
            action_spec = env.behavior_specs[name].action_spec
            if action_spec.is_continuous():
                print(f"- {name}: continuous_size={action_spec.continuous_size}")
            else:
                print(f"- {name}: discrete_branches={action_spec.discrete_branches}")

        if speed_limit is None:
            print("Speed limit: disabled")
        else:
            print(
                "Speed limit: enabled "
                f"(max |throttle| = {speed_limit:.3f}, index={args.continuous_throttle_index})"
            )

        if collection_enabled:
            collector = JsonlCollector(
                dataset_path=args.dataset_path,
                overwrite=args.dataset_overwrite,
                append=args.dataset_append,
            )
            run_meta = {
                "created_at_unix": int(time.time()),
                "config_path": os.path.abspath(args.config_path),
                "mode": args.mode,
                "behaviors": _serialize_behavior_specs(env),
            }
            if args.dataset_append and collector.existed_before and os.path.exists(collector.meta_path):
                print(f"Append mode: preserving existing metadata file {collector.meta_path}")
            else:
                collector.write_metadata(run_meta)
            print(
                f"Collection enabled. Writing samples to {collector.dataset_path} and metadata to {collector.meta_path}"
            )

        step_count = 0
        while RUNNING and step_count < args.steps:
            for behavior_name, behavior_spec in env.behavior_specs.items():
                decision_steps, _ = env.get_steps(behavior_name)
                n_agents = len(decision_steps)
                if n_agents == 0:
                    continue

                action_spec = behavior_spec.action_spec
                if manual_mode:
                    steer = 0.0
                    throttle = 0.0
                    if keyboard_state is not None:
                        steer, throttle = keyboard_state.axes()
                    elif pygame_keyboard_enabled:
                        steer, throttle = _manual_axes_from_keyboard()

                    if controller_state is not None:
                        gamepad_steer, gamepad_throttle = controller_state.axes()
                        if abs(gamepad_steer) >= abs(steer):
                            steer = gamepad_steer
                        if abs(gamepad_throttle) >= abs(throttle):
                            throttle = gamepad_throttle

                    if action_spec.is_continuous():
                        action = _build_manual_continuous_action(
                            n_agents=n_agents,
                            size=action_spec.continuous_size,
                            steer=steer,
                            throttle=throttle,
                            steer_index=args.continuous_steer_index,
                            throttle_index=args.continuous_throttle_index,
                            invert_steer=args.invert_steer,
                            invert_throttle=args.invert_throttle,
                            steer_scale=args.manual_steer_scale,
                            throttle_scale=args.manual_throttle_scale,
                        )
                    else:
                        action = _build_manual_discrete_action(
                            n_agents=n_agents,
                            branches=action_spec.discrete_branches,
                            steer=steer,
                            throttle=throttle,
                        )
                elif action_spec.is_continuous():
                    if args.mode == "collect-random":
                        action = _build_random_continuous_action(
                            n_agents=n_agents,
                            size=action_spec.continuous_size,
                        )
                    else:
                        action = _build_continuous_action(
                            n_agents=n_agents,
                            size=action_spec.continuous_size,
                            throttle_value=args.throttle,
                            throttle_index=args.continuous_throttle_index,
                            invert_throttle=args.invert_throttle,
                        )
                else:
                    if args.mode == "collect-random":
                        action = _build_random_discrete_action(
                            n_agents=n_agents,
                            branches=action_spec.discrete_branches,
                        )
                    else:
                        action = _build_discrete_action(
                            n_agents=n_agents,
                            branches=action_spec.discrete_branches,
                        )

                if speed_limit is not None and action_spec.is_continuous():
                    throttle_idx = max(0, min(action_spec.continuous_size - 1, args.continuous_throttle_index))
                    action.continuous[:, throttle_idx] = np.clip(
                        action.continuous[:, throttle_idx], -speed_limit, speed_limit
                    )

                if collection_enabled and collector is not None:
                    action_array = action.continuous if action_spec.is_continuous() else action.discrete
                    action_type = "continuous" if action_spec.is_continuous() else "discrete"

                    for row_idx, agent_id in enumerate(decision_steps.agent_id):
                        obs_flat: list[float] = []
                        obs_shapes: list[list[int]] = []
                        for obs_block in decision_steps.obs:
                            current_obs = np.asarray(obs_block[row_idx], dtype=np.float32)
                            obs_shapes.append(list(current_obs.shape))
                            obs_flat.extend(current_obs.reshape(-1).tolist())

                        record = {
                            "env_step": step_count,
                            "behavior": behavior_name,
                            "agent_id": int(agent_id),
                            "reward": float(decision_steps.reward[row_idx]),
                            "observation": obs_flat,
                            "observation_shapes": obs_shapes,
                            "action_type": action_type,
                            "action": np.asarray(action_array[row_idx]).reshape(-1).tolist(),
                        }
                        collector.write_sample(record, flush_every=args.flush_every)

                env.set_actions(behavior_name, action)

            env.step()
            step_count += 1
            if args.sleep > 0:
                time.sleep(args.sleep)

        if collector is not None:
            print(f"Collected {collector.samples_written} samples")
        print(f"Stopped after {step_count} steps")
        return 0
    finally:
        if collector is not None:
            collector.close()
        if keyboard_state is not None:
            keyboard_state.stop()
        if controller_state is not None:
            controller_state.stop()
        if pygame_keyboard_enabled and pygame is not None and pygame.get_init():
            pygame.quit()
        env.close()


if __name__ == "__main__":
    raise SystemExit(main())
