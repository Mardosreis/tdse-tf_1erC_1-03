#ifndef LEDS_H
#define LEDS_H

#include <stdbool.h>

void Leds_Init(void);
void Leds_Movimiento(bool on);
void Leds_Puerta(bool on);
void Leds_SetUp(bool on);
void Leds_Falla(bool on);
void Leds_Emergencia(bool on);

#endif
