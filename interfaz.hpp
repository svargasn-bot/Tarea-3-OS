#ifndef INTERFAZ_HPP
#define INTERFAZ_HPP

#include "supermercado.hpp"

// Inicialización y finalización de ncurses
void inicializarNcurses();
void finalizarNcurses();

// Dibuja una caja registradora específica
void dibujarCajaVisual(int y_base, CajaRegistradora* caja);

// Muestra el menú de configuración interactivo. Retorna false si el usuario decide salir (presiona 'q' o 'Q').
bool mostrarMenuConfiguracion(int &num_cajas, int &capacidad_cinta, int &clientes_por_caja, int &productos_por_cliente, int &vel_caja1, int &vel_caja2);

// Muestra la pantalla de finalización de la simulación
void mostrarPantallaFinal();

#endif
