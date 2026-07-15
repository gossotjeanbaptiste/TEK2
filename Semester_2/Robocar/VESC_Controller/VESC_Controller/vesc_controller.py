#!/usr/bin/env python3
"""Simple command-line controller for a VESC motor controller.

Examples:
  python3 vesc_controller.py --port /dev/ttyACM0 --firmware
  python3 vesc_controller.py --port /dev/ttyACM0 --duty 0.1 --duration 2
  python3 vesc_controller.py --port /dev/ttyACM0 --servo 0.5
"""

from __future__ import annotations

import argparse
import sys
import time
from contextlib import suppress

try:
    import serial
    from pyvesc import VESC
except ImportError as exc:
    print(
        "Missing dependency: install pyvesc and pyserial before running this script.",
        file=sys.stderr,
    )
    raise SystemExit(1) from exc


DEFAULT_BAUDRATE = 115200
DEFAULT_TIMEOUT = 0.1


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Control a VESC over USB/UART using pyvesc."
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
        "--firmware",
        action="store_true",
        help="Print the firmware version reported by the VESC.",
    )
    parser.add_argument(
        "--duty",
        type=float,
        help="Set motor duty cycle. Example: 0.1 for 10%% forward.",
    )
    parser.add_argument(
        "--servo",
        type=float,
        help="Set servo output position in the range [0.0, 1.0].",
    )
    parser.add_argument(
        "--duration",
        type=float,
        default=0.0,
        help="Duration in seconds to keep the command active before stopping.",
    )
    parser.add_argument(
        "--stop",
        action="store_true",
        help="Send a stop command (set duty cycle to 0 and servo to center).",
    )
    return parser.parse_args()


def clamp(value: float, minimum: float, maximum: float) -> float:
    return max(minimum, min(maximum, value))


def stop_outputs(vesc: VESC) -> None:
    with suppress(Exception):
        vesc.set_duty_cycle(0.0)
        vesc.set_servo(0.5)
        vesc.stop_heartbeat()


def main() -> int:
    args = parse_args()

    if args.duty is None and args.servo is None and not args.firmware and not args.stop:
        print(
            "No action requested. Use --firmware, --duty, --servo or --stop.",
            file=sys.stderr,
        )
        return 1

    try:
        serial_port = serial.Serial(
            args.port,
            baudrate=args.baudrate,
            timeout=args.timeout,
        )
    except serial.SerialException as exc:
        print(f"Unable to open serial port {args.port}: {exc}", file=sys.stderr)
        return 2

    try:
        with VESC(serial_port=serial_port) as vesc:
            try:
                if args.firmware:
                    firmware_version = vesc.get_firmware_version()
                    print(f"Connected to VESC on {args.port}")
                    print(f"Firmware version: {firmware_version}")

                if args.stop:
                    stop_outputs(vesc)
                    print("Stop command sent.")

                if args.servo is not None:
                    servo_value = clamp(args.servo, 0.0, 1.0)
                    if servo_value != args.servo:
                        print(
                            f"Servo value clamped from {args.servo} to {servo_value}",
                            file=sys.stderr,
                        )
                    vesc.set_servo(servo_value)
                    print(f"Servo set to {servo_value}")

                if args.duty is not None:
                    vesc.set_duty_cycle(args.duty)
                    print(f"Duty cycle set to {args.duty}")

                if args.duration > 0 and (args.duty is not None or args.servo is not None):
                    time.sleep(args.duration)
                    stop_outputs(vesc)
                    print("Command duration elapsed, outputs reset.")
                elif args.firmware and not args.stop:
                    stop_outputs(vesc)
            except KeyboardInterrupt:
                print("Interrupted by user, stopping outputs...")
                stop_outputs(vesc)
                return 130
    except KeyboardInterrupt:
        return 130
    finally:
        serial_port.close()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
