#include "disk_scheduler.h"

DiskScheduler::DiskScheduler() : headPosition(0), currentAlgorithm(DiskAlgorithm::FCFS) {}

void DiskScheduler::addRequest(int track, int processId) {
    if (track < 0 || track > 199) {
        std::cout << "❌ Pista inválida. Debe estar entre 0 y 199.\n";
        return;
    }
    requests.push_back({track, processId});
    std::cout << "✅ Solicitud agregada: Pista " << track << " (Proceso " << processId << ")\n";
}

void DiskScheduler::setAlgorithm(DiskAlgorithm algorithm) {
    currentAlgorithm = algorithm;
    std::string algoName;
    switch (algorithm) {
        case DiskAlgorithm::FCFS: algoName = "FCFS"; break;
        case DiskAlgorithm::SSTF: algoName = "SSTF"; break;
        case DiskAlgorithm::SCAN: algoName = "SCAN"; break;
    }
    std::cout << "🔧 Algoritmo cambiado a: " << algoName << "\n";
}

void DiskScheduler::schedule() {
    if (requests.empty()) {
        std::cout << "❌ No hay solicitudes para planificar\n";
        return;
    }

    std::cout << "\n--- EJECUTANDO PLANIFICACIÓN DE DISCO ---\n";
    
    std::string algoName;
    switch (currentAlgorithm) {
        case DiskAlgorithm::FCFS: 
            algoName = "FCFS (First-Come, First-Served)";
            break;
        case DiskAlgorithm::SSTF: 
            algoName = "SSTF (Shortest Seek Time First)";
            break;
        case DiskAlgorithm::SCAN: 
            algoName = "SCAN (Elevator Algorithm)";
            break;
    }
    std::cout << "Algoritmo: " << algoName << "\n";

    movementHistory.clear();
    movementHistory.push_back(headPosition);
    int totalMovement = 0;
    int currentHead = headPosition;

    std::vector<DiskRequest> tempRequests = requests;

    switch (currentAlgorithm) {
        case DiskAlgorithm::FCFS: {
            for (const auto& req : tempRequests) {
                int movement = std::abs(req.track - currentHead);
                totalMovement += movement;
                std::cout << "➡️  Mover cabezal a pista " << req.track << " (Movimiento: " << movement << ")\n";
                currentHead = req.track;
                movementHistory.push_back(currentHead);
            }
            break;
        }
        
        case DiskAlgorithm::SSTF: {
            while (!tempRequests.empty()) {
                int closestIndex = -1;
                int minDistance = 9999;
                
                for (size_t i = 0; i < tempRequests.size(); ++i) {
                    int distance = std::abs(tempRequests[i].track - currentHead);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestIndex = i;
                    }
                }
                
                if (closestIndex != -1) {
                    totalMovement += minDistance;
                    std::cout << "➡️  Mover cabezal a pista " << tempRequests[closestIndex].track 
                              << " (Movimiento: " << minDistance << ")\n";
                    currentHead = tempRequests[closestIndex].track;
                    movementHistory.push_back(currentHead);
                    tempRequests.erase(tempRequests.begin() + closestIndex);
                }
            }
            break;
        }
        
        case DiskAlgorithm::SCAN: {
            // Ordenar solicitudes
            std::sort(tempRequests.begin(), tempRequests.end(), 
                     [](const DiskRequest& a, const DiskRequest& b) { 
                         return a.track < b.track; 
                     });
            
            // Encontrar posición inicial
            auto it = std::lower_bound(tempRequests.begin(), tempRequests.end(), currentHead,
                                      [](const DiskRequest& req, int head) { return req.track < head; });
            
            // Mover hacia el final
            for (auto currentIt = it; currentIt != tempRequests.end(); ++currentIt) {
                int movement = std::abs(currentIt->track - currentHead);
                totalMovement += movement;
                std::cout << "➡️  Mover cabezal a pista " << currentIt->track << " (Movimiento: " << movement << ")\n";
                currentHead = currentIt->track;
                movementHistory.push_back(currentHead);
            }
            
            // Mover hacia el inicio (si hay solicitudes atrás)
            if (it != tempRequests.begin()) {
                for (auto currentIt = std::prev(it); currentIt >= tempRequests.begin(); --currentIt) {
                    int movement = std::abs(currentIt->track - currentHead);
                    totalMovement += movement;
                    std::cout << "➡️  Mover cabezal a pista " << currentIt->track << " (Movimiento: " << movement << ")\n";
                    currentHead = currentIt->track;
                    movementHistory.push_back(currentHead);
                }
            }
            break;
        }
    }

    std::cout << "📊 Movimiento total: " << totalMovement << " pistas\n";
    
    std::cout << "🔄 Historial de movimiento: ";
    for (size_t i = 0; i < movementHistory.size(); ++i) {
        std::cout << movementHistory[i];
        if (i < movementHistory.size() - 1) std::cout << " → ";
    }
    std::cout << "\n";
}

void DiskScheduler::showDiskVisualization() const {
    std::cout << "\n--- VISUALIZACIÓN DE DISCO ---\n";
    std::cout << "Pistas: 0 - 199 | Cabezal en: " << headPosition << "\n";
    
    // Mostrar representación visual simplificada
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 20; ++col) {
            int track = row * 20 + col;
            
            if (track == headPosition) {
                std::cout << "[H] ";
            } else {
                bool hasRequest = false;
                for (const auto& req : requests) {
                    if (req.track == track) {
                        hasRequest = true;
                        break;
                    }
                }
                std::cout << "[" << (hasRequest ? "X" : " ") << "] ";
            }
        }
        std::cout << "\n";
    }
    
    if (!movementHistory.empty()) {
        std::cout << "Historial de movimiento: ";
        for (size_t i = 0; i < movementHistory.size(); ++i) {
            std::cout << movementHistory[i];
            if (i < movementHistory.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n";
        std::cout << "Movimiento total: " << movementHistory.size() - 1 << " pistas\n";
    }
}

void DiskScheduler::setHeadPosition(int position) {
    if (position < 0 || position > 199) {
        std::cout << "❌ Posición inválida. Debe estar entre 0 y 199.\n";
        return;
    }
    headPosition = position;
    std::cout << "📍 Cabezal movido a pista " << position << "\n";
}

void DiskScheduler::clearRequests() {
    requests.clear();
    movementHistory.clear();
    std::cout << "🗑️  Todas las solicitudes de disco eliminadas\n";
}

void DiskScheduler::compareAlgorithms() {
    if (requests.empty()) {
        std::cout << "❌ No hay solicitudes para comparar\n";
        return;
    }

    std::cout << "\n--- COMPARACIÓN DE ALGORITMOS ---\n";
    
    std::vector<int> tracks;
    for (const auto& req : requests) {
        tracks.push_back(req.track);
    }
    
    int initialHead = headPosition;
    
    // FCFS
    int fcfsMovement = 0;
    int current = initialHead;
    for (int track : tracks) {
        fcfsMovement += std::abs(track - current);
        current = track;
    }
    
    // SSTF
    int sstfMovement = 0;
    current = initialHead;
    std::vector<int> tempTracks = tracks;
    while (!tempTracks.empty()) {
        int closestTrack = -1;
        int minDist = 9999;
        for (int track : tempTracks) {
            int dist = std::abs(track - current);
            if (dist < minDist) {
                minDist = dist;
                closestTrack = track;
            }
        }
        sstfMovement += minDist;
        current = closestTrack;
        tempTracks.erase(std::remove(tempTracks.begin(), tempTracks.end(), closestTrack), tempTracks.end());
    }
    
    // SCAN
    int scanMovement = 0;
    current = initialHead;
    std::vector<int> sortedTracks = tracks;
    std::sort(sortedTracks.begin(), sortedTracks.end());
    
    auto it = std::lower_bound(sortedTracks.begin(), sortedTracks.end(), current);
    
    // Hacia el final
    for (auto scanIt = it; scanIt != sortedTracks.end(); ++scanIt) {
        scanMovement += std::abs(*scanIt - current);
        current = *scanIt;
    }
    
    // Hacia el inicio
    if (it != sortedTracks.begin()) {
        for (auto scanIt = std::prev(it); scanIt >= sortedTracks.begin(); --scanIt) {
            scanMovement += std::abs(*scanIt - current);
            current = *scanIt;
        }
    }
    
    std::cout << "📊 RESULTADOS:\n";
    std::cout << "FCFS:  " << fcfsMovement << " pistas\n";
    std::cout << "SSTF:  " << sstfMovement << " pistas\n";
    std::cout << "SCAN:  " << scanMovement << " pistas\n";
    
    int best = std::min({fcfsMovement, sstfMovement, scanMovement});
    if (best == sstfMovement) {
        std::cout << "🎉 SSTF es el más eficiente para este caso\n";
    } else if (best == scanMovement) {
        std::cout << "🎉 SCAN es el más eficiente para este caso\n";
    } else {
        std::cout << "🎉 FCFS es el más eficiente para este caso\n";
    }
}