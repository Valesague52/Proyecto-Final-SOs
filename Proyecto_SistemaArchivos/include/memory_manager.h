//memory_manager.h
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>

enum class PageReplacement { FIFO, LRU, WORKING_SET };

struct Page {
    int pageId;
    int processId;
    bool referenced;
    bool modified;
    int lastUsed;
    std::vector<int> accessHistory;  // Para Working Set
};

class MemoryManager {
private:
    int totalPages;
    int usedPages;
    std::vector<Page> pageTable;
    std::queue<int> fifoQueue;
    std::map<int, int> pageUsage;
    PageReplacement replacementPolicy;
    int accessCounter;
    int pageFaults;           // AÑADIDO
    int pageHits;             // AÑADIDO
    int workingSetWindow;     // AÑADIDO
    
    int findVictimPage();
    int findWorkingSetVictim();      // AÑADIDO
    void updatePageUsage(int pageIndex);
    void updateWorkingSet(int pageIndex);  // AÑADIDO

public:
    MemoryManager(int total = 16);
    bool allocate(int pages, int processId);
    void free(int pages, int processId);
    void freeProcessPages(int processId);
    int getUsedPages() const;
    int getTotalPages() const;
    void showMemoryStatus() const;
    void setReplacementPolicy(PageReplacement policy);
    void accessPage(int processId, int virtualPage);
    void showPageTable() const;
    void showStatistics() const;        // AÑADIDO
    void setWorkingSetWindow(int window);  // AÑADIDO
};

#endif