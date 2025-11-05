#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

enum class DeviceType { PRINTER, DISK_DRIVE, NETWORK, KEYBOARD };
enum class InterruptType { TIMER, IO_COMPLETE, PAGE_FAULT, SYSTEM_CALL };

struct IORequest {
    int processId;
    DeviceType device;
    std::string data;
    int priority;
    int duration;
};

struct Interrupt {
    InterruptType type;
    int processId;
    int data;
};

class DeviceManager {
private:
    std::vector<std::queue<IORequest>> deviceQueues;
    std::vector<bool> deviceBusy;
    std::vector<std::unique_ptr<std::mutex>> deviceMutexes;
    std::vector<std::unique_ptr<std::condition_variable>> deviceCVs;
    std::vector<std::thread> deviceThreads;
    
    std::queue<Interrupt> interruptQueue;
    mutable std::mutex interruptMutex;  // CORREGIDO: agregar 'mutable'
    
    bool running;
    bool verboseMode;
    
    void deviceWorker(int deviceIndex);
    void generateInterruptSilent(InterruptType type, int processId, int data = 0);

public:
    DeviceManager();
    ~DeviceManager();
    
    bool requestIO(int processId, DeviceType device, const std::string& data, int priority, int duration);
    void listDeviceQueues() const;
    void generateInterrupt(InterruptType type, int processId, int data = 0);
    void processNextInterrupt();
    void showInterruptStatus() const;
    void setVerboseMode(bool verbose);
};

#endif