#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <memory>
#include <atomic>

class Semaphore {
private:
    int count;
    std::mutex mtx;
    std::condition_variable cv;

public:
    Semaphore(int initial = 1);
    
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    
    Semaphore(Semaphore&&) = default;
    Semaphore& operator=(Semaphore&&) = default;
    
    void wait();
    void signal();
};

class Philosopher {
private:
    int id;
    Semaphore& leftFork;
    Semaphore& rightFork;
    std::mutex& outputMutex;
    std::atomic<bool>& running;

public:
    Philosopher(int id, Semaphore& left, Semaphore& right, std::mutex& outMtx, std::atomic<bool>& run);
    void dine();
};

class SyncManager {
private:
    std::vector<std::unique_ptr<Semaphore>> forks;
    std::vector<std::unique_ptr<Philosopher>> philosophers;
    std::vector<std::thread> threads;
    std::mutex outputMutex;
    std::atomic<bool> running;

public:
    SyncManager();
    ~SyncManager();
    void startDining();
    void stopDining();
    bool isRunning() const { return running; }
    void producerConsumerDemo();
    void readerWriterDemo();
};

#endif