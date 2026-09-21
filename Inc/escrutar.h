/**
 * escrutar.h
 * Etapa "ESCRUTAR": lee entradas físicas (botones, reed switches,
 * dip switches, celda de carga) y empuja EVENTOS a la cola.
 * No decide nada, no actúa sobre salidas: solo mira y reporta.
 */
#ifndef ESCRUTAR_H
#define ESCRUTAR_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MODO_NORMAL = 0,
    MODO_SET_UP,
    MODO_FALLA
} modo_sistema_t;

void Escrutar_Init(void);

/* Llamar en cada vuelta del super-loop (no bloqueante) */
void Escrutar_Actualizar(void);

/* Se lee UNA sola vez al boot para decidir el modo de arranque */
modo_sistema_t Escrutar_LeerDipSwitchesModo(void);

#endif /* ESCRUTAR_H */
