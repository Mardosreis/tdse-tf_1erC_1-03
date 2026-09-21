/**
 * lcd_i2c.c
 * Driver para LCD 16x2 con backpack I2C PCF8574 (muy común, ej. "YwRobot").
 *
 * NOTA IMPORTANTE sobre bloqueo:
 * Las funciones HAL_I2C_Master_Transmit acá usadas son bloqueantes pero
 * de MUY corta duración (unos pocos cientos de µs para 1 byte a 100kHz).
 * Se llaman solo en transiciones de estado (no en cada vuelta del loop),
 * por lo que no rompen el presupuesto de "1 vuelta < 1ms" en la práctica.
 * Si se requiere garantía dura, migrar a HAL_I2C_Master_Transmit_IT/DMA.
 */
#include "lcd_i2c.h"
#include "config.h"
#include "main.h"

extern I2C_HandleTypeDef I2C_BUS_HANDLE;

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RS_DATA   0x01
#define LCD_RS_CMD    0x00

static void LCD_WriteNibble(uint8_t nibble, uint8_t modo)
{
    uint8_t data = (nibble & 0xF0) | modo | LCD_BACKLIGHT;
    uint8_t buf_en   = data | LCD_ENABLE;
    uint8_t buf_noEn = data & ~LCD_ENABLE;

    HAL_I2C_Master_Transmit(&I2C_BUS_HANDLE, LCD_I2C_ADDR, &buf_en, 1, 5);
    HAL_I2C_Master_Transmit(&I2C_BUS_HANDLE, LCD_I2C_ADDR, &buf_noEn, 1, 5);
}

static void LCD_WriteByte(uint8_t valor, uint8_t modo)
{
    LCD_WriteNibble(valor & 0xF0, modo);
    LCD_WriteNibble((valor << 4) & 0xF0, modo);
}

static void LCD_Comando(uint8_t cmd) { LCD_WriteByte(cmd, LCD_RS_CMD); }
static void LCD_Dato(uint8_t dato)   { LCD_WriteByte(dato, LCD_RS_DATA); }

void LCD_Init(void)
{
    /* Secuencia de inicialización estándar HD44780 en modo 4 bits */
    HAL_Delay(50); /* única vez, al boot, aceptable */
    LCD_WriteNibble(0x30, LCD_RS_CMD);
    HAL_Delay(5);
    LCD_WriteNibble(0x30, LCD_RS_CMD);
    LCD_WriteNibble(0x30, LCD_RS_CMD);
    LCD_WriteNibble(0x20, LCD_RS_CMD); /* modo 4 bits */

    LCD_Comando(0x28); /* 4 bits, 2 líneas, 5x8 */
    LCD_Comando(0x0C); /* display ON, cursor OFF */
    LCD_Comando(0x06); /* incrementa cursor */
    LCD_Clear();
}

void LCD_Clear(void)
{
    LCD_Comando(0x01);
}

void LCD_SetCursor(uint8_t col, uint8_t fila)
{
    static const uint8_t offsets[] = {0x00, 0x40};
    LCD_Comando(0x80 | (col + offsets[fila & 0x01]));
}

void LCD_Print(const char *texto)
{
    while (*texto) {
        LCD_Dato((uint8_t)*texto++);
    }
}

void LCD_PrintLinea(uint8_t fila, const char *texto)
{
    LCD_SetCursor(0, fila);
    LCD_Print("                "); /* limpia la línea (16 espacios) */
    LCD_SetCursor(0, fila);
    LCD_Print(texto);
}
