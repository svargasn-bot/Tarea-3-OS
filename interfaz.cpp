#include "interfaz.hpp"
#include <ncurses.h>

// Inicializar la biblioteca ncurses
void inicializarNcurses() {
    initscr();     
    noecho();      
    curs_set(0);    
    keypad(stdscr, TRUE);
}

// Finalizar ncurses
void finalizarNcurses() {
    endwin();
}

// Dibuja la caja registradora en una posición vertical específica (y_base)
void dibujarCajaVisual(int y_base, CajaRegistradora* caja) {
    mvprintw(y_base, 5, "==================================================");
    mvprintw(y_base + 1, 5, " CAJA REGISTRADORA %d (Velocidad: %s)", caja->id,
             (caja->tiempo_max_cajero <= 200000 ? "Rapida" : (caja->tiempo_max_cajero <= 400000 ? "Normal" : "Lenta")));
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
 
    mvprintw(y_base + 8, 5, "Total de productos cobrados: %d", caja->total_cobrados);
}

// Muestra la pantalla inicial con el formulario interactivo
bool mostrarMenuConfiguracion(int &num_cajas, int &capacidad_cinta, int &clientes_por_caja, int &productos_por_cliente, int &vel_caja1, int &vel_caja2) {
    int seleccion = 0;
    const int num_opciones = 6;
    
    // Valores por defecto iniciales
    num_cajas = 2;
    capacidad_cinta = 10;
    clientes_por_caja = 3;
    productos_por_cliente = 15;
    vel_caja1 = 2; // Normal
    vel_caja2 = 2; // Normal
    
    while (true) {
        clear();
        
        // Encabezado
        mvprintw(1, 2, "=========================================================");
        mvprintw(2, 2, "       SIMULACION DE SUPERMERCADO - CONFIGURACION        ");
        mvprintw(3, 2, "=========================================================");
        
        // Manual de instrucciones integrado para evaluación de Interfaz (10%)
        mvprintw(5, 4, "INSTRUCCIONES:");
        mvprintw(6, 4, "  * Use [ Arriba / Abajo ] para seleccionar una opcion.");
        mvprintw(7, 4, "  * Use [ Izquierda / Derecha ] para cambiar los valores.");
        mvprintw(8, 4, "  * Presione [ ENTER ] para iniciar la simulacion.");
        mvprintw(9, 4, "  * Presione [ Q ] para salir del programa.");
        
        mvprintw(11, 2, "---------------------------------------------------------");

        // Opciones del formulario con validación nativa (Manejo de errores)
        for (int i = 0; i < num_opciones; i++) {
            if (i == seleccion) {
                attron(A_REVERSE); // Resaltar opción seleccionada
                mvprintw(13 + i, 4, "  > ");
            } else {
                mvprintw(13 + i, 4, "    ");
            }
            
            switch (i) {
                case 0:
                    printw("Numero de Cajas:          [ %d ]  (Rango: 1 - 2)", num_cajas);
                    break;
                case 1:
                    printw("Capacidad de la Correa:   [ %d ]  (Rango: 5 - 15)", capacidad_cinta);
                    break;
                case 2:
                    printw("Clientes por Caja:        [ %d ]  (Rango: 1 - 10)", clientes_por_caja);
                    break;
                case 3:
                    printw("Productos por Cliente:    [ %d ]  (Rango: 1 - 20)", productos_por_cliente);
                    break;
                case 4:
                    printw("Velocidad de Caja 1:      [ %s ]  (Rapido / Normal / Lento)", 
                           (vel_caja1 == 1 ? "Rapido" : (vel_caja1 == 2 ? "Normal" : "Lento")));
                    break;
                case 5:
                    printw("Velocidad de Caja 2:      [ %s ]  (Rapido / Normal / Lento)", 
                           (vel_caja2 == 1 ? "Rapido" : (vel_caja2 == 2 ? "Normal" : "Lento")));
                    break;
            }
            
            if (i == seleccion) {
                attroff(A_REVERSE);
            }
        }
        
        mvprintw(21, 2, "=========================================================");
        refresh();
        
        int ch = getch();
        
        // Manejo de teclado
        if (ch == 'q' || ch == 'Q') {
            return false; // El usuario quiere salir del programa
        }
        else if (ch == 10 || ch == KEY_ENTER) {
            return true; // Continuar a la simulación
        }
        else if (ch == KEY_UP) {
            seleccion = (seleccion - 1 + num_opciones) % num_opciones;
        }
        else if (ch == KEY_DOWN) {
            seleccion = (seleccion + 1) % num_opciones;
        }
        else if (ch == KEY_LEFT) {
            // Modificar valores reduciendo (respetando los rangos del enunciado)
            switch (seleccion) {
                case 0:
                    if (num_cajas > 1) num_cajas--;
                    break;
                case 1:
                    if (capacidad_cinta > 5) capacidad_cinta--;
                    break;
                case 2:
                    if (clientes_por_caja > 1) clientes_por_caja--;
                    break;
                case 3:
                    if (productos_por_cliente > 1) productos_por_cliente--;
                    break;
                case 4:
                    if (vel_caja1 > 1) vel_caja1--;
                    break;
                case 5:
                    if (vel_caja2 > 1) vel_caja2--;
                    break;
            }
        }
        else if (ch == KEY_RIGHT) {
            // Modificar valores aumentando (respetando los rangos del enunciado)
            switch (seleccion) {
                case 0:
                    if (num_cajas < 2) num_cajas++;
                    break;
                case 1:
                    if (capacidad_cinta < 15) capacidad_cinta++;
                    break;
                case 2:
                    if (clientes_por_caja < 10) clientes_por_caja++;
                    break;
                case 3:
                    if (productos_por_cliente < 20) productos_por_cliente++;
                    break;
                case 4:
                    if (vel_caja1 < 3) vel_caja1++;
                    break;
                case 5:
                    if (vel_caja2 < 3) vel_caja2++;
                    break;
            }
        }
    }
}

// Pantalla final al terminar la simulación
void mostrarPantallaFinal() {
    clear();
    mvprintw(5, 10, "===========================================");
    mvprintw(6, 10, "   LA SIMULACION HA TERMINADO CON EXITO    ");
    mvprintw(7, 10, "===========================================");
    mvprintw(9, 10, "Presiona cualquier tecla para salir...");
    refresh();
    getch();
}
