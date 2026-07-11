/**
 * loadcell.c
 * Driver reescrito para el ADC HX711 (Bit-Banging GPIO).
 */
#include "loadcell.h"
#include "config.h"
#include "tick.h"

static int32_t g_offset_cero = 0;
static float   g_escala_gr_por_cuenta = 1.0f;

void LoadCell_Init(void)
{
    /* El pin SCK debe iniciar en LOW para que el HX711 despierte */
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
    g_offset_cero = 0;
    g_escala_gr_por_cuenta = 1.0f;
    HAL_Delay(10); 
}

/* El HX711 avisa que tiene un dato listo poniendo el pin DT en estado LOW */
bool LoadCell_HayDatoListo(void)
{
    return (HAL_GPIO_ReadPin(HX711_DT_PORT, HX711_DT_PIN) == GPIO_PIN_RESET);
}

static int32_t LoadCell_LeerCrudo(void)
{
    int32_t valor = 0;

    /* Timeout de seguridad: si se desconecta el cable, no colgamos el microcontrolador */
    uint32_t inicio = Tick_GetMs();
    while (HAL_GPIO_ReadPin(HX711_DT_PORT, HX711_DT_PIN) == GPIO_PIN_SET) {
        if (Tick_GetMs() - inicio > 100) return 0; 
    }

    /* Leemos los 24 bits generando pulsos de reloj (Bit-Banging) */
    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
        valor = valor << 1;
        HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
        
        if (HAL_GPIO_ReadPin(HX711_DT_PORT, HX711_DT_PIN) == GPIO_PIN_SET) {
            valor++;
        }
    }

    /* Pulso n° 25 para configurar la ganancia (Gain = 128) para la próxima lectura */
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);

    /* Extensión de signo (el HX711 entrega datos en complemento a 2) */
    if (valor & 0x800000) {
        valor |= 0xFF000000;
    }

    return valor;
}

int32_t LoadCell_LeerGramos(void)
{
    int32_t crudo = LoadCell_LeerCrudo();
    int32_t neto  = crudo - g_offset_cero;
    return (int32_t)(neto * g_escala_gr_por_cuenta);
}

void LoadCell_Tara(void)
{
    g_offset_cero = LoadCell_LeerCrudo();
}

void LoadCell_CalibrarConPatron(int32_t gramos_patron)
{
    int32_t crudo_con_peso = LoadCell_LeerCrudo();
    int32_t delta_crudo = crudo_con_peso - g_offset_cero;
    if (delta_crudo != 0) {
        g_escala_gr_por_cuenta = (float)gramos_patron / (float)delta_crudo;
    }
}