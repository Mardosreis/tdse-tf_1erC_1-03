#include "task_hm10.h"
#include "main.h"
#include "logger.h"
#include "app.h"

// Traemos el manejador del puerto serie 1 que generó el CubeMX
extern UART_HandleTypeDef huart1;

void task_hm10_init(void *parameters) {
    LOGGER_LOG("Tarea HM10 Inicializada\r\n");
}

void task_hm10_update(void *parameters) {
    shared_data_type *shared_data = (shared_data_type *) parameters;
    uint8_t rx_byte = 0;

    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == SET) {
        // Leemos el dato que llegó y lo guardamos en rx_byte
        HAL_UART_Receive(&huart1, &rx_byte, 1, 0);

        // Evaluamos rx_byte (el carácter que mandó el celular)
                if (rx_byte == 'M') {
                    // Entramos al Menú de Configuración
                    shared_data->estado_actual = ESTADO_SET_UP;
                }
                else if (rx_byte == 'T' && shared_data->estado_actual == ESTADO_SET_UP) {
                    // Disparamos la calibración solo si estamos dentro del menú
                    shared_data->flag_tarar = true;
                }
                else if (rx_byte == 'S' && shared_data->estado_actual == ESTADO_SET_UP) {
                    // Salimos del menú y volvemos a la normalidad
                    shared_data->estado_actual = ESTADO_ESPERANDO_LLAVE;
                }
                // Tu código original para los pisos se mantiene intacto:
                else if (rx_byte == '1' && shared_data->estado_actual != ESTADO_SET_UP) {
                    shared_data->piso_destino = 1;
                    shared_data->boton_piso_presionado = true;
                } else if (rx_byte == '2' && shared_data->estado_actual != ESTADO_SET_UP) {
                    shared_data->piso_destino = 2;
                    shared_data->boton_piso_presionado = true;
                }
                else if (rx_byte == '3' && shared_data->estado_actual != ESTADO_SET_UP) {
                    shared_data->piso_destino = 3;
                    shared_data->boton_piso_presionado = true;
                }
    }

}
