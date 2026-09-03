#include "scheduler.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

namespace {

void reset(std::vector<Process>& processes) {
    for (auto& process : processes) {
        process.setRemainingTime(process.getBurstTime());
        process.setStartTime(-1);
        process.setFinishTime(-1);
        process.setWaitingTime(-1);
        process.setTurnaroundTime(-1);
        process.setCompletionTime(-1);
        process.setResponseTime(-1);
    }
}

void addSegment(std::vector<std::pair<int, int>>& chart, int id, int duration) {
    if (duration <= 0) {
        return;
    }

    if (!chart.empty() && chart.back().first == id) {
        chart.back().second += duration;
    } else {
        chart.emplace_back(id, duration);
    }
}

void finishProcess(Process& process, int time) {
    process.setFinishTime(time);
    process.setCompletionTime(time);
    process.setTurnaroundTime(time - process.getArrivalTime());
    process.setWaitingTime(process.getTurnaroundTime() - process.getBurstTime());
}

} // namespace

void SchedulingAlgorithm::printGanttChart() {
    if (gantt_chart.empty()) {
        std::cout << "\nGantt Chart: empty\n";
        return;
    }

    std::cout << "\nGantt Chart:\n|";
    for (const auto& [id, duration] : gantt_chart) {
        (void)duration;
        std::cout << " " << (id == 0 ? "IDLE" : "P" + std::to_string(id)) << " |";
    }

    std::cout << "\n0";
    int time = 0;
    for (const auto& [id, duration] : gantt_chart) {
        (void)id;
        time += duration;
        std::cout << std::setw(7) << time;
    }
    std::cout << "\n";
}

void SchedulingAlgorithm::plotGanttChart() {
    if (gantt_chart.empty()) {
        return;
    }

    // Keep Python outside the C++ process. This avoids embedded-Python/NumPy
    // DLL issues on Windows/MSYS2 while retaining the graphical chart feature.
    std::ostringstream command;
    command << "python3 scripts/plot_gantt.py";
    for (const auto& [id, duration] : gantt_chart) {
        command << ' ' << id << ':' << duration;
    }

    const int result = std::system(command.str().c_str());
    if (result != 0) {
        std::cerr << "\nUnable to open graphical Gantt chart. "
                  << "Ensure Python 3 and Matplotlib are installed and "
                  << "run the scheduler from the project root.\n";
    }
}

void SchedulingAlgorithm::printMetrics(const std::vector<Process>& processes) {
    if (processes.empty()) {
        return;
    }

    double totalTurnaround = 0.0;
    double totalWaiting = 0.0;
    double totalResponse = 0.0;
    int totalBurst = 0;
    int lastCompletion = 0;

    std::cout << "\n============================================================================================\n"
              << "| Process | Arrival | Burst | Priority | Completion | Turnaround | Waiting | Response |\n"
              << "--------------------------------------------------------------------------------------------\n";

    for (const auto& process : processes) {
        const int completion = process.getCompletionTime() >= 0
                                   ? process.getCompletionTime()
                                   : process.getFinishTime();
        const int turnaround = completion - process.getArrivalTime();
        const int waiting = turnaround - process.getBurstTime();
        const int response = process.getResponseTime() >= 0
                                 ? process.getResponseTime()
                                 : process.getStartTime() - process.getArrivalTime();

        totalTurnaround += turnaround;
        totalWaiting += waiting;
        totalResponse += response;
        totalBurst += process.getBurstTime();
        lastCompletion = std::max(lastCompletion, completion);

        std::cout << "| " << std::setw(7) << ("P" + std::to_string(process.getId()))
                  << " | " << std::setw(7) << process.getArrivalTime()
                  << " | " << std::setw(5) << process.getBurstTime()
                  << " | " << std::setw(8) << process.getPriority()
                  << " | " << std::setw(10) << completion
                  << " | " << std::setw(10) << turnaround
                  << " | " << std::setw(7) << waiting
                  << " | " << std::setw(8) << response << " |\n";
    }

    const double count = static_cast<double>(processes.size());
    std::cout << "============================================================================================\n"
              << std::fixed << std::setprecision(2)
              << "Average Turnaround Time: " << totalTurnaround / count << "\n"
              << "Average Waiting Time:    " << totalWaiting / count << "\n"
              << "Average Response Time:   " << totalResponse / count << "\n";

    if (lastCompletion > 0) {
        std::cout << "CPU Utilization:         "
                  << 100.0 * static_cast<double>(totalBurst) / lastCompletion << "%\n";
    }
}

void FCFS::schedule(std::vector<Process>& processes) {
    reset(processes);
    gantt_chart.clear();

    std::stable_sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) {
            return a.getArrivalTime() < b.getArrivalTime();
        }
        return a.getId() < b.getId();
    });

    int time = 0;
    for (auto& process : processes) {
        if (time < process.getArrivalTime()) {
            addSegment(gantt_chart, 0, process.getArrivalTime() - time);
            time = process.getArrivalTime();
        }

        process.setStartTime(time);
        process.setResponseTime(time - process.getArrivalTime());
        time += process.getBurstTime();
        finishProcess(process, time);
        addSegment(gantt_chart, process.getId(), process.getBurstTime());
    }
}

void SJF::schedule(std::vector<Process>& processes) {
    reset(processes);
    gantt_chart.clear();

    int time = 0;
    size_t completed = 0;

    while (completed < processes.size()) {
        Process* best = nullptr;

        for (auto& process : processes) {
            if (process.getFinishTime() >= 0 || process.getArrivalTime() > time) {
                continue;
            }

            if (!best || process.getBurstTime() < best->getBurstTime()
                || (process.getBurstTime() == best->getBurstTime()
                    && process.getArrivalTime() < best->getArrivalTime())
                || (process.getBurstTime() == best->getBurstTime()
                    && process.getArrivalTime() == best->getArrivalTime()
                    && process.getId() < best->getId())) {
                best = &process;
            }
        }

        if (!best) {
            int nextArrival = INT_MAX;
            for (const auto& process : processes) {
                if (process.getFinishTime() < 0) {
                    nextArrival = std::min(nextArrival, process.getArrivalTime());
                }
            }
            addSegment(gantt_chart, 0, nextArrival - time);
            time = nextArrival;
            continue;
        }

        best->setStartTime(time);
        best->setResponseTime(time - best->getArrivalTime());
        time += best->getBurstTime();
        finishProcess(*best, time);
        addSegment(gantt_chart, best->getId(), best->getBurstTime());
        ++completed;
    }
}

void PreemptiveSJF::schedule(std::vector<Process>& processes) {
    reset(processes);
    gantt_chart.clear();

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) {
            return a.getArrivalTime() < b.getArrivalTime();
        }
        return a.getId() < b.getId();
    });

    auto compare = [](const Process* a, const Process* b) {
        if (a->getRemainingTime() != b->getRemainingTime()) {
            return a->getRemainingTime() > b->getRemainingTime();
        }
        if (a->getArrivalTime() != b->getArrivalTime()) {
            return a->getArrivalTime() > b->getArrivalTime();
        }
        return a->getId() > b->getId();
    };

    std::priority_queue<Process*, std::vector<Process*>, decltype(compare)> ready(compare);
    int time = 0;
    int completed = 0;
    size_t next = 0;

    while (completed < static_cast<int>(processes.size())) {
        while (next < processes.size() && processes[next].getArrivalTime() <= time) {
            ready.push(&processes[next++]);
        }

        if (ready.empty()) {
            const int nextArrival = processes[next].getArrivalTime();
            addSegment(gantt_chart, 0, nextArrival - time);
            time = nextArrival;
            continue;
        }

        Process* process = ready.top();
        ready.pop();

        if (process->getStartTime() < 0) {
            process->setStartTime(time);
            process->setResponseTime(time - process->getArrivalTime());
        }

        process->setRemainingTime(process->getRemainingTime() - 1);
        addSegment(gantt_chart, process->getId(), 1);
        ++time;

        if (process->getRemainingTime() == 0) {
            finishProcess(*process, time);
            ++completed;
        } else {
            ready.push(process);
        }
    }
}

void PriorityScheduling::schedule(std::vector<Process>& processes) {
    reset(processes);
    gantt_chart.clear();

    int time = 0;
    size_t completed = 0;

    while (completed < processes.size()) {
        Process* best = nullptr;

        for (auto& process : processes) {
            if (process.getFinishTime() >= 0 || process.getArrivalTime() > time) {
                continue;
            }

            if (!best || process.getPriority() < best->getPriority()
                || (process.getPriority() == best->getPriority()
                    && process.getArrivalTime() < best->getArrivalTime())
                || (process.getPriority() == best->getPriority()
                    && process.getArrivalTime() == best->getArrivalTime()
                    && process.getId() < best->getId())) {
                best = &process;
            }
        }

        if (!best) {
            int nextArrival = INT_MAX;
            for (const auto& process : processes) {
                if (process.getFinishTime() < 0) {
                    nextArrival = std::min(nextArrival, process.getArrivalTime());
                }
            }
            addSegment(gantt_chart, 0, nextArrival - time);
            time = nextArrival;
            continue;
        }

        best->setStartTime(time);
        best->setResponseTime(time - best->getArrivalTime());
        time += best->getBurstTime();
        finishProcess(*best, time);
        addSegment(gantt_chart, best->getId(), best->getBurstTime());
        ++completed;
    }
}

void PreemptivePriorityScheduling::schedule(std::vector<Process>& processes) {
    reset(processes);
    gantt_chart.clear();

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) {
            return a.getArrivalTime() < b.getArrivalTime();
        }
        return a.getId() < b.getId();
    });

    auto compare = [](const Process* a, const Process* b) {
        if (a->getPriority() != b->getPriority()) {
            return a->getPriority() > b->getPriority();
        }
        if (a->getArrivalTime() != b->getArrivalTime()) {
            return a->getArrivalTime() > b->getArrivalTime();
        }
        return a->getId() > b->getId();
    };

    std::priority_queue<Process*, std::vector<Process*>, decltype(compare)> ready(compare);
    int time = 0;
    int completed = 0;
    size_t next = 0;

    while (completed < static_cast<int>(processes.size())) {
        while (next < processes.size() && processes[next].getArrivalTime() <= time) {
            ready.push(&processes[next++]);
        }

        if (ready.empty()) {
            const int nextArrival = processes[next].getArrivalTime();
            addSegment(gantt_chart, 0, nextArrival - time);
            time = nextArrival;
            continue;
        }

        Process* process = ready.top();
        ready.pop();

        if (process->getStartTime() < 0) {
            process->setStartTime(time);
            process->setResponseTime(time - process->getArrivalTime());
        }

        process->setRemainingTime(process->getRemainingTime() - 1);
        addSegment(gantt_chart, process->getId(), 1);
        ++time;

        if (process->getRemainingTime() == 0) {
            finishProcess(*process, time);
            ++completed;
        } else {
            ready.push(process);
        }
    }
}

void RoundRobin::schedule(std::vector<Process>& processes) {
    reset(processes);
    gantt_chart.clear();

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) {
            return a.getArrivalTime() < b.getArrivalTime();
        }
        return a.getId() < b.getId();
    });

    std::queue<Process*> ready;
    int time = 0;
    int completed = 0;
    size_t next = 0;

    while (completed < static_cast<int>(processes.size())) {
        while (next < processes.size() && processes[next].getArrivalTime() <= time) {
            ready.push(&processes[next++]);
        }

        if (ready.empty()) {
            const int nextArrival = processes[next].getArrivalTime();
            addSegment(gantt_chart, 0, nextArrival - time);
            time = nextArrival;
            continue;
        }

        Process* process = ready.front();
        ready.pop();

        if (process->getStartTime() < 0) {
            process->setStartTime(time);
            process->setResponseTime(time - process->getArrivalTime());
        }

        const int runTime = std::min(time_quantum, process->getRemainingTime());
        addSegment(gantt_chart, process->getId(), runTime);
        time += runTime;
        process->setRemainingTime(process->getRemainingTime() - runTime);

        while (next < processes.size() && processes[next].getArrivalTime() <= time) {
            ready.push(&processes[next++]);
        }

        if (process->getRemainingTime() == 0) {
            finishProcess(*process, time);
            ++completed;
        } else {
            ready.push(process);
        }
    }
}

#define DELEGATE(Type) \
    void Type::printGanttChart() { SchedulingAlgorithm::printGanttChart(); } \
    void Type::plotGanttChart() { SchedulingAlgorithm::plotGanttChart(); } \
    void Type::printMetrics(const std::vector<Process>& p) { SchedulingAlgorithm::printMetrics(p); }

DELEGATE(FCFS)
DELEGATE(SJF)
DELEGATE(PreemptiveSJF)
DELEGATE(PriorityScheduling)
DELEGATE(PreemptivePriorityScheduling)
DELEGATE(RoundRobin)

#undef DELEGATE
