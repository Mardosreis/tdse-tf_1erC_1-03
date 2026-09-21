#ifndef TASK_INC_TASK_LCD_H_
#define TASK_INC_TASK_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

// Inicializa la tarea del LCD y la pantalla misma.
void task_lcd_init(void *parameters);

// Actualiza la pantalla (se llamará repetidamente en el bucle principal).
void task_lcd_update(void *parameters);

#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_LCD_H_ */
