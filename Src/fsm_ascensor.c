/**
 * fsm_ascensor.c
 * Máquina de estados principal del ascensor.
 *
 * Cada estado tiene una acción de "entrada" (lo que se hace UNA vez al
 * llegar a ese estado: prender motor, LED, buzzer, etc. -> etapa
 * ACTUAR) y reacciona a eventos que llegan desde la etapa ESCRUTAR.
 *
 * Ningún HAL_Delay(): las esperas se hacen con temporizador no
 * bloqueante (tick.h) chequeado en FsmAscensor_Tick().
 */
#include "fsm_ascensor.h"
#include "config.h"
#include "config_eeprom.h"
#include "tick.h"
#include "motor.h"
#include "puerta.h"
#include "leds.h"
#include "buzzer.h"
#include "lcd_i2c.h"

static estado_ascensor_t g_estado = EST_IDLE;
static piso_t             g_piso_actual = PISO_PB;
static piso_t             g_piso_destino = PISO_PB;
static temporizador_t     g_tmr_puerta;
static temporizador_t     g_tmr_viaje;
static bool               g_pedido_pendiente_piso2 = false;
static bool               g_pedido_pendiente_pb    = false;
static bool               g_sobrecarga = false;

/* ---------- Helpers de actuación (entrada a cada estado) ---------- */

static void Entrar_Idle(void)
{
    g_estado = EST_IDLE;
    Motor_Detener();
    Leds_Movimiento(false);
    LCD_PrintLinea(0, g_piso_actual == PISO_PB ? "Piso: PB" : "Piso: 2");
    LCD_PrintLinea(1, "Esperando...");
}

static void Entrar_PuertaAbriendo(void)
{
    g_estado = EST_PUERTA_ABRIENDO;
    Puerta_Abrir();
    Leds_Puerta(true);
    LCD_PrintLinea(1, "Abriendo puerta");
    /* la transición a PUERTA_ABIERTA ocurre al recibir el evento del
     * reed switch (EV_PUERTA_ABIERTA_SENSOR), no por tiempo fijo */
}

static void Entrar_PuertaAbierta(void)
{
    g_estado = EST_PUERTA_ABIERTA;
    LCD_PrintLinea(1, "Puerta abierta");
    Timer_Start(&g_tmr_puerta, g_config.tiempo_puerta_abierta_ms);
}

static void Entrar_PuertaCerrando(void)
{
    g_estado = EST_PUERTA_CERRANDO;
    Puerta_Cerrar();
    Leds_Puerta(false);
    LCD_PrintLinea(1, "Cerrando puerta");
}

static void Entrar_Viajando(piso_t destino)
{
    g_piso_destino = destino;
    g_estado = (destino == PISO_2) ? EST_SUBIENDO : EST_BAJANDO;

    if (destino == PISO_2) { Motor_Subir(); LCD_PrintLinea(1, "Subiendo..."); }
    else                   { Motor_Bajar(); LCD_PrintLinea(1, "Bajando...");  }

    Leds_Movimiento(true);
    Timer_Start(&g_tmr_viaje, TIMEOUT_VIAJE_MS); /* watchdog de viaje */
}

static void Entrar_Emergencia(void)
{
    g_estado = EST_EMERGENCIA;
    Motor_Detener();
    Leds_Movimiento(false);
    Leds_Emergencia(true);
    Puerta_Abrir();
    Buzzer_Disparar(BUZZER_PATRON_EMERGENCIA);
    LCD_PrintLinea(0, "EMERGENCIA");
    LCD_PrintLinea(1, "Ascensor detenido");
}

static void Entrar_Falla(const char *motivo)
{
    g_estado = EST_FALLA;
    Motor_Detener();
    Leds_Movimiento(false);
    Leds_Falla(true);
    Buzzer_Disparar(BUZZER_PATRON_FALLA);
    LCD_PrintLinea(0, "FALLA");
    LCD_PrintLinea(1, motivo);
}

/* ---------- API pública ---------- */

void FsmAscensor_Init(void)
{
    g_piso_actual = PISO_PB;
    g_pedido_pendiente_piso2 = false;
    g_pedido_pendiente_pb    = false;
    g_sobrecarga = false;
    Entrar_Idle();
}

estado_ascensor_t FsmAscensor_GetEstado(void)      { return g_estado; }
piso_t            FsmAscensor_GetPisoActual(void)  { return g_piso_actual; }

static void AtenderPedidosPendientes(void)
{
    if (g_estado != EST_IDLE) return; /* solo se atiende en reposo */

    if (g_piso_actual == PISO_PB && g_pedido_pendiente_piso2) {
        g_pedido_pendiente_piso2 = false;
        Entrar_PuertaCerrando(); /* por las dudas, asegura puerta cerrada antes de viajar */
    } else if (g_piso_actual == PISO_2 && g_pedido_pendiente_pb) {
        g_pedido_pendiente_pb = false;
        Entrar_PuertaCerrando();
    }
}

void FsmAscensor_ProcesarEvento(const evento_t *ev)
{
    /* ---- Emergencia y sobrecarga tienen prioridad sobre cualquier estado ---- */
    if (ev->id == EV_EMERGENCIA_ON) {
        Entrar_Emergencia();
        return;
    }
    if (ev->id == EV_EMERGENCIA_OFF && g_estado == EST_EMERGENCIA) {
        Buzzer_Detener();
        Leds_Emergencia(false);
        Entrar_Idle();
        return;
    }
    if (ev->id == EV_SOBRECARGA_ON) {
        g_sobrecarga = true;
        Buzzer_Disparar(BUZZER_PATRON_SOBRECARGA);
        if (g_estado == EST_IDLE || g_estado == EST_PUERTA_ABIERTA) {
            LCD_PrintLinea(1, "SOBRECARGA!");
        }
        return; /* no bloquea estados en curso, pero evita arrancar viajes nuevos (ver abajo) */
    }
    if (ev->id == EV_SOBRECARGA_OFF) {
        g_sobrecarga = false;
        return;
    }

    switch (g_estado) {

    case EST_IDLE:
        if (ev->id == EV_PEDIDO_PISO2 && g_piso_actual == PISO_PB) {
            if (g_sobrecarga) { Buzzer_Disparar(BUZZER_PATRON_SOBRECARGA); break; }
            Entrar_PuertaCerrando();
            g_piso_destino = PISO_2;
        } else if (ev->id == EV_PEDIDO_PB && g_piso_actual == PISO_2) {
            if (g_sobrecarga) { Buzzer_Disparar(BUZZER_PATRON_SOBRECARGA); break; }
            Entrar_PuertaCerrando();
            g_piso_destino = PISO_PB;
        } else if (ev->id == EV_PEDIDO_PISO2 && g_piso_actual == PISO_2) {
            /* ya está en destino: solo reabre puerta como cortesía */
            Entrar_PuertaAbriendo();
        } else if (ev->id == EV_PEDIDO_PB && g_piso_actual == PISO_PB) {
            Entrar_PuertaAbriendo();
        }
        break;

    case EST_PUERTA_CERRANDO:
        if (ev->id == EV_PUERTA_CERRADA_SENSOR) {
            Buzzer_Disparar(BUZZER_PATRON_TECLA);
            Entrar_Viajando(g_piso_destino);
        }
        break;

    case EST_SUBIENDO:
        if (ev->id == EV_LLEGADA_PISO2) {
            Timer_Stop(&g_tmr_viaje);
            g_piso_actual = PISO_2;
            Motor_Detener();
            Buzzer_Disparar(BUZZER_PATRON_LLEGADA);
            Entrar_PuertaAbriendo();
        }
        break;

    case EST_BAJANDO:
        if (ev->id == EV_LLEGADA_PB) {
            Timer_Stop(&g_tmr_viaje);
            g_piso_actual = PISO_PB;
            Motor_Detener();
            Buzzer_Disparar(BUZZER_PATRON_LLEGADA);
            Entrar_PuertaAbriendo();
        }
        break;

    case EST_PUERTA_ABRIENDO:
        if (ev->id == EV_PUERTA_ABIERTA_SENSOR) {
            Entrar_PuertaAbierta();
        }
        break;

    case EST_PUERTA_ABIERTA:
        /* Si llega un pedido del OTRO piso mientras la puerta está
         * abierta, se guarda como pendiente para atenderlo al cerrar. */
        if (ev->id == EV_PEDIDO_PISO2 && g_piso_actual == PISO_PB) {
            g_pedido_pendiente_piso2 = true;
        } else if (ev->id == EV_PEDIDO_PB && g_piso_actual == PISO_2) {
            g_pedido_pendiente_pb = true;
        } else if (ev->id == EV_TIMEOUT_PUERTA) {
            Entrar_PuertaCerrando();
        }
        break;

    case EST_FALLA:
        /* Requiere intervención: por ejemplo, un futuro EV_RESET_FALLA
         * desde el menú SET_UP para volver a NORMAL. Placeholder. */
        break;

    case EST_EMERGENCIA:
        /* manejado arriba (prioritario) */
        break;

    default:
        break;
    }
}

void FsmAscensor_Tick(void)
{
    /* Timeout de puerta abierta -> generar evento de cierre */
    if (g_estado == EST_PUERTA_ABIERTA && Timer_Vencido(&g_tmr_puerta)) {
        evento_t ev_local = { EV_TIMEOUT_PUERTA, 0 };
        FsmAscensor_ProcesarEvento(&ev_local);
    }

    /* Watchdog de viaje: si no llegó a destino en el tiempo esperado -> FALLA.
     * Esto cubre, por ejemplo, un reed switch que no detecta por estar
     * mal alineado o el motor trabado. */
    if ((g_estado == EST_SUBIENDO || g_estado == EST_BAJANDO) &&
        Timer_Vencido(&g_tmr_viaje)) {
        Entrar_Falla("Timeout viaje");
    }

    AtenderPedidosPendientes();
}
