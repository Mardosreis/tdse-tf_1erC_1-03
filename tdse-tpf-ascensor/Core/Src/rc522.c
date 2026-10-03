#include "rc522.h"

// CAMBIAMOS hspi2 POR hspi1
extern SPI_HandleTypeDef hspi1;

#define RC522_CS_LOW()      HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_RESET)
#define RC522_CS_HIGH()     HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_SET)

void MFRC522_WriteRegister(uint8_t addr, uint8_t val) {
    uint8_t txData[2];
    txData[0] = addr;
    txData[1] = val;
    RC522_CS_LOW();
    // ACÁ TAMBIÉN USAMOS hspi1
    HAL_SPI_Transmit(&hspi1, txData, 2, 100);
    RC522_CS_HIGH();
}

uint8_t MFRC522_ReadRegister(uint8_t addr) {
    uint8_t txData[2] = {addr | 0x80, 0x00};
    uint8_t rxData[2] = {0};
    RC522_CS_LOW();
    // ACÁ TAMBIÉN USAMOS hspi1
    HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, 100);
    RC522_CS_HIGH();
    return rxData[1];
}

void MFRC522_SetBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadRegister(reg);
    MFRC522_WriteRegister(reg, tmp | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadRegister(reg);
    MFRC522_WriteRegister(reg, tmp & (~mask));
}

void MFRC522_Reset(void) {
    MFRC522_WriteRegister(CommandReg, PCD_RESETPHASE);
    HAL_Delay(10);
}

void MFRC522_Init(void) {
    HAL_GPIO_WritePin(RC522_RST_GPIO_Port, RC522_RST_Pin, GPIO_PIN_SET);
    MFRC522_Reset();
    MFRC522_WriteRegister(TModeReg, 0x8D);
    MFRC522_WriteRegister(TPrescalerReg, 0x3E);
    MFRC522_WriteRegister(TReloadRegL, 30);
    MFRC522_WriteRegister(TReloadRegH, 0);
    MFRC522_WriteRegister(TxAutoReg, 0x40);
    MFRC522_WriteRegister(ModeReg, 0x3D);

    // Encender antena
    uint8_t temp = MFRC522_ReadRegister(TxControlReg);
    if (!(temp & 0x03)) {
        MFRC522_SetBitMask(TxControlReg, 0x03);
    }
}

uint8_t MFRC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen) {
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00, waitIRq = 0x00;
    uint8_t lastBits, n;
    uint16_t i;

    if (command == PCD_TRANSCEIVE) {
        irqEn = 0x77; waitIRq = 0x30;
    }

    MFRC522_WriteRegister(ComIEnReg, irqEn | 0x80);
    MFRC522_ClearBitMask(ComIrqReg, 0x80);
    MFRC522_SetBitMask(FIFOLevelReg, 0x80);

    MFRC522_WriteRegister(CommandReg, PCD_IDLE);

    for (i = 0; i < sendLen; i++) {
        MFRC522_WriteRegister(FIFODataReg, sendData[i]);
    }

    MFRC522_WriteRegister(CommandReg, command);
    if (command == PCD_TRANSCEIVE) {
        MFRC522_SetBitMask(BitFramingReg, 0x80);
    }

    i = 2000;
    do {
        n = MFRC522_ReadRegister(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    MFRC522_ClearBitMask(BitFramingReg, 0x80);

    if (i != 0) {
        if (!(MFRC522_ReadRegister(ErrorReg) & 0x1B)) {
            status = MI_OK;
            if (n & irqEn & 0x01) { status = MI_NOTAGERR; }
            if (command == PCD_TRANSCEIVE) {
                n = MFRC522_ReadRegister(FIFOLevelReg);
                lastBits = MFRC522_ReadRegister(ControlReg) & 0x07;
                if (lastBits) { *backLen = (n - 1) * 8 + lastBits; } else { *backLen = n * 8; }
                if (n == 0) { n = 1; }
                if (n > 16) { n = 16; }
                for (i = 0; i < n; i++) { backData[i] = MFRC522_ReadRegister(FIFODataReg); }
            }
        } else { status = MI_ERR; }
    }
    return status;
}

uint8_t MFRC522_Request(uint8_t reqMode, uint8_t *TagType) {
    uint8_t status;
    uint16_t backBits;
    MFRC522_WriteRegister(BitFramingReg, 0x07);
    TagType[0] = reqMode;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
    if ((status != MI_OK) || (backBits != 0x10)) { status = MI_ERR; }
    return status;
}

uint8_t MFRC522_Anticoll(uint8_t *serNum) {
    uint8_t status, i;
    uint8_t serNumCheck = 0;
    uint16_t unLen;
    MFRC522_WriteRegister(BitFramingReg, 0x00);
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
    if (status == MI_OK) {
        for (i = 0; i < 4; i++) { serNumCheck ^= serNum[i]; }
        if (serNumCheck != serNum[4]) { status = MI_ERR; }
    }
    return status;
}
