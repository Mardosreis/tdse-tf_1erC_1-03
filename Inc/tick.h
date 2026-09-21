/**
 * tick.h
 * Base de tiempo de 1ms (SysTick -> Callback) y utilidad de
 * temporizadores NO bloqueantes.
 *
 * Regla de oro del proyecto: NUNCA usar HAL_Delay() en la lógica de
 * aplicación. Todo tiempo se mide comparando contra Tick_GetMs().
 */
#ifndef TICK_H
#define TICK_H

#include <stdint.h>
#include <stdbool.h>

/* Timer de software no bloqueante */
typedef struct {
    uint32_t inicio_ms;
    uint32_t duracion_ms;
    bool     activo;
} temporizador_t;

/* --- Base de tiempo --- */
void     Tick_Init(void);
uint32_t Tick_GetMs(void);

/* Llamar UNA vez desde el HAL_SYSTICK_Callback (ver tick.c) */
void Tick_Callback(void);

/* --- Temporizador no bloqueante --- */
void Timer_Start(temporizador_t *t, uint32_t duracion_ms);
void Timer_Stop(temporizador_t *t);
bool Timer_Vencido(temporizador_t *t);   /* true una sola vez al vencer, y detiene el timer */
bool Timer_Activo(temporizador_t *t);

#endif /* TICK_H */
