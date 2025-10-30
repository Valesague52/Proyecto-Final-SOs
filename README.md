# Simulador de Sistema Operativo en C++

Este proyecto es un **simulador educativo de sistema operativo** escrito en **C++**, que representa de forma simplificada cómo funcionan algunos componentes básicos de un SO:  
- Manejo de archivos (sistema de archivos y disco simulado)  
- Administración de memoria  
- Gestión de procesos (creación, ejecución, finalización y prioridades)

---



## Ejecución
Compila el programa con:
g++ main.cpp file_system.cpp disk_manager.cpp process_manager.cpp memory_manager.cpp -o simulador

Y ejecútalo con:
./simulador



## Menú principal
Al iniciar, verás el menú principal del sistema:

===== SISTEMA OPERATIVO SIMULADO =====
1. Crear archivo
2. Listar archivos
3. Leer archivo del disco
4. Escribir en archivo existente
5. Crear proceso
6. Listar procesos
7. Terminar proceso
8. Ver estado de memoria
9. Salir



## Sistema de Archivos y Disco
Crear un archivo
Cuando eliges la opción 1, se te pedirá:

--- Creación de Archivo ---
Nombre del archivo: a
Contenido del archivo: hola
✅ Archivo creado correctamente.

El tamaño se calcula automáticamente según el contenido ingresado:
por ejemplo, "hola" equivale a 4 bytes.

Listar archivos
--- Archivos en el sistema ---
Nombre: a | Tamaño: 4 bytes

Leer archivo del disco
--- Lectura de Archivo ---
Ingrese el nombre del archivo a leer: a
Contenido: hola

Escribir en archivo existente
--- Escritura en Archivo ---
Ingrese el nombre del archivo existente: a
Nuevo contenido a agregar: mundo
✅ Archivo actualizado correctamente.

El tamaño del archivo se actualizará automáticamente según el contenido agregado.



## Gestión de Procesos
Crear proceso
--- Creación de Proceso ---
ID del proceso: 1
Burst time (tiempo total de CPU): 3
Tiempo de llegada: 1
Prioridad (0 a 3, donde 0 es la más alta): 2
Memoria requerida (en páginas): 4
✅ Proceso agregado correctamente.

Listar procesos
--- Lista de Procesos ---
ID: 1 | Burst: 3 | Llegada: 1 | Prioridad: 2 | Memoria: 4 | Estado: Pendiente

Ejecutar proceso
=== Ejecución de Procesos ===
Ejecutando proceso ID 1...
Memoria asignada: 4 páginas. Total usada: 4/16
✅ Proceso 1 ejecutado correctamente.
CPU usada: 62%
Tiempo total de ejecución: 5 unidades

Luego de ejecutar, el estado cambia:

--- Lista de Procesos ---
ID: 1 | Burst: 3 | Llegada: 1 | Prioridad: 2 | Memoria: 4 | Estado: Ejecutado

Terminar proceso
--- Terminación de Proceso ---
Ingrese el ID del proceso a terminar: 1
✅ Proceso 1 terminado y memoria liberada.

Estado de la Memoria
El simulador usa 16 páginas de memoria total (se puede ajustar este valor en memory_manager.h). Al ver el estado de memoria:

--- Estado de Memoria ---
Memoria usada actualmente: 4/16 páginas
--- Archivos en el sistema ---
Nombre: a | Tamaño: 4 bytes



## Lógica Interna

Cada archivo ocupa memoria según su tamaño (1 byte por carácter ingresado).
Cada proceso usa una cantidad de páginas definida al crearse.
El planificador asigna prioridades (0 = más alta, 3 = más baja).
La memoria se libera automáticamente al terminar un proceso.



## Componentes del Proyecto

main.cpp: Controla el menú principal y la interacción del usuario.
file_system.*: Manejo de archivos y almacenamiento lógico.
disk_manager.*: Simula la lectura/escritura en disco.
process_manager.*: Gestión de procesos y su ejecución.
memory_manager.*: Control de asignación y liberación de memoria.



## Notas finales
Es una simulación didáctica, no un sistema operativo real.
Todos los datos se mantienen solo durante la ejecución (no se guardan en archivos físicos).
Las operaciones están diseñadas para mostrar de manera clara el funcionamiento básico de un SO.



## Referencias 
Tanenbaum, A. S., & Bos, H. J. (2015). *Modern Operating Systems* (4th ed.). Pearson Higher Education.

