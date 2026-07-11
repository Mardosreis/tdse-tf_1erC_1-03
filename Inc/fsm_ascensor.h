#ifndef FSM_ASCENSOR_H
#define FSM_ASCENSOR_H

#include "events.h"

typedef enum {
    EST_IDLE = 0,          /* detenido en un piso, puerta cerrada, esperando pedido */
    EST_PUERTA_ABRIENDO,
    EST_PUERTA_ABIERTA,    /* timer de espera antes de cerrar */
    EST_PUERTA_CERRANDO,
    EST_SUBIENDO,
    EST_BAJANDO,
    EST_EMERGENCIA,
    EST_FALLA
} estado_ascensor_t;

typedef enum {
    PISO_PB = 0,
    PISO_2  = 1
} piso_t;

void FsmAscensor_Init(void);

/* Procesa UN evento por llamada (se llama repetidamente hasta vaciar la cola) */
void FsmAscensor_ProcesarEvento(const evento_t *ev);

/* Se llama en cada vuelta del loop, aun sin eventos nuevos, para poder
 * chequear vencimiento de temporizadores internos (timeouts). */
void FsmAscensor_Tick(void);

estado_ascensor_t FsmAscensor_GetEstado(void);
piso_t            FsmAscensor_GetPisoActual(void);

#endif
