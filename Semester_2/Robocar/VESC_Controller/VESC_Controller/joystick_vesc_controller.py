#!/usr/bin/env python3
"""Gamepad controller for a VESC motor controller.

Default mapping:
    - Left joystick left/right: steering
    - L2 / LT: reverse (marche arrière)
    - R2 / RT: accelerate
    - Button L1 / LB: emergency brake
    - Button A / Cross / South: emergency stop
    - Button Start / Options: quit

Examples:
  python3 joystick_vesc_controller.py --port /dev/ttyACM0
  python3 joystick_vesc_controller.py --port /dev/ttyACM0 --max-duty 0.2
"""

from __future__ import annotations

import argparse
import sys
import time
from dataclasses import dataclass
from contextlib import suppress

try:
    import pygame
    import serial
    from pyvesc.protocol.interface import encode
    from pyvesc.VESC.messages.setters import Alive, SetDutyCycle, SetServoPosition
except ImportError as exc:
    print(
        "Missing dependency: install pyvesc, pyserial and pygame before running this script.",
        file=sys.stderr,
    )
    raise SystemExit(1) from exc


DEFAULT_BAUDRATE = 115200
DEFAULT_TIMEOUT = 0.1
DEFAULT_DEADZONE = 0.12
DEFAULT_MAX_DUTY = 0.15
DEFAULT_MAX_STEERING = 0.35
DEFAULT_CENTER_SERVO = 0.5
DEFAULT_LOOP_HZ = 50
DEFAULT_HEARTBEAT_PERIOD = 0.1
DEFAULT_MACRO_BUTTONS = "7"
DEFAULT_QUIT_BUTTON = 11
DEFAULT_MACRO_CENTER_SECONDS = 0.5
DEFAULT_MACRO_TURN_SECONDS = 1.0
DEFAULT_MACRO_RAMP_SECONDS = 2.0
DEFAULT_MACRO_PAUSE_SECONDS = 0.25


@dataclass
class MacroState:
    active: bool = False
    start_time: float = 0.0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Control a VESC with a gamepad using pygame."
    )
    parser.add_argument(
        "--port",
        default="/dev/ttyACM0",
        help="Serial port of the VESC (default: /dev/ttyACM0).",
    )
    parser.add_argument(
        "--baudrate",
        type=int,
        default=DEFAULT_BAUDRATE,
        help="Serial baudrate (default: 115200).",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=DEFAULT_TIMEOUT,
        help="Serial timeout in seconds (default: 0.1).",
    )
    parser.add_argument(
        "--joystick",
        type=int,
        default=0,
        help="Joystick index to use (default: 0).",
    )
    parser.add_argument(
        "--brake-button",
        type=int,
        default=4,
        help="Button index to use as emergency brake (default: 4 - L1 on many controllers).",
    )
    parser.add_argument(
        "--macro-buttons",
        default=DEFAULT_MACRO_BUTTONS,
        help="Comma-separated button indices that can start the automatic sequence (default: 7).",
    )
    parser.add_argument(
        "--quit-button",
        type=int,
        default=DEFAULT_QUIT_BUTTON,
        help="Button index to quit the program (default: 11 - right stick press on many controllers).",
    )
    parser.add_argument(
        "--deadzone",
        type=float,
        default=DEFAULT_DEADZONE,
        help="Deadzone applied to stick axes (default: 0.12).",
    )
    parser.add_argument(
        "--max-duty",
        type=float,
        default=DEFAULT_MAX_DUTY,
        help="Maximum absolute duty cycle sent to the motor (default: 0.15).",
    )
    parser.add_argument(
        "--max-steering",
        type=float,
        default=DEFAULT_MAX_STEERING,
        help="Maximum servo deflection from center (default: 0.35).",
    )
    parser.add_argument(
        "--center-servo",
        type=float,
        default=DEFAULT_CENTER_SERVO,
        help="Servo center position in the range [0.0, 1.0] (default: 0.5).",
    )
    parser.add_argument(
        "--loop-hz",
        type=int,
        default=DEFAULT_LOOP_HZ,
        help="Control loop frequency in Hz (default: 50).",
    )
    parser.add_argument(
        "--transition-delay",
        type=float,
        default=0.25,
        help="Delay in seconds used to stop before switching direction (default: 0.25).",
    )
    parser.add_argument(
        "--macro-turn-seconds",
        type=float,
        default=DEFAULT_MACRO_TURN_SECONDS,
        help="Duration of each steering phase in the START macro (default: 1.0).",
    )
    parser.add_argument(
        "--macro-center-seconds",
        type=float,
        default=DEFAULT_MACRO_CENTER_SECONDS,
        help="Duration of the centered steering phase at the start of the START macro (default: 0.5).",
    )
    parser.add_argument(
        "--macro-ramp-seconds",
        type=float,
        default=DEFAULT_MACRO_RAMP_SECONDS,
        help="Duration of the forward/reverse ramps in the START macro (default: 2.0).",
    )
    parser.add_argument(
        "--macro-pause-seconds",
        type=float,
        default=DEFAULT_MACRO_PAUSE_SECONDS,
        help="Pause inserted between forward and reverse in the START macro (default: 0.25).",
    )
    parser.add_argument(
        "--firmware",
        action="store_true",
        help="Print the firmware version before starting the control loop.",
    )
    parser.add_argument(
        "--debug-buttons",
        action="store_true",
        help="Print every joystick button index when pressed.",
    )
    return parser.parse_args()


def clamp(value: float, minimum: float, maximum: float) -> float:
    return max(minimum, min(maximum, value))


def parse_button_list(raw_value: str) -> set[int]:
    button_ids: set[int] = set()
    for item in raw_value.split(","):
        item = item.strip()
        if not item:
            continue
        button_ids.add(int(item))
    return button_ids


def apply_deadzone(value: float, deadzone: float) -> float:
    if abs(value) < deadzone:
        return 0.0
    return value


def map_servo(axis_value: float, center_servo: float, max_steering: float) -> float:
    servo_value = center_servo + axis_value * max_steering
    return clamp(servo_value, 0.0, 1.0)


def map_duty(axis_value: float, max_duty: float) -> float:
    return clamp(axis_value * max_duty, -abs(max_duty), abs(max_duty))


def apply_macro(now: float, start_time: float, args: argparse.Namespace) -> tuple[float, float, bool]:
    left_servo = 0.0
    right_servo = 1.0
    center_servo = args.center_servo

    center_seconds = max(args.macro_center_seconds, 0.0)
    turn_seconds = max(args.macro_turn_seconds, 0.0)
    ramp_seconds = max(args.macro_ramp_seconds, 0.0)
    pause_seconds = max(args.macro_pause_seconds, 0.0)

    elapsed = now - start_time
    center_end = center_seconds
    left_end = center_end + turn_seconds
    right_end = left_end + turn_seconds
    forward_end = right_end + ramp_seconds
    pause_end = forward_end + pause_seconds
    reverse_end = pause_end + ramp_seconds

    if elapsed < center_end:
        return center_servo, 0.0, True
    if elapsed < left_end:
        return left_servo, 0.0, True
    if elapsed < right_end:
        return right_servo, 0.0, True
    if elapsed < forward_end and ramp_seconds > 0.0:
        progress = (elapsed - right_end) / ramp_seconds
        return center_servo, map_duty(progress, args.max_duty), True
    if elapsed < pause_end:
        return center_servo, 0.0, True
    if elapsed < reverse_end and ramp_seconds > 0.0:
        progress = (elapsed - pause_end) / ramp_seconds
        return center_servo, -map_duty(progress, args.max_duty), True

    return center_servo, 0.0, False


def read_trigger(joystick: pygame.joystick.Joystick, axis_index: int, button_index: int) -> float:
    if axis_index < joystick.get_numaxes():
        value = joystick.get_axis(axis_index)
        if value != 0.0:
            return clamp((value + 1.0) / 2.0, 0.0, 1.0)

    if button_index < joystick.get_numbuttons() and joystick.get_button(button_index):
        return 1.0

    return 0.0


def send_packet(serial_port: serial.Serial, message: object) -> None:
    serial_port.write(encode(message))


def stop_outputs(serial_port: serial.Serial, center_servo: float) -> None:
    with suppress(Exception):
        send_packet(serial_port, SetDutyCycle(0.0))
        send_packet(serial_port, SetServoPosition(center_servo))


def stop_motor(serial_port: serial.Serial) -> None:
    with suppress(Exception):
        send_packet(serial_port, SetDutyCycle(0.0))


def start_macro(macro_state: MacroState) -> None:
    macro_state.active = True
    macro_state.start_time = time.monotonic()


def main() -> int:
    args = parse_args()

    if args.max_duty <= 0:
        print("--max-duty must be greater than 0.", file=sys.stderr)
        return 1
    if args.transition_delay < 0:
        print("--transition-delay must be greater than or equal to 0.", file=sys.stderr)
        return 1
    if args.macro_turn_seconds < 0 or args.macro_ramp_seconds < 0 or args.macro_pause_seconds < 0:
        print("Macro durations must be greater than or equal to 0.", file=sys.stderr)
        return 1
    if args.max_steering < 0:
        print("--max-steering must be positive.", file=sys.stderr)
        return 1
    if not 0.0 <= args.center_servo <= 1.0:
        print("--center-servo must be in the range [0.0, 1.0].", file=sys.stderr)
        return 1
    if not 0.0 <= args.deadzone < 1.0:
        print("--deadzone must be in the range [0.0, 1.0).", file=sys.stderr)
        return 1

    try:
        macro_buttons = parse_button_list(args.macro_buttons)
    except ValueError:
        print("--macro-buttons must be a comma-separated list of integers.", file=sys.stderr)
        return 1

    pygame.init()
    pygame.joystick.init()

    try:
        serial_port = serial.Serial(
            args.port,
            baudrate=args.baudrate,
            timeout=args.timeout,
        )
    except serial.SerialException as exc:
        print(f"Unable to open serial port {args.port}: {exc}", file=sys.stderr)
        pygame.quit()
        return 2

    if pygame.joystick.get_count() == 0:
        print("No joystick detected.", file=sys.stderr)
        serial_port.close()
        pygame.quit()
        return 3

    joystick_index = clamp(args.joystick, 0, pygame.joystick.get_count() - 1)
    joystick = pygame.joystick.Joystick(int(joystick_index))
    joystick.init()

    print(f"Using joystick: {joystick.get_name()}")
    print("Controls: left stick X = steering, L2/LT = reverse, R2/RT = accelerate, L1 = emergency brake")
    print("Press START to launch the automatic sequence, A/Cross/South to stop, and ESC to quit.")
    print(f"Macro buttons enabled: {sorted(macro_buttons)}")

    try:
        clock = pygame.time.Clock()
        running = True
        last_duty = 0.0
        last_servo = args.center_servo
        last_heartbeat_time = 0.0
        pending_direction = 0
        pending_switch_at = 0.0
        current_direction = 0
        macro_state = MacroState()

        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                    running = False
                elif event.type == pygame.JOYBUTTONDOWN:
                    if args.debug_buttons:
                        print(f"Joystick button pressed: {event.button}")
                    if event.button == 0:
                        stop_outputs(serial_port, args.center_servo)
                        last_duty = 0.0
                        last_servo = args.center_servo
                        pending_direction = 0
                        current_direction = 0
                        macro_state.active = False
                    elif event.button in macro_buttons:
                        start_macro(macro_state)
                        pending_direction = 0
                        current_direction = 0
                        print("START macro launched.")
                    elif event.button == args.quit_button:
                        running = False

            if not running:
                break

            now = time.monotonic()

            if macro_state.active:
                servo_value, duty_cycle, macro_running = apply_macro(now, macro_state.start_time, args)

                if servo_value != last_servo:
                    send_packet(serial_port, SetServoPosition(servo_value))
                    last_servo = servo_value

                if duty_cycle != last_duty:
                    send_packet(serial_port, SetDutyCycle(duty_cycle))
                    last_duty = duty_cycle

                if not macro_running:
                    macro_state.active = False
                    stop_outputs(serial_port, args.center_servo)
                    last_duty = 0.0
                    last_servo = args.center_servo
                else:
                    if now - last_heartbeat_time >= DEFAULT_HEARTBEAT_PERIOD:
                        send_packet(serial_port, Alive())
                        last_heartbeat_time = now

                    clock.tick(args.loop_hz)
                    continue

            steering_axis = apply_deadzone(joystick.get_axis(0), args.deadzone)
            brake_value = read_trigger(joystick, 2, 6)
            accelerate_value = read_trigger(joystick, 5, 7)
            brake_button_pressed = False
            if 0 <= args.brake_button < joystick.get_numbuttons():
                brake_button_pressed = bool(joystick.get_button(args.brake_button))

            servo_value = map_servo(steering_axis, args.center_servo, args.max_steering)

            # Emergency brake takes precedence and clears any pending direction change.
            if brake_button_pressed:
                if last_duty != 0.0:
                    stop_motor(serial_port)
                    last_duty = 0.0
                pending_direction = 0
                current_direction = 0
            else:
                desired_direction = 0
                desired_strength = 0.0

                if accelerate_value > args.deadzone and brake_value > args.deadzone:
                    desired_direction = 0
                elif accelerate_value > args.deadzone:
                    desired_direction = 1
                    desired_strength = accelerate_value
                elif brake_value > args.deadzone:
                    desired_direction = -1
                    desired_strength = brake_value

                if desired_direction == 0:
                    if last_duty != 0.0:
                        stop_motor(serial_port)
                        last_duty = 0.0
                    pending_direction = 0
                    current_direction = 0
                elif current_direction == desired_direction:
                    duty_cycle = map_duty(desired_strength, args.max_duty)
                    if desired_direction < 0:
                        duty_cycle = -duty_cycle
                    if duty_cycle != last_duty:
                        send_packet(serial_port, SetDutyCycle(duty_cycle))
                        last_duty = duty_cycle
                    pending_direction = 0
                else:
                    if pending_direction != desired_direction:
                        pending_direction = desired_direction
                        pending_switch_at = now + args.transition_delay
                        if last_duty != 0.0:
                            stop_motor(serial_port)
                            last_duty = 0.0
                        current_direction = 0
                    elif now >= pending_switch_at:
                        duty_cycle = map_duty(desired_strength, args.max_duty)
                        if desired_direction < 0:
                            duty_cycle = -duty_cycle
                        if duty_cycle != last_duty:
                            send_packet(serial_port, SetDutyCycle(duty_cycle))
                            last_duty = duty_cycle
                        current_direction = desired_direction
                        pending_direction = 0

            if servo_value != last_servo:
                send_packet(serial_port, SetServoPosition(servo_value))
                last_servo = servo_value

            if now - last_heartbeat_time >= DEFAULT_HEARTBEAT_PERIOD:
                send_packet(serial_port, Alive())
                last_heartbeat_time = now

            clock.tick(args.loop_hz)
    except KeyboardInterrupt:
        pass
    finally:
        stop_outputs(serial_port, args.center_servo)
        serial_port.close()
        joystick.quit()
        pygame.joystick.quit()
        pygame.quit()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())