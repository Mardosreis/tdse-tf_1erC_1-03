#include "task_pwm.h"
#include "main.h"
#include "app.h" // <-- Necesario para leer shared_data_type y los estados

// Variable del Timer 2 autogenerada por CubeIDE
extern TIM_HandleTypeDef htim2;

void task_pwm_init(void *parameters) {
    // Inicia la generación de la señal PWM en el pin PA0
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    // Frena el motor por seguridad al arrancar
    HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);

    // Velocidad inicial a 0
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
}

void task_pwm_update(void *parameters) {
    shared_data_type *shared_data = (shared_data_type *) parameters;

    // El motor reacciona de forma segura según el estado actual del ascensor
    if (shared_data->estado_actual == ESTADO_MOVIENDO) {
        // Sentido de giro para subir (IN1 en HIGH, IN2 en LOW)
        HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);

        // Seteamos velocidad (por ejemplo, 500 de PWM)
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 150);

        // Guardamos en la variable compartida para el log
        shared_data->pwm_active = 500;
    }
    else {
        // En cualquier otro estado (Sobrecarga, Esperando llave, etc.), FRENAR por seguridad.
        HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);

        // PWM a 0
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);

        shared_data->pwm_active = 0;
    }
}
