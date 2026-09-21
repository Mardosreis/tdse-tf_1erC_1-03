#ifndef BLE_HM10_H
#define BLE_HM10_H

void BLE_Init(void);

/* Llamar en cada vuelta del super-loop: procesa bytes ya recibidos
 * por interrupción y, al completar un comando, empuja el EVENTO
 * correspondiente a la cola (mismo mecanismo que un botón físico). */
void BLE_Actualizar(void);

void BLE_EnviarEstado(const char *texto);

#endif
