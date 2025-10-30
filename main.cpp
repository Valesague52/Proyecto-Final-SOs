//main.cpp
#include <iostream>
#include <string>
#include "file_system.h"
#include "disk_manager.h"
#include "process_manager.h"
#include "memory_manager.h"

int main() {
    FileSystem fs;
    DiskManager dm;
    MemoryManager mm;
    ProcessManager pm(&mm); // le pasamos el memory manager para que administre páginas

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
        std::cout << "8. Ver estado de memoria\n";
        std::cout << "9. Salir\n";
        std::cout << "Seleccione una opción: ";
        if (!(std::cin >> opcion)) {
            std::cin.clear();
            std::string skip;
            std::getline(std::cin, skip);
            std::cout << "Entrada inválida. Intente nuevamente.\n";
            continue;
        }
        std::cout << "\n" << std::endl;
        std::string nombre, contenido;
        int prioridad, rafaga, pid, llegada, memreq;

        switch (opcion) {
            case 1:
                std::cout << "Nombre del archivo: ";
                std::cin >> nombre;
                std::cin.ignore();
                std::cout << "Contenido inicial: ";
                std::getline(std::cin, contenido);
                fs.createFile(nombre, contenido);
                break;

            case 2:
                fs.listFiles();
                break;

            case 3:
                std::cout << "Nombre del archivo a leer: ";
                std::cin >> nombre;
                dm.readFromDisk(fs, nombre);
                break;

            case 4: {
                File* f = fs.selectFileForWrite();
                if (f) {
                    std::cin.ignore();
                    std::cout << "Texto a agregar al archivo '" << f->name << "': ";
                    std::string contenido;
                    std::getline(std::cin, contenido);
                    dm.writeToDisk(fs, f->name, contenido);
                }
                break;
            }

            case 5:
                // Crear proceso: pedimos ID numérico + demás parámetros
                std::cout << "--- Creación de Proceso ---\n";
                std::cout << "ID del proceso: ";
                if (!(std::cin >> pid)) {
                    std::cin.clear();
                    std::string dump; std::getline(std::cin, dump);
                    std::cout << "ID inválido. Debe ser un número entero.\n";
                    break;
                }
                std::cout << "Burst time (tiempo total de CPU): ";
                if (!(std::cin >> rafaga)) { std::cin.clear(); std::string dump; std::getline(std::cin, dump); std::cout << "Valor inválido.\n"; break; }
                std::cout << "Tiempo de llegada: ";
                if (!(std::cin >> llegada)) { std::cin.clear(); std::string dump; std::getline(std::cin, dump); std::cout << "Valor inválido.\n"; break; }
                std::cout << "Prioridad (0 a 3, donde 0 es la más alta): ";
                if (!(std::cin >> prioridad)) { std::cin.clear(); std::string dump; std::getline(std::cin, dump); std::cout << "Valor inválido.\n"; break; }
                std::cout << "Memoria requerida (en páginas): ";
                if (!(std::cin >> memreq)) { std::cin.clear(); std::string dump; std::getline(std::cin, dump); std::cout << "Valor inválido.\n"; break; }

                // crear y luego, opcionalmente, ejecutar inmediatamente si hay memoria
                if (pm.createProcess(pid, rafaga, llegada, prioridad, memreq)) {
                    // no ejecutar automáticamente: dejamos que el usuario decida
                }
                break;

            case 6:
                pm.listProcesses();
                break;

            case 7:
                std::cout << "PID del proceso a terminar: ";
                if (!(std::cin >> pid)) {
                    std::cin.clear(); std::string dump; std::getline(std::cin, dump);
                    std::cout << "ID inválido.\n";
                    break;
                }
                // Si el proceso está pendiente o ejecutado, terminateProcess lo maneja (libera memoria si correspondía)
                pm.terminateProcess(pid);
                break;

            case 8:
                std::cout << "\nMemoria usada actualmente: " << mm.getUsedPages() << "/" << mm.getTotalPages() << " páginas\n";
                fs.listFiles();
                break;

            case 9:
                std::cout << "Saliendo del simulador...\n";
                break;

            default:
                std::cout << "Opción inválida.\n";
                break;
        }

        // adicional: opción extra para ejecutar procesos (no estaba en el menú principal)
        // Para mantener compatibilidad con tu flujo, permitimos ejecutar con un prompt rápido:
        // (el usuario puede invocar en cualquier momento tecleando 100 y luego el PID)
        if (opcion == 100) {
            std::cout << "Ingrese PID a ejecutar: ";
            if (std::cin >> pid) {
                pm.executeProcess(pid);
            } else {
                std::cin.clear(); std::string dump; std::getline(std::cin, dump);
                std::cout << "PID inválido.\n";
            }
        }

    } while (opcion != 9);

    return 0;
}