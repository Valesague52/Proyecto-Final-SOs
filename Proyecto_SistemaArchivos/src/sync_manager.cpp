#include "sync_manager.h"

Semaphore::Semaphore(int initial) : count(initial) {}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return count > 0; });
    count--;
}

void Semaphore::signal() {
    std::lock_guard<std::mutex> lock(mtx);
    count++;
    cv.notify_one();
}

// CORREGIDO: std::atomic<bool>& en lugar de bool&
Philosopher::Philosopher(int id, Semaphore& left, Semaphore& right, std::mutex& outMtx, std::atomic<bool>& run)
    : id(id), leftFork(left), rightFork(right), outputMutex(outMtx), running(run) {}

void Philosopher::dine() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 3000);
    
    while (running) {
        // Pensar
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "🤔 Filósofo " << id << " está pensando...\n" << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        
        if (!running) break;
        
        // Tomar tenedores
        leftFork.wait();
        if (!running) {
            leftFork.signal();
            break;
        }
        
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "🍴 Filósofo " << id << " tomó tenedor izquierdo\n" << std::flush;
        }
        
        rightFork.wait();
        if (!running) {
            rightFork.signal();
            leftFork.signal();
            break;
        }
        
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "🍴 Filósofo " << id << " tomó tenedor derecho\n" << std::flush;
        }
        
        // Comer
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "🍝 Filósofo " << id << " está comiendo...\n" << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        
        // Soltar tenedores
        rightFork.signal();
        leftFork.signal();
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "🔄 Filósofo " << id << " soltó los tenedores\n" << std::flush;
        }
    }
}

SyncManager::SyncManager() : running(false) {
    for (int i = 0; i < 5; ++i) {
        forks.push_back(std::make_unique<Semaphore>(1));
    }
    
    for (int i = 0; i < 5; ++i) {
        philosophers.push_back(
            std::make_unique<Philosopher>(
                i, 
                *forks[i], 
                *forks[(i + 1) % 5], 
                outputMutex, 
                running
            )
        );
    }
}

SyncManager::~SyncManager() {
    stopDining();
}

void SyncManager::startDining() {
    if (running) {
        std::cout << "⚠️  La cena ya está en ejecución.\n";
        return;
    }
    
    running = true;
    threads.clear();
    
    std::cout << "🍽️  INICIANDO CENA DE FILÓSOFOS\n";
    std::cout << "💡 Presiona ENTER para detener la cena\n\n";
    
    for (auto& philosopher : philosophers) {
        threads.emplace_back(&Philosopher::dine, philosopher.get());
    }
}

void SyncManager::stopDining() {
    if (!running) {
        return;
    }
    
    running = false;
    
    // Esperar a que todos los hilos terminen
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    threads.clear();
    std::cout << "\n🛑 Cena de filósofos detenida\n";
}

void SyncManager::producerConsumerDemo() {
    std::cout << "\n--- DEMO PRODUCTOR-CONSUMIDOR ---\n";
    
    const int BUFFER_SIZE = 5;
    std::vector<int> buffer;
    std::mutex bufferMutex;
    std::condition_variable bufferNotEmpty;
    std::condition_variable bufferNotFull;
    bool demoRunning = true;
    
    auto producer = [&](int id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(500, 1500);
        
        for (int i = 0; i < 3; ++i) {
            std::unique_lock<std::mutex> lock(bufferMutex);
            bufferNotFull.wait(lock, [&]() { return buffer.size() < BUFFER_SIZE || !demoRunning; });
            
            if (!demoRunning) break;
            
            int item = id * 10 + i;
            buffer.push_back(item);
            std::cout << "📦 Productor " << id << " produjo: " << item << " | Buffer: " << buffer.size() << "/" << BUFFER_SIZE << "\n";
            
            lock.unlock();
            bufferNotEmpty.notify_one();
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        }
    };
    
    auto consumer = [&](int id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(800, 2000);
        
        for (int i = 0; i < 3; ++i) {
            std::unique_lock<std::mutex> lock(bufferMutex);
            bufferNotEmpty.wait(lock, [&]() { return !buffer.empty() || !demoRunning; });
            
            if (!demoRunning && buffer.empty()) break;
            
            int item = buffer.front();
            buffer.erase(buffer.begin());
            std::cout << "📥 Consumidor " << id << " consumió: " << item << " | Buffer: " << buffer.size() << "/" << BUFFER_SIZE << "\n";
            
            lock.unlock();
            bufferNotFull.notify_one();
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        }
    };
    
    std::vector<std::thread> demoThreads;
    demoThreads.emplace_back(producer, 1);
    demoThreads.emplace_back(producer, 2);
    demoThreads.emplace_back(consumer, 1);
    demoThreads.emplace_back(consumer, 2);
    
    for (auto& thread : demoThreads) {
        thread.join();
    }
    
    std::cout << "✅ DEMO PRODUCTOR-CONSUMIDOR COMPLETADO\n";
}

void SyncManager::readerWriterDemo() {
    std::cout << "\n--- DEMO LECTORES-ESCRITORES ---\n";
    
    std::mutex rwMutex;
    std::mutex readerCountMutex;
    int readerCount = 0;
    int sharedData = 0;
    bool demoRunning = true;
    
    auto reader = [&](int id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(300, 800);
        
        for (int i = 0; i < 2; ++i) {
            {
                std::lock_guard<std::mutex> lock(readerCountMutex);
                readerCount++;
                if (readerCount == 1) {
                    rwMutex.lock();
                }
            }
            
            std::cout << "📖 Lector " << id << " leyó: " << sharedData << " | Lectores activos: " << readerCount << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
            
            {
                std::lock_guard<std::mutex> lock(readerCountMutex);
                readerCount--;
                if (readerCount == 0) {
                    rwMutex.unlock();
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        }
    };
    
    auto writer = [&](int id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(500, 1200);
        
        for (int i = 0; i < 2; ++i) {
            rwMutex.lock();
            
            sharedData = id * 100 + i;
            std::cout << "✍️  Escritor " << id << " escribió: " << sharedData << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
            
            rwMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        }
    };
    
    std::vector<std::thread> demoThreads;
    demoThreads.emplace_back(reader, 1);
    demoThreads.emplace_back(reader, 2);
    demoThreads.emplace_back(writer, 1);
    demoThreads.emplace_back(reader, 3);
    demoThreads.emplace_back(writer, 2);
    
    for (auto& thread : demoThreads) {
        thread.join();
    }
    
    std::cout << "✅ DEMO LECTORES-ESCRITORES COMPLETADO\n";
}