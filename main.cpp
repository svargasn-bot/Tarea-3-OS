#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <ncurses.h>
#include "supermercado.hpp"
#include "interfaz.hpp"

using namespace std;

int main() {
    srand(time(NULL));
    inicializarNcurses();

    // Parámetros de la simulación que serán configurados en el menú interactivo
    int num_cajas;
    int capacidad_cinta;
    int clientes_por_caja;
    int productos_por_cliente;
    int vel_caja1;
    int vel_caja2;

    // Lanzar el menú interactivo. Si retorna false, el usuario canceló la ejecución (Q)
    if (!mostrarMenuConfiguracion(num_cajas, capacidad_cinta, clientes_por_caja, productos_por_cliente, vel_caja1, vel_caja2)) {
        finalizarNcurses();
        return 0;
    }

    // Inicializar estructuras de datos basadas en los parámetros configurados
    vector<CajaRegistradora> cajas(num_cajas);
    vector<pthread_t> hebras_cajeros(num_cajas);
    
    vector<pthread_t> hebras_clientes;
    vector<DatosCliente> datos_clientes;
    // Reservamos memoria para evitar la invalidación de punteros (prevenir SIGSEGV)
    datos_clientes.reserve(num_cajas * clientes_por_caja);

    // 1. Inicializar las cajas y lanzar los cajeros (Consumidores)
    for (int i = 0; i < num_cajas; i++) {
        cajas[i].id = i + 1;
        cajas[i].capacidad_correa = capacidad_cinta;
        cajas[i].correa.assign(capacidad_cinta, 0); // Llenamos de 0 (vacío)
        cajas[i].indice_entrada = 0;
        cajas[i].indice_salida = 0;
        cajas[i].caja_abierta = true;
        cajas[i].total_cobrados = 0;

        // Configuración de tiempos de cobro basados en la velocidad elegida
        int vel_seleccionada = (i == 0 ? vel_caja1 : vel_caja2);
        if (vel_seleccionada == 1) { // Rápido
            cajas[i].tiempo_min_cajero = 50000;
            cajas[i].tiempo_max_cajero = 180000;
        } else if (vel_seleccionada == 2) { // Normal
            cajas[i].tiempo_min_cajero = 150000;
            cajas[i].tiempo_max_cajero = 380000;
        } else { // Lento
            cajas[i].tiempo_min_cajero = 300000;
            cajas[i].tiempo_max_cajero = 600000;
        }

        // Tiempos que tardan los clientes en esta caja
        cajas[i].tiempo_min_cliente = 100000;
        cajas[i].tiempo_max_cliente = 300000;

        sem_init(&cajas[i].sem_espacios_vacios, 0, capacidad_cinta);
        sem_init(&cajas[i].sem_productos_listos, 0, 0);
        pthread_mutex_init(&cajas[i].mutex_correa, NULL);

        pthread_create(&hebras_cajeros[i], NULL, funcionCajero_Consumidor, (void*)&cajas[i]);
    }

    // 2. Crear y lanzar los clientes (Productores)
    for (int i = 0; i < num_cajas; i++) {
        for (int j = 0; j < clientes_por_caja; j++) {
            DatosCliente nuevo_cliente = {j + 1, productos_por_cliente, &cajas[i]};
            datos_clientes.push_back(nuevo_cliente);
            
            pthread_t hebra_cliente;
            pthread_create(&hebra_cliente, NULL, funcionCliente_Productor, (void*)&datos_clientes.back());
            hebras_clientes.push_back(hebra_cliente);
        }
    }

    // 3. Bucle visual de la Interfaz (Actualiza la pantalla)
    bool simulacion_activa = true;
    while (simulacion_activa) {
        clear();
        
        mvprintw(1, 5, "SIMULACION DE SUPERMERCADO - PRODUCTOR / CONSUMIDOR");
        
        for (int i = 0; i < num_cajas; i++) {
            dibujarCajaVisual(4 + (i * 12), &cajas[i]);
        }
        
        refresh();
        
        // Condición de término: Si ya se cobraron todos los productos de todos los clientes
        simulacion_activa = false;
        for (int i = 0; i < num_cajas; i++) {
            int total_esperado_por_caja = clientes_por_caja * productos_por_cliente;
            if (cajas[i].total_cobrados < total_esperado_por_caja) {
                simulacion_activa = true;
            }
        }
        
        usleep(50000); // Tasa de refresco de la pantalla (50ms)
    }

    // 4. Limpieza y Cierre Seguro
    // Esperamos a que los clientes terminen (Productores)
    for (size_t i = 0; i < hebras_clientes.size(); i++) {
        pthread_join(hebras_clientes[i], NULL);
    }

    // Cerramos las cajas y avisamos a los cajeros para que salgan de su bucle
    for (int i = 0; i < num_cajas; i++) {
        cajas[i].caja_abierta = false;
        sem_post(&cajas[i].sem_productos_listos); // Despierta al cajero si quedó esperando
        pthread_join(hebras_cajeros[i], NULL);
        
        // Destruir semáforos y mutex
        sem_destroy(&cajas[i].sem_espacios_vacios);
        sem_destroy(&cajas[i].sem_productos_listos);
        pthread_mutex_destroy(&cajas[i].mutex_correa);
    }

    // Pantalla Final
    mostrarPantallaFinal();
    finalizarNcurses();

    return 0;
}