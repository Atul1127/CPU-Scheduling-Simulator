#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <utility>
#include <vector>
#include "process.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

// Base interface shared by all CPU scheduling algorithms.
class SchedulingAlgorithm {
public:
    virtual ~SchedulingAlgorithm() = default;

    virtual void schedule(std::vector<Process>& processes) = 0;
    virtual void printGanttChart();
    virtual void plotGanttChart();
    virtual void printMetrics(const std::vector<Process>& processes);

protected:
    // {process_id, duration}; process_id 0 represents CPU idle time.
    std::vector<std::pair<int, int>> gantt_chart;
};

class FCFS : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
};

class SJF : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
};

class PreemptiveSJF : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
};

class PriorityScheduling : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
};

class PreemptivePriorityScheduling : public SchedulingAlgorithm {
public:
    void schedule(std::vector<Process>& processes) override;
};

class RoundRobin : public SchedulingAlgorithm {
public:
    explicit RoundRobin(int tq) : time_quantum(tq) {}
    void schedule(std::vector<Process>& processes) override;

private:
    int time_quantum;
};

#endif // SCHEDULER_H
