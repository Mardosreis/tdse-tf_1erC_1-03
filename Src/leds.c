#include "leds.h"
#include "config.h"

static inline void Set(GPIO_TypeDef *port, uint16_t pin, bool on)
{
    HAL_GPIO_WritePin(port, pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Leds_Init(void)
{
    Leds_Movimiento(false);
    Leds_Puerta(false);
    Leds_SetUp(false);
    Leds_Falla(false);
    Leds_Emergencia(false);
}

void Leds_Movimiento(bool on) { Set(LED_MOVIMIENTO_PORT, LED_MOVIMIENTO_PIN, on); }
void Leds_Puerta(bool on)     { Set(LED_PUERTA_PORT,     LED_PUERTA_PIN,     on); }
void Leds_SetUp(bool on)      { Set(LED_SETUP_PORT,      LED_SETUP_PIN,      on); }
void Leds_Falla(bool on)      { Set(LED_FALLA_PORT,      LED_FALLA_PIN,      on); }
void Leds_Emergencia(bool on) { Set(LED_EMERGENCIA_PORT, LED_EMERGENCIA_PIN, on); }
