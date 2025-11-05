//process_manager.cpp
#include "process_manager.h"

ProcessManager::ProcessManager(MemoryManager *mm) 
    : memoryManager(mm), schedulerType(SchedulerType::RR), timeQuantum(2), 
      schedulerRunning(false), autoExecute(false) {
    // NO iniciar el planificador automáticamente
}

ProcessManager::~ProcessManager() {
    stopScheduler();
}

int ProcessManager::findIndexByPid(int pid) const {
    for (size_t i = 0; i < processes.size(); ++i)
        if (processes[i].pid == pid)
            return static_cast<int>(i);
    return -1;
}

bool ProcessManager::createProcess(int pid, int burstTime, int arrivalTime, int priority, int memoryRequired) {
    std::lock_guard<std::mutex> lock(mtx);
    
    if (findIndexByPid(pid) != -1) {
        std::cout << "Ya existe un proceso con ID " << pid << ".\n";
        return false;
    }

    if (priority < 0 || priority > 3) {
        std::cout << "Prioridad inválida. Debe estar entre 0 y 3.\n";
        return false;
    }

    if (!memoryManager->allocate(memoryRequired, pid)) {
        std::cout << "No se pudo asignar memoria para el proceso.\n";
        return false;
    }

    Process p{pid, burstTime, arrivalTime, priority, memoryRequired, 
              ProcessState::Nuevo, burstTime, 0};
    processes.push_back(p);
    
    // Dejar el proceso en estado Nuevo, NO agregarlo a la cola de listos automáticamente
    std::cout << "Proceso " << pid << " creado exitosamente.\n";
    std::cout << "💡 Use la opción 6 para listar procesos o la opción 7 para ejecutarlo manualmente.\n";
    return true;
}

void ProcessManager::scheduleRR() {
    if (readyQueue.empty()) return;

    int pid = readyQueue.front();
    readyQueue.pop();
    
    int idx = findIndexByPid(pid);
    if (idx == -1) return;

    Process &p = processes[idx];
    
    // No ejecutar procesos suspendidos o terminados
    if (p.state == ProcessState::Suspendido || p.state == ProcessState::Terminado) {
        return;
    }
    
    p.state = ProcessState::Ejecutando;
    
    std::cout << "⏰ EJECUTANDO RR - Proceso " << pid << " (Quantum: " << timeQuantum << ")\n" << std::flush;
    
    int executionTime = std::min(timeQuantum, p.remainingTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    p.remainingTime -= executionTime;
    
    if (p.remainingTime > 0) {
        p.state = ProcessState::Listo;
        readyQueue.push(pid);
        std::cout << "↩️  Proceso " << pid << " vuelve a cola (Restante: " << p.remainingTime << ")\n" << std::flush;
    } else {
        p.state = ProcessState::Listo;
        std::cout << "✅ Proceso " << pid << " completó su ejecución (Restante: 0)\n" << std::flush;
        std::cout << "💡 Use la opción 7 para terminar formalmente el proceso.\n" << std::flush;
    }
}

void ProcessManager::scheduleSJF() {
    if (readyQueue.empty()) return;

    std::vector<int> readyPids;
    std::queue<int> tempQueue = readyQueue;
    
    while (!tempQueue.empty()) {
        readyPids.push_back(tempQueue.front());
        tempQueue.pop();
    }

    int shortestIdx = -1;
    int shortestTime = 9999;
    
    for (int pid : readyPids) {
        int idx = findIndexByPid(pid);
        if (idx != -1 && processes[idx].state == ProcessState::Listo && processes[idx].remainingTime < shortestTime) {
            shortestTime = processes[idx].remainingTime;
            shortestIdx = idx;
        }
    }

    if (shortestIdx == -1) return;

    Process &p = processes[shortestIdx];
    p.state = ProcessState::Ejecutando;
    
    std::cout << "⏰ EJECUTANDO SJF - Proceso " << p.pid << " (Tiempo: " << p.remainingTime << ")\n" << std::flush;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    
    p.remainingTime = 0;
    p.state = ProcessState::Listo;
    
    std::cout << "✅ Proceso " << p.pid << " completó su ejecución (SJF)\n" << std::flush;
    std::cout << "💡 Use la opción 7 para terminar formalmente el proceso.\n" << std::flush;
    
    std::queue<int> newQueue;
    for (int pid : readyPids) {
        if (pid != p.pid) {
            newQueue.push(pid);
        }
    }
    readyQueue = newQueue;
}

void ProcessManager::schedulerLoop() {
    while (schedulerRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        if (!autoExecute) continue; // No ejecutar si autoExecute está desactivado
        
        std::lock_guard<std::mutex> lock(mtx);
        
        if (!readyQueue.empty()) {
            switch (schedulerType) {
                case SchedulerType::RR:
                    scheduleRR();
                    break;
                case SchedulerType::SJF:
                    scheduleSJF();
                    break;
            }
        }
    }
}

bool ProcessManager::executeProcess(int pid) {
    std::lock_guard<std::mutex> lock(mtx);
    int idx = findIndexByPid(pid);
    if (idx == -1) {
        std::cout << "No existe el proceso con ID " << pid << ".\n";
        return false;
    }

    Process &p = processes[idx];
    if (p.state == ProcessState::Terminado) {
        std::cout << "El proceso ya fue terminado.\n";
        return false;
    }

    if (p.state == ProcessState::Nuevo || p.state == ProcessState::Suspendido) {
        p.state = ProcessState::Listo;
        readyQueue.push(pid);
        std::cout << "Proceso " << pid << " agregado a cola de listos.\n";
        
        // Ejecutar inmediatamente si el planificador automático está desactivado
        if (!autoExecute) {
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            mtx.lock();
            
            // Ejecutar manualmente
            if (schedulerType == SchedulerType::RR) {
                scheduleRR();
            } else {
                scheduleSJF();
            }
        }
    } else {
        std::cout << "El proceso ya está en estado: " << (p.state == ProcessState::Listo ? "Listo" : "Ejecutando") << "\n";
    }
    
    return true;
}

bool ProcessManager::suspendProcess(int pid) {
    std::lock_guard<std::mutex> lock(mtx);
    int idx = findIndexByPid(pid);
    if (idx == -1) {
        std::cout << "No existe el proceso con ID " << pid << ".\n";
        return false;
    }
    
    if (processes[idx].state == ProcessState::Terminado) {
        std::cout << "No se puede suspender un proceso terminado.\n";
        return false;
    }

    processes[idx].state = ProcessState::Suspendido;
    
    // Remover de la cola de listos si está ahí
    std::queue<int> newQueue;
    while (!readyQueue.empty()) {
        int currentPid = readyQueue.front();
        readyQueue.pop();
        if (currentPid != pid) {
            newQueue.push(currentPid);
        }
    }
    readyQueue = newQueue;
    
    std::cout << "Proceso " << pid << " suspendido.\n";
    return true;
}

bool ProcessManager::resumeProcess(int pid) {
    std::lock_guard<std::mutex> lock(mtx);
    int idx = findIndexByPid(pid);
    if (idx == -1) {
        std::cout << "No existe el proceso con ID " << pid << ".\n";
        return false;
    }
    
    if (processes[idx].state != ProcessState::Suspendido) {
        std::cout << "El proceso no está suspendido.\n";
        return false;
    }

    processes[idx].state = ProcessState::Listo;
    readyQueue.push(pid);
    std::cout << "Proceso " << pid << " reanudado y en cola de listos.\n";
    return true;
}

bool ProcessManager::terminateProcess(int pid) {
    std::lock_guard<std::mutex> lock(mtx);
    int idx = findIndexByPid(pid);
    if (idx == -1) {
        std::cout << "No existe el proceso con ID " << pid << ".\n";
        return false;
    }
    
    if (processes[idx].state == ProcessState::Terminado) {
        std::cout << "El proceso ya está terminado.\n";
        return false;
    }

    processes[idx].state = ProcessState::Terminado;
    processes[idx].remainingTime = 0;
    memoryManager->freeProcessPages(pid);
    
    std::queue<int> newQueue;
    while (!readyQueue.empty()) {
        int currentPid = readyQueue.front();
        readyQueue.pop();
        if (currentPid != pid) {
            newQueue.push(currentPid);
        }
    }
    readyQueue = newQueue;
    
    std::cout << "Proceso " << pid << " terminado.\n";
    return true;
}

void ProcessManager::listProcesses() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mtx));
    
    std::cout << "\n--- Lista de Procesos ---\n";
    if (processes.empty()) {
        std::cout << "(Sin procesos creados)\n";
        return;
    }

    for (const auto &p : processes) {
        std::string estado;
        switch (p.state) {
            case ProcessState::Nuevo: estado = "Nuevo"; break;
            case ProcessState::Listo: estado = "Listo"; break;
            case ProcessState::Ejecutando: estado = "Ejecutando"; break;
            case ProcessState::Suspendido: estado = "Suspendido"; break;
            case ProcessState::Terminado: estado = "Terminado"; break;
        }
        
        std::cout << "ID: " << p.pid
                  << " | Burst: " << p.burstTime
                  << " | Restante: " << p.remainingTime
                  << " | Llegada: " << p.arrivalTime
                  << " | Prioridad: " << p.priority
                  << " | Memoria: " << p.memoryRequired
                  << " | Estado: " << estado << "\n";
    }
    
    std::cout << "Cola de listos: ";
    std::queue<int> temp = readyQueue;
    if (temp.empty()) {
        std::cout << "(vacía)";
    }
    while (!temp.empty()) {
        std::cout << temp.front() << " ";
        temp.pop();
    }
    std::cout << "\n";
}

void ProcessManager::setScheduler(SchedulerType type, int quantum) {
    std::lock_guard<std::mutex> lock(mtx);
    schedulerType = type;
    timeQuantum = quantum;
    std::cout << "Planificador cambiado a " << (type == SchedulerType::RR ? "Round Robin" : "SJF") << "\n";
}

void ProcessManager::startScheduler() {
    if (schedulerRunning) {
        std::cout << "⚠️  El planificador ya está en ejecución.\n";
        return;
    }
    
    schedulerRunning = true;
    schedulerThread = std::thread(&ProcessManager::schedulerLoop, this);
    std::cout << "🔄 Planificador iniciado (modo manual)\n";
    std::cout << "💡 Los procesos NO se ejecutan automáticamente. Use las opciones del menú para controlarlos.\n";
}

void ProcessManager::stopScheduler() {
    if (!schedulerRunning) return;
    
    schedulerRunning = false;
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }
    std::cout << "🛑 Planificador detenido\n";
}

void ProcessManager::setAutoExecute(bool enable) {
    std::lock_guard<std::mutex> lock(mtx);
    autoExecute = enable;
    std::cout << "Ejecución automática de procesos: " << (enable ? "ACTIVADA" : "DESACTIVADA") << "\n";
    if (enable) {
        std::cout << "⚠️  Los procesos en cola de listos se ejecutarán automáticamente.\n";
    } else {
        std::cout << "💡 Use las opciones del menú para ejecutar procesos manualmente.\n";
    }
}