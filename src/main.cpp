#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include "process.h"
#include "scheduler.h"

int main() {
    int num_processes;
    std::cout << "Enter the number of processes: ";
    if (!(std::cin >> num_processes) || num_processes <= 0) {
        std::cerr << "Invalid number of processes.\n";
        return 1;
    }

    std::cout << "\nChoose scheduling algorithm:\n"
              << "1. FCFS\n2. SJF\n3. Preemptive SJF (SRTF)\n"
              << "4. Priority Scheduling\n5. Preemptive Priority Scheduling\n"
              << "6. Round Robin\nChoice: ";

    int choice;
    if (!(std::cin >> choice) || choice < 1 || choice > 6) {
        std::cerr << "Invalid scheduling algorithm.\n";
        return 1;
    }

    const bool needs_priority = choice == 4 || choice == 5;
    std::vector<Process> processes;
    processes.reserve(num_processes);

    for (int i = 0; i < num_processes; ++i) {
        int arrival, burst, priority = 0;
        std::cout << "\nProcess " << i + 1 << ":\n";

        std::cout << "Arrival Time: ";
        if (!(std::cin >> arrival) || arrival < 0) {
            std::cerr << "Arrival time must be a non-negative integer.\n";
            return 1;
        }

        std::cout << "Burst Time: ";
        if (!(std::cin >> burst) || burst <= 0) {
            std::cerr << "Burst time must be greater than zero.\n";
            return 1;
        }

        if (needs_priority) {
            std::cout << "Priority (smaller number = higher priority): ";
            if (!(std::cin >> priority) || priority < 0) {
                std::cerr << "Priority must be a non-negative integer.\n";
                return 1;
            }
        }
        processes.emplace_back(i + 1, arrival, burst, priority);
    }

    std::unique_ptr<SchedulingAlgorithm> scheduler;
    switch (choice) {
        case 1: scheduler = std::make_unique<FCFS>(); break;
        case 2: scheduler = std::make_unique<SJF>(); break;
        case 3: scheduler = std::make_unique<PreemptiveSJF>(); break;
        case 4: scheduler = std::make_unique<PriorityScheduling>(); break;
        case 5: scheduler = std::make_unique<PreemptivePriorityScheduling>(); break;
        case 6: {
            int quantum;
            std::cout << "Time Quantum: ";
            if (!(std::cin >> quantum) || quantum <= 0) {
                std::cerr << "Time quantum must be greater than zero.\n";
                return 1;
            }
            scheduler = std::make_unique<RoundRobin>(quantum);
            break;
        }
        default: return 1;
    }

    scheduler->schedule(processes);
    scheduler->printGanttChart();
    scheduler->printMetrics(processes);

    std::cout << "\nOpen the graphical Gantt chart? (y/n): ";
    char plot;
    if (std::cin >> plot && (plot == 'y' || plot == 'Y')) {
        scheduler->plotGanttChart();
    }
    return 0;
}
