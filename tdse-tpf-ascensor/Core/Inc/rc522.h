#ifndef RC522_H_
#define RC522_H_

#include "main.h"

/* Comandos RC522 */
#define PCD_IDLE              0x00
#define PCD_AUTHENT           0x0E
#define PCD_RECEIVE           0x08
#define PCD_TRANSMIT          0x04
#define PCD_TRANSCEIVE        0x0C
#define PCD_RESETPHASE        0x0F
#define PCD_CALCCRC           0x03

/* Comandos de la Tarjeta (PICC) */
#define PICC_REQIDL           0x26
#define PICC_ANTICOLL         0x93

/* Registros RC522 */
#define CommandReg            0x01 << 1
#define ComIEnReg             0x02 << 1
#define DivlEnReg             0x03 << 1
#define ComIrqReg             0x04 << 1
#define DivIrqReg             0x05 << 1
#define ErrorReg              0x06 << 1
#define Status1Reg            0x07 << 1
#define Status2Reg            0x08 << 1
#define FIFODataReg           0x09 << 1
#define FIFOLevelReg          0x0A << 1
#define ControlReg            0x0C << 1
#define BitFramingReg         0x0D << 1
#define CollReg               0x0E << 1
#define ModeReg               0x11 << 1
#define TxModeReg             0x12 << 1
#define RxModeReg             0x13 << 1
#define TxControlReg          0x14 << 1
#define TxAutoReg             0x15 << 1
#define TModeReg              0x2A << 1
#define TPrescalerReg         0x2B << 1
#define TReloadRegH           0x2C << 1
#define TReloadRegL           0x2D << 1

/* Códigos de estado */
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2

void MFRC522_Init(void);
uint8_t MFRC522_Request(uint8_t reqMode, uint8_t *TagType);
uint8_t MFRC522_Anticoll(uint8_t *serNum);

#endif /* RC522_H_ */
