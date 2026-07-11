/**
 * main.c
 * Ascensor embebido - TA134 Taller de Sistemas Embebidos
 *
 * Arquitectura: Super-Loop (Ejecutor Cíclico) con polling + interrupts,
 * tick de 1ms vía SysTick, tareas no bloqueantes, máquina de estados,
 * cola de eventos como interfaz entre Escrutar y Procesar.
 *
 * IMPORTANTE: este archivo asume que ya existe un proyecto generado
 * por STM32CubeMX (o CubeIDE) con los periféricos habilitados:
 *   - GPIO (botones, dip switches, reed switches, LEDs, buzzer, motor)
 *   - I2C1 (LCD, EEPROM, NAU7802) a 100kHz o 400kHz
 *   - USART2 (HM-10) con interrupción RX habilitada
 *   - SysTick a 1ms (default de HAL_Init())
 * Los handles (hi2c1, huart2) deben existir tal como los genera CubeMX;
 * ajustar los #define de config.h si tu proyecto usa otros periféricos.
 */
#include "main.h"
#include "config.h"
#include "tick.h"
#include "events.h"
#include "escrutar.h"
#include "motor.h"
#include "puerta.h"
#include "leds.h"
#include "buzzer.h"
#include "lcd_i2c.h"
#include "loadcell.h"
#include "config_eeprom.h"
#include "ble_hm10.h"
#include "fsm_ascensor.h"
#include "setup_menu.h"

/* Handles de periféricos generados por CubeMX (declarar/inicializar
 * según tu propio main generado; acá se declaran extern porque se
 * asume que ya existen en algún lado del proyecto real). */
I2C_HandleTypeDef  hi2c1;
UART_HandleTypeDef huart2;

static modo_sistema_t g_modo = MODO_NORMAL;

/* Prototipos de inicialización de periféricos (generados por CubeMX
 * en un proyecto real: SystemClock_Config, MX_GPIO_Init, MX_I2C1_Init,
 * MX_USART2_UART_Init). Se declaran acá como referencia. */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

/* Reinterpreta el evento "crudo" de escrutinio según el modo activo
 * (NORMAL: control del ascensor / SET_UP: navegación del menú). */
static void DespacharEvento(const evento_t *ev)
{
    if (g_modo == MODO_SET_UP) {
        evento_t remapeado = *ev;
        switch (ev->id) {
            case EV_PEDIDO_PB:      remapeado.id = EV_SETUP_NAV;        break;
            case EV_PEDIDO_PISO2:   remapeado.id = EV_SETUP_INCREMENTAR;break;
            case EV_EMERGENCIA_ON:  remapeado.id = EV_SETUP_CONFIRMAR;  break;
            default: return; /* otros eventos se ignoran en SET_UP */
        }
        SetupMenu_ProcesarEvento(&remapeado);
    } else {
        FsmAscensor_ProcesarEvento(ev);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    Tick_Init();
    Eventos_Init();
    Escrutar_Init();
    Motor_Init();
    Puerta_Init();
    Leds_Init();
    Buzzer_Init();
    LCD_Init();
    LoadCell_Init();
    BLE_Init();
    ConfigEeprom_Init();

    /* Modo de arranque: se lee UNA sola vez, de los dip switches */
    g_modo = Escrutar_LeerDipSwitchesModo();

    if (g_modo == MODO_SET_UP) {
        SetupMenu_Entrar();
    } else {
        FsmAscensor_Init();
    }

    /* ===================== SUPER-LOOP ===================== 
     * Cada vuelta debe ejecutarse en < 1ms. Ninguna de las llamadas
     * siguientes bloquea por tiempos largos (HAL_Delay solo se usa
     * una vez en la inicialización del LCD, nunca acá). */
    while (1)
    {
        /* ---- ESCRUTAR ---- */
        Escrutar_Actualizar();
        BLE_Actualizar();

        /* ---- PROCESAR ---- */
        evento_t ev;
        while (Eventos_Pop(&ev)) {
            DespacharEvento(&ev);
        }

        if (g_modo != MODO_SET_UP) {
            FsmAscensor_Tick(); /* chequeo de timeouts internos de la FSM */
        }

        /* ---- ACTUAR (tareas periódicas no bloqueantes) ---- */
        Buzzer_Actualizar();

        /* Aquí podría agregarse una medición de WCET por tarea,
         * ej. leyendo Tick_GetMs() al principio y al final del loop
         * y registrando el máximo observado, para el informe de
         * Memoria_Video_Codigo.md (WCET y Factor de Uso de CPU). */
    }
}

/* ===================== Stubs de inicialización HAL =====================
 * En un proyecto real generado por STM32CubeMX estas funciones ya
 * existen con el contenido correcto para tu placa. Se dejan como
 * placeholder para que el archivo sea autocontenido a nivel de
 * estructura; reemplazar por las funciones generadas por CubeMX. */

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef gi = {0};

    /* Botones + emergencia + reed switches + dip switches: entrada con pull-up */
    gi.Mode = GPIO_MODE_INPUT;
    gi.Pull = GPIO_PULLUP;

    gi.Pin = BTN_PISO2_EXT_PIN | BTN_PISO1_EXT_PIN | BTN_PB_EXT_PIN |
             BTN_PISO2_INT_PIN | BTN_PISO1_INT_PIN | BTN_PB_INT_PIN |
             BTN_EMERGENCIA_PIN;
    HAL_GPIO_Init(GPIOB, &gi);

    gi.Pin = REED_PUERTA_PIN | REED_PISO_PB_PIN | REED_PISO2_PIN;
    HAL_GPIO_Init(GPIOA, &gi);

    gi.Pin = DIP1_PIN | DIP2_PIN | DIP3_PIN | DIP4_PIN;
    HAL_GPIO_Init(GPIOC, &gi);

    /* Salidas: motor, LEDs, buzzer */
    gi.Mode = GPIO_MODE_OUTPUT_PP;
    gi.Pull = GPIO_NOPULL;
    gi.Speed = GPIO_SPEED_FREQ_LOW;

    gi.Pin = MOTOR_IN1_PIN | MOTOR_IN2_PIN | MOTOR_ENA_PIN;
    HAL_GPIO_Init(GPIOB, &gi);

    gi.Pin = LED_MOVIMIENTO_PIN | LED_PUERTA_PIN | LED_SETUP_PIN |
             LED_FALLA_PIN | LED_EMERGENCIA_PIN | BUZZER_PIN;
    HAL_GPIO_Init(GPIOC, &gi);
}

static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600; /* HM-10 default */
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
}
