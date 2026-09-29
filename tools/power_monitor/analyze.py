#!/usr/bin/env python3
"""Correlate an iot_recoder-win power-capture CSV with a serial debug-log CSV
to find periods where current stays abnormally elevated with no matching
wake-cause in the log (unexplained wake / STOP-not-re-entered class of bug).

Inputs:
  --power   iot_recoder-win CSV: time,Voltage(V),Current(mA),raw voltage data(V),raw current data(mA)
            'time' is epoch nanoseconds.
  --log     capture_serial.py CSV: epoch_ns,iso_timestamp,line

Output:
  --report  Markdown report path (default: report.md next to --power)

No third-party dependencies (stdlib only) so it runs on a bare Python 3.12
install without needing pandas/numpy.
"""
import argparse
import csv
import datetime as dt
import sys
from dataclasses import dataclass, field

# Substrings (case-insensitive) that mark "something is actively happening"
# in the debug log, grouped by subsystem. Each group has its own hold time:
# short bursts (WMBus TX, cellular AT commands, BLE connect) only need a few
# seconds either side of the log line; a slave-meter session, per this
# project's architecture (CLAUDE.md), can legitimately sit quiet between AT
# commands for stretches approaching its 60s idle timeout while still
# blocking STOP -- so it gets a much longer hold to bridge those gaps
# instead of losing track of "still in session".
MARKER_GROUPS = {
    "wmbus": (["[WMBUS]", "TX >>", "TX DONE", "RX <<", "RADIO"], 3.0),
    "cellular": (["[CELL]", "AT+", "SENDB", "NETOPEN", "CIPOPEN", "CIPSEND",
                  "CIPCLOSE", "NTP", "CNTP", "CCLK"], 5.0),
    "ble": (["APP_BLE_CONNECTED", "FAST_ADV", "CONNECTED", "GAPC_CONNECT"], 3.0),
    "slave": (["[SLAVE]", "WAKE"], 65.0),
    "fuota": (["FUOTA"], 3.0),
}

# Lines that indicate a session/activity just ENDED -- used to anchor the
# "stuck active after this point" check.
END_MARKERS = [
    "CMD_RESET", "CIPCLOSE", "DISCONNECT", "ARMSLAVEWAKEUP", "SLOW_ADV",
    "END:", "IDLE",
]


@dataclass
class LogLine:
    epoch_ns: int
    iso: str
    text: str


@dataclass
class Sample:
    epoch_ns: int
    current_ua: float


@dataclass
class Anomaly:
    kind: str
    start_ns: int
    end_ns: int
    avg_current_ua: float
    detail: str = ""


def load_power(path: str) -> list:
    samples = []
    with open(path, newline="", encoding="utf-8-sig") as f:
        r = csv.DictReader(f)
        for row in r:
            try:
                t = int(float(row["time"]))
                cur_ma = float(row["Current(mA)"])
            except (KeyError, ValueError):
                continue
            samples.append(Sample(epoch_ns=t, current_ua=cur_ma * 1000.0))
    samples.sort(key=lambda s: s.epoch_ns)
    return samples


def load_log(path: str) -> list:
    lines = []
    with open(path, newline="", encoding="utf-8-sig") as f:
        r = csv.DictReader(f)
        for row in r:
            try:
                t = int(row["epoch_ns"])
            except (KeyError, ValueError, TypeError):
                continue
            lines.append(LogLine(epoch_ns=t, iso=row.get("iso_timestamp", ""),
                                  text=row.get("line", "")))
    lines.sort(key=lambda l: l.epoch_ns)
    return lines


def bucket_samples(samples, bucket_s: float):
    """Return list of (bucket_start_ns, avg_current_ua)."""
    if not samples:
        return []
    bucket_ns = int(bucket_s * 1e9)
    start = samples[0].epoch_ns - (samples[0].epoch_ns % bucket_ns)
    buckets = {}
    for s in samples:
        b = start + ((s.epoch_ns - start) // bucket_ns) * bucket_ns
        buckets.setdefault(b, []).append(s.current_ua)
    return sorted((b, sum(v) / len(v)) for b, v in buckets.items())


def _merge_windows(raw):
    if not raw:
        return []
    raw.sort()
    merged = [list(raw[0])]
    for s, e in raw[1:]:
        if s <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], e)
        else:
            merged.append([s, e])
    return [tuple(m) for m in merged]


def build_active_windows(lines, hold_scale: float = 1.0):
    # Symmetric hold around each matching log line: the current spike a line
    # describes can land in the same 1s bucket as the line's timestamp even
    # if the line itself is logged a fraction of a second into that bucket
    # (e.g. "Tx >>" printed at 10.08s still belongs to the 10.0s bucket).
    # Each subsystem group (see MARKER_GROUPS) has its own hold duration --
    # merge per group first, then union across groups.
    raw = []
    for markers, hold_s in MARKER_GROUPS.values():
        hold_ns = int(hold_s * hold_scale * 1e9)
        for l in lines:
            up = l.text.upper()
            if any(m in up for m in markers):
                raw.append((l.epoch_ns - hold_ns, l.epoch_ns + hold_ns))
    return _merge_windows(raw)


def build_end_events(lines):
    out = []
    for l in lines:
        up = l.text.upper()
        if any(m in up for m in END_MARKERS):
            out.append(l.epoch_ns)
    return out


def overlaps(t, windows):
    for s, e in windows:
        if s <= t <= e:
            return True
    return False


def context_lines(lines, center_ns, span_s):
    span_ns = int(span_s * 1e9)
    return [l for l in lines if center_ns - span_ns <= l.epoch_ns <= center_ns + span_ns]


def fmt_ts(epoch_ns: int) -> str:
    return dt.datetime.fromtimestamp(epoch_ns / 1e9, tz=dt.timezone.utc).isoformat()


def find_unexplained_wakes(buckets, active_windows, active_threshold_ua):
    anomalies = []
    run = []
    for t, cur in buckets:
        elevated = cur > active_threshold_ua
        explained = overlaps(t, active_windows)
        if elevated and not explained:
            run.append((t, cur))
        else:
            if run:
                anomalies.append(_finalize_run(run, "unexplained_wake"))
                run = []
    if run:
        anomalies.append(_finalize_run(run, "unexplained_wake"))
    return anomalies


def _finalize_run(run, kind):
    start = run[0][0]
    end = run[-1][0]
    avg = sum(c for _, c in run) / len(run)
    return Anomaly(kind=kind, start_ns=start, end_ns=end, avg_current_ua=avg)


def find_stuck_active(end_events, buckets, active_windows, active_threshold_ua,
                       settle_s, check_span_s):
    settle_ns = int(settle_s * 1e9)
    span_ns = int(check_span_s * 1e9)
    anomalies = []
    for end_t in end_events:
        win_start = end_t + settle_ns
        win_end = win_start + span_ns
        vals = [cur for t, cur in buckets if win_start <= t <= win_end]
        if not vals:
            continue
        avg = sum(vals) / len(vals)
        if avg > active_threshold_ua and not overlaps(win_start, active_windows):
            anomalies.append(Anomaly(
                kind="stuck_active",
                start_ns=win_start, end_ns=win_end, avg_current_ua=avg,
                detail=f"current stayed elevated >= {settle_s:.0f}s after an end-marker log line"))
    return anomalies


def find_missing_return_to_baseline(buckets, active_windows, baseline_ua, tolerance_ua):
    if not buckets:
        return []
    last_active_end = active_windows[-1][1] if active_windows else buckets[0][0]
    tail = [(t, cur) for t, cur in buckets if t >= last_active_end]
    if not tail:
        return []
    if all(cur > baseline_ua + tolerance_ua for _, cur in tail):
        avg = sum(c for _, c in tail) / len(tail)
        return [Anomaly(
            kind="missing_return_to_baseline",
            start_ns=tail[0][0], end_ns=tail[-1][0], avg_current_ua=avg,
            detail="current never dropped back to baseline for the rest of the run")]
    return []


def dedupe_overlapping(anomalies):
    """Merge stuck_active / unexplained_wake anomalies that overlap in time."""
    anomalies = sorted(anomalies, key=lambda a: a.start_ns)
    out = []
    for a in anomalies:
        if out and a.start_ns <= out[-1].end_ns and a.kind == out[-1].kind:
            out[-1] = Anomaly(
                kind=a.kind, start_ns=out[-1].start_ns, end_ns=max(out[-1].end_ns, a.end_ns),
                avg_current_ua=(out[-1].avg_current_ua + a.avg_current_ua) / 2,
                detail=out[-1].detail or a.detail)
        else:
            out.append(a)
    return out


def write_report(path, samples, buckets, lines, active_windows, anomalies, args):
    total_buckets = len(buckets)
    baseline_n = sum(1 for _, c in buckets
                      if abs(c - args.baseline_ua) <= args.tolerance_ua)
    elevated_n = sum(1 for _, c in buckets if c > args.active_threshold_ua)

    with open(path, "w", encoding="utf-8") as f:
        f.write("# Power / Log Correlation Report\n\n")
        if samples:
            f.write(f"- Run span: {fmt_ts(samples[0].epoch_ns)} -> "
                     f"{fmt_ts(samples[-1].epoch_ns)}\n")
        f.write(f"- Power samples: {len(samples)}, log lines: {len(lines)}, "
                 f"1s buckets: {total_buckets}\n")
        f.write(f"- Baseline definition: {args.baseline_ua:.0f} uA "
                 f"+/- {args.tolerance_ua:.0f} uA\n")
        f.write(f"- Active-current threshold: {args.active_threshold_ua:.0f} uA\n")
        f.write(f"- Active (log-explained) windows: {len(active_windows)}\n\n")

        if total_buckets:
            f.write("## Summary\n\n")
            f.write(f"- Buckets at baseline: {baseline_n} "
                     f"({100.0 * baseline_n / total_buckets:.1f}%)\n")
            f.write(f"- Buckets elevated (> threshold): {elevated_n} "
                     f"({100.0 * elevated_n / total_buckets:.1f}%)\n")
            f.write(f"- Anomalies found: {len(anomalies)}\n\n")

        if not anomalies:
            f.write("No anomalies found with the current thresholds.\n")
            return

        f.write("## Anomalies\n\n")
        for i, a in enumerate(anomalies, 1):
            dur_s = (a.end_ns - a.start_ns) / 1e9
            f.write(f"### {i}. `{a.kind}` — {fmt_ts(a.start_ns)} "
                     f"({dur_s:.1f}s, avg {a.avg_current_ua:.0f} uA)\n\n")
            if a.detail:
                f.write(f"{a.detail}\n\n")
            ctx = context_lines(lines, a.start_ns, args.context_seconds)
            if ctx:
                f.write("Nearby log lines:\n\n```\n")
                for l in ctx:
                    f.write(f"{l.iso}  {l.text}\n")
                f.write("```\n\n")
            else:
                f.write("(no log lines within the context window — MCU may be "
                         "silent, which is itself notable if current is high)\n\n")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--power", required=True, help="iot_recoder-win CSV path")
    ap.add_argument("--log", required=True, help="capture_serial.py CSV path")
    ap.add_argument("--report", default=None, help="Output report.md path")
    ap.add_argument("--baseline-ua", type=float, default=55.0)
    ap.add_argument("--tolerance-ua", type=float, default=20.0)
    ap.add_argument("--active-threshold-ua", type=float, default=200.0)
    ap.add_argument("--settle-seconds", type=float, default=5.0)
    ap.add_argument("--check-span-seconds", type=float, default=10.0)
    ap.add_argument("--event-hold-scale", type=float, default=1.0,
                     help="Multiplier applied to each marker group's hold time "
                          "(see MARKER_GROUPS in the source)")
    ap.add_argument("--bucket-seconds", type=float, default=1.0)
    ap.add_argument("--context-seconds", type=float, default=5.0)
    args = ap.parse_args()

    report_path = args.report or (args.power.rsplit(".", 1)[0] + "_report.md")

    samples = load_power(args.power)
    lines = load_log(args.log)
    if not samples:
        print("No power samples parsed -- check --power CSV format.", file=sys.stderr)
        return 1

    buckets = bucket_samples(samples, args.bucket_seconds)
    active_windows = build_active_windows(lines, args.event_hold_scale)
    end_events = build_end_events(lines)

    anomalies = []
    anomalies += find_unexplained_wakes(buckets, active_windows, args.active_threshold_ua)
    anomalies += find_stuck_active(end_events, buckets, active_windows,
                                    args.active_threshold_ua, args.settle_seconds,
                                    args.check_span_seconds)
    anomalies += find_missing_return_to_baseline(buckets, active_windows,
                                                   args.baseline_ua, args.tolerance_ua)
    anomalies = dedupe_overlapping(anomalies)

    write_report(report_path, samples, buckets, lines, active_windows, anomalies, args)
    print(f"Report written to {report_path} ({len(anomalies)} anomalies)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
