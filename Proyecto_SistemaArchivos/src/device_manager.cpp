//device_manager.cpp
#include "device_manager.h"
#include <random>

DeviceManager::DeviceManager() : running(true), verboseMode(false) {
    deviceQueues.resize(4);
    deviceBusy.resize(4, false);
    
    for (int i = 0; i < 4; ++i) {
        deviceMutexes.push_back(std::make_unique<std::mutex>());
        deviceCVs.push_back(std::make_unique<std::condition_variable>());
    }
    
    for (int i = 0; i < 4; ++i) {
        deviceThreads.emplace_back(&DeviceManager::deviceWorker, this, i);
    }
    
    std::cout << "🖥️  Device Manager iniciado con 4 dispositivos (modo silencioso)\n";
}

DeviceManager::~DeviceManager() {
    running = false;
    for (auto& cv : deviceCVs) {
        cv->notify_all();
    }
    for (auto& thread : deviceThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    std::cout << "🛑 Device Manager detenido\n";
}

void DeviceManager::deviceWorker(int deviceIndex) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(500, 2000);
    
    while (running) {
        IORequest request;
        bool hasWork = false;
        
        {
            std::unique_lock<std::mutex> lock(*deviceMutexes[deviceIndex]);
            deviceCVs[deviceIndex]->wait_for(lock, std::chrono::milliseconds(100), 
                [this, deviceIndex]() { return !deviceQueues[deviceIndex].empty() || !running; });
            
            if (!running) break;
            
            if (!deviceQueues[deviceIndex].empty()) {
                request = deviceQueues[deviceIndex].front();
                deviceQueues[deviceIndex].pop();
                deviceBusy[deviceIndex] = true;
                hasWork = true;
            }
        }
        
        if (hasWork) {
            std::string deviceName;
            switch (deviceIndex) {
                case 0: deviceName = "Impresora"; break;
                case 1: deviceName = "Disco"; break;
                case 2: deviceName = "Red"; break;
                case 3: deviceName = "Teclado"; break;
            }
            
            // MODO SILENCIOSO: Solo imprimir si está en verbose mode
            if (verboseMode) {
                std::cout << "🔄 " << deviceName << " procesando solicitud del proceso " << request.processId << "...\n";
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(request.duration));
            
            if (verboseMode) {
                std::cout << "✅ " << deviceName << " completó solicitud del proceso " << request.processId << "\n";
            }
            
            // Generar interrupción de forma silenciosa
            generateInterruptSilent(InterruptType::IO_COMPLETE, request.processId, deviceIndex);
            
            deviceBusy[deviceIndex] = false;
        }
    }
}

bool DeviceManager::requestIO(int processId, DeviceType device, const std::string& data, int priority, int duration) {
    int deviceIndex = static_cast<int>(device);
    
    if (deviceIndex < 0 || deviceIndex >= 4) {
        std::cout << "❌ Dispositivo inválido\n";
        return false;
    }
    
    IORequest request{processId, device, data, priority, duration};
    
    {
        std::lock_guard<std::mutex> lock(*deviceMutexes[deviceIndex]);
        deviceQueues[deviceIndex].push(request);
    }
    
    deviceCVs[deviceIndex]->notify_one();
    
    std::string deviceName;
    switch (device) {
        case DeviceType::PRINTER: deviceName = "Impresora"; break;
        case DeviceType::DISK_DRIVE: deviceName = "Disco"; break;
        case DeviceType::NETWORK: deviceName = "Red"; break;
        case DeviceType::KEYBOARD: deviceName = "Teclado"; break;
    }
    
    std::cout << "📨 Solicitud de E/S enviada a " << deviceName << " para proceso " << processId 
              << " (se procesará en segundo plano)\n";
    return true;
}

void DeviceManager::listDeviceQueues() const {
    std::cout << "\n--- COLAS DE DISPOSITIVOS ---\n";
    std::vector<std::string> deviceNames = {"Impresora", "Disco", "Red", "Teclado"};
    
    for (int i = 0; i < 4; ++i) {
        std::lock_guard<std::mutex> lock(*deviceMutexes[i]);
        std::cout << deviceNames[i] << ": " << deviceQueues[i].size() 
                  << " solicitudes | Ocupado: " << (deviceBusy[i] ? "Sí" : "No") << "\n";
        
        std::queue<IORequest> temp = deviceQueues[i];
        int count = 0;
        while (!temp.empty() && count < 3) {
            std::cout << "   - Proceso " << temp.front().processId 
                      << " (Pri: " << temp.front().priority << ")\n";
            temp.pop();
            count++;
        }
        if (deviceQueues[i].size() > 3) {
            std::cout << "   ... y " << (deviceQueues[i].size() - 3) << " más\n";
        }
    }
}

void DeviceManager::generateInterrupt(InterruptType type, int processId, int data) {
    std::lock_guard<std::mutex> lock(interruptMutex);
    interruptQueue.push(Interrupt{type, processId, data});
    
    std::string typeName;
    switch (type) {
        case InterruptType::TIMER: typeName = "TIMER"; break;
        case InterruptType::IO_COMPLETE: typeName = "IO_COMPLETE"; break;
        case InterruptType::PAGE_FAULT: typeName = "PAGE_FAULT"; break;
        case InterruptType::SYSTEM_CALL: typeName = "SYSTEM_CALL"; break;
    }
    
    std::cout << "⚠️  INTERRUPCIÓN generada: " << typeName << " para proceso " << processId << "\n";
}

// NUEVA FUNCIÓN: Generar interrupción sin mensaje
void DeviceManager::generateInterruptSilent(InterruptType type, int processId, int data) {
    std::lock_guard<std::mutex> lock(interruptMutex);
    interruptQueue.push(Interrupt{type, processId, data});
}

void DeviceManager::processNextInterrupt() {
    std::lock_guard<std::mutex> lock(interruptMutex);
    
    if (interruptQueue.empty()) {
        std::cout << "❌ No hay interrupciones pendientes\n";
        return;
    }
    
    Interrupt interrupt = interruptQueue.front();
    interruptQueue.pop();
    
    std::string typeName;
    switch (interrupt.type) {
        case InterruptType::TIMER: 
            typeName = "TIMER";
            std::cout << "⏰ Procesando interrupción de timer - Cambio de contexto\n";
            break;
        case InterruptType::IO_COMPLETE:
            typeName = "IO_COMPLETE";
            std::cout << "✅ Procesando interrupción de E/S completada - Proceso " 
                      << interrupt.processId << " puede continuar\n";
            break;
        case InterruptType::PAGE_FAULT:
            typeName = "PAGE_FAULT";
            std::cout << "🔄 Procesando page fault - Cargando página para proceso " 
                      << interrupt.processId << "\n";
            break;
        case InterruptType::SYSTEM_CALL:
            typeName = "SYSTEM_CALL";
            std::cout << "📞 Procesando system call del proceso " << interrupt.processId << "\n";
            break;
    }
    
    std::cout << "✅ Interrupción " << typeName << " procesada\n";
}

void DeviceManager::showInterruptStatus() const {
    std::lock_guard<std::mutex> lock(interruptMutex);
    
    std::cout << "\n--- ESTADO DE INTERRUPCIONES ---\n";
    std::cout << "Interrupciones pendientes: " << interruptQueue.size() << "\n";
    
    if (interruptQueue.empty()) {
        std::cout << "(Sin interrupciones pendientes)\n";
        return;
    }
    
    std::queue<Interrupt> temp = interruptQueue;
    int count = 1;
    while (!temp.empty()) {
        std::string typeName;
        switch (temp.front().type) {
            case InterruptType::TIMER: typeName = "TIMER"; break;
            case InterruptType::IO_COMPLETE: typeName = "IO_COMPLETE"; break;
            case InterruptType::PAGE_FAULT: typeName = "PAGE_FAULT"; break;
            case InterruptType::SYSTEM_CALL: typeName = "SYSTEM_CALL"; break;
        }
        
        std::cout << count << ". " << typeName << " - Proceso: " << temp.front().processId << "\n";
        temp.pop();
        count++;
    }
}

void DeviceManager::setVerboseMode(bool verbose) {
    verboseMode = verbose;
    if (verbose) {
        std::cout << "🔊 Modo verbose activado - Se mostrarán mensajes de dispositivos\n";
    } else {
        std::cout << "🔇 Modo silencioso activado - Dispositivos trabajan en segundo plano\n";
    }
}