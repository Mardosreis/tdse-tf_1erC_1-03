#include "motor.h"
#include "config.h"

static motor_estado_t g_estado = MOTOR_STOP;

void Motor_Init(void)
{
    Motor_Detener();
}

void Motor_Subir(void)
{
    HAL_GPIO_WritePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_ENA_PORT, MOTOR_ENA_PIN, GPIO_PIN_SET);
    g_estado = MOTOR_SUBIENDO;
}

void Motor_Bajar(void)
{
    HAL_GPIO_WritePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_ENA_PORT, MOTOR_ENA_PIN, GPIO_PIN_SET);
    g_estado = MOTOR_BAJANDO;
}

void Motor_Detener(void)
{
    HAL_GPIO_WritePin(MOTOR_IN1_PORT, MOTOR_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_PORT, MOTOR_IN2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_ENA_PORT, MOTOR_ENA_PIN, GPIO_PIN_RESET);
    g_estado = MOTOR_STOP;
}

motor_estado_t Motor_GetEstado(void)
{
    return g_estado;
}
