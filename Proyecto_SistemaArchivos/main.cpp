//main.cpp
#include <iostream>
#include <string>
#include <limits>
#include "file_system.h"
#include "disk_manager.h"
#include "process_manager.h"
#include "memory_manager.h"
#include "sync_manager.h"
#include "disk_scheduler.h"
#include "device_manager.h"

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    FileSystem fs;
    DiskManager dm;
    MemoryManager mm;
    ProcessManager pm(&mm);
    SyncManager sync;
    DiskScheduler diskSched;
    DeviceManager devManager;

    int opcion;

    do {
        std::cout << "\n===== SISTEMA OPERATIVO SIMULADO =====\n";
        std::cout << "1. Crear archivo\n";
        std::cout << "2. Listar archivos\n";
        std::cout << "3. Leer archivo del disco\n";
        std::cout << "4. Escribir en archivo existente\n";
        std::cout << "5. Crear proceso\n";
        std::cout << "6. Listar procesos\n";
        std::cout << "7. Terminar proceso\n";
        std::cout << "8. Suspender proceso \n";
        std::cout << "9. Reanudar proceso\n";
        std::cout << "10. Ver estado de memoria\n";
        std::cout << "11. Configurar planificador\n";
        std::cout << "12. Configurar política de memoria\n";
        std::cout << "13. Simular acceso a página\n";
        std::cout << "14. Sincronización - Cena de Filósofos\n";
        std::cout << "15. Sincronización - Demos\n";
        std::cout << "16. Planificación de Disco\n";
        std::cout << "17. Gestión de Dispositivos\n";
        std::cout << "18. Gestión de Interrupciones\n";
        std::cout << "19. Salir\n";
        std::cout << "Seleccione una opción: ";
        
        if (!(std::cin >> opcion)) {
            clearInputBuffer();
            std::cout << "❌ Entrada inválida. Intente nuevamente.\n";
            continue;
        }
        clearInputBuffer();
        
        std::string nombre, contenido;
        int prioridad, rafaga, pid, llegada, memreq;

        switch (opcion) {
            case 1:
                std::cout << "\n--- Crear Archivo ---\n";
                std::cout << "Nombre del archivo: ";
                std::cin >> nombre;
                clearInputBuffer();
                std::cout << "Contenido inicial: ";
                std::getline(std::cin, contenido);
                fs.createFile(nombre, contenido);
                break;

            case 2:
                std::cout << "\n";
                fs.listFiles();
                break;

            case 3:
                std::cout << "\n--- Leer Archivo ---\n";
                std::cout << "Nombre del archivo a leer: ";
                std::cin >> nombre;
                clearInputBuffer();
                dm.readFromDisk(fs, nombre);
                break;

            case 4: {
                File* f = fs.selectFileForWrite();
                if (f) {
                    std::cin.ignore(1000, '\n');
                    
                    std::cout << "Texto a agregar al archivo '" << f->name << "': ";
                    std::string contenido;
                    std::getline(std::cin, contenido);
                    dm.writeToDisk(fs, f->name, contenido);
                }
                break;
            }
            case 5:
                std::cout << "\n--- Creación de Proceso ---\n";
                std::cout << "ID del proceso: ";
                if (!(std::cin >> pid)) {
                    clearInputBuffer();
                    std::cout << "❌ ID inválido. Debe ser un número entero.\n";
                    break;
                }
                std::cout << "Burst time (tiempo total de CPU): ";
                if (!(std::cin >> rafaga)) { clearInputBuffer(); std::cout << "❌ Valor inválido.\n"; break; }
                std::cout << "Tiempo de llegada: ";
                if (!(std::cin >> llegada)) { clearInputBuffer(); std::cout << "❌ Valor inválido.\n"; break; }
                std::cout << "Prioridad (0 a 3, donde 0 es la más alta): ";
                if (!(std::cin >> prioridad)) { clearInputBuffer(); std::cout << "❌ Valor inválido.\n"; break; }
                std::cout << "Memoria requerida (en páginas): ";
                if (!(std::cin >> memreq)) { clearInputBuffer(); std::cout << "❌ Valor inválido.\n"; break; }
                clearInputBuffer();

                pm.createProcess(pid, rafaga, llegada, prioridad, memreq);
                break;

            case 6:
                std::cout << "\n";
                pm.listProcesses();
                break;

            case 7:
                std::cout << "\n--- Terminar Proceso ---\n";
                std::cout << "PID del proceso a terminar: ";
                if (!(std::cin >> pid)) {
                    clearInputBuffer();
                    std::cout << "❌ ID inválido.\n";
                    break;
                }
                clearInputBuffer();
                pm.terminateProcess(pid);
                break;

            case 8:
                std::cout << "\n--- Suspender Proceso ---\n";
                std::cout << "PID del proceso a suspender: ";
                if (!(std::cin >> pid)) {
                    clearInputBuffer();
                    std::cout << "❌ ID inválido.\n";
                    break;
                }
                clearInputBuffer();
                pm.suspendProcess(pid);
                break;

            case 9:
                std::cout << "\n--- Reanudar Proceso ---\n";
                std::cout << "PID del proceso a reanudar: ";
                if (!(std::cin >> pid)) {
                    clearInputBuffer();
                    std::cout << "❌ ID inválido.\n";
                    break;
                }
                clearInputBuffer();
                pm.resumeProcess(pid);
                break;

            case 10:
                std::cout << "\n";
                mm.showMemoryStatus();
                mm.showPageTable();
                break;

            case 11:
                {
                    int algo, quantum;
                    std::cout << "\n--- Configurar Planificador ---\n";
                    std::cout << "Seleccione algoritmo:\n";
                    std::cout << "1. Round Robin\n";
                    std::cout << "2. SJF (Shortest Job First)\n";
                    std::cout << "Opción: ";
                    if (!(std::cin >> algo)) {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                        break;
                    }
                    
                    if (algo == 1) {
                        std::cout << "Quantum para Round Robin: ";
                        if (!(std::cin >> quantum)) {
                            clearInputBuffer();
                            std::cout << "❌ Valor inválido.\n";
                            break;
                        }
                        clearInputBuffer();
                        pm.setScheduler(SchedulerType::RR, quantum);
                    } else if (algo == 2) {
                        clearInputBuffer();
                        pm.setScheduler(SchedulerType::SJF);
                    } else {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                    }
                }
                break;

            case 12:
                {
                    int politica;
                    std::cout << "\n--- Configurar Política de Memoria ---\n";
                    std::cout << "Seleccione política de reemplazo:\n";
                    std::cout << "1. FIFO\n";
                    std::cout << "2. LRU\n";
                    std::cout << "3. Working Set\n";
                    std::cout << "Opción: ";
                    if (!(std::cin >> politica)) {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                        break;
                    }
                    clearInputBuffer();
                    
                    if (politica == 1) {
                        mm.setReplacementPolicy(PageReplacement::FIFO);
                    } else if (politica == 2) {
                        mm.setReplacementPolicy(PageReplacement::LRU);
                    } else if (politica == 3) {
                        mm.setReplacementPolicy(PageReplacement::WORKING_SET);
                    } else {
                        std::cout << "❌ Opción inválida.\n";
                    }
                }
                break;

            case 13:
                {
                    int pagina;
                    std::cout << "\n--- Simular Acceso a Página ---\n";
                    std::cout << "PID del proceso: ";
                    if (!(std::cin >> pid)) {
                        clearInputBuffer();
                        std::cout << "❌ PID inválido.\n";
                        break;
                    }
                    std::cout << "Número de página virtual a acceder: ";
                    if (!(std::cin >> pagina)) {
                        clearInputBuffer();
                        std::cout << "❌ Página inválida.\n";
                        break;
                    }
                    clearInputBuffer();
                    mm.accessPage(pid, pagina);
                }
                break;

            case 14:
                {
                    int subopcion;
                    std::cout << "\n🍽️  CENA DE FILÓSOFOS\n";
                    std::cout << "=====================================\n";
                    std::cout << "Simula 5 filósofos que necesitan 2 tenedores para comer.\n";
                    std::cout << "Demuestra sincronización y prevención de deadlocks.\n";
                    std::cout << "=====================================\n";
                    
                    if (sync.isRunning()) {
                        std::cout << "⚠️  La cena está en ejecución actualmente\n";
                        std::cout << "💡 Presiona ENTER para detenerla\n\n";
                        
                        std::cin.get();
                        sync.stopDining();
                    } else {
                        std::cout << "1. Iniciar cena\n";
                        std::cout << "2. Volver al menú\n";
                        std::cout << "\n💡 Durante la cena: presiona ENTER para detenerla\n";
                        std::cout << "Opción: ";
                        if (!(std::cin >> subopcion)) {
                            clearInputBuffer();
                            std::cout << "❌ Opción inválida.\n";
                            break;
                        }
                        clearInputBuffer();
                        
                        if (subopcion == 1) {
                            sync.startDining();
                            
                            std::cout << "\n[Presiona ENTER para detener la cena]\n";
                            std::cin.get();
                            
                            sync.stopDining();
                        } else if (subopcion == 2) {
                            // Volver al menú
                        } else {
                            std::cout << "❌ Opción inválida.\n";
                        }
                    }
                }
                break;

            case 15:
                {
                    int demo;
                    std::cout << "\n🔄 DEMOS DE SINCRONIZACIÓN\n";
                    std::cout << "=====================================\n";
                    std::cout << "1. Productor-Consumidor (buffer compartido)\n";
                    std::cout << "2. Lectores-Escritores (acceso concurrente)\n";
                    std::cout << "Opción: ";
                    if (!(std::cin >> demo)) {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                        break;
                    }
                    clearInputBuffer();
                    
                    if (demo == 1) {
                        sync.producerConsumerDemo();
                    } else if (demo == 2) {
                        sync.readerWriterDemo();
                    } else {
                        std::cout << "❌ Opción inválida.\n";
                    }
                }
                break;

            case 16:
                {
                    int subopcion;
                    std::cout << "\n💿 PLANIFICACIÓN DE DISCO\n";
                    std::cout << "=====================================\n";
                    std::cout << "1. Agregar solicitud de disco\n";
                    std::cout << "2. Configurar algoritmo (FCFS/SSTF/SCAN)\n";
                    std::cout << "3. Ejecutar planificación\n";
                    std::cout << "4. Mostrar visualización\n";
                    std::cout << "5. Mover cabezal\n";
                    std::cout << "6. Limpiar solicitudes\n";
                    std::cout << "7. Comparar algoritmos 📊\n";
                    std::cout << "Opción: ";
                    if (!(std::cin >> subopcion)) {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                        break;
                    }
                    
                    if (subopcion == 1) {
                        int pista;
                        std::cout << "Número de pista (0-199): ";
                        if (!(std::cin >> pista)) {
                            clearInputBuffer();
                            std::cout << "❌ Pista inválida.\n";
                            break;
                        }
                        clearInputBuffer();
                        diskSched.addRequest(pista, 0);
                    } else if (subopcion == 2) {
                        int algo;
                        std::cout << "1. FCFS (First-Come First-Served)\n";
                        std::cout << "2. SSTF (Shortest Seek Time First)\n";
                        std::cout << "3. SCAN (Elevator Algorithm)\n";
                        std::cout << "Opción: ";
                        if (!(std::cin >> algo)) {
                            clearInputBuffer();
                            std::cout << "❌ Opción inválida.\n";
                            break;
                        }
                        clearInputBuffer();
                        if (algo == 1) diskSched.setAlgorithm(DiskAlgorithm::FCFS);
                        else if (algo == 2) diskSched.setAlgorithm(DiskAlgorithm::SSTF);
                        else if (algo == 3) diskSched.setAlgorithm(DiskAlgorithm::SCAN);
                        else std::cout << "❌ Opción inválida.\n";
                    } else if (subopcion == 3) {
                        clearInputBuffer();
                        diskSched.schedule();
                    } else if (subopcion == 4) {
                        clearInputBuffer();
                        diskSched.showDiskVisualization();
                    } else if (subopcion == 5) {
                        int pos;
                        std::cout << "Nueva posición del cabezal (0-199): ";
                        if (!(std::cin >> pos)) {
                            clearInputBuffer();
                            std::cout << "❌ Posición inválida.\n";
                            break;
                        }
                        clearInputBuffer();
                        diskSched.setHeadPosition(pos);
                    } else if (subopcion == 6) {
                        clearInputBuffer();
                        diskSched.clearRequests();
                    } else if (subopcion == 7) {
                        clearInputBuffer();
                        diskSched.compareAlgorithms();
                    } else {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                    }
                }
                break;

            case 17:
                {
                    int subopcion;
                    std::cout << "\n🖥️  GESTIÓN DE DISPOSITIVOS\n";
                    std::cout << "=====================================\n";
                    std::cout << "Dispositivos disponibles:\n";
                    std::cout << "  🖨️  Impresora (1)\n";
                    std::cout << "  💿 Disco (2)\n";
                    std::cout << "  🌐 Red (3)\n";
                    std::cout << "=====================================\n";
                    std::cout << "1. Solicitar E/S a impresora\n";
                    std::cout << "2. Solicitar E/S a disco\n";
                    std::cout << "3. Solicitar E/S a red\n";
                    std::cout << "4. Mostrar colas de dispositivos\n";
                    std::cout << "5. Alternar modo verbose (mensajes de dispositivos)\n";
                    std::cout << "Opción: ";
                    if (!(std::cin >> subopcion)) {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                        break;
                    }
                    
                    if (subopcion >= 1 && subopcion <= 3) {
                        std::string data;
                        std::cout << "PID del proceso: ";
                        if (!(std::cin >> pid)) {
                            clearInputBuffer();
                            std::cout << "❌ PID inválido.\n";
                            break;
                        }
                        clearInputBuffer();
                        std::cout << "Datos a enviar: ";
                        std::getline(std::cin, data);
                        
                        DeviceType device;
                        if (subopcion == 1) device = DeviceType::PRINTER;
                        else if (subopcion == 2) device = DeviceType::DISK_DRIVE;
                        else device = DeviceType::NETWORK;
                        
                        devManager.requestIO(pid, device, data, 1, 1500);
                    } else if (subopcion == 4) {
                        clearInputBuffer();
                        devManager.listDeviceQueues();
                    } else if (subopcion == 5) {
                        clearInputBuffer();
                        static bool verbose = false;
                        verbose = !verbose;
                        devManager.setVerboseMode(verbose);
                    } else {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                    }
                }
                break;

            case 18:
                {
                    int subopcion;
                    std::cout << "\n⚠️  GESTIÓN DE INTERRUPCIONES\n";
                    std::cout << "=====================================\n";
                    std::cout << "1. Generar interrupción de timer\n";
                    std::cout << "2. Generar interrupción de system call\n";
                    std::cout << "3. Procesar siguiente interrupción\n";
                    std::cout << "4. Mostrar estado de interrupciones\n";
                    std::cout << "Opción: ";
                    if (!(std::cin >> subopcion)) {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                        break;
                    }
                    
                    if (subopcion == 1) {
                        std::cout << "PID del proceso: ";
                        if (!(std::cin >> pid)) {
                            clearInputBuffer();
                            std::cout << "❌ PID inválido.\n";
                            break;
                        }
                        clearInputBuffer();
                        devManager.generateInterrupt(InterruptType::TIMER, pid);
                    } else if (subopcion == 2) {
                        std::cout << "PID del proceso: ";
                        if (!(std::cin >> pid)) {
                            clearInputBuffer();
                            std::cout << "❌ PID inválido.\n";
                            break;
                        }
                        clearInputBuffer();
                        devManager.generateInterrupt(InterruptType::SYSTEM_CALL, pid);
                    } else if (subopcion == 3) {
                        clearInputBuffer();
                        devManager.processNextInterrupt();
                    } else if (subopcion == 4) {
                        clearInputBuffer();
                        devManager.showInterruptStatus();
                    } else {
                        clearInputBuffer();
                        std::cout << "❌ Opción inválida.\n";
                    }
                }
                break;

            case 19:
                std::cout << "\n👋 Saliendo del simulador...\n";
                sync.stopDining();
                break;

            default:
                std::cout << "\n❌ Opción inválida. Por favor seleccione una opción válida (1-19).\n";
                break;
        }

    } while (opcion != 19);

    return 0;
}
