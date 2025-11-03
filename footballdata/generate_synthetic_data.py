#!/usr/bin/env python3
"""
Generate synthetic college-average QB rows and append to an existing merged CSV
without requiring pandas/numpy. Writes combined CSV to --out.

Example:
  python footballdata/generate_synthetic_no_pandas.py \
    --in footballdata/merged_stats_reduced.csv \
    --out footballdata/merged_stats_reduced_with_synth.csv \
    --total 100000 --seed 42
"""
import csv
import argparse
import random
from pathlib import Path

DEFAULT_COLS = [
    "NFL_Player_Display",
    "college_avg_touchdowns",
    "college_avg_yards",
    "college_avg_interceptions",
    "college_avg_wins",
    "nfl_avg_touchdowns",
    "nfl_avg_yards",
    "nfl_avg_interceptions",
    "nfl_avg_wins",
]


def read_existing(infile):
    with infile.open("r", newline="", encoding="utf-8") as f:
        r = csv.reader(f)
        try:
            header = next(r)
        except StopIteration:
            return [], []
        rows = [row for row in r]
    return header, rows


def write_rows(outfile, header, rows):
    with outfile.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(header)
        w.writerows(rows)


def make_synth_row(idx, seed=None):
    # seed is only used to initialize global RNG; here we use random
    # Touchdowns: mean 25 sd 6
    td = max(0.0, random.gauss(25.0, 6.0))
    yds = max(0.0, random.gauss(3200.0, 900.0))
    ints = max(0.0, random.gauss(7.0, 3.0))
    wins = min(16.0, max(0.0, random.gauss(8.0, 2.0)))
    idstr = f"S{idx:07d}"
    # return values as strings (similar formatting to existing CSV floats)
    return [
        idstr,
        repr(td),
        repr(yds),
        repr(ints),
        repr(wins),
        "",  # nfl_avg_touchdowns
        "",
        "",
        ""
    ]


def remap_row_to_header(old_header, old_row, out_header):
    # Build a dict from old_header->value
    mapping = {k: v for k, v in zip(old_header, old_row)}
    out_row = []
    for col in out_header:
        out_row.append(mapping.get(col, ""))
    return out_row


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="infile", default="footballdata/merged_stats_reduced.csv")
    ap.add_argument("--out", dest="outfile", default="footballdata/merged_stats_reduced_with_synth.csv")
    ap.add_argument("--total", dest="total", type=int, default=100000)
    ap.add_argument("--seed", dest="seed", type=int, default=42)
    args = ap.parse_args()

    infile = Path(args.infile)
    outfile = Path(args.outfile)

    if not infile.exists():
        raise SystemExit(f"Input not found: {infile}")

    random.seed(args.seed)

    old_header, old_rows = read_existing(infile)
    existing = len(old_rows)
    need = args.total - existing
    print(f"Existing rows: {existing}; target total: {args.total}; need to add: {max(0, need)}")

    # Determine output header (use DEFAULT_COLS). If input has extra columns,
    # they'll be ignored; if input lacks some expected cols we'll create them.
    out_header = DEFAULT_COLS

    # Remap existing rows to out_header
    mapped_existing = [remap_row_to_header(old_header, row, out_header) for row in old_rows]

    # Generate synthetic rows
    if need > 0:
        synth_rows = []
        # choose start id to avoid trivial collisions; we use 1..need
        start_id = 1
        for i in range(need):
            if (i + 1) % 10000 == 0:
                print(f"  generating {i + 1}/{need} synthetic rows...")
            synth_rows.append(make_synth_row(start_id + i))
    else:
        synth_rows = []

    combined = mapped_existing + synth_rows
    print(f"Writing {len(combined)} rows to {outfile}")
    outfile.parent.mkdir(parents=True, exist_ok=True)
    write_rows(outfile, out_header, combined)
    print("Done.")


if __name__ == "__main__":
    main()