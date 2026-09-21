#include "events.h"

static evento_t g_cola[COLA_EVENTOS_TAM];
static uint8_t  g_head = 0;
static uint8_t  g_tail = 0;
static uint8_t  g_count = 0;

void Eventos_Init(void)
{
    g_head = 0;
    g_tail = 0;
    g_count = 0;
}

bool Eventos_Push(evento_id_t id, int32_t dato)
{
    if (g_count >= COLA_EVENTOS_TAM) {
        return false; /* cola llena: se descarta (o se podría contar overflow) */
    }
    g_cola[g_tail].id   = id;
    g_cola[g_tail].dato = dato;
    g_tail = (g_tail + 1) % COLA_EVENTOS_TAM;
    g_count++;
    return true;
}

bool Eventos_Pop(evento_t *out)
{
    if (g_count == 0) return false;

    *out = g_cola[g_head];
    g_head = (g_head + 1) % COLA_EVENTOS_TAM;
    g_count--;
    return true;
}

bool Eventos_Vacia(void)
{
    return (g_count == 0);
}
