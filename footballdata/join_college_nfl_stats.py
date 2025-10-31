#!/usr/bin/env python3
import argparse
import re
from pathlib import Path
from typing import List, Optional
import pandas as pd

SUFFIXES = {"jr", "sr", "ii", "iii", "iv", "v"}

def normalize_name(name: str) -> str:
    if pd.isna(name):
        return ""
    s = name.lower()
    s = re.sub(r"[^\w\s]", " ", s)
    s = re.sub(r"\s+", " ", s).strip()
    parts = s.split()
    if parts and parts[-1] in SUFFIXES:
        parts = parts[:-1]
    return " ".join(parts)

def coerce_numeric(df: pd.DataFrame, cols: List[str]) -> pd.DataFrame:
    for c in cols:
        if c in df.columns:
            df[c] = (
                df[c]
                .astype(str)
                .str.replace(r"[,\s]", "", regex=True)
                .str.replace(r"[^\d\.\-]", "", regex=True)
            )
            df[c] = pd.to_numeric(df[c], errors="coerce")
    return df

YEAR_MAP = {str(i): 2009 + i for i in range(10)}

def infer_year_from_filename(fname: str) -> Optional[int]:
    m = re.search(r"\((\d)\)", fname) or re.search(r"(\d)\D*$", fname)
    if m:
        idx = m.group(1)
        return YEAR_MAP.get(idx)
    return None

def load_college_dir(college_dir: Path, infer_year: bool=False) -> pd.DataFrame:
    files = sorted(college_dir.glob("*.csv"))
    if not files:
        raise FileNotFoundError(f"No CSV files found in {college_dir}")
    frames = []
    for f in files:
        df = pd.read_csv(f)
        df["__source_file"] = f.name
        if infer_year:
            yr = infer_year_from_filename(f.name)
            if yr is not None:
                df["season_year"] = yr
        if "Stat Type" in df.columns:
            maybe_passing = df["Stat Type"].astype(str).str.contains("pass", case=False, na=False)
            if maybe_passing.any():
                df = df[maybe_passing].copy()
        frames.append(df)
    out = pd.concat(frames, ignore_index=True)
    return out


def _pick_col(cols, preferred, must_contain=None, avoid=None):
    cols_l = [c.lower() for c in cols]
    by_lower = {c.lower(): c for c in cols}

    for p in preferred:
        if p in by_lower:
            return by_lower[p]

    for c_low in cols_l:
        ok = True
        if must_contain:
            for token in must_contain:
                if token not in c_low:
                    ok = False
                    break
        if avoid and ok:
            for bad in avoid:
                if bad in c_low:
                    ok = False
                    break
        if ok:
            return by_lower[c_low]
    return None

def autodetect_nfl_cols(csv_path: Path):

    header = pd.read_csv(csv_path, nrows=0)
    cols = list(header.columns)


    name_col = _pick_col(cols, ["player_name","player","name"])
    position_col = _pick_col(cols, ["position","pos"])


    yds_pref = [
        "career_average_passing_yards",
        "season_average_passing_yards",
        "career_passing_yards",
        "season_passing_yards",
        "passing_yards",
    ]
    td_pref = [
        "career_average_pass_touchdown",
        "season_average_pass_touchdown",
        "career_pass_touchdown",
        "season_pass_touchdown",
        "pass_touchdown",
    ]
    int_pref = [
        "career_average_interception",
        "season_average_interception",
        "career_interception",
        "season_interception",
        "interception",
    ]

    yds_col = _pick_col(cols, yds_pref) or _pick_col(cols, ["passing_yards","pass_yds","pass_yards","yds"], must_contain=["pass","yard"], avoid=["rush","def"]) or _pick_col(cols, ["yds"])
    td_col  = _pick_col(cols, td_pref)  or _pick_col(cols, ["passing_touchdowns","pass_td","pass_tds","td"], must_contain=["pass","td"], avoid=["rush","def"]) or _pick_col(cols, ["td"])
    int_col = _pick_col(cols, int_pref) or _pick_col(cols, ["interceptions","passing_interceptions","int"], must_contain=["int"], avoid=["def_return","rush"]) or _pick_col(cols, ["int"])

    # Passthroughs
    draft_year = _pick_col(cols, ["draft_year","draftyr","draft-year"])
    college    = _pick_col(cols, ["college"])
    team       = _pick_col(cols, ["team","nfl_team"])

    detected = {
        "name": name_col,
        "position": position_col,
        "yds": yds_col,
        "td": td_col,
        "int": int_col,
        "draft_year": draft_year,
        "college": college,
        "team": team,
    }
    usecols = [c for c in detected.values() if c]
    return detected, usecols

def compute_avgs(df: pd.DataFrame, name_col: str, td_col: str, yds_col: str, int_col: str,
                 prefix: str, use_normalize: bool=True) -> pd.DataFrame:

    needed = [name_col, td_col, yds_col, int_col]
    for c in needed:
        if c not in df.columns:
            raise KeyError(f"Missing required column {c}")
    df = df.copy()
    # Choose join key
    if use_normalize:
        df["join_key"] = df[name_col].apply(normalize_name)
    else:
        df["join_key"] = df[name_col].astype(str).str.strip()


    coerce_numeric(df, [td_col, yds_col, int_col])

    agg_dict = {
        f"{prefix}_avg_touchdowns": (td_col, "mean"),
        f"{prefix}_avg_yards": (yds_col, "mean"),
        f"{prefix}_avg_interceptions": (int_col, "mean"),
        f"{prefix}_total_touchdowns": (td_col, "sum"),
        f"{prefix}_total_yards": (yds_col, "sum"),
        f"{prefix}_total_interceptions": (int_col, "sum"),
        f"{prefix}_seasons_count": (td_col, "count"),
    }
    if prefix == "college" and "season_year" in df.columns:
        agg_dict["college_last_year"] = ("season_year", "max")

    grouped = df.groupby("join_key", dropna=True).agg(**agg_dict).reset_index()
    return grouped

def main():
    ap = argparse.ArgumentParser(description="Join college QB stats with NFL stats by player and compute per-player averages.")
    ap.add_argument("--college_dir", required=True, type=Path, help="Directory containing per-year college CSV files.")
    ap.add_argument("--nfl_csv", required=True, type=Path, help="CSV file with NFL stats (full Kaggle file OK).")
    ap.add_argument("--out", required=True, type=Path, help="Output CSV path.")

    # Matching behavior
    ap.add_argument("--no_normalize", action="store_true", help="Join on exact names (no normalization).")
    ap.add_argument("--infer_college_year_from_filename", action="store_true", help="Add season_year from filename indices (0->2009..9->2018).")


    ap.add_argument("--nfl_only_qb", action="store_true", help="If set, keep only NFL rows where position == QB.")
    ap.add_argument("--nfl_autodetect", action="store_true", help="Auto-detect NFL columns (prefer career averages).")
    ap.add_argument("--nfl_position_col", default="position", help="NFL CSV column for position (default: position)")
    ap.add_argument("--nfl_name_col", default="Player", help="NFL CSV column for player name (default: Player)")
    ap.add_argument("--nfl_td_col", default="TD", help="NFL CSV column for touchdowns (default: TD)")
    ap.add_argument("--nfl_yds_col", default="Yds", help="NFL CSV column for yards (default: Yds)")
    ap.add_argument("--nfl_int_col", default="Int", help="NFL CSV column for interceptions (default: Int)")
    ap.add_argument("--nfl_passthrough_cols", nargs="*", default=["draft_year","college","team","position"],
                    help="Extra NFL columns to include in output if present.")


    ap.add_argument("--college_name_col", default="Player", help="College CSV column for player name (default: Player)")
    ap.add_argument("--college_td_col", default="TD", help="College CSV column for touchdowns (default: TD)")
    ap.add_argument("--college_yds_col", default="Yds", help="College CSV column for yards (default: Yds)")
    ap.add_argument("--college_int_col", default="Int", help="College CSV column for interceptions (default: Int)")

    args = ap.parse_args()
    use_normalize = not args.no_normalize


    college_df = load_college_dir(args.college_dir, infer_year=args.infer_college_year_from_filename)

    if args.nfl_autodetect:
        detected, usecols = autodetect_nfl_cols(args.nfl_csv)
        if detected["name"]: args.nfl_name_col = detected["name"]
        if detected["position"]: args.nfl_position_col = detected["position"]
        if detected["yds"]: args.nfl_yds_col = detected["yds"]
        if detected["td"]: args.nfl_td_col = detected["td"]
        if detected["int"]: args.nfl_int_col = detected["int"]
        for k in ["draft_year","college","team"]:
            if detected.get(k) and detected[k] not in args.nfl_passthrough_cols:
                args.nfl_passthrough_cols.append(detected[k])
        nfl_df = pd.read_csv(
            args.nfl_csv,
            usecols=list(set([args.nfl_name_col, args.nfl_position_col, args.nfl_td_col,
                              args.nfl_yds_col, args.nfl_int_col] + args.nfl_passthrough_cols))
        )
    else:
        nfl_df = pd.read_csv(args.nfl_csv)

    # QB filter
    if args.nfl_only_qb and args.nfl_position_col in nfl_df.columns:
        nfl_df = nfl_df[nfl_df[args.nfl_position_col].astype(str).str.upper() == 'QB'].copy()

    college_agg = compute_avgs(
        college_df,
        name_col=args.college_name_col,
        td_col=args.college_td_col,
        yds_col=args.college_yds_col,
        int_col=args.college_int_col,
        prefix="college",
        use_normalize=use_normalize,
    )

    nfl_agg = compute_avgs(
        nfl_df,
        name_col=args.nfl_name_col,
        td_col=args.nfl_td_col,
        yds_col=args.nfl_yds_col,
        int_col=args.nfl_int_col,
        prefix="nfl",
        use_normalize=use_normalize,
    )

    name_key = args.nfl_name_col
    if use_normalize:
        nfl_df_tmp = nfl_df.assign(join_key=nfl_df[name_key].apply(normalize_name))
    else:
        nfl_df_tmp = nfl_df.assign(join_key=nfl_df[name_key].astype(str).str.strip())

    aggs = {name_key: lambda s: s.value_counts().idxmax() if len(s) else None}
    for col in args.nfl_passthrough_cols:
        if col in nfl_df_tmp.columns:
            aggs[col] = lambda s: s.mode().iat[0] if not s.mode().empty else (s.dropna().iloc[0] if not s.dropna().empty else None)

    nfl_display = nfl_df_tmp.groupby("join_key").agg(aggs).reset_index()
    if name_key in nfl_display.columns:
        nfl_display = nfl_display.rename(columns={name_key: "NFL_Player_Display"})

    # Join (players in both datasets)
    merged = pd.merge(nfl_agg, college_agg, on="join_key", how="inner")
    merged = merged.merge(nfl_display, on="join_key", how="left")

    # Column order
    preferred = [
        "join_key",
        "NFL_Player_Display",
        "college_last_year",
        "college_seasons_count",
        "college_avg_touchdowns",
        "college_avg_yards",
        "college_avg_interceptions",
        "college_total_touchdowns",
        "college_total_yards",
        "college_total_interceptions",
        "nfl_seasons_count",
        "nfl_avg_touchdowns",
        "nfl_avg_yards",
        "nfl_avg_interceptions",
        "nfl_total_touchdowns",
        "nfl_total_yards",
        "nfl_total_interceptions",
        "draft_year",
        "college",
        "team",
        "position",
    ]
    cols_present = [c for c in preferred if c in merged.columns]
    merged = merged[cols_present]

    # Save
    args.out.parent.mkdir(parents=True, exist_ok=True)
    merged.to_csv(args.out, index=False)

    print(f"Merged players (college ∩ NFL): {len(merged):,}")
    print(f"Output written to: {args.out}")

if __name__ == "__main__":
    main()
