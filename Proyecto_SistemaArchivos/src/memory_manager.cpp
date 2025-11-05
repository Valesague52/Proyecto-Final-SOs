//memory_manager.cpp
#include "memory_manager.h"
#include <iostream>
#include <algorithm>

MemoryManager::MemoryManager(int total) 
    : totalPages(total), usedPages(0), replacementPolicy(PageReplacement::FIFO), 
      accessCounter(0), pageFaults(0), pageHits(0), workingSetWindow(5) {
    pageTable.resize(total);
    for (int i = 0; i < total; ++i) {
        pageTable[i] = Page{-1, -1, false, false, 0, std::vector<int>()};
    }
}

bool MemoryManager::allocate(int pages, int processId) {
    if (usedPages + pages > totalPages) {
        std::cout << "Memoria insuficiente. Necesario liberar " << (usedPages + pages - totalPages) << " páginas.\n";
        return false;
    }

    int allocated = 0;
    for (int i = 0; i < totalPages && allocated < pages; ++i) {
        if (pageTable[i].processId == -1) {
            pageTable[i].processId = processId;
            pageTable[i].pageId = allocated;
            pageTable[i].referenced = true;
            pageTable[i].modified = false;
            pageTable[i].lastUsed = accessCounter++;
            pageTable[i].accessHistory.push_back(accessCounter);
            
            fifoQueue.push(i);
            pageUsage[i] = accessCounter;
            allocated++;
            usedPages++;
        }
    }

    std::cout << "✅ Asignadas " << allocated << " páginas al proceso " << processId << "\n";
    return allocated == pages;
}

void MemoryManager::free(int pages, int processId) {
    int freed = 0;
    for (int i = 0; i < totalPages && freed < pages; ++i) {
        if (pageTable[i].processId == processId) {
            pageTable[i].processId = -1;
            pageTable[i].pageId = -1;
            pageTable[i].referenced = false;
            freed++;
            usedPages--;
        }
    }
    std::cout << "🔄 Liberadas " << freed << " páginas del proceso " << processId << "\n";
}

void MemoryManager::freeProcessPages(int processId) {
    int freed = 0;
    for (int i = 0; i < totalPages; ++i) {
        if (pageTable[i].processId == processId) {
            pageTable[i].processId = -1;
            pageTable[i].pageId = -1;
            pageTable[i].referenced = false;
            freed++;
            usedPages--;
        }
    }
    std::cout << "🗑️  Liberadas todas (" << freed << ") páginas del proceso " << processId << "\n";
}

int MemoryManager::findVictimPage() {
    switch (replacementPolicy) {
        case PageReplacement::FIFO:
            while (!fifoQueue.empty()) {
                int pageIndex = fifoQueue.front();
                fifoQueue.pop();
                if (pageTable[pageIndex].processId != -1) {
                    return pageIndex;
                }
            }
            break;
            
        case PageReplacement::LRU:
            {
                int lruPage = -1;
                int minTime = accessCounter + 1;
                
                for (int i = 0; i < totalPages; ++i) {
                    if (pageTable[i].processId != -1 && pageUsage[i] < minTime) {
                        minTime = pageUsage[i];
                        lruPage = i;
                    }
                }
                return lruPage;
            }
            break;
            
        case PageReplacement::WORKING_SET:
            return findWorkingSetVictim();
            break;
    }
    return -1;
}

int MemoryManager::findWorkingSetVictim() {
    int victim = -1;
    int oldestAccess = accessCounter + 1;
    
    for (int i = 0; i < totalPages; ++i) {
        if (pageTable[i].processId != -1) {
            // Calcular working set: páginas accedidas en la ventana de tiempo
            int recentAccesses = 0;
            for (int access : pageTable[i].accessHistory) {
                if (access > (accessCounter - workingSetWindow)) {
                    recentAccesses++;
                }
            }
            
            // Si no ha sido accedida recientemente, es candidata
            if (recentAccesses == 0 && pageTable[i].lastUsed < oldestAccess) {
                oldestAccess = pageTable[i].lastUsed;
                victim = i;
            }
        }
    }
    
    // Si no encontramos página fuera del working set, usar LRU
    if (victim == -1) {
        for (int i = 0; i < totalPages; ++i) {
            if (pageTable[i].processId != -1 && pageTable[i].lastUsed < oldestAccess) {
                oldestAccess = pageTable[i].lastUsed;
                victim = i;
            }
        }
    }
    
    return victim;
}

void MemoryManager::updatePageUsage(int pageIndex) {
    pageTable[pageIndex].referenced = true;
    pageTable[pageIndex].lastUsed = accessCounter;
    pageUsage[pageIndex] = accessCounter++;
}

void MemoryManager::updateWorkingSet(int pageIndex) {
    pageTable[pageIndex].referenced = true;
    pageTable[pageIndex].lastUsed = accessCounter;
    pageTable[pageIndex].accessHistory.push_back(accessCounter);
    
    // Mantener solo los últimos accesos en el historial
    if (pageTable[pageIndex].accessHistory.size() > 10) {
        pageTable[pageIndex].accessHistory.erase(
            pageTable[pageIndex].accessHistory.begin()
        );
    }
    
    accessCounter++;
}

void MemoryManager::accessPage(int processId, int virtualPage) {
    // Buscar página en memoria
    for (int i = 0; i < totalPages; ++i) {
        if (pageTable[i].processId == processId && pageTable[i].pageId == virtualPage) {
            if (replacementPolicy == PageReplacement::WORKING_SET) {
                updateWorkingSet(i);
            } else {
                updatePageUsage(i);
            }
            pageHits++;
            std::cout << "✅ HIT - Página " << virtualPage << " del proceso " << processId 
                      << " en marco " << i << " | Hits: " << pageHits << " | Faults: " << pageFaults << "\n";
            return;
        }
    }
    
    // Page fault - necesitamos cargar la página
    pageFaults++;
    std::cout << "❌ PAGE FAULT - Página " << virtualPage << " del proceso " << processId 
              << " | Hits: " << pageHits << " | Faults: " << pageFaults << "\n";
    
    // Encontrar marco libre o víctima
    int freeFrame = -1;
    for (int i = 0; i < totalPages; ++i) {
        if (pageTable[i].processId == -1) {
            freeFrame = i;
            break;
        }
    }
    
    if (freeFrame == -1) {
        freeFrame = findVictimPage();
        std::cout << "🔁 Reemplazando página en marco " << freeFrame 
                  << " (Proceso " << pageTable[freeFrame].processId 
                  << ", Página " << pageTable[freeFrame].pageId << ")\n";
    }
    
    // Cargar nueva página
    pageTable[freeFrame].processId = processId;
    pageTable[freeFrame].pageId = virtualPage;
    pageTable[freeFrame].referenced = true;
    pageTable[freeFrame].modified = false;
    
    if (replacementPolicy == PageReplacement::WORKING_SET) {
        updateWorkingSet(freeFrame);
    } else {
        updatePageUsage(freeFrame);
    }
    
    if (replacementPolicy == PageReplacement::FIFO) {
        fifoQueue.push(freeFrame);
    }
}

int MemoryManager::getUsedPages() const {
    return usedPages;
}

int MemoryManager::getTotalPages() const {
    return totalPages;
}

void MemoryManager::showMemoryStatus() const {
    std::cout << "\n--- ESTADO DE MEMORIA ---\n";
    std::cout << "Memoria usada: " << usedPages << "/" << totalPages << " páginas\n";
    
    std::string policyName;
    switch (replacementPolicy) {
        case PageReplacement::FIFO: policyName = "FIFO"; break;
        case PageReplacement::LRU: policyName = "LRU"; break;
        case PageReplacement::WORKING_SET: policyName = "WORKING SET"; break;
    }
    std::cout << "Política de reemplazo: " << policyName << "\n";
    
    showStatistics();
}

void MemoryManager::setReplacementPolicy(PageReplacement policy) {
    replacementPolicy = policy;
    std::string policyName;
    switch (policy) {
        case PageReplacement::FIFO: policyName = "FIFO"; break;
        case PageReplacement::LRU: policyName = "LRU"; break;
        case PageReplacement::WORKING_SET: policyName = "WORKING SET"; break;
    }
    std::cout << "Política de reemplazo cambiada a " << policyName << "\n";
}

void MemoryManager::showPageTable() const {
    std::cout << "\n--- TABLA DE PÁGINAS (Visualización con Colores) ---\n";
    
    // Estadísticas por proceso
    std::map<int, int> processPages;
    for (const auto& page : pageTable) {
        if (page.processId != -1) {
            processPages[page.processId]++;
        }
    }
    
    std::cout << "Distribución por proceso: ";
    for (const auto& [pid, count] : processPages) {
        std::cout << "P" << pid << ":" << count << " ";
    }
    std::cout << "\n\n";
    
    // Mostrar tabla con colores
    for (int i = 0; i < totalPages; i += 8) {
        for (int j = 0; j < 8 && i + j < totalPages; ++j) {
            int idx = i + j;
            if (pageTable[idx].processId == -1) {
                std::cout << "[\033[90m Libre \033[0m] ";  // Gris
            } else {
                // Color diferente por proceso
                int colorCode = 31 + (pageTable[idx].processId % 6); // 31-36: rojo, verde, amarillo, azul, magenta, cian
                std::cout << "[\033[" << colorCode << "mP" << pageTable[idx].processId 
                          << "-" << pageTable[idx].pageId << "\033[0m] ";
            }
        }
        std::cout << "\n";
    }
    
    // Leyenda de colores
    std::cout << "\nLeyenda: ";
    for (const auto& [pid, count] : processPages) {
        int colorCode = 31 + (pid % 6);
        std::cout << "[\033[" << colorCode << "mP" << pid << "\033[0m] ";
    }
    std::cout << "[\033[90mLibre\033[0m]\n";
}

void MemoryManager::showStatistics() const {
    std::cout << "\n--- ESTADÍSTICAS DE MEMORIA ---\n";
    int totalAccesses = pageHits + pageFaults;
    double hitRate = totalAccesses > 0 ? (double)pageHits / totalAccesses * 100.0 : 0.0;
    double faultRate = totalAccesses > 0 ? (double)pageFaults / totalAccesses * 100.0 : 0.0;
    
    std::cout << "Total de accesos: " << totalAccesses << "\n";
    std::cout << "Page Hits: " << pageHits << " (" << hitRate << "%)\n";
    std::cout << "Page Faults: " << pageFaults << " (" << faultRate << "%)\n";
    
    // Gráfico simple de hits vs faults
    if (totalAccesses > 0) {
        std::cout << "📊 Distribución: ";
        int totalBars = 20;
        int hitBars = (pageHits * totalBars) / totalAccesses;
        int faultBars = (pageFaults * totalBars) / totalAccesses;
        
        std::cout << "\033[32m"; // Verde para hits
        for (int i = 0; i < hitBars; ++i) std::cout << "█";
        std::cout << "\033[31m"; // Rojo para faults
        for (int i = 0; i < faultBars; ++i) std::cout << "█";
        std::cout << "\033[0m\n";
        
        std::cout << "            \033[32mHits\033[0m  \033[31mFaults\033[0m\n";
    }
}

void MemoryManager::setWorkingSetWindow(int window) {
    workingSetWindow = window;
    std::cout << "Ventana del Working Set configurada a " << window << " accesos\n";
}