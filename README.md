# CPU Scheduling Simulator

A C++17 command-line simulator for learning and comparing classic CPU scheduling algorithms. It calculates scheduling metrics and can optionally display a graphical Gantt chart using Matplotlib.

## Algorithms

- FCFS — First Come First Served
- SJF — Shortest Job First (non-preemptive)
- SRTF — Shortest Remaining Time First (preemptive SJF)
- Priority Scheduling — non-preemptive
- Preemptive Priority Scheduling
- Round Robin — configurable time quantum

## Metrics

For every process, the simulator reports:

- Completion Time (CT)
- Turnaround Time (TAT = CT − AT)
- Waiting Time (WT = TAT − BT)
- Response Time (RT = first start − AT)
- Average TAT, WT, and RT
- CPU utilization

Idle CPU intervals are represented explicitly in the Gantt chart.

## Project Structure

```text
CPU-Scheduling-Simulator/
├── src/
│   ├── main.cpp
│   ├── process.cpp
│   ├── process.h
│   ├── scheduler.cpp
│   ├── scheduler.h
│   └── matplotlibcpp.h
├── docs/
├── Makefile
└── README.md
```

## Requirements

- C++17 compiler (GCC recommended)
- Python 3
- NumPy
- Matplotlib
- `python3-config`

Install the Python dependencies with:

```bash
python3 -m pip install numpy matplotlib
```

## Build and Run

```bash
make
./scheduler
```

Clean the build with:

```bash
make clean
```

The Makefile detects the active Python installation instead of assuming a specific Python version or operating-system framework.

## Example

For three processes:

```text
P1: Arrival=0, Burst=5
P2: Arrival=1, Burst=3
P3: Arrival=2, Burst=1
```

Select an algorithm, enter the process data, and the simulator prints the Gantt chart and calculated metrics. A graphical chart can be opened at the end when desired.

## Notes

This project is intended as a fresher-level operating-systems project and emphasizes clear implementations of scheduling algorithms rather than production-scale infrastructure.
