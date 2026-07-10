# Bitácora de Desarrollo - Tarea 3: Sistemas Operativos
**Curso:** Sistemas Operativos (IN1077C)  
**Integrantes:**
- Sebastián Vargas 
- Francisco Muñoz
- Vicente Arias

---

## Resumen de Actividades y Sesiones

```
+------------------+-----------------------------------------------+------------------------+
| Fecha            | Actividad / Hito Principal                    | Estado                 |
+------------------+-----------------------------------------------+------------------------+
| Lun 29 Jun 2026  | Sesión 1: Reunión Inicial y Diseño Técnico    | Completado             |
| Mar 30 Jun 2026  | Sesión 2: Codificación del Núcleo Concurrente | Completado             |
| Jue 02 Jul 2026  | Sesión 3: Interfaz (Ncurses) y Ajustes Finales| Completado             |
+------------------+-----------------------------------------------+------------------------+
```

---

## Detalle de las Sesiones

### Reunion 1: Reunión Inicial de Coordinación y Diseño Técnico
* **Fecha:** Lunes, 29 de junio de 2026  
* **Participantes:** Todo el grupo.  
* **Duración:** 1 hora.  
* **Objetivos:** 
  - Lectura y análisis del enunciado de la Tarea 3.
  - Comprensión del esquema de sincronización (`Esquema.png`) y la plantilla (`prodcons.c`).
  - Definición de la estructura modular del proyecto.
* **Descripción:**  
  Nos reunimos para coordinar el trabajo. Analizamos detalladamente la plantilla dada en C (`prodcons.c`) y decidimos estructurar el proyecto utilizando modularidad limpia (dividido en `main`, `supermercado` e `interfaz`). Definimos las estructuras de datos y cómo interactuarían los semáforos.

---

### Reunion 2: Desarrollo e Implementación del Código
* **Fecha:** Martes, 30 de junio de 2026  
* **Participantes:** Todo el grupo.  
* **Duración:** 1.5 horas.  
* **Objetivos:**  
  - Implementar la lógica del productor (Cliente) y consumidor (Cajero).
  - Configurar semáforos y mutex de la correa transportadora.
* **Descripción:**  
  Procedimos al desarrollo de las funciones concurrentes (`funcionCliente_Productor` y `funcionCajero_Consumidor`) y la inicialización de los hilos mediante `pthread`. Gracias a que tomamos como base directa la lógica de sincronización de `prodcons.c`, la implementación y la integración del buffer circular se completaron de manera fluida, validando el correcto flujo de datos con pruebas de impresión por terminal.

---

### Reunion 3: Interfaz Visual (Ncurses) y Ajustes Finales
* **Fecha:** Jueves, 2 de julio de 2026  
* **Participantes:** Todo el grupo.  
* **Duración:** 2.5 horas.  
* **Objetivos:**  
  - Diseñar el menú interactivo con validación de rangos.
  - Implementar el bucle visual de simulación de cajas en tiempo real.
  - Limpieza final de recursos y pruebas.
* **Descripción:**  
  Desarrollamos la interfaz gráfica en modo texto usando la biblioteca `ncurses`. Implementamos el menú de configuración interactivo que valida y restringe los rangos requeridos por la pauta. Para finalizar, aseguramos la salida ordenada de los hilos de los cajeros y liberamos de manera segura toda la memoria y los semáforos.
