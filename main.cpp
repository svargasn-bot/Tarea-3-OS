#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>
#include "supermercado.hpp"

using namespace std;

// Función para inicializar ncurses
void inicializarNcurses() {
    initscr();     
    noecho();      
    curs_set(0);    
    keypad(stdscr, TRUE);
}

// =========================================================
// DIBUJAR CAJA: Genera la interfaz visual de las casillas
// =========================================================
void dibujarCajaVisual(int y_base, CajaRegistradora* caja) {
    mvprintw(y_base, 5, "==================================================");
    mvprintw(y_base + 1, 5, " CAJA REGISTRADORA %d ", caja->id);
    mvprintw(y_base + 2, 5, "==================================================");
    
    mvprintw(y_base + 4, 5, "Cinta Transportadora (Capacidad: %d)", caja->capacidad_correa);
    
    // Bloqueamos brevemente para leer la cinta sin que el cajero la mueva a la mitad
    pthread_mutex_lock(&caja->mutex_correa);
    
    int start_x = 5;
    for (int i = 0; i < caja->capacidad_correa; i++) {
        if (caja->correa[i] == 0) {
            // Espacio vacío
            mvprintw(y_base + 6, start_x + (i * 6), "[   ]"); 
        } else {
            // Espacio ocupado por un producto (mostramos el número de producto)
            mvprintw(y_base + 6, start_x + (i * 6), "[%3d]", caja->correa[i]);
        }
    }
    
    pthread_mutex_unlock(&caja->mutex_correa);

    mvprintw(y_base + 8, 5, "Total de productos procesados: %d", caja->total_cobrados);
}

// =========================================================
// FUNCIÓN PRINCIPAL
// =========================================================
int main() {
    srand(time(NULL));
    inicializarNcurses();

    // Configuración inicial según las restricciones de la tarea
    int num_cajas = 2; // Puede ser 1 o 2
    int capacidad_cinta = 10; // Rango permitido: 5 a 15
    int clientes_por_caja = 3; // Máximo 10
    int productos_por_cliente = 15; // Máximo 20

    vector<CajaRegistradora> cajas(num_cajas);
    vector<pthread_t> hebras_cajeros(num_cajas);
    
    // Almacenamos las hebras de los clientes y sus datos
    vector<pthread_t> hebras_clientes;
    vector<DatosCliente> datos_clientes;

    // 1. Inicializar las cajas y lanzar los cajeros (Consumidores)
    for (int i = 0; i < num_cajas; i++) {
        cajas[i].id = i + 1;
        cajas[i].capacidad_correa = capacidad_cinta;
        cajas[i].correa.assign(capacidad_cinta, 0); // Llenamos de 0 (vacío)
        cajas[i].indice_entrada = 0;
        cajas[i].indice_salida = 0;
        cajas[i].caja_abierta = true;
        cajas[i].total_cobrados = 0;

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
    // Se ejecuta mientras queden clientes poniendo productos
    bool simulacion_activa = true;
    while (simulacion_activa) {
        clear();
        
        mvprintw(1, 5, "SIMULACION DE SUPERMERCADO - PRODUCTOR / CONSUMIDOR");
        
        for (int i = 0; i < num_cajas; i++) {
            // Dibuja la caja 1 arriba y la caja 2 más abajo
            dibujarCajaVisual(4 + (i * 12), &cajas[i]);
        }
        
        refresh();
        
        // Condición simple de término: Si ya se cobraron todos los productos de todos los clientes
        int total_esperado = clientes_por_caja * productos_por_cliente;
        simulacion_activa = false;
        for (int i = 0; i < num_cajas; i++) {
            if (cajas[i].total_cobrados < total_esperado) {
                simulacion_activa = true;
            }
        }
        
        usleep(50000); // Tasa de refresco de la pantalla
    }

    // 4. Limpieza y Cierre Seguro
    // Esperamos a que los clientes terminen (Productores)
    for (size_t i = 0; i < hebras_clientes.size(); i++) {
        pthread_join(hebras_clientes[i], NULL);
    }

    // Cerramos las cajas y avisamos a los cajeros para que salgan de su bucle
    for (int i = 0; i < num_cajas; i++) {
        cajas[i].caja_abierta = false;
        sem_post(&cajas[i].sem_productos_listos); // Despierta al cajero si se quedó esperando
        pthread_join(hebras_cajeros[i], NULL);
        
        // Destruir semáforos y mutex
        sem_destroy(&cajas[i].sem_espacios_vacios);
        sem_destroy(&cajas[i].sem_productos_listos);
        pthread_mutex_destroy(&cajas[i].mutex_correa);
    }

    // Pantalla Final
    clear();
    mvprintw(5, 10, "===========================================");
    mvprintw(6, 10, "   LA SIMULACION HA TERMINADO CON EXITO    ");
    mvprintw(7, 10, "===========================================");
    mvprintw(9, 10, "Presiona cualquier tecla para salir...");
    refresh();
    getch();

    endwin();
    return 0;
}