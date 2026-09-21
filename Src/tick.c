#include "tick.h"
#include "main.h"

static volatile uint32_t g_tick_ms = 0;

void Tick_Init(void)
{
    g_tick_ms = 0;
    /* El SysTick ya lo configura HAL_Init() a 1ms por defecto (HAL_InitTick).
     * Solo necesitamos engancharnos en el callback (ver abajo). */
}

uint32_t Tick_GetMs(void)
{
    return g_tick_ms;
}

/* Este callback lo llama la HAL automáticamente cada 1ms desde el
 * handler de interrupción SysTick_Handler -> HAL_SYSTICK_IRQHandler
 * -> HAL_SYSTICK_Callback (débilmente enlazado, lo redefinimos acá). */
void HAL_SYSTICK_Callback(void)
{
    Tick_Callback();
}

void Tick_Callback(void)
{
    g_tick_ms++;
}

/* ===================== Temporizador no bloqueante ===================== */

void Timer_Start(temporizador_t *t, uint32_t duracion_ms)
{
    t->inicio_ms   = g_tick_ms;
    t->duracion_ms = duracion_ms;
    t->activo      = true;
}

void Timer_Stop(temporizador_t *t)
{
    t->activo = false;
}

bool Timer_Activo(temporizador_t *t)
{
    return t->activo;
}

bool Timer_Vencido(temporizador_t *t)
{
    if (!t->activo) return false;

    if ((uint32_t)(g_tick_ms - t->inicio_ms) >= t->duracion_ms) {
        t->activo = false; /* se auto-detiene: dispara una sola vez */
        return true;
    }
    return false;
}
