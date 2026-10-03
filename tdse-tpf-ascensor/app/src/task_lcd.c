#include "task_lcd.h"
#include "i2c_lcd.h"
#include "main.h"
#include "logger.h"
#include "app.h"
#include <stdio.h>

// Traemos el puerto I2C1 que ya está configurado en tu placa
extern I2C_HandleTypeDef hi2c1;

// ¡CREAMOS EL OBJETO DEL LCD ACÁ MISMO!
I2C_LCD_HandleTypeDef hlcd;

void task_lcd_init(void *parameters) {
    // 1. Vinculamos el LCD al puerto I2C de tu placa
    hlcd.hi2c = &hi2c1;

    // 2. Le pasamos la dirección (0x27 desplazado 1 bit a la izquierda)
    hlcd.address = (0x27 << 1);

    // --- PARCHE DE SOFTWARE ANTI-BASURA ---
    // Le damos 200 milisegundos al display para que se limpie
    // y estabilice internamente tras el reinicio del micro.
    HAL_Delay(200);
    // --------------------------------------

    // 3. Inicializamos la pantalla físicamente
    lcd_init(&hlcd);
    lcd_clear(&hlcd);

    LOGGER_LOG("Tarea LCD Inicializada y Pantalla OK\r\n");
}
void task_lcd_update(void *parameters) {
    shared_data_type *shared_data = (shared_data_type *) parameters;
    static uint8_t estado_anterior = 255;

    // 1. DIBUJO ESTÁTICO (Solo se ejecuta cuando el ascensor cambia de estado)
    if (shared_data->estado_actual != estado_anterior) {
        switch(shared_data->estado_actual) {
            case ESTADO_ESPERANDO_LLAVE:
            {
                char buffer_linea[20];
                sprintf(buffer_linea, "Usted esta en P%d", shared_data->piso_actual);
                lcd_gotoxy(&hlcd, 0, 0);
                lcd_puts(&hlcd, buffer_linea);
                lcd_gotoxy(&hlcd, 0, 1);
                lcd_puts(&hlcd, "Pase su tarjeta ");
                break;
            }
            case ESTADO_ESPERANDO_BOTON:
                lcd_gotoxy(&hlcd, 0, 0);
                lcd_puts(&hlcd, "Bienvenido      ");
                lcd_gotoxy(&hlcd, 0, 1);
                lcd_puts(&hlcd, "Selec piso o M  ");
                break;
            case ESTADO_MOVIENDO:
                lcd_gotoxy(&hlcd, 0, 0);
                lcd_puts(&hlcd, "Moviendo...     ");
                lcd_gotoxy(&hlcd, 0, 1);
                lcd_puts(&hlcd, "Ascensor activo ");
                break;
            case ESTADO_SOBRECARGA:
                lcd_gotoxy(&hlcd, 0, 0);
                lcd_puts(&hlcd, "ATENCION:       ");
                lcd_gotoxy(&hlcd, 0, 1);
                lcd_puts(&hlcd, " SOBRECARGA     ");
                break;
            case ESTADO_EMERGENCIA:
                lcd_gotoxy(&hlcd, 0, 0);
                lcd_puts(&hlcd, "EMERGENCIA      ");
                lcd_gotoxy(&hlcd, 0, 1);
                lcd_puts(&hlcd, "LLAMANDO TECNICO");
                break;
            case ESTADO_SET_UP:
                // Solo limpiamos la pantalla de "basura" anterior para dejarle lugar al modo en vivo
                lcd_gotoxy(&hlcd, 0, 0);
                lcd_puts(&hlcd, "                ");
                lcd_gotoxy(&hlcd, 0, 1);
                lcd_puts(&hlcd, "                ");
                break;
        }
        estado_anterior = shared_data->estado_actual;
    }

    // 2. DIBUJO DINÁMICO (Se ejecuta SIEMPRE para actualizar en tiempo real)
    if (shared_data->estado_actual == ESTADO_SET_UP) {
        char buffer_linea[20];

        // Fila 0: Imprimimos el peso actualizándose en vivo
        sprintf(buffer_linea, "Peso:%.2f Kg    ", shared_data->peso_kg);
        lcd_gotoxy(&hlcd, 0, 0);
        lcd_puts(&hlcd, buffer_linea);

        // Fila 1: Lógica del cronómetro para el cartel
        lcd_gotoxy(&hlcd, 0, 1);
        if (shared_data->timer_tara > 0) {
            lcd_puts(&hlcd, "Calibrado OK!   ");
            shared_data->timer_tara--; // Va descontando el tiempo hasta llegar a cero
        } else {
            lcd_puts(&hlcd, "T:Conf S:Salir  "); // Vuelve al menú normal
        }
    }
}
