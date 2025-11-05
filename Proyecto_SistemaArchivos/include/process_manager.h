
//process_manager.h
#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "memory_manager.h"

enum class ProcessState { Nuevo, Listo, Ejecutando, Suspendido, Terminado };
enum class SchedulerType { RR, SJF };

struct Process {
    int pid;
    int burstTime;
    int arrivalTime;
    int priority;  // 0 a 3 (0 es mayor prioridad)
    int memoryRequired;
    ProcessState state;
    int remainingTime;
    int waitTime;
};

class ProcessManager {
private:
    std::vector<Process> processes;
    std::queue<int> readyQueue;
    MemoryManager *memoryManager;
    SchedulerType schedulerType;
    int timeQuantum;
    std::mutex mtx;
    std::condition_variable cv;
    bool schedulerRunning;
    std::thread schedulerThread;

    int findIndexByPid(int pid) const;
    void scheduleRR();
    void scheduleSJF();
    void schedulerLoop();
    void executeProcessInternal(int pid);
    bool autoExecute;

public:
    ProcessManager(MemoryManager *mm);
    ~ProcessManager();
    bool createProcess(int pid, int burstTime, int arrivalTime, int priority, int memoryRequired);
    bool executeProcess(int pid);
    bool suspendProcess(int pid);
    bool resumeProcess(int pid);
    bool terminateProcess(int pid);
    void listProcesses() const;
    void setScheduler(SchedulerType type, int quantum = 2);
    void startScheduler();
    void stopScheduler();
    void setAutoExecute(bool enable);
};

#endif