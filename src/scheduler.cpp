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
void reset(std::vector<Process>& ps) {
    for (auto& p : ps) {
        p.setRemainingTime(p.getBurstTime());
        p.setStartTime(-1); p.setFinishTime(-1); p.setWaitingTime(-1);
        p.setTurnaroundTime(-1); p.setCompletionTime(-1); p.setResponseTime(-1);
    }
}
void add_segment(std::vector<std::pair<int,int>>& g, int id, int duration) {
    if (duration <= 0) return;
    if (!g.empty() && g.back().first == id) g.back().second += duration;
    else g.push_back({id, duration});
}
void finish(Process& p, int t) {
    p.setFinishTime(t); p.setCompletionTime(t);
    p.setTurnaroundTime(t - p.getArrivalTime());
    p.setWaitingTime(p.getTurnaroundTime() - p.getBurstTime());
}
}

void SchedulingAlgorithm::printGanttChart() {
    if (gantt_chart.empty()) { std::cout << "\nGantt Chart: empty\n"; return; }
    std::cout << "\nGantt Chart:\n|";
    int t = 0;
    for (const auto& [id, d] : gantt_chart) {
        std::cout << " " << (id == 0 ? "IDLE" : "P" + std::to_string(id)) << " |";
        t += d;
    }
    std::cout << "\n";
    t = 0; std::cout << t;
    for (const auto& [id, d] : gantt_chart) { t += d; std::cout << std::setw(7) << t; }
    std::cout << "\n";
}

void SchedulingAlgorithm::plotGanttChart() {
    if (gantt_chart.empty()) return;

    // Do not embed Python/NumPy inside the C++ process. On Windows/MSYS2,
    // embedded Python can fail to resolve NumPy's native DLL dependencies
    // even though `python3 -c "import numpy"` works normally. Instead,
    // pass the already-computed Gantt segments to a normal Python process.
    std::ostringstream command;
    command << "python3 scripts/plot_gantt.py";
    for (const auto& [id, duration] : gantt_chart) {
        command << ' ' << id << ':' << duration;
    }

    const int result = std::system(command.str().c_str());
    if (result != 0) {
        std::cerr << "\nUnable to open graphical Gantt chart. "
                  << "Make sure Python 3 and Matplotlib are installed and "
                  << "run the program from the project root.\n";
    }
}

void SchedulingAlgorithm::printMetrics(const std::vector<Process>& ps) {
    if (ps.empty()) return;
    double tat=0, wt=0, rt=0; int burst=0, last=0;
    std::cout << "\n============================================================================================\n";
    std::cout << "| Process | Arrival | Burst | Priority | Completion | Turnaround | Waiting | Response |\n";
    std::cout << "--------------------------------------------------------------------------------------------\n";
    for (const auto& p : ps) {
        int c=p.getCompletionTime() >= 0 ? p.getCompletionTime() : p.getFinishTime();
        int a=c-p.getArrivalTime(), w=a-p.getBurstTime();
        int r=p.getResponseTime() >= 0 ? p.getResponseTime() : p.getStartTime()-p.getArrivalTime();
        tat+=a; wt+=w; rt+=r; burst+=p.getBurstTime(); last=std::max(last,c);
        std::cout << "| " << std::setw(7) << ("P"+std::to_string(p.getId()))
                  << " | " << std::setw(7) << p.getArrivalTime() << " | " << std::setw(5) << p.getBurstTime()
                  << " | " << std::setw(8) << p.getPriority() << " | " << std::setw(10) << c
                  << " | " << std::setw(10) << a << " | " << std::setw(7) << w << " | " << std::setw(8) << r << " |\n";
    }
    std::cout << "============================================================================================\n" << std::fixed << std::setprecision(2);
    std::cout << "Average Turnaround Time: " << tat/ps.size() << "\nAverage Waiting Time:    " << wt/ps.size()
              << "\nAverage Response Time:   " << rt/ps.size() << "\n";
    if (last > 0) std::cout << "CPU Utilization:         " << 100.0*burst/last << "%\n";
}

void FCFS::schedule(std::vector<Process>& ps) {
    reset(ps); gantt_chart.clear();
    std::stable_sort(ps.begin(), ps.end(), [](const Process& a,const Process& b){
        return a.getArrivalTime()!=b.getArrivalTime()?a.getArrivalTime()<b.getArrivalTime():a.getId()<b.getId(); });
    int t=0;
    for(auto& p:ps){ if(t<p.getArrivalTime()){add_segment(gantt_chart,0,p.getArrivalTime()-t);t=p.getArrivalTime();}
        p.setStartTime(t); p.setResponseTime(t-p.getArrivalTime()); t+=p.getBurstTime(); finish(p,t); add_segment(gantt_chart,p.getId(),p.getBurstTime()); }
}

void SJF::schedule(std::vector<Process>& ps) {
    reset(ps); gantt_chart.clear(); int t=0; size_t done=0;
    while(done<ps.size()){
        Process* best=nullptr;
        for(auto& p:ps) if(p.getFinishTime()<0 && p.getArrivalTime()<=t &&
            (!best || p.getBurstTime()<best->getBurstTime() ||
            (p.getBurstTime()==best->getBurstTime() && p.getArrivalTime()<best->getArrivalTime()) ||
            (p.getBurstTime()==best->getBurstTime() && p.getArrivalTime()==best->getArrivalTime() && p.getId()<best->getId()))) best=&p;
        if(!best){int next=INT_MAX; for(auto& p:ps) if(p.getFinishTime()<0) next=std::min(next,p.getArrivalTime()); add_segment(gantt_chart,0,next-t);t=next;continue;}
        best->setStartTime(t); best->setResponseTime(t-best->getArrivalTime()); t+=best->getBurstTime(); finish(*best,t); add_segment(gantt_chart,best->getId(),best->getBurstTime()); ++done;
    }
}

void PreemptiveSJF::schedule(std::vector<Process>& ps) {
    reset(ps); gantt_chart.clear();
    std::sort(ps.begin(),ps.end(),[](const Process&a,const Process&b){return a.getArrivalTime()!=b.getArrivalTime()?a.getArrivalTime()<b.getArrivalTime():a.getId()<b.getId();});
    auto cmp=[](const Process*a,const Process*b){
        if(a->getRemainingTime()!=b->getRemainingTime()) return a->getRemainingTime()>b->getRemainingTime();
        if(a->getArrivalTime()!=b->getArrivalTime()) return a->getArrivalTime()>b->getArrivalTime();
        return a->getId()>b->getId();
    };
    std::priority_queue<Process*,std::vector<Process*>,decltype(cmp)> q(cmp);
    int t=0,done=0; size_t i=0;
    while(done<(int)ps.size()){
        while(i<ps.size()&&ps[i].getArrivalTime()<=t) q.push(&ps[i++]);
        if(q.empty()){int next=ps[i].getArrivalTime();add_segment(gantt_chart,0,next-t);t=next;continue;}
        Process*p=q.top();q.pop(); if(p->getStartTime()<0){p->setStartTime(t);p->setResponseTime(t-p->getArrivalTime());}
        p->setRemainingTime(p->getRemainingTime()-1); add_segment(gantt_chart,p->getId(),1); ++t;
        if(p->getRemainingTime()==0){finish(*p,t);++done;} else q.push(p);
    }
}

void PriorityScheduling::schedule(std::vector<Process>& ps) {
    reset(ps); gantt_chart.clear(); int t=0; size_t done=0;
    while(done<ps.size()){
        Process*best=nullptr;
        for(auto&p:ps) if(p.getFinishTime()<0&&p.getArrivalTime()<=t&&(!best||p.getPriority()<best->getPriority()||(p.getPriority()==best->getPriority()&&p.getArrivalTime()<best->getArrivalTime())||(p.getPriority()==best->getPriority()&&p.getArrivalTime()==best->getArrivalTime()&&p.getId()<best->getId()))) best=&p;
        if(!best){int next=INT_MAX;for(auto&p:ps)if(p.getFinishTime()<0)next=std::min(next,p.getArrivalTime());add_segment(gantt_chart,0,next-t);t=next;continue;}
        best->setStartTime(t);best->setResponseTime(t-best->getArrivalTime());t+=best->getBurstTime();finish(*best,t);add_segment(gantt_chart,best->getId(),best->getBurstTime());++done;
    }
}

void PreemptivePriorityScheduling::schedule(std::vector<Process>& ps) {
    reset(ps); gantt_chart.clear();
    std::sort(ps.begin(),ps.end(),[](const Process&a,const Process&b){return a.getArrivalTime()!=b.getArrivalTime()?a.getArrivalTime()<b.getArrivalTime():a.getId()<b.getId();});
    auto cmp=[](const Process*a,const Process*b){
        if(a->getPriority()!=b->getPriority()) return a->getPriority()>b->getPriority();
        if(a->getArrivalTime()!=b->getArrivalTime()) return a->getArrivalTime()>b->getArrivalTime();
        return a->getId()>b->getId();
    };
    std::priority_queue<Process*,std::vector<Process*>,decltype(cmp)>q(cmp);int t=0,done=0;size_t i=0;
    while(done<(int)ps.size()){
        while(i<ps.size()&&ps[i].getArrivalTime()<=t)q.push(&ps[i++]);
        if(q.empty()){int next=ps[i].getArrivalTime();add_segment(gantt_chart,0,next-t);t=next;continue;}
        Process*p=q.top();q.pop();
        if(p->getStartTime()<0){p->setStartTime(t);p->setResponseTime(t-p->getArrivalTime());}
        p->setRemainingTime(p->getRemainingTime()-1);add_segment(gantt_chart,p->getId(),1);++t;
        if(p->getRemainingTime()==0){finish(*p,t);++done;}else q.push(p);
    }
}

void RoundRobin::schedule(std::vector<Process>& ps) {
    reset(ps);gantt_chart.clear();
    std::sort(ps.begin(),ps.end(),[](const Process&a,const Process&b){return a.getArrivalTime()!=b.getArrivalTime()?a.getArrivalTime()<b.getArrivalTime():a.getId()<b.getId();});
    std::queue<Process*>q;int t=0,done=0;size_t i=0;
    while(done<(int)ps.size()){
        while(i<ps.size()&&ps[i].getArrivalTime()<=t)q.push(&ps[i++]);
        if(q.empty()){int next=ps[i].getArrivalTime();add_segment(gantt_chart,0,next-t);t=next;continue;}
        Process*p=q.front();q.pop();if(p->getStartTime()<0){p->setStartTime(t);p->setResponseTime(t-p->getArrivalTime());}
        int run=std::min(time_quantum,p->getRemainingTime());add_segment(gantt_chart,p->getId(),run);t+=run;p->setRemainingTime(p->getRemainingTime()-run);
        while(i<ps.size()&&ps[i].getArrivalTime()<=t)q.push(&ps[i++]);
        if(p->getRemainingTime()==0){finish(*p,t);++done;}else q.push(p);
    }
}

#define DELEGATE(T) \
void T::printGanttChart(){SchedulingAlgorithm::printGanttChart();} \
void T::plotGanttChart(){SchedulingAlgorithm::plotGanttChart();} \
void T::printMetrics(const std::vector<Process>&p){SchedulingAlgorithm::printMetrics(p);}
DELEGATE(FCFS)
DELEGATE(SJF)
DELEGATE(PreemptiveSJF)
DELEGATE(PriorityScheduling)
DELEGATE(PreemptivePriorityScheduling)
void RoundRobin::printGanttChart(){SchedulingAlgorithm::printGanttChart();}
void RoundRobin::plotGanttChart(){SchedulingAlgorithm::plotGanttChart();}
