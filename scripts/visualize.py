#!/usr/bin/env python3

import argparse
import csv
import os
from collections import defaultdict

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError:  # pragma: no cover
    print("Matplotlib is required for visualization. Run: python3 -m pip install -r requirements.txt")
    raise SystemExit(1)


def load_final_frame(path: str):
    frame_rows = defaultdict(list)
    with open(path, newline="") as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            frame_rows[int(row["frame"])].append(row)

    if not frame_rows:
        raise ValueError(f"No simulation data found in '{path}'")

    final_frame = max(frame_rows)
    return frame_rows[final_frame]


def main():
    parser = argparse.ArgumentParser(description="Visualize final entity positions from the sandbox simulation.")
    parser.add_argument("--input", default="output/entity_state.csv", help="Path to the CSV output from the C++ simulation.")
    parser.add_argument("--output", default="output/entity_positions.png", help="Destination file for the generated plot.")
    args = parser.parse_args()

    rows = load_final_frame(args.input)
    xs = [float(row["x"]) for row in rows]
    ys = [float(row["y"]) for row in rows]

    os.makedirs(os.path.dirname(args.output) or ".", exist_ok=True)

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.scatter(xs, ys, s=18, c="tab:blue", alpha=0.8)
    ax.set_title("Sandbox Entity Mechanics Prototype")
    ax.set_xlabel("World X")
    ax.set_ylabel("World Y")
    ax.set_aspect("equal")
    ax.grid(True, linestyle="--", alpha=0.3)

    fig.tight_layout()
    fig.savefig(args.output, dpi=200)
    print(f"Visualization saved to: {args.output}")


if __name__ == "__main__":
    main()
