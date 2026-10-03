/**
 * loadcell.c
 * Driver básico para el amplificador NAU7802 (I2C) + celda de carga.
 *
 * IMPORTANTE: los nombres/direcciones de registro están basados en el
 * datasheet público del NAU7802. Antes de compilar, verificar cada
 * dirección de registro contra la hoja de datos de tu unidad puntual
 * (puede variar levemente según revisión de silicio).
 */
#include "loadcell.h"
#include "config.h"
#include "main.h"

extern I2C_HandleTypeDef I2C_BUS_HANDLE;

/* ---- Registros NAU7802 (verificar contra datasheet) ---- */
#define NAU7802_REG_PU_CTRL   0x00
#define NAU7802_REG_CTRL1     0x01
#define NAU7802_REG_CTRL2     0x02
#define NAU7802_REG_ADCO_B2   0x12
#define NAU7802_REG_ADCO_B1   0x13
#define NAU7802_REG_ADCO_B0   0x14

#define PU_CTRL_RR    (1 << 0) /* Register Reset */
#define PU_CTRL_PUD   (1 << 1) /* Power Up Digital */
#define PU_CTRL_PUA   (1 << 2) /* Power Up Analog */
#define PU_CTRL_CS    (1 << 4) /* Cycle Start (inicia conversión) */
#define PU_CTRL_CR    (1 << 5) /* Cycle Ready (dato listo) */

static int32_t g_offset_cero = 0;  /* valor crudo con tara */
static float   g_escala_gr_por_cuenta = 1.0f;

static bool NAU_WriteReg(uint8_t reg, uint8_t valor)
{
    uint8_t buf[2] = { reg, valor };
    return HAL_I2C_Master_Transmit(&I2C_BUS_HANDLE, NAU7802_I2C_ADDR, buf, 2, 10) == HAL_OK;
}

static bool NAU_ReadReg(uint8_t reg, uint8_t *valor)
{
    if (HAL_I2C_Master_Transmit(&I2C_BUS_HANDLE, NAU7802_I2C_ADDR, &reg, 1, 10) != HAL_OK)
        return false;
    return HAL_I2C_Master_Receive(&I2C_BUS_HANDLE, NAU7802_I2C_ADDR, valor, 1, 10) == HAL_OK;
}

void LoadCell_Init(void)
{
    NAU_WriteReg(NAU7802_REG_PU_CTRL, PU_CTRL_RR);
    HAL_Delay(1);
    NAU_WriteReg(NAU7802_REG_PU_CTRL, PU_CTRL_PUD);
    HAL_Delay(1);
    NAU_WriteReg(NAU7802_REG_PU_CTRL, PU_CTRL_PUD | PU_CTRL_PUA);

    /* Ganancia y velocidad de muestreo: ajustar según CTRL1/CTRL2 del datasheet */
    NAU_WriteReg(NAU7802_REG_CTRL1, 0x07); /* ganancia x128 (ejemplo) */

    /* Inicia conversiones continuas */
    uint8_t pu;
    NAU_ReadReg(NAU7802_REG_PU_CTRL, &pu);
    NAU_WriteReg(NAU7802_REG_PU_CTRL, pu | PU_CTRL_CS);

    g_offset_cero = 0;
    g_escala_gr_por_cuenta = 1.0f;
}

bool LoadCell_HayDatoListo(void)
{
    uint8_t pu = 0;
    if (!NAU_ReadReg(NAU7802_REG_PU_CTRL, &pu)) return false;
    return (pu & PU_CTRL_CR) != 0;
}

static int32_t LoadCell_LeerCrudo(void)
{
    uint8_t b2 = 0, b1 = 0, b0 = 0;
    NAU_ReadReg(NAU7802_REG_ADCO_B2, &b2);
    NAU_ReadReg(NAU7802_REG_ADCO_B1, &b1);
    NAU_ReadReg(NAU7802_REG_ADCO_B0, &b0);

    int32_t valor = ((int32_t)b2 << 16) | ((int32_t)b1 << 8) | b0;
    if (valor & 0x00800000) valor |= 0xFF000000; /* extensión de signo 24->32 bits */
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
