#include "escrutar.h"
#include "config.h"
#include "events.h"
#include "tick.h"
#include "loadcell.h"
#include <string.h>

/* ---------- Antirrebote genérico por entrada digital ---------- */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    GPIO_PinState estado_estable;
    GPIO_PinState estado_candidato;
    uint32_t      t_cambio;
} entrada_debounce_t;

/* Botones (activos en LOW, pull-up) */
static entrada_debounce_t btn_piso2_ext, btn_piso1_ext, btn_pb_ext;
static entrada_debounce_t btn_piso2_int, btn_piso1_int, btn_pb_int;
static entrada_debounce_t btn_emergencia;

/* Reed switches */
static entrada_debounce_t reed_puerta, reed_pb, reed_piso2;

/* Sobrecarga: histéresis para no generar eventos por ruido */
static bool sobrecarga_activa = false;

static void Debounce_Init(entrada_debounce_t *e, GPIO_TypeDef *port, uint16_t pin)
{
    e->port = port;
    e->pin  = pin;
    e->estado_estable   = HAL_GPIO_ReadPin(port, pin);
    e->estado_candidato = e->estado_estable;
    e->t_cambio = Tick_GetMs();
}

/* Devuelve true si el estado ESTABLE cambió en esta llamada (flanco confirmado).
 * nuevo_estado_out entrega el nuevo estado estable. */
static bool Debounce_Actualizar(entrada_debounce_t *e, GPIO_PinState *nuevo_estado_out)
{
    GPIO_PinState leido = HAL_GPIO_ReadPin(e->port, e->pin);

    if (leido != e->estado_candidato) {
        e->estado_candidato = leido;
        e->t_cambio = Tick_GetMs();
        return false;
    }

    if (leido != e->estado_estable &&
        (uint32_t)(Tick_GetMs() - e->t_cambio) >= DEBOUNCE_MS) {
        e->estado_estable = leido;
        if (nuevo_estado_out) *nuevo_estado_out = leido;
        return true;
    }
    return false;
}

void Escrutar_Init(void)
{
    Debounce_Init(&btn_piso2_ext,  BTN_PISO2_EXT_PORT,  BTN_PISO2_EXT_PIN);
    Debounce_Init(&btn_piso1_ext,  BTN_PISO1_EXT_PORT,  BTN_PISO1_EXT_PIN);
    Debounce_Init(&btn_pb_ext,     BTN_PB_EXT_PORT,     BTN_PB_EXT_PIN);
    Debounce_Init(&btn_piso2_int,  BTN_PISO2_INT_PORT,  BTN_PISO2_INT_PIN);
    Debounce_Init(&btn_piso1_int,  BTN_PISO1_INT_PORT,  BTN_PISO1_INT_PIN);
    Debounce_Init(&btn_pb_int,     BTN_PB_INT_PORT,     BTN_PB_INT_PIN);
    Debounce_Init(&btn_emergencia, BTN_EMERGENCIA_PORT, BTN_EMERGENCIA_PIN);

    Debounce_Init(&reed_puerta, REED_PUERTA_PORT,  REED_PUERTA_PIN);
    Debounce_Init(&reed_pb,     REED_PISO_PB_PORT, REED_PISO_PB_PIN);
    Debounce_Init(&reed_piso2,  REED_PISO2_PORT,   REED_PISO2_PIN);

    sobrecarga_activa = false;
}

modo_sistema_t Escrutar_LeerDipSwitchesModo(void)
{
    /* DIP1 = ON (LOW) -> entra directo a modo SET_UP al bootear.
     * Resto de los dips quedan disponibles para futuras opciones. */
    if (HAL_GPIO_ReadPin(DIP1_PORT, DIP1_PIN) == GPIO_PIN_RESET) {
        return MODO_SET_UP;
    }
    return MODO_NORMAL;
}

/* Nota: NOTA_1 == GPIO_PIN_RESET para botones/reed activos en bajo (pull-up) */

static void Procesar_Boton(entrada_debounce_t *e, evento_id_t ev_al_presionar)
{
    GPIO_PinState nuevo;
    if (Debounce_Actualizar(e, &nuevo)) {
        if (nuevo == GPIO_PIN_RESET) { /* flanco de bajada = presionado */
            Eventos_Push(ev_al_presionar, 0);
        }
    }
}

void Escrutar_Actualizar(void)
{
    /* ---- Botones de pedido de piso (externos e internos = mismo evento) ---- */
    Procesar_Boton(&btn_pb_ext,    EV_PEDIDO_PB);
    Procesar_Boton(&btn_pb_int,    EV_PEDIDO_PB);
    Procesar_Boton(&btn_piso2_ext, EV_PEDIDO_PISO2);
    Procesar_Boton(&btn_piso2_int, EV_PEDIDO_PISO2);
    /* btn_piso1_* quedan disponibles si se agrega un 3er piso a futuro */

    /* ---- Emergencia: genera ON al presionar y OFF al soltar ---- */
    {
        GPIO_PinState nuevo;
        if (Debounce_Actualizar(&btn_emergencia, &nuevo)) {
            Eventos_Push(nuevo == GPIO_PIN_RESET ? EV_EMERGENCIA_ON : EV_EMERGENCIA_OFF, 0);
        }
    }

    /* ---- Reed switch de puerta ---- */
    {
        GPIO_PinState nuevo;
        if (Debounce_Actualizar(&reed_puerta, &nuevo)) {
            /* Definición: imán presente (LOW) = puerta cerrada */
            Eventos_Push(nuevo == GPIO_PIN_RESET ? EV_PUERTA_CERRADA_SENSOR
                                                  : EV_PUERTA_ABIERTA_SENSOR, 0);
        }
    }

    /* ---- Reed switches de piso (detectan llegada real de la cabina) ---- */
    {
        GPIO_PinState nuevo;
        if (Debounce_Actualizar(&reed_pb, &nuevo) && nuevo == GPIO_PIN_RESET) {
            Eventos_Push(EV_LLEGADA_PB, 0);
        }
        if (Debounce_Actualizar(&reed_piso2, &nuevo) && nuevo == GPIO_PIN_RESET) {
            Eventos_Push(EV_LLEGADA_PISO2, 0);
        }
    }

    /* ---- Celda de carga (NAU7802): polling con histéresis ----
     * Se lee cada vez que hay dato nuevo disponible (no bloqueante,
     * LoadCell_HayDatoListo() consulta un flag seteado por IT/DMA
     * o simplemente se puede llamar a baja frecuencia). */
    if (LoadCell_HayDatoListo()) {
        int32_t gramos = LoadCell_LeerGramos();

        if (!sobrecarga_activa && gramos >= SOBRECARGA_UMBRAL_GR_DEFAULT) {
            sobrecarga_activa = true;
            Eventos_Push(EV_SOBRECARGA_ON, gramos);
        } else if (sobrecarga_activa && gramos < (SOBRECARGA_UMBRAL_GR_DEFAULT - 100)) {
            /* histéresis de 100gr para evitar rebote en el límite */
            sobrecarga_activa = false;
            Eventos_Push(EV_SOBRECARGA_OFF, gramos);
        }
    }
}
