#include "buzzer.h"
#include "config.h"
#include "tick.h"
#include <stdbool.h>

static buzzer_patron_t g_patron = BUZZER_PATRON_NINGUNO;
static temporizador_t  g_tmr;
static uint8_t         g_repeticiones_restantes = 0;
static bool            g_pin_actual = false;

static void Pin(bool on)
{
    g_pin_actual = on;
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Buzzer_Init(void)
{
    Pin(false);
    g_patron = BUZZER_PATRON_NINGUNO;
    Timer_Stop(&g_tmr);
}

void Buzzer_Detener(void)
{
    Pin(false);
    g_patron = BUZZER_PATRON_NINGUNO;
    Timer_Stop(&g_tmr);
}

void Buzzer_Disparar(buzzer_patron_t patron)
{
    g_patron = patron;
    Pin(true);

    switch (patron) {
        case BUZZER_PATRON_LLEGADA:
            g_repeticiones_restantes = 1;
            Timer_Start(&g_tmr, BUZZER_BEEP_CORTO_MS);
            break;
        case BUZZER_PATRON_TECLA:
            g_repeticiones_restantes = 1;
            Timer_Start(&g_tmr, 40);
            break;
        case BUZZER_PATRON_SOBRECARGA:
            g_repeticiones_restantes = 3 * 2; /* 3 beeps = 6 flancos on/off */
            Timer_Start(&g_tmr, BUZZER_BEEP_CORTO_MS);
            break;
        case BUZZER_PATRON_EMERGENCIA:
            g_repeticiones_restantes = 0xFF; /* continúa hasta Buzzer_Detener() */
            Timer_Start(&g_tmr, BUZZER_BEEP_CORTO_MS);
            break;
        case BUZZER_PATRON_FALLA:
            g_repeticiones_restantes = 0xFF;
            Timer_Start(&g_tmr, BUZZER_BEEP_LARGO_MS);
            break;
        default:
            Buzzer_Detener();
            break;
    }
}

void Buzzer_Actualizar(void)
{
    if (g_patron == BUZZER_PATRON_NINGUNO) return;

    if (Timer_Vencido(&g_tmr)) {
        /* Toggle del pin para generar el siguiente flanco del patrón */
        Pin(!g_pin_actual);

        if (g_repeticiones_restantes != 0xFF) {
            if (g_repeticiones_restantes > 0) g_repeticiones_restantes--;
            if (g_repeticiones_restantes == 0) {
                Buzzer_Detener();
                return;
            }
        }

        uint32_t duracion = (g_patron == BUZZER_PATRON_FALLA) ? BUZZER_BEEP_LARGO_MS
                                                               : BUZZER_BEEP_CORTO_MS;
        Timer_Start(&g_tmr, duracion);
    }
}
