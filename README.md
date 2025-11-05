
# Simulador de Sistema Operativo en C++

Este proyecto es un **simulador educativo de sistema operativo** escrito en **C++**, que representa de forma simplificada cómo funcionan los componentes principales de un SO real.
Permite observar de manera didáctica la planificación de procesos, la gestión de memoria virtual, la sincronización entre procesos, la planificación de disco y la interacción mediante consola (CLI).

---

## Ejecución

Compila el programa con:

```
g++ main.cpp file_system.cpp disk_manager.cpp process_manager.cpp memory_manager.cpp sync_manager.cpp device_manager.cpp interrupt_handler.cpp -o simulador -pthread
```

Y ejecútalo con:

```
./simulador
```

---

## Menú principal

Al iniciar, se muestra el menú principal del sistema:

```
===== SISTEMA OPERATIVO SIMULADO =====
1. Crear archivo
2. Listar archivos
3. Leer archivo del disco
4. Escribir en archivo existente
5. Crear proceso
6. Listar procesos
7. Suspender proceso
8. Reanudar proceso
9. Terminar proceso
10. Ver estado de memoria
11. Configurar planificador
12. Configurar política de memoria
13. Simular acceso a página
14. Sincronización - Cena de Filósofos
15. Sincronización - Productor/Consumidor
16. Planificación de Disco
17. Gestión de Dispositivos
18. Gestión de Interrupciones
19. Salir
```

---

## Sistema de Archivos y Disco

### Crear un archivo

```
--- Creación de Archivo ---
Nombre del archivo: datos.txt
Contenido del archivo: hola mundo
✅ Archivo creado correctamente.
```

Cada archivo calcula su tamaño según el contenido ingresado (1 byte por carácter).
Ejemplo: “hola mundo” equivale a **10 bytes**.

### Listar archivos

```
--- Archivos en el sistema ---
Nombre: datos.txt | Tamaño: 10 bytes
```

### Leer archivo del disco

```
--- Lectura de Archivo ---
Ingrese el nombre del archivo a leer: datos.txt
Contenido: hola mundo
```

### Escribir en archivo existente

```
--- Escritura en Archivo ---
Ingrese el nombre del archivo existente: datos.txt
Nuevo contenido a agregar: desde simulador
✅ Archivo actualizado correctamente.
```

El tamaño se actualiza automáticamente según el nuevo contenido.

---

## Gestión de Procesos

### Crear proceso

```
--- Creación de Proceso ---
ID del proceso: 1
Burst time (tiempo total de CPU): 5
Tiempo de llegada: 0
Prioridad (0 a 3, donde 0 es la más alta): 1
Memoria requerida (en páginas): 3
✅ Proceso agregado correctamente.
```

### Listar procesos

```
--- Lista de Procesos ---
ID: 1 | Burst: 5 | Llegada: 0 | Prioridad: 1 | Memoria: 3 | Estado: Pendiente
```

### Ejecución simulada

```
=== Ejecución de Procesos ===
Ejecutando proceso ID 1...
Memoria asignada: 4 páginas. Total usada: 4/16
✅ Proceso 1 ejecutado correctamente.
CPU usada: 62%
Tiempo total de ejecución: 5 unidades

```

Al finalizar, el estado cambia a *Ejecutado* y la memoria asignada se libera.

---

## Estado de la Memoria

La memoria se gestiona por **paginación simulada**. Cada proceso ocupa un número determinado de páginas.

```
--- Estado de Memoria ---
Páginas totales: 16
Páginas usadas: 3
Porcentaje de uso: 18%
Política actual: FIFO
```

Puedes cambiar la política de reemplazo:

* FIFO (First-In, First-Out)
* LRU (Least Recently Used)
* Working Set (Gestión avanzada)

Durante las simulaciones, se muestran los **hits** y **fallos de página** en tiempo real.

---

## Sincronización de Procesos

### Cena de los Filósofos

Simula la sincronización mediante semáforos y mutex.
Cada filósofo alterna entre pensar y comer sin caer en interbloqueo.

```
--- Cena de los Filósofos ---
Filósofo 1 está pensando...
Filósofo 2 está comiendo 
Filósofo 3 está esperando...
Filósofo 4 está pensando...
Filósofo 5 está comiendo 
```

Puedes **detener la simulación** presionando **Enter o espacio** en cualquier momento.

### Productor / Consumidor

Simula un buffer compartido con sincronización mediante semáforos.

```
Productor produce ítem 1
Consumidor consume ítem 1
Productor produce ítem 2
Consumidor consume ítem 2
```

---

## Planificación de Disco

Permite observar cómo distintos algoritmos mueven el cabezal del disco según las solicitudes.

### Ejemplo con FCFS:

```
Posición inicial: 50
Solicitudes: 82 170 43 140 24 16 190 10 70 100
Movimiento total: 642 cilindros
```

### Ejemplo con SCAN (Elevator):

```
Dirección inicial: derecha
Posición inicial: 60
Solicitudes: 40 180 30 70 120 10 90 150
Movimiento total: 280 cilindros
```

El sistema muestra el recorrido del cabezal y el movimiento total por algoritmo.

---

## Interfaz de Usuario del Núcleo (CLI)

La CLI permite al usuario:

* Crear y terminar procesos
* Configurar el planificador (Round Robin o SJF)
* Cambiar políticas de memoria
* Ver estadísticas del sistema
* Simular interrupciones y E/S

Ejemplo:

```
> listar procesos
> simular acceso página 3
> iniciar disco scan
> iniciar filósofos
```

---

## Componentes del Proyecto

main.cpp
Control principal del sistema y menú principal.

file_system.*
Gestión de archivos y almacenamiento lógico.

disk_manager.*
Simulación de acceso a disco con FCFS, SSTF y SCAN.

process_manager.*
Gestión de procesos, estados, planificación y ejecución.

memory_manager.*
Asignación, liberación y reemplazo de páginas (FIFO, LRU, Working Set).

sync_manager.*
Simulaciones de sincronización (Cena de los Filósofos, Productor-Consumidor).

device_manager.*
Simulación de acceso a dispositivos y recursos compartidos.

interrupt_handler.*
Manejo básico de interrupciones y bloqueos de procesos.

---

## Scripts de prueba (carpeta /scripts)

Incluye datos de simulación para ejecutar casos predefinidos:

mem_fifo.txt
mem_ws.txt
disk_fcfs.txt
disk_scan.txt
proc_rr.txt
proc_sjf.txt

Cada uno contiene configuraciones o secuencias que el sistema puede leer para pruebas automatizadas.

---

## Notas finales

Este simulador es un **entorno didáctico**, no un sistema operativo real.
Todos los datos son temporales y se pierden al finalizar la ejecución.
Su propósito es demostrar el funcionamiento de un núcleo básico y las interacciones entre CPU, memoria, disco y procesos.
Diseñado para cursos de **Sistemas Operativos** a nivel universitario.

---

## Referencias 
Tanenbaum, A. S., & Bos, H. J. (2015). *Modern Operating Systems* (4th ed.). Pearson Higher Education.

