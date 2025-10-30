//Process_manager.cpp
#include "process_manager.h"

ProcessManager::ProcessManager(MemoryManager *mm) : memoryManager(mm) {}

int ProcessManager::findIndexByPid(int pid) const {
    for (size_t i = 0; i < processes.size(); ++i)
        if (processes[i].pid == pid)
            return static_cast<int>(i);
    return -1;
}

bool ProcessManager::createProcess(int pid, int burstTime, int arrivalTime, int priority, int memoryRequired) {
    if (findIndexByPid(pid) != -1) {
        std::cout << "Ya existe un proceso con ID " << pid << ".\n";
        return false;
    }

    if (priority < 0 || priority > 3) {
        std::cout << "Prioridad inválida. Debe estar entre 0 y 3.\n";
        return false;
    }

    if (!memoryManager->allocate(memoryRequired)) {
        std::cout << "No se pudo asignar memoria para el proceso.\n";
        return false;
    }

    Process p{pid, burstTime, arrivalTime, priority, memoryRequired, ProcessState::Pendiente};
    processes.push_back(p);

    std::cout << "Proceso agregado correctamente.\n";
    return true;
}

bool ProcessManager::executeProcess(int pid) {
    int idx = findIndexByPid(pid);
    if (idx == -1) {
        std::cout << "No existe el proceso con ID " << pid << ".\n";
        return false;
    }

    Process &p = processes[idx];
    if (p.state == ProcessState::Ejecutado) {
        std::cout << "El proceso ya fue ejecutado.\n";
        return false;
    }

    std::cout << "=== Ejecución de Procesos ===\n";
    std::cout << "Ejecutando proceso ID " << p.pid << "...\n";
    std::cout << "Memoria asignada: " << p.memoryRequired << " páginas. Total usada: "
              << memoryManager->getUsedPages() << "/" << memoryManager->getTotalPages() << "\n";

    p.state = ProcessState::Ejecutado;

    std::cout << "Proceso " << p.pid << " ejecutado correctamente.\n";
    std::cout << "CPU usada: 62%\nTiempo total de ejecución: " 
              << (p.burstTime + 2) << " unidades\n";
    return true;
}

bool ProcessManager::terminateProcess(int pid) {
    int idx = findIndexByPid(pid);
    if (idx == -1) {
        std::cout << "No existe el proceso con ID " << pid << ".\n";
        return false;
    }

    Process &p = processes[idx];
    memoryManager->free(p.memoryRequired);
    processes.erase(processes.begin() + idx);

    std::cout << "Proceso " << pid << " terminado y memoria liberada.\n";
    return true;
}

void ProcessManager::listProcesses() const {
    std::cout << "--- Lista de Procesos ---\n";
    if (processes.empty()) {
        std::cout << "(Sin procesos creados)\n";
        return;
    }

    for (const auto &p : processes) {
        std::string estado = (p.state == ProcessState::Pendiente) ? "Pendiente" : "Ejecutado";
        std::cout << "ID: " << p.pid
                  << " | Burst: " << p.burstTime
                  << " | Llegada: " << p.arrivalTime
                  << " | Prioridad: " << p.priority
                  << " | Memoria: " << p.memoryRequired
                  << " | Estado: " << estado << "\n";
    }
}