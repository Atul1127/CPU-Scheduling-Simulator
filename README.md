# CPU Scheduling Simulator

A C++17 CPU scheduling simulator that implements six classic scheduling algorithms, calculates standard performance metrics, and renders graphical Gantt charts.

## Features

- Six scheduling algorithms: FCFS, SJF, SRTF, Priority, Preemptive Priority, and Round Robin
- Arrival-time-aware scheduling with CPU idle periods
- Completion, turnaround, waiting, and response time for every process
- Average scheduling metrics and CPU utilization
- Configurable Round Robin time quantum
- Terminal Gantt chart plus optional graphical Gantt chart
- Input validation for process and scheduling parameters
- Portable Makefile for GCC-based environments

## Algorithms

| Algorithm | Type | Selection rule |
|---|---|---|
| FCFS | Non-preemptive | Earliest arrival |
| SJF | Non-preemptive | Shortest burst |
| SRTF | Preemptive | Shortest remaining time |
| Priority | Non-preemptive | Highest priority (smallest number) |
| Preemptive Priority | Preemptive | Highest priority (smallest number) |
| Round Robin | Preemptive | Circular queue with time quantum |

## Metrics

For each process, the simulator reports:

- **Completion Time (CT):** time at which the process finishes
- **Turnaround Time (TAT):** `CT - Arrival Time`
- **Waiting Time (WT):** `TAT - Burst Time`
- **Response Time (RT):** `First Start Time - Arrival Time`
- **CPU Utilization:** total CPU busy time relative to the elapsed schedule time

CPU idle intervals are explicitly shown in the Gantt chart.

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
├── scripts/
│   └── plot_gantt.py
├── docs/
├── Makefile
├── .gitignore
└── README.md
```

## Requirements

### C++

- GCC with C++17 support
- GNU Make

### Python

- Python 3
- NumPy
- Matplotlib

The graphical chart uses the system Python process, so Python/NumPy are **runtime dependencies for plotting**, not C++ embedding dependencies.

## Installation — MSYS2 UCRT64 on Windows

The recommended Windows environment is **MSYS2 UCRT64**.

Install the compiler/toolchain and Python packages from the MSYS2 UCRT64 terminal:

```bash
pacman -Syu
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain git
pacman -S --needed mingw-w64-ucrt-x86_64-python mingw-w64-ucrt-x86_64-python-numpy mingw-w64-ucrt-x86_64-python-matplotlib
```

Verify the environment:

```bash
g++ --version
make --version
python3 --version
python3 -c "import numpy, matplotlib; print(numpy.__version__, matplotlib.__version__)"
```

## Build and Run

Clone the repository:

```bash
git clone https://github.com/Atul1127/CPU-Scheduling-Simulator.git
cd CPU-Scheduling-Simulator
```

Build:

```bash
make
```

Run:

```bash
./scheduler
```

Clean generated build files:

```bash
make clean
```

## Usage

1. Enter the number of processes.
2. Select one of the six scheduling algorithms.
3. Enter arrival and burst times.
4. Enter priorities when using a priority-based algorithm.
5. Enter the time quantum for Round Robin.
6. Review the terminal Gantt chart and scheduling metrics.
7. Enter `y` when prompted to open the graphical Gantt chart.

### Example input

```text
Number of processes: 4

P1: Arrival=0, Burst=8, Priority=3
P2: Arrival=1, Burst=4, Priority=2
P3: Arrival=2, Burst=2, Priority=1
P4: Arrival=3, Burst=1, Priority=4
```

For Round Robin, additionally use a positive time quantum such as `2`.

## Standalone Gantt Plotter

The plotting script can also be run independently:

```bash
python3 scripts/plot_gantt.py 0:3 2:3 3:3 1:4
```

Each argument uses the format `process_id:duration`; process ID `0` represents CPU idle time.

## Design Notes

The scheduling implementations account for processes that arrive after time zero, preserve deterministic tie-breaking, and merge adjacent execution segments belonging to the same process. Preemptive algorithms simulate execution at unit-time granularity so newly arrived processes can affect scheduling decisions.

Graphical plotting is intentionally separated from the C++ scheduling core. This avoids embedding Python and NumPy inside the scheduler process and makes the simulator more reliable across GCC/MSYS2 environments.

## Troubleshooting

### `make` cannot find `g++` or `make`

Run the project from the **MSYS2 UCRT64** terminal and install the UCRT64 toolchain.

### NumPy or Matplotlib cannot be imported

Verify both packages from the same terminal used to run the simulator:

```bash
python3 -c "import numpy, matplotlib; print('Python plotting dependencies OK')"
```

If this fails, reinstall the corresponding MSYS2 UCRT64 Python packages.

### The graphical chart does not open

Test the plotting component independently:

```bash
python3 scripts/plot_gantt.py 0:3 1:4 2:2
```

If this works, rerun `./scheduler` and answer `y` to the graphical chart prompt.

## Educational Scope

This project is designed as an operating-systems / C++ project for understanding CPU scheduling concepts, algorithm behavior, and performance metrics. It prioritizes readable implementations and deterministic results over production-scale infrastructure.

## License

No license has been specified yet.
