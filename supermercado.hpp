#ifndef SUPERMERCADO_HPP
#define SUPERMERCADO_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estructura que representa la correa transportadora de una caja
struct CajaRegistradora {
    int id;
    int capacidad_correa;
    std::vector<int> correa; // El buffer circular
    int indice_entrada;
    int indice_salida;
    
    // Semáforos y Mutex obligatorios
    sem_t sem_espacios_vacios; 
    sem_t sem_productos_listos; 
    pthread_mutex_t mutex_correa; 
    
    bool caja_abierta;
    int total_cobrados; // NUEVO: Para la interfaz visual
};

// Datos que se le pasan al hilo del Cliente (Productor)
struct DatosCliente {
    int id_cliente;
    int total_productos;
    CajaRegistradora* caja;
};

int generarAleatorio(int min, int max);

void* funcionCliente_Productor(void* arg);
void* funcionCajero_Consumidor(void* arg);

#endif