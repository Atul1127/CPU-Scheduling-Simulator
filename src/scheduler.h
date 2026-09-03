#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <utility>
#include <vector>
#include "process.h"

class SchedulingAlgorithm {
public:
    virtual void schedule(std::vector<Process>& processes) = 0;
    virtual void printGanttChart();
    virtual void plotGanttChart();
    virtual void printMetrics(const std::vector<Process>& processes);
    virtual ~SchedulingAlgorithm() = default;

protected:
    std::vector<std::pair<int, int>> gantt_chart;
};

class FCFS : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
    void printGanttChart() override;
    void plotGanttChart() override;
    void printMetrics(const std::vector<Process>& processes) override;
};

class SJF : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
    void printGanttChart() override;
    void plotGanttChart() override;
    void printMetrics(const std::vector<Process>& processes) override;
};

class PreemptiveSJF : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
    void printGanttChart() override;
    void plotGanttChart() override;
    void printMetrics(const std::vector<Process>& processes) override;
};

class PriorityScheduling : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
    void printGanttChart() override;
    void plotGanttChart() override;
    void printMetrics(const std::vector<Process>& processes) override;
};

class PreemptivePriorityScheduling : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
    void printGanttChart() override;
    void plotGanttChart() override;
    void printMetrics(const std::vector<Process>& processes) override;
};

class RoundRobin : public SchedulingAlgorithm {
private:
    int time_quantum;

public:
    explicit RoundRobin(int tq) : time_quantum(tq) {}
    void schedule(std::vector<Process>& processes) override;
    void printGanttChart() override;
    void plotGanttChart() override;
    void printMetrics(const std::vector<Process>& processes) override;
};

#endif // SCHEDULER_H
