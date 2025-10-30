//process_manager.h
#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <iostream>
#include <vector>
#include "memory_manager.h"

enum class ProcessState { Pendiente, Ejecutado };

struct Process {
    int pid;
    int burstTime;
    int arrivalTime;
    int priority;  // 0 a 3 (0 es mayor prioridad)
    int memoryRequired;
    ProcessState state;
};

class ProcessManager {
private:
    std::vector<Process> processes;
    MemoryManager *memoryManager;

    int findIndexByPid(int pid) const;

public:
    ProcessManager(MemoryManager *mm);
    bool createProcess(int pid, int burstTime, int arrivalTime, int priority, int memoryRequired);
    bool executeProcess(int pid);
    bool terminateProcess(int pid);
    void listProcesses() const;
};

#endif