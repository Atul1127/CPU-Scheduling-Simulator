#include "scheduler.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

namespace {
void reset_processes(std::vector<Process>& processes) {
    for (auto& p : processes) {
        p.setRemainingTime(p.getBurstTime());
        p.setStartTime(-1);
        p.setFinishTime(-1);
        p.setWaitingTime(-1);
        p.setTurnaroundTime(-1);
        p.setCompletionTime(-1);
        p.setResponseTime(-1);
    }
}

void add_segment(std::vector<std::pair<int, int>>& chart, int id, int duration) {
    if (duration <= 0) return;
    if (!chart.empty() && chart.back().first == id) {
        chart.back().second += duration;
    } else {
        chart.push_back({id, duration});
    }
}

void finish_process(Process& p, int completion) {
    p.setFinishTime(completion);
    p.setCompletionTime(completion);
    p.setTurnaroundTime(completion - p.getArrivalTime());
    p.setWaitingTime(p.getTurnaroundTime() - p.getBurstTime());
}

struct ArrivalOrder {
    bool operator()(const Process* a, const Process* b) const {
        if (a->getArrivalTime() != b->getArrivalTime())
            return a->getArrivalTime() > b->getArrivalTime();
        return a->getId() > b->getId();
    }
};
}

void SchedulingAlgorithm::printGanttChart() {
    if (gantt_chart.empty()) {
        std::cout << "\nGantt Chart: empty\n";
        return;
    }

    std::cout << "\nGantt Chart:\n";
    std::cout << "--------------------------------------------------\n|";
    int time = 0;
    for (const auto& [id, duration] : gantt_chart) {
        std::cout << " " << (id == 0 ? "IDLE" : "P" + std::to_string(id)) << " |";
        time += duration;
    }
    std::cout << "\n--------------------------------------------------\n";

    time = 0;
    std::cout << time;
    for (const auto& segment : gantt_chart) {
        time += segment.second;
        std::cout << std::setw(7) << time;
    }
    std::cout << "\n";
}

void SchedulingAlgorithm::plotGanttChart() {
    if (gantt_chart.empty()) return;

    std::vector<double> starts;
    std::vector<double> ends;
    std::vector<double> y;
    std::vector<std::string> labels;

    double time = 0;
    int row = 0;
    for (const auto& [id, duration] : gantt_chart) {
        if (duration <= 0) continue;
        starts.push_back(time);
        ends.push_back(time + duration);
        y.push_back(row++);
        labels.push_back(id == 0 ? "IDLE" : "P" + std::to_string(id));
        time += duration;
    }

    for (size_t i = 0; i < starts.size(); ++i) {
        const double h = 0.25;
        plt::fill({starts[i], ends[i], ends[i], starts[i]},
                  {y[i] - h, y[i] - h, y[i] + h, y[i] + h});
    }
    plt::yticks(y, labels);
    plt::xlabel("Time");
    plt::ylabel("Execution segment");
    plt::title("CPU Scheduling Gantt Chart");
    plt::grid(true);
    plt::show();
}

void SchedulingAlgorithm::printMetrics(const std::vector<Process>& processes) {
    if (processes.empty()) return;

    double total_tat = 0, total_wt = 0, total_rt = 0;
    int total_burst = 0;
    int last_completion = 0;

    std::cout << "\n============================================================================================\n";
    std::cout << "| Process | Arrival | Burst | Priority | Completion | Turnaround | Waiting | Response |\n";
    std::cout << "--------------------------------------------------------------------------------------------\n";

    for (const auto& p : processes) {
        const int completion = p.getCompletionTime() >= 0 ? p.getCompletionTime() : p.getFinishTime();
        const int tat = completion - p.getArrivalTime();
        const int wt = tat - p.getBurstTime();
        const int rt = p.getResponseTime() >= 0 ? p.getResponseTime() : p.getStartTime() - p.getArrivalTime();

        total_tat += tat;
        total_wt += wt;
        total_rt += rt;
        total_burst += p.getBurstTime();
        last_completion = std::max(last_completion, completion);

        std::cout << "| " << std::setw(7) << ("P" + std::to_string(p.getId()))
                  << " | " << std::setw(7) << p.getArrivalTime()
                  << " | " << std::setw(5) << p.getBurstTime()
                  << " | " << std::setw(8) << p.getPriority()
                  << " | " << std::setw(10) << completion
                  << " | " << std::setw(10) << tat
                  << " | " << std::setw(7) << wt
                  << " | " << std::setw(8) << rt << " |\n";
    }

    std::cout << "============================================================================================\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Turnaround Time: " << total_tat / processes.size() << "\n";
    std::cout << "Average Waiting Time:    " << total_wt / processes.size() << "\n";
    std::cout << "Average Response Time:   " << total_rt / processes.size() << "\n";
    if (last_completion > 0)
        std::cout << "CPU Utilization:         " << (100.0 * total_burst / last_completion) << "%\n";
}

void FCFS::schedule(std::vector<Process>& processes) {
    reset_processes(processes);
    gantt_chart.clear();
    std::stable_sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() < b.getArrivalTime();
        return a.getId() < b.getId();
    });

    int current = 0;
    for (auto& p : processes) {
        if (current < p.getArrivalTime()) {
            add_segment(gantt_chart, 0, p.getArrivalTime() - current);
            current = p.getArrivalTime();
        }
        p.setStartTime(current);
        p.setResponseTime(current - p.getArrivalTime());
        current += p.getBurstTime();
        finish_process(p, current);
        add_segment(gantt_chart, p.getId(), p.getBurstTime());
    }
}

void SJF::schedule(std::vector<Process>& processes) {
    reset_processes(processes);
    gantt_chart.clear();
    std::vector<Process*> remaining;
    for (auto& p : processes) remaining.push_back(&p);

    int current = 0;
    size_t done = 0;
    while (done < processes.size()) {
        Process* chosen = nullptr;
        for (auto* p : remaining) {
            if (p->getFinishTime() >= 0 || p->getArrivalTime() > current) continue;
            if (!chosen || p->getBurstTime() < chosen->getBurstTime() ||
                (p->getBurstTime() == chosen->getBurstTime() && p->getArrivalTime() < chosen->getArrivalTime()) ||
                (p->getBurstTime() == chosen->getBurstTime() && p->getArrivalTime() == chosen->getArrivalTime() && p->getId() < chosen->getId()))
                chosen = p;
        }
        if (!chosen) {
            int next = INT_MAX;
            for (auto* p : remaining) if (p->getFinishTime() < 0) next = std::min(next, p->getArrivalTime());
            add_segment(gantt_chart, 0, next - current);
            current = next;
            continue;
        }
        chosen->setStartTime(current);
        chosen->setResponseTime(current - chosen->getArrivalTime());
        current += chosen->getBurstTime();
        finish_process(*chosen, current);
        add_segment(gantt_chart, chosen->getId(), chosen->getBurstTime());
        ++done;
    }
}

void PreemptiveSJF::schedule(std::vector<Process>& processes) {
    reset_processes(processes);
    gantt_chart.clear();
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() < b.getArrivalTime();
        return a.getId() < b.getId();
    });

    auto cmp = [](const Process* a, const Process* b) {
        if (a->getRemainingTime() != b->getRemainingTime()) return a->getRemainingTime() > b->getRemainingTime();
        if (a->getArrivalTime() != b->getArrivalTime()) return a->getArrivalTime() > b->getArrivalTime();
        return a->getId() > b->getId();
    };
    std::priority_queue<Process*, std::vector<Process*>, decltype(cmp)> ready(cmp);

    int current = 0, completed = 0;
    size_t idx = 0;
    while (completed < static_cast<int>(processes.size())) {
        while (idx < processes.size() && processes[idx].getArrivalTime() <= current) ready.push(&processes[idx++]);
        if (ready.empty()) {
            int next = processes[idx].getArrivalTime();
            add_segment(gantt_chart, 0, next - current);
            current = next;
            continue;
        }
        Process* p = ready.top(); ready.pop();
        if (p->getStartTime() < 0) {
            p->setStartTime(current);
            p->setResponseTime(current - p->getArrivalTime());
        }
        --p->setRemainingTime, 0;
    }
}

void PriorityScheduling::schedule(std::vector<Process>& processes) {
    reset_processes(processes);
    gantt_chart.clear();
    int current = 0;
    size_t completed = 0;
    while (completed < processes.size()) {
        Process* chosen = nullptr;
        for (auto& p : processes) {
            if (p.getFinishTime() >= 0 || p.getArrivalTime() > current) continue;
            if (!chosen || p.getPriority() < chosen->getPriority() ||
                (p.getPriority() == chosen->getPriority() && p.getArrivalTime() < chosen->getArrivalTime()) ||
                (p.getPriority() == chosen->getPriority() && p.getArrivalTime() == chosen->getArrivalTime() && p.getId() < chosen->getId())) chosen = &p;
        }
        if (!chosen) {
            int next = INT_MAX;
            for (auto& p : processes) if (p.getFinishTime() < 0) next = std::min(next, p.getArrivalTime());
            add_segment(gantt_chart, 0, next - current);
            current = next;
            continue;
        }
        chosen->setStartTime(current);
        chosen->setResponseTime(current - chosen->getArrivalTime());
        current += chosen->getBurstTime();
        finish_process(*chosen, current);
        add_segment(gantt_chart, chosen->getId(), chosen->getBurstTime());
        ++completed;
    }
}

void PreemptivePriorityScheduling::schedule(std::vector<Process>& processes) {
    reset_processes(processes);
    gantt_chart.clear();
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() < b.getArrivalTime();
        return a.getId() < b.getId();
    });

    auto cmp = [](const Process* a, const Process* b) {
        if (a->getPriority() != b->getPriority()) return a->getPriority() > b->getPriority();
        if (a->getArrivalTime() != b->getArrivalTime()) return a->getArrivalTime() > b->getArrivalTime();
        return a->getId() > b->getId();
    };
    std::priority_queue<Process*, std::vector<Process*>, decltype(cmp)> ready(cmp);

    int current = 0, completed = 0;
    size_t idx = 0;
    while (completed < static_cast<int>(processes.size())) {
        while (idx < processes.size() && processes[idx].getArrivalTime() <= current) ready.push(&processes[idx++]);
        if (ready.empty()) {
            int next = processes[idx].getArrivalTime();
            add_segment(gantt_chart, 0, next - current);
            current = next;
            continue;
        }
        Process* p = ready.top(); ready.pop();
        if (p->getStartTime() < 0) {
            p->setStartTime(current);
            p->setResponseTime(current - p->getArrivalTime());
        }
        p->setRemainingTime(p->getRemainingTime() - 1);
        add_segment(gantt_chart, p->getId(), 1);
        ++current;
        if (p->getRemainingTime() == 0) {
            finish_process(*p, current);
            ++completed;
        } else ready.push(p);
    }
}

void RoundRobin::schedule(std::vector<Process>& processes) {
    reset_processes(processes);
    gantt_chart.clear();
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.getArrivalTime() != b.getArrivalTime()) return a.getArrivalTime() < b.getArrivalTime();
        return a.getId() < b.getId();
    });

    std::queue<Process*> ready;
    int current = 0, completed = 0;
    size_t idx = 0;
    while (completed < static_cast<int>(processes.size())) {
        while (idx < processes.size() && processes[idx].getArrivalTime() <= current) ready.push(&processes[idx++]);
        if (ready.empty()) {
            int next = processes[idx].getArrivalTime();
            add_segment(gantt_chart, 0, next - current);
            current = next;
            continue;
        }

        Process* p = ready.front(); ready.pop();
        if (p->getStartTime() < 0) {
            p->setStartTime(current);
            p->setResponseTime(current - p->getArrivalTime());
        }
        const int exec = std::min(time_quantum, p->getRemainingTime());
        add_segment(gantt_chart, p->getId(), exec);
        current += exec;
        p->setRemainingTime(p->getRemainingTime() - exec);

        while (idx < processes.size() && processes[idx].getArrivalTime() <= current) ready.push(&processes[idx++]);
        if (p->getRemainingTime() == 0) {
            finish_process(*p, current);
            ++completed;
        } else ready.push(p);
    }
}

void FCFS::printGanttChart() { SchedulingAlgorithm::printGanttChart(); }
void FCFS::plotGanttChart() { SchedulingAlgorithm::plotGanttChart(); }
void FCFS::printMetrics(const std::vector<Process>& p) { SchedulingAlgorithm::printMetrics(p); }
void SJF::printGanttChart() { SchedulingAlgorithm::printGanttChart(); }
void SJF::plotGanttChart() { SchedulingAlgorithm::plotGanttChart(); }
void SJF::printMetrics(const std::vector<Process>& p) { SchedulingAlgorithm::printMetrics(p); }
void PreemptiveSJF::printGanttChart() { SchedulingAlgorithm::printGanttChart(); }
void PreemptiveSJF::plotGanttChart() { SchedulingAlgorithm::plotGanttChart(); }
void PreemptiveSJF::printMetrics(const std::vector<Process>& p) { SchedulingAlgorithm::printMetrics(p); }
void PriorityScheduling::printGanttChart() { SchedulingAlgorithm::printGanttChart(); }
void PriorityScheduling::plotGanttChart() { SchedulingAlgorithm::plotGanttChart(); }
void PriorityScheduling::printMetrics(const std::vector<Process>& p) { SchedulingAlgorithm::printMetrics(p); }
void PreemptivePriorityScheduling::printGanttChart() { SchedulingAlgorithm::printGanttChart(); }
void PreemptivePriorityScheduling::plotGanttChart() { SchedulingAlgorithm::plotGanttChart(); }
void PreemptivePriorityScheduling::printMetrics(const std::vector<Process>& p) { SchedulingAlgorithm::printMetrics(p); }
void RoundRobin::printGanttChart() { SchedulingAlgorithm::printGanttChart(); }
void RoundRobin::plotGanttChart() { SchedulingAlgorithm::plotGanttChart(); }
