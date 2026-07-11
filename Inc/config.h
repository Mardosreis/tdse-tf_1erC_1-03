/**
 * config.h
 * Configuración central de pines y parámetros del sistema.
 * ACTUALIZADO 100% A TU TABLA DE PINES Y ESQUEMA
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "main.h"

/* ==================== BOTONES EXTERNOS (LLAMADAS) ==================== */
#define BTN_PB_EXT_PORT      GPIOC
#define BTN_PB_EXT_PIN       GPIO_PIN_9
#define BTN_PISO1_EXT_PORT   GPIOC
#define BTN_PISO1_EXT_PIN    GPIO_PIN_10
#define BTN_PISO2_EXT_PORT   GPIOC
#define BTN_PISO2_EXT_PIN    GPIO_PIN_11

/* ==================== BOTONES INTERNOS (CABINA) ==================== */
#define BTN_PB_INT_PORT      GPIOC
#define BTN_PB_INT_PIN       GPIO_PIN_12
#define BTN_PISO1_INT_PORT   GPIOB
#define BTN_PISO1_INT_PIN    GPIO_PIN_1
#define BTN_PISO2_INT_PORT   GPIOB
#define BTN_PISO2_INT_PIN    GPIO_PIN_2

/* ==================== SEGURIDAD (DIP EMERGENCIA) ==================== */
#define BTN_EMERGENCIA_PORT  GPIOB
#define BTN_EMERGENCIA_PIN   GPIO_PIN_12

/* ==================== SENSORES DE PISO (REED SWITCHES) ==================== */
#define REED_PISO_PB_PORT    GPIOC
#define REED_PISO_PB_PIN     GPIO_PIN_6
#define REED_PISO1_PORT      GPIOC
#define REED_PISO1_PIN       GPIO_PIN_7
#define REED_PISO2_PORT      GPIOC
#define REED_PISO2_PIN       GPIO_PIN_8

/* Reed Switch de Puerta (Asignado a un pin libre en caso de usarse a futuro) */
#define REED_PUERTA_PORT     GPIOA
#define REED_PUERTA_PIN      GPIO_PIN_1

/* ==================== SALIDAS (LEDS Y BUZZER) ==================== */
#define LED_MOVIMIENTO_PORT   GPIOB
#define LED_MOVIMIENTO_PIN    GPIO_PIN_3  /* Tu LED PB */
#define LED_PUERTA_PORT       GPIOB
#define LED_PUERTA_PIN        GPIO_PIN_4  /* Tu LED P1 */
#define LED_SETUP_PORT        GPIOB
#define LED_SETUP_PIN         GPIO_PIN_5  /* Tu LED P2 */

/* Al no haber LEDs extra para fallas, reutilizamos el de P2 (parpadeará) */
#define LED_FALLA_PORT        GPIOB
#define LED_FALLA_PIN         GPIO_PIN_5  
#define LED_EMERGENCIA_PORT   GPIOB
#define LED_EMERGENCIA_PIN    GPIO_PIN_5  

#define BUZZER_PORT           GPIOB
#define BUZZER_PIN            GPIO_PIN_13

/* ==================== MOTOR (L298N) ==================== */
#define MOTOR_ENA_PORT       GPIOA
#define MOTOR_ENA_PIN        GPIO_PIN_0   /* TIM2_CH1 PWM */
#define MOTOR_IN1_PORT       GPIOC
#define MOTOR_IN1_PIN        GPIO_PIN_0
#define MOTOR_IN2_PORT       GPIOC
#define MOTOR_IN2_PIN        GPIO_PIN_1

/* ==================== I2C (LCD) ==================== */
#define I2C_BUS_HANDLE        hi2c1
#define LCD_I2C_ADDR          (0x27 << 1)

/* ==================== UART (HM-10) ==================== */
#define HM10_UART_HANDLE      huart3

/* ==================== SPI (RFID RC522) ==================== */
#define RC522_SPI_HANDLE      hspi1
#define RC522_CS_PORT         GPIOB
#define RC522_CS_PIN          GPIO_PIN_6
#define RC522_RST_PORT        GPIOA
#define RC522_RST_PIN         GPIO_PIN_9

/* ==================== HX711 (CELDA DE CARGA) ==================== */
#define HX711_DT_PORT         GPIOC
#define HX711_DT_PIN          GPIO_PIN_4
#define HX711_SCK_PORT        GPIOC
#define HX711_SCK_PIN         GPIO_PIN_5

/* ==================== PARÁMETROS GENERALES ==================== */
#define DEBOUNCE_MS               30
#define TIEMPO_PUERTA_ABIERTA_MS  3000
#define TIMEOUT_VIAJE_MS          8000
#define BUZZER_BEEP_CORTO_MS      100
#define BUZZER_BEEP_LARGO_MS      400
#define SOBRECARGA_UMBRAL_GR_DEFAULT 1500
#define SOBRECARGA_MIN_GR_VALIDO     100

/* Modo SETUP: Reutilizamos el pin del botón externo de Planta Baja. 
   Si mantenés apretado el botón PB de la pared al encender la placa, 
   entra al menú de configuración en pantalla. */
#define DIP1_PORT GPIOC
#define DIP1_PIN  GPIO_PIN_9

#endif /* CONFIG_H */