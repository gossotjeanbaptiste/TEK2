#!/usr/bin/env python3
import argparse
import json
import os
import signal
import time
from typing import Any

import joblib
import numpy as np
from mlagents_envs.base_env import ActionTuple
from mlagents_envs.environment import UnityEnvironment


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


def _obs_row(decision_steps: Any, row_idx: int) -> list[float]:
    flat: list[float] = []
    for obs_block in decision_steps.obs:
        current_obs = np.asarray(obs_block[row_idx], dtype=np.float32)
        flat.extend(current_obs.reshape(-1).tolist())
    return flat


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run trained supervised policy in Unity simulator")
    parser.add_argument(
        "--sim-path",
        default=os.path.abspath(os.path.join(
            PROJECT_ROOT, "..", "RacingSimulator.x86_64")),
        help="Path to simulator binary",
    )
    parser.add_argument(
        "--config-path",
        default=os.path.join(PROJECT_ROOT, "configs", "agents_config.json"),
        help="Path to simulator agent config",
    )
    parser.add_argument("--connect-only", action="store_true",
                        help="Connect to running simulator")
    parser.add_argument("--base-port", type=int, default=5004,
                        help="Unity ML-Agents base port")
    parser.add_argument("--steps", type=int, default=5000, help="Steps to run")
    parser.add_argument("--sleep", type=float, default=0.0,
                        help="Optional sleep after each step")
    parser.add_argument(
        "--model-path",
        default=os.path.join(PROJECT_ROOT, "models", "policy_mlp.joblib"),
        help="Trained model artifact path",
    )
    parser.add_argument("--policy-gain", type=float, default=1.0,
                        help="Scale factor applied to predicted actions")
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
        "--throttle-index",
        type=int,
        default=0,
        help="Continuous action index used as throttle/brake channel",
    )
    parser.add_argument(
        "--dataset-path",
        default=None,
        help="Optional output dataset path to collect model predictions",
    )
    parser.add_argument(
        "--dataset-append",
        action="store_true",
        help="Append to output dataset file if it already exists",
    )
    parser.add_argument(
        "--dataset-overwrite",
        action="store_true",
        help="Overwrite output dataset file if it already exists",
    )
    parser.add_argument(
        "--flush-every",
        type=int,
        default=200,
        help="Flush JSONL file every N samples (0 disables periodic flush)",
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

    artifact = joblib.load(args.model_path)
    model = artifact["model"]
    obs_dim = int(artifact["obs_dim"])
    action_dim = int(artifact["action_dim"])

    signal.signal(signal.SIGINT, _handle_signal)
    signal.signal(signal.SIGTERM, _handle_signal)

    file_name = None if args.connect_only else args.sim_path
    additional_args = ["--config-path", os.path.abspath(args.config_path)]
    env = UnityEnvironment(
        file_name=file_name,
        base_port=args.base_port,
        worker_id=0,
        additional_args=additional_args,
        no_graphics=False,
    )

    collector = None
    try:
        # Initialize collector if dataset path provided
        if args.dataset_path:
            collector = JsonlCollector(
                dataset_path=args.dataset_path,
                overwrite=args.dataset_overwrite,
                append=args.dataset_append,
            )
            run_meta = {
                "created_at_unix": int(time.time()),
                "config_path": os.path.abspath(args.config_path),
                "model_path": os.path.abspath(args.model_path),
                "policy_gain": args.policy_gain,
                "speed_limit": speed_limit,
            }
            collector.write_metadata(run_meta)
            print(
                f"Collection enabled. Writing samples to {collector.dataset_path} and metadata to {collector.meta_path}"
            )

        env.reset()
        behavior_names = list(env.behavior_specs.keys())
        if not behavior_names:
            print("No behaviors found.")
            return 1

        print(f"Loaded policy from {os.path.abspath(args.model_path)}")
        print(f"Model expects obs_dim={obs_dim}, action_dim={action_dim}")
        if speed_limit is None:
            print("Speed limit: disabled")
        else:
            print(
                f"Speed limit: enabled (max |throttle| = {speed_limit:.3f}, index={args.throttle_index})")

        step_count = 0
        while RUNNING and step_count < args.steps:
            for behavior_name, behavior_spec in env.behavior_specs.items():
                action_spec = behavior_spec.action_spec
                decision_steps, _ = env.get_steps(behavior_name)
                n_agents = len(decision_steps)
                if n_agents == 0:
                    continue

                if not action_spec.is_continuous():
                    actions = np.zeros(
                        (n_agents, len(action_spec.discrete_branches)), dtype=np.int32)
                    env.set_actions(
                        behavior_name, ActionTuple(discrete=actions))
                    continue

                pred = np.zeros(
                    (n_agents, action_spec.continuous_size), dtype=np.float32)
                for row_idx in range(n_agents):
                    obs = _obs_row(decision_steps, row_idx)
                    if len(obs) != obs_dim:
                        raise RuntimeError(
                            f"Observation dimension mismatch: got {len(obs)}, expected {obs_dim}. "
                            "Train and run with the same simulator config."
                        )

                    sample = np.asarray([obs], dtype=np.float32)
                    out = np.asarray(model.predict(sample),
                                     dtype=np.float32).reshape(-1)
                    out = np.clip(out * args.policy_gain, -1.0, 1.0)

                    if speed_limit is not None and len(out) > 0:
                        idx = max(0, min(len(out) - 1, args.throttle_index))
                        out[idx] = np.clip(out[idx], -speed_limit, speed_limit)

                    n_copy = min(len(out), action_spec.continuous_size)
                    pred[row_idx, :n_copy] = out[:n_copy]

                    # Collect data if enabled
                    if collector is not None:
                        record = {
                            "env_step": step_count,
                            "behavior": behavior_name,
                            "agent_id": int(decision_steps.agent_id[row_idx]),
                            "reward": float(decision_steps.reward[row_idx]),
                            "observation": obs,
                            "observation_shapes": [list(obs_spec.shape) for obs_spec in behavior_spec.observation_specs],
                            "action_type": "continuous",
                            "action": np.asarray(pred[row_idx, :n_copy]).reshape(-1).tolist(),
                        }
                        collector.write_sample(
                            record, flush_every=args.flush_every)

                env.set_actions(behavior_name, ActionTuple(continuous=pred))

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
        env.close()


if __name__ == "__main__":
    raise SystemExit(main())
