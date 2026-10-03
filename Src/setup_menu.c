/**
 * setup_menu.c
 * Menú interactivo del modo SET_UP. Permite editar los parámetros
 * guardados en EEPROM usando los mismos botones físicos que en modo
 * NORMAL, pero reinterpretados:
 *
 *   Botón PB    -> NAV        (pasar al siguiente parámetro)
 *   Botón PISO2 -> INCREMENTAR (sumar un paso al parámetro actual, con wrap)
 *   Botón EMERGENCIA (en SET_UP) -> CONFIRMAR (en el ítem "Guardar y Salir")
 *
 * Esta reinterpretación la hace quien despacha los eventos (ver main.c),
 * no el módulo de escrutinio: el escrutinio siempre reporta lo mismo,
 * el significado depende del modo activo.
 */
#include "setup_menu.h"
#include "config_eeprom.h"
#include "lcd_i2c.h"
#include "buzzer.h"
#include "leds.h"
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    PARAM_TIEMPO_PUERTA = 0,
    PARAM_UMBRAL_SOBRECARGA,
    PARAM_VELOCIDAD_MOTOR,
    PARAM_GUARDAR_SALIR,
    PARAM_CANTIDAD
} parametro_t;

static parametro_t g_param_actual = PARAM_TIEMPO_PUERTA;

static void MostrarParametroActual(void)
{
    char linea[17];

    switch (g_param_actual) {
        case PARAM_TIEMPO_PUERTA:
            LCD_PrintLinea(0, "T. puerta (ms)");
            snprintf(linea, sizeof(linea), "%u", g_config.tiempo_puerta_abierta_ms);
            LCD_PrintLinea(1, linea);
            break;
        case PARAM_UMBRAL_SOBRECARGA:
            LCD_PrintLinea(0, "Umbral sobrec.(g)");
            snprintf(linea, sizeof(linea), "%u", g_config.umbral_sobrecarga_gr);
            LCD_PrintLinea(1, linea);
            break;
        case PARAM_VELOCIDAD_MOTOR:
            LCD_PrintLinea(0, "Velocidad (%)");
            snprintf(linea, sizeof(linea), "%u", g_config.velocidad_motor_pct);
            LCD_PrintLinea(1, linea);
            break;
        case PARAM_GUARDAR_SALIR:
            LCD_PrintLinea(0, "Guardar y salir?");
            LCD_PrintLinea(1, "-> CONFIRMAR");
            break;
        default:
            break;
    }
}

void SetupMenu_Entrar(void)
{
    g_param_actual = PARAM_TIEMPO_PUERTA;
    Leds_SetUp(true);
    MostrarParametroActual();
}

void SetupMenu_Salir(void)
{
    Leds_SetUp(false);
}

static void Navegar(void)
{
    g_param_actual = (parametro_t)((g_param_actual + 1) % PARAM_CANTIDAD);
    Buzzer_Disparar(BUZZER_PATRON_TECLA);
    MostrarParametroActual();
}

static void Incrementar(void)
{
    switch (g_param_actual) {
        case PARAM_TIEMPO_PUERTA:
            g_config.tiempo_puerta_abierta_ms += 500;
            if (g_config.tiempo_puerta_abierta_ms > 10000) g_config.tiempo_puerta_abierta_ms = 1000;
            break;
        case PARAM_UMBRAL_SOBRECARGA:
            g_config.umbral_sobrecarga_gr += 100;
            if (g_config.umbral_sobrecarga_gr > 3000) g_config.umbral_sobrecarga_gr = 100;
            break;
        case PARAM_VELOCIDAD_MOTOR:
            g_config.velocidad_motor_pct += 10;
            if (g_config.velocidad_motor_pct > 100) g_config.velocidad_motor_pct = 10;
            break;
        default:
            break;
    }
    Buzzer_Disparar(BUZZER_PATRON_TECLA);
    MostrarParametroActual();
}

static void Confirmar(void)
{
    if (g_param_actual == PARAM_GUARDAR_SALIR) {
        bool ok = ConfigEeprom_Guardar();
        LCD_PrintLinea(0, ok ? "Guardado OK" : "Error guardando");
        Buzzer_Disparar(BUZZER_PATRON_LLEGADA);
    }
}

void SetupMenu_ProcesarEvento(const evento_t *ev)
{
    switch (ev->id) {
        case EV_SETUP_NAV:        Navegar();     break;
        case EV_SETUP_INCREMENTAR: Incrementar(); break;
        case EV_SETUP_CONFIRMAR:  Confirmar();   break;
        default: break;
    }
}
