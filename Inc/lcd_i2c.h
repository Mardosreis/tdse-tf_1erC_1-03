#ifndef LCD_I2C_H
#define LCD_I2C_H

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t fila);
void LCD_Print(const char *texto);
void LCD_PrintLinea(uint8_t fila, const char *texto);

#endif
