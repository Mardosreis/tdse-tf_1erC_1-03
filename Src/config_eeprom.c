/**
 * config_eeprom.c
 * Persistencia del SET_UP migrada a la Flash interna de la STM32F103.
 * No requiere componentes externos. Usa la última página de la memoria.
 */
#include "config_eeprom.h"
#include "main.h"
#include <string.h>

/* Usamos la página 127 (la última de la STM32F103RB de 128KB) */
#define FLASH_ADDR_CONFIG 0x0801FC00

config_setup_t g_config;

static uint8_t Checksum(const config_setup_t *c)
{
    const uint8_t *p = (const uint8_t *)c;
    uint8_t sum = 0;
    for (size_t i = 0; i < sizeof(config_setup_t) - 1; i++) sum += p[i];
    return sum;
}

void ConfigEeprom_CargarDefaults(void)
{
    g_config.tiempo_puerta_abierta_ms = 3000;
    g_config.umbral_sobrecarga_gr     = 1500;
    g_config.velocidad_motor_pct      = 100;
    g_config.checksum = Checksum(&g_config);
}

bool ConfigEeprom_Guardar(void)
{
    g_config.checksum = Checksum(&g_config);
    
    HAL_FLASH_Unlock();
    
    /* 1. Borrar la página completa */
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_ADDR_CONFIG;
    EraseInitStruct.NbPages = 1;
    
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }
    
    /* 2. Escribir los datos en bloques de 32 bits (Words) */
    uint32_t buffer[2] = {0}; // Asume que la estructura ocupa menos de 8 bytes
    memcpy(buffer, &g_config, sizeof(config_setup_t));
    
    for (int i = 0; i < 2; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_CONFIG + (i * 4), buffer[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }
    
    HAL_FLASH_Lock();
    return true;
}

bool ConfigEeprom_Cargar(void)
{
    config_setup_t temp;
    
    /* Leer directo desde la dirección de memoria Flash */
    memcpy(&temp, (const void*)FLASH_ADDR_CONFIG, sizeof(config_setup_t));

    if (Checksum(&temp) != temp.checksum) return false; /* Memoria virgen o corrupta */

    g_config = temp;
    return true;
}

void ConfigEeprom_Init(void)
{
    if (!ConfigEeprom_Cargar()) {
        ConfigEeprom_CargarDefaults();
        ConfigEeprom_Guardar(); /* Graba los valores por defecto en la Flash */
    }
}