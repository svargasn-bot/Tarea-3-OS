#include "supermercado.hpp"
#include <cstdlib>
#include <unistd.h>

int generarAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

// =========================================================
// PRODUCTOR: El Cliente que pone cosas en la cinta
// =========================================================
void* funcionCliente_Productor(void* arg) {
    DatosCliente* datos = (DatosCliente*)arg;
    CajaRegistradora* caja = datos->caja;

    for (int i = 1; i <= datos->total_productos; i++) {
        // 1. Espera a que haya un espacio vacío en la cinta
        sem_wait(&caja->sem_espacios_vacios); 
        
        // 2. Bloquea la cinta (Sección Crítica)
        pthread_mutex_lock(&caja->mutex_correa);
        
        // Pone el producto (representado por su número)
        caja->correa[caja->indice_entrada] = i; 
        caja->indice_entrada = (caja->indice_entrada + 1) % caja->capacidad_correa;
        
        // Suelta la cinta
        pthread_mutex_unlock(&caja->mutex_correa);
        
        // 3. Le avisa al cajero que hay un nuevo producto listo
        sem_post(&caja->sem_productos_listos);
        
        // Simula el tiempo que tarda en sacar el producto del carro
        usleep(generarAleatorio(caja->tiempo_min_cliente, caja->tiempo_max_cliente)); 
    }
    return NULL;
}

// =========================================================
// CONSUMIDOR: El Cajero que escanea los productos
// =========================================================
void* funcionCajero_Consumidor(void* arg) {
    CajaRegistradora* caja = (CajaRegistradora*)arg;

    while (true) {
        // 1. Espera a que haya al menos 1 producto en la cinta
        sem_wait(&caja->sem_productos_listos);
        
        // 2. Bloquea la cinta (Sección Crítica)
        pthread_mutex_lock(&caja->mutex_correa);
        
        // Condición de salida segura: Si la caja cierra y ya no hay nada en la cinta
        if (!caja->caja_abierta && caja->correa[caja->indice_salida] == 0) {
            pthread_mutex_unlock(&caja->mutex_correa);
            break;
        }
        
        // Retira el producto para cobrarlo y deja un 0 (espacio vacío)
        if (caja->correa[caja->indice_salida] != 0) {
            caja->correa[caja->indice_salida] = 0; 
            caja->indice_salida = (caja->indice_salida + 1) % caja->capacidad_correa;
            caja->total_cobrados++;
        }
        
        // Suelta la cinta
        pthread_mutex_unlock(&caja->mutex_correa);
        
        // 3. Le avisa al cliente que quedó un espacio vacío nuevo en la cinta
        sem_post(&caja->sem_espacios_vacios);
        
        // Simula el tiempo que tarda el cajero en escanear el producto
        usleep(generarAleatorio(caja->tiempo_min_cajero, caja->tiempo_max_cajero)); 
    }
    return NULL;
}