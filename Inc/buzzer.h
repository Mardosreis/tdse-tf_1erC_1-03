#ifndef BUZZER_H
#define BUZZER_H

typedef enum {
    BUZZER_PATRON_NINGUNO = 0,
    BUZZER_PATRON_LLEGADA,      /* 1 beep corto */
    BUZZER_PATRON_TECLA,        /* 1 beep muy corto */
    BUZZER_PATRON_SOBRECARGA,   /* 3 beeps */
    BUZZER_PATRON_EMERGENCIA,   /* beep continuo mientras dure el patrón activo */
    BUZZER_PATRON_FALLA         /* beep largo repetido */
} buzzer_patron_t;

void Buzzer_Init(void);
void Buzzer_Disparar(buzzer_patron_t patron);
void Buzzer_Detener(void);

/* Llamar en cada vuelta del super-loop (no bloqueante) */
void Buzzer_Actualizar(void);

#endif
