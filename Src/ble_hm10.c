/**
 * ble_hm10.c
 * Recepción de comandos por Bluetooth (HM-10) vía UART con
 * interrupción, sin bloquear el super-loop.
 *
 * Comandos esperados desde la app (texto simple, terminados en '\n'):
 *   "SUBIR\n"   -> equivalente a botón Piso 2
 *   "BAJAR\n"   -> equivalente a botón PB
 *   "ESTADO\n"  -> pide que se le responda el estado actual
 */
#include "ble_hm10.h"
#include "config.h"
#include "events.h"
#include "main.h"
#include <string.h>

extern UART_HandleTypeDef HM10_UART_HANDLE;

#define BLE_BUF_TAM 32

static volatile uint8_t g_byte_rx;
static char    g_linea[BLE_BUF_TAM];
static uint8_t g_idx = 0;
static volatile bool g_linea_lista = false;

void BLE_Init(void)
{
    g_idx = 0;
    g_linea_lista = false;
    HAL_UART_Receive_IT(&HM10_UART_HANDLE, (uint8_t *)&g_byte_rx, 1);
}

/* Callback de HAL: se dispara por interrupción cada vez que llega 1 byte */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != HM10_UART_HANDLE.Instance) return;

    if (!g_linea_lista) {
        if (g_byte_rx == '\n' || g_byte_rx == '\r') {
            if (g_idx > 0) {
                g_linea[g_idx] = '\0';
                g_linea_lista = true;
            }
        } else if (g_idx < (BLE_BUF_TAM - 1)) {
            g_linea[g_idx++] = (char)g_byte_rx;
        }
    }

    /* Rearmar la recepción del siguiente byte (no bloqueante) */
    HAL_UART_Receive_IT(&HM10_UART_HANDLE, (uint8_t *)&g_byte_rx, 1);
}

void BLE_Actualizar(void)
{
    if (!g_linea_lista) return;

    if (strcmp(g_linea, "SUBIR") == 0) {
        Eventos_Push(EV_PEDIDO_PISO2, 0);
    } else if (strcmp(g_linea, "BAJAR") == 0) {
        Eventos_Push(EV_PEDIDO_PB, 0);
    } else if (strcmp(g_linea, "ESTADO") == 0) {
        BLE_EnviarEstado("OK\n"); /* completar con estado real desde la FSM si se desea */
    }

    g_idx = 0;
    g_linea_lista = false;
}

void BLE_EnviarEstado(const char *texto)
{
    /* Transmit_IT para no bloquear; si ya hay una transmisión en curso
     * se podría encolar, pero para mensajes cortos y esporádicos alcanza. */
    HAL_UART_Transmit_IT(&HM10_UART_HANDLE, (uint8_t *)texto, strlen(texto));
}
