# CPU Scheduling Simulator

A C++17 CPU scheduling simulator implementing six classic operating-system scheduling algorithms, standard performance metrics, and graphical Gantt charts.

> **Focus:** correct arrival-aware scheduling, preemption, deterministic tie-breaking, measurable performance, and a clean educational implementation.

## Features

- 6 CPU scheduling algorithms
- Arrival-time-aware scheduling and explicit CPU idle periods
- Preemptive scheduling with remaining-time tracking
- Completion, turnaround, waiting, and response time
- Average TAT, WT, RT, and CPU utilization
- Terminal Gantt chart
- Graphical Gantt chart with Python + Matplotlib
- Configurable Round Robin time quantum
- Input validation
- GCC/Make-based build system
- GitHub Actions build + smoke test

## Algorithms

| Algorithm | Type | Scheduling rule |
|---|---|---|
| **FCFS** | Non-preemptive | Earliest arrival |
| **SJF** | Non-preemptive | Shortest burst time |
| **SRTF** | Preemptive | Shortest remaining time |
| **Priority** | Non-preemptive | Lowest priority number first |
| **Preemptive Priority** | Preemptive | Lowest priority number first |
| **Round Robin** | Preemptive | FIFO queue + time quantum |

## Metrics

For every process:

- **Completion Time (CT)** = time when the process finishes
- **Turnaround Time (TAT)** = `CT - Arrival Time`
- **Waiting Time (WT)** = `TAT - Burst Time`
- **Response Time (RT)** = `First Start Time - Arrival Time`
- **CPU Utilization** = busy CPU time / elapsed scheduling time × 100

The simulator also reports average TAT, WT, and RT.

## Project Structure

```text
CPU-Scheduling-Simulator/
├── src/
│   ├── main.cpp
│   ├── process.cpp
│   ├── process.h
│   ├── scheduler.cpp
│   └── scheduler.h
├── scripts/
│   └── plot_gantt.py
├── tests/
│   └── smoke_test.sh
├── .github/
│   └── workflows/
│       └── ci.yml
├── docs/
├── .gitignore
├── Makefile
└── README.md
```

## Requirements

### Core

- GCC with C++17 support
- GNU Make

### Graphical plotting

- Python 3
- NumPy
- Matplotlib

Python is used as a separate plotting process; the C++ scheduler does not embed Python/NumPy at runtime.

## Windows Setup — MSYS2 UCRT64

The recommended Windows environment is **MSYS2 UCRT64**.

From the MSYS2 UCRT64 terminal:

```bash
pacman -Syu
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain git
pacman -S --needed mingw-w64-ucrt-x86_64-python mingw-w64-ucrt-x86_64-python-numpy mingw-w64-ucrt-x86_64-python-matplotlib
```

Verify:

```bash
g++ --version
make --version
python3 --version
python3 -c "import numpy, matplotlib; print(numpy.__version__, matplotlib.__version__)"
```

## Build & Run

```bash
git clone https://github.com/Atul1127/CPU-Scheduling-Simulator.git
cd CPU-Scheduling-Simulator
make
./scheduler
```

To remove generated build files:

```bash
make clean
```

## Usage

1. Enter the number of processes.
2. Select one of the six algorithms.
3. Enter arrival and burst times.
4. Enter priorities for Priority algorithms.
5. Enter a positive time quantum for Round Robin.
6. Review the terminal Gantt chart and metrics.
7. Enter `y` to open the graphical Gantt chart.

### Example workload

```text
P1: Arrival=0, Burst=8, Priority=3
P2: Arrival=1, Burst=4, Priority=2
P3: Arrival=2, Burst=2, Priority=1
P4: Arrival=3, Burst=1, Priority=4
```

For Round Robin, try a time quantum of `2`.

## Standalone Gantt Plotter

The graphical component can also be tested independently:

```bash
python3 scripts/plot_gantt.py 0:3 2:3 3:3 1:4
```

Arguments use `process_id:duration`. Process ID `0` represents CPU idle time.

## Testing

Build and run the automated smoke test:

```bash
make
bash tests/smoke_test.sh
```

GitHub Actions runs the build and smoke test automatically on pushes and pull requests.

## Edge Cases

The simulator handles:

- Processes arriving after time `0`
- CPU idle intervals
- Multiple processes arriving at the same time
- Equal burst times and deterministic tie-breaking
- Equal priorities and deterministic tie-breaking
- Preemption when a better process becomes available
- Round Robin time slicing
- Invalid process counts, arrival times, burst times, priorities, algorithm choices, and quantum values

## Design Notes

Non-preemptive algorithms run the selected process to completion. Preemptive algorithms simulate execution at unit-time granularity so newly arrived processes can change the scheduling decision.

Gantt segments are merged when the same process continues executing, keeping the timeline readable. The plotting layer is separated from the C++ scheduling core to avoid embedded-Python/NumPy DLL issues across GCC/MSYS2 environments.

## Troubleshooting

**`g++` or `make` not found**

Run the project from the MSYS2 UCRT64 terminal and install the UCRT64 toolchain.

**NumPy/Matplotlib import error**

Test the Python environment directly:

```bash
python3 -c "import numpy, matplotlib; print('Python plotting dependencies OK')"
```

If required, reinstall the MSYS2 UCRT64 packages listed above.

**Graphical chart does not open**

Test the plotter independently:

```bash
python3 scripts/plot_gantt.py 0:3 1:4 2:2
```

Then rebuild and rerun:

```bash
make clean
make
./scheduler
```

## Educational Scope

This project is intended as an operating-systems and C++ portfolio project. It emphasizes understandable implementations, correct scheduling behavior, and transparent performance metrics rather than production-scale infrastructure.

## License

No open-source license has been specified yet.
