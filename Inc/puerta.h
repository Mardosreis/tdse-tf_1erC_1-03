#ifndef PUERTA_H
#define PUERTA_H

#include <stdbool.h>

void Puerta_Init(void);
void Puerta_Abrir(void);   /* comando de apertura (o solo habilita LED/indicación) */
void Puerta_Cerrar(void);
bool Puerta_EstaCerradaPorSensor(void); /* lee el último estado reportado por el reed switch */
void Puerta_ActualizarEstadoSensor(bool cerrada); /* llamado desde la FSM al recibir el evento del reed */

#endif
