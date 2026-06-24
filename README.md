# Simulación de Supermercado - Problema del Productor-Consumidor (Sistemas Operativos)

Este proyecto es una simulación interactiva en terminal de un supermercado que modela el clásico **problema del Productor-Consumidor con buffer acotado**, desarrollado en **C++** utilizando hilos POSIX (`pthread`), semáforos (`sem_t`) y exclusión mutua (`pthread_mutex_t`). La visualización e interacción se gestionan en tiempo real a través de la biblioteca **`ncurses`**.

---

## 🚀 Características

- **Menú de Configuración Interactivo:** Permite parametrizar dinámicamente la simulación antes de iniciar:
  - Número de cajas de cobro (1 o 2).
  - Capacidad de la correa transportadora (5 a 15 espacios).
  - Cantidad de clientes por caja (1 a 10).
  - Cantidad de productos por cliente (1 a 20).
  - Velocidad de procesamiento independiente para cada caja (Rápido, Normal o Lento).
- **Modelo de Sincronización Concurrente (Productor-Consumidor):**
  - **Clientes (Productores):** Cada cliente es un hilo que coloca productos secuencialmente en la correa transportadora (buffer circular) de su caja correspondiente.
  - **Cajeros (Consumidores):** Cada cajero es un hilo que retira los productos de la correa para procesarlos/cobrarlos.
  - **Correa (Buffer Acotado):** Implementada como un buffer circular compartido de tamaño configurable.
- **Interfaz Visual en Tiempo Real:** Representación visual de cada caja registradora, la correa transportadora con sus productos en tránsito y el total de productos procesados.
- **Finalización Segura (Graceful Shutdown):** Liberación ordenada de recursos (destrucción de semáforos, mutexes y finalización de hilos) al terminar la simulación.

---

## 🛠️ Requisitos de Sistema

Para compilar y ejecutar este programa, necesitas:
- Un compilador compatible con **C++11** o superior (como `g++`).
- Sistema operativo compatible con POSIX (Linux, macOS, WSL en Windows).
- La biblioteca **`ncurses`** instalada en tu sistema.

### Instalación de dependencias en Debian/Ubuntu:
```bash
sudo apt update
sudo apt install build-essential libncurses5-dev libncursesw5-dev
```

---

## 📂 Estructura del Proyecto

El proyecto se compone de los siguientes archivos fuente:

- **[main.cpp](file:///home/seba/Documentos/codigoGH/Tarea-3-OS/Tarea-3-OS/main.cpp):** Punto de entrada del programa. Coordina la inicialización de estructuras, la creación y unión de hilos de clientes y cajeros, el bucle de renderizado visual y la limpieza de recursos.
- **[supermercado.hpp](file:///home/seba/Documentos/codigoGH/Tarea-3-OS/Tarea-3-OS/supermercado.hpp) / [supermercado.cpp](file:///home/seba/Documentos/codigoGH/Tarea-3-OS/Tarea-3-OS/supermercado.cpp):** Contiene la definición de estructuras como `CajaRegistradora` y `DatosCliente`, y las funciones que ejecutan los hilos de clientes (`funcionCliente_Productor`) y cajeros (`funcionCajero_Consumidor`).
- **[interfaz.hpp](file:///home/seba/Documentos/codigoGH/Tarea-3-OS/Tarea-3-OS/interfaz.hpp) / [interfaz.cpp](file:///home/seba/Documentos/codigoGH/Tarea-3-OS/Tarea-3-OS/interfaz.cpp):** Implementa el menú interactivo de ncurses y las funciones de renderizado gráfico de las cajas registradoras en la terminal.

---

## ⚙️ Sincronización y Mecanismos Concurrentes

El flujo de productos entre clientes y cajeros se sincroniza mediante:
1. **`sem_espacios_vacios`:** Semáforo contador inicializado con la capacidad máxima de la correa. Controla que los clientes no coloquen productos si la correa está llena.
2. **`sem_productos_listos`:** Semáforo contador inicializado en 0. Controla que el cajero no intente escanear si la correa está vacía.
3. **`mutex_correa`:** Mutex para asegurar la exclusión mutua en la sección crítica, la cual ocurre al insertar o extraer un elemento en la estructura del buffer circular.

---

## 🔧 Compilación y Ejecución

### Compilar el programa:
Puedes compilar el programa usando `g++` enlazando las bibliotecas `pthread` y `ncurses`:

```bash
g++ -std=c++11 main.cpp supermercado.cpp interfaz.cpp -o tarea3 -lpthread -lncurses
```

### Ejecutar la simulación:
```bash
./tarea3
```

---

## 🎮 Instrucciones de Uso

1. Al abrir la aplicación, verás el menú de configuración.
2. Usa las teclas de **Flecha Arriba / Flecha Abajo** para moverte entre los parámetros.
3. Usa las teclas de **Flecha Izquierda / Flecha Derecha** para decrementar o incrementar los valores.
4. Presiona **ENTER** para iniciar la simulación con la configuración actual.
5. Puedes presionar **Q** en cualquier momento en el menú para salir del programa de forma segura.