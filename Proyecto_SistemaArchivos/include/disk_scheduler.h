#ifndef DISK_SCHEDULER_H
#define DISK_SCHEDULER_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>

enum class DiskAlgorithm { FCFS, SSTF, SCAN };

struct DiskRequest {
    int track;
    int processId;
};

class DiskScheduler {
private:
    std::vector<DiskRequest> requests;
    int headPosition;
    DiskAlgorithm currentAlgorithm;
    std::vector<int> movementHistory;

public:
    DiskScheduler();
    void addRequest(int track, int processId);
    void setAlgorithm(DiskAlgorithm algorithm);
    void schedule();
    void showDiskVisualization() const;
    void setHeadPosition(int position);
    void clearRequests();
    void compareAlgorithms();
};

#endif