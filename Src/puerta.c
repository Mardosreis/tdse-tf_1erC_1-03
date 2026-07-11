#include "puerta.h"
#include "config.h"

static bool g_cerrada = true;

void Puerta_Init(void)
{
    g_cerrada = true;
}

void Puerta_Abrir(void)
{
    /* Si la puerta es fija/simulada, acá solo se prende el LED indicador
     * (eso lo hace la etapa Actuar en base al estado de la FSM).
     * Si en el futuro se agrega servo, acá se manda el pulso PWM. */
}

void Puerta_Cerrar(void)
{
    /* Idem: en la versión con puerta fija, no hay actuador físico que mover. */
}

bool Puerta_EstaCerradaPorSensor(void)
{
    return g_cerrada;
}

void Puerta_ActualizarEstadoSensor(bool cerrada)
{
    g_cerrada = cerrada;
}
