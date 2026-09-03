#!/usr/bin/env python3
"""Render a CPU scheduling Gantt chart from id:duration command-line segments."""

import sys

import matplotlib.pyplot as plt


def parse_segments(arguments):
    segments = []
    for item in arguments:
        try:
            process_id, duration = item.split(":", 1)
            process_id = int(process_id)
            duration = int(duration)
        except ValueError as exc:
            raise ValueError(f"Invalid Gantt segment: {item!r}") from exc
        if duration <= 0:
            raise ValueError(f"Duration must be positive: {item!r}")
        segments.append((process_id, duration))
    return segments


def plot(segments):
    starts = []
    ends = []
    labels = []
    time = 0

    for process_id, duration in segments:
        starts.append(time)
        time += duration
        ends.append(time)
        labels.append("IDLE" if process_id == 0 else f"P{process_id}")

    fig, ax = plt.subplots(figsize=(max(9, len(segments) * 1.25), 4.5))

    for start, end, label in zip(starts, ends, labels):
        ax.barh(
            0,
            end - start,
            left=start,
            height=0.55,
            edgecolor="black",
            linewidth=1.2,
        )
        ax.text(
            (start + end) / 2,
            0,
            label,
            ha="center",
            va="center",
            fontsize=10,
            fontweight="bold",
        )

    boundaries = starts + ends[-1:]
    ax.set_xticks(boundaries)
    ax.set_xlim(0, ends[-1])
    ax.set_yticks([])
    ax.set_xlabel("Time")
    ax.set_title("CPU Scheduling Gantt Chart")
    ax.grid(axis="x", linestyle="--", alpha=0.35)
    ax.spines["left"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.spines["top"].set_visible(False)

    plt.tight_layout()
    plt.show()


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 scripts/plot_gantt.py 1:5 2:3 0:2", file=sys.stderr)
        return 1

    try:
        segments = parse_segments(sys.argv[1:])
        plot(segments)
    except (ValueError, RuntimeError) as exc:
        print(f"Gantt plotting error: {exc}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
