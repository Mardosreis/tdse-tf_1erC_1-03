#include "task_rc522.h"
#include "rc522.h"
#include "logger.h"
#include "main.h"
#include "app.h"

// Definimos el tamaño máximo del arreglo de datos (16 bytes)
#define MAX_LEN 16

// Variable para controlar la frecuencia de lectura y no saturar el bus
static uint32_t rc522_tick = 0;

void task_rc522_init(void *parameters) {
    MFRC522_Init();
    LOGGER_LOG("Tarea RC522 Inicializada\r\n");
}

void task_rc522_update(void *parameters) {
    shared_data_type *shared_data = (shared_data_type *) parameters;
    uint8_t status;
    uint8_t str[MAX_LEN];

    if ((HAL_GetTick() - rc522_tick) > 500) {
        rc522_tick = HAL_GetTick();
        status = MFRC522_Request(PICC_REQIDL, str);

        if (status == MI_OK) {
            LOGGER_LOG("RFID: Tarjeta detectada!\r\n");
            status = MFRC522_Anticoll(str);
            if (status == MI_OK) {
                shared_data->tarjeta_leida = true;
            }
        }
    }
}
