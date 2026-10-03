#ifndef CONFIG_EEPROM_H
#define CONFIG_EEPROM_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t tiempo_puerta_abierta_ms;
    uint16_t umbral_sobrecarga_gr;
    uint8_t  velocidad_motor_pct;  /* 0-100, para futuro control PWM */
    uint8_t  checksum;
} config_setup_t;

/* Config vigente en RAM (se usa en todo el firmware) */
extern config_setup_t g_config;

void ConfigEeprom_Init(void);              /* carga desde EEPROM o pone default si falla */
bool ConfigEeprom_Guardar(void);
bool ConfigEeprom_Cargar(void);
void ConfigEeprom_CargarDefaults(void);

#endif
