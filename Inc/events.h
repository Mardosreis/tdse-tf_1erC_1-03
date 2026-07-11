/**
 * events.h
 * Cola de eventos (array de estructuras, FIFO circular) que conecta
 * la etapa de ESCRUTAR (botones, reed switches, BLE, sensores) con la
 * etapa de PROCESAR (máquina de estados).
 *
 * Cualquier fuente de entrada (botón físico, comando por Bluetooth,
 * sensor) empuja el mismo tipo de evento a la misma cola: la máquina
 * de estados no distingue el origen.
 */
#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    EV_NONE = 0,

    /* Pedidos de piso (de botón físico o de BLE, es indistinto) */
    EV_PEDIDO_PB,
    EV_PEDIDO_PISO2,

    /* Seguridad */
    EV_EMERGENCIA_ON,
    EV_EMERGENCIA_OFF,
    EV_SOBRECARGA_ON,
    EV_SOBRECARGA_OFF,

    /* Sensores de posición / puerta */
    EV_LLEGADA_PB,
    EV_LLEGADA_PISO2,
    EV_PUERTA_ABIERTA_SENSOR,
    EV_PUERTA_CERRADA_SENSOR,

    /* Timeouts internos (generados por la propia FSM/temporizadores) */
    EV_TIMEOUT_PUERTA,
    EV_TIMEOUT_VIAJE,

    /* Modo / configuración */
    EV_ENTRAR_SETUP,
    EV_SALIR_SETUP,
    EV_SETUP_NAV,        /* botón "navegar" dentro del menú */
    EV_SETUP_INCREMENTAR,
    EV_SETUP_CONFIRMAR,

    EV_CANTIDAD /* debe quedar último */
} evento_id_t;

typedef struct {
    evento_id_t id;
    int32_t     dato;   /* opcional: valor asociado (ej. gramos leídos) */
} evento_t;

#define COLA_EVENTOS_TAM 16

void Eventos_Init(void);
bool Eventos_Push(evento_id_t id, int32_t dato);
bool Eventos_Pop(evento_t *out);
bool Eventos_Vacia(void);

#endif /* EVENTS_H */
