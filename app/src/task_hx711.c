#include "task_hx711.h"
#include "main.h"
#include "logger.h"
#include "app.h"

// Variable para guardar el peso crudo
int32_t peso_crudo = 0;

// Función interna para leer los 24 bits
int32_t hx711_read(void) {
    uint32_t count = 0;

    // 1. DESACTIVAR INTERRUPCIONES: Nadie interrumpe este bloque
    __disable_irq();

    // Generamos 24 pulsos de reloj para leer los datos bit a bit
    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(HX_SCK_GPIO_Port, HX_SCK_Pin, GPIO_PIN_SET);
        for(volatile int nop = 0; nop < 10; nop++) __NOP();

        count = count << 1;
        HAL_GPIO_WritePin(HX_SCK_GPIO_Port, HX_SCK_Pin, GPIO_PIN_RESET);
        for(volatile int nop = 0; nop < 10; nop++) __NOP();

        if (HAL_GPIO_ReadPin(HX_DT_GPIO_Port, HX_DT_Pin) == GPIO_PIN_SET) {
            count++;
        }
    }

    // Pulso número 25
    HAL_GPIO_WritePin(HX_SCK_GPIO_Port, HX_SCK_Pin, GPIO_PIN_SET);
    for(volatile int nop = 0; nop < 10; nop++) __NOP();


    // Ajuste del formato
    count = count ^ 0x800000;

    HAL_GPIO_WritePin(HX_SCK_GPIO_Port, HX_SCK_Pin, GPIO_PIN_RESET);

    // 2. VOLVER A ACTIVAR INTERRUPCIONES: El sistema vuelve a la normalidad
    __enable_irq();

    return count;
}

void task_hx711_init(void *parameters) {
    // Aseguramos que el reloj inicie en estado bajo
    HAL_GPIO_WritePin(HX_SCK_GPIO_Port, HX_SCK_Pin, GPIO_PIN_RESET);
    LOGGER_LOG("Tarea HX711 Inicializada\r\n");
}

void task_hx711_update(void *parameters) {
    shared_data_type *shared_data = (shared_data_type *) parameters;

    if (HAL_GPIO_ReadPin(HX_DT_GPIO_Port, HX_DT_Pin) == GPIO_PIN_RESET) {
        // Guardamos el peso directamente en la variable global
        shared_data->peso_actual = hx711_read();
    }
}
