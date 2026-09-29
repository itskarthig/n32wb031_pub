#!/usr/bin/env python3
"""Capture debug UART lines from the DUT with wall-clock timestamps.

Writes CSV rows: epoch_ns,iso_timestamp,line

Usage:
    python capture_serial.py --port COM8 --baud 115200 --duration 3600 --out serial_log.csv
"""
import argparse
import csv
import datetime
import sys
import time

import serial


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--port", required=True, help="Serial port, e.g. COM8")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--duration", type=float, default=0,
                     help="Seconds to capture (0 = run until Ctrl-C)")
    ap.add_argument("--out", required=True, help="Output CSV path")
    args = ap.parse_args()

    deadline = time.monotonic() + args.duration if args.duration > 0 else None

    try:
        ser = serial.Serial(args.port, args.baud, timeout=1)
    except serial.SerialException as exc:
        print(f"Failed to open {args.port}: {exc}", file=sys.stderr)
        return 1

    print(f"Capturing {args.port} @ {args.baud} -> {args.out}"
          + (f" for {args.duration:.0f}s" if deadline else " until Ctrl-C"))

    with ser, open(args.out, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["epoch_ns", "iso_timestamp", "line"])
        f.flush()
        try:
            while deadline is None or time.monotonic() < deadline:
                raw = ser.readline()
                if not raw:
                    continue
                epoch_ns = time.time_ns()
                iso_ts = datetime.datetime.fromtimestamp(
                    epoch_ns / 1e9, tz=datetime.timezone.utc
                ).isoformat()
                line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
                w.writerow([epoch_ns, iso_ts, line])
                f.flush()
        except KeyboardInterrupt:
            print("\nStopped by user.")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
