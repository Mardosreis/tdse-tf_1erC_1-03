#ifndef MOTOR_H
#define MOTOR_H

typedef enum {
    MOTOR_STOP = 0,
    MOTOR_SUBIENDO,
    MOTOR_BAJANDO
} motor_estado_t;

void Motor_Init(void);
void Motor_Subir(void);
void Motor_Bajar(void);
void Motor_Detener(void);
motor_estado_t Motor_GetEstado(void);

#endif
