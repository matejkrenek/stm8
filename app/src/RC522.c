#include "stm8s.h"
#include "RC522.h"
#include "milis.h"

Pin *RC522_SDA;
Pin *RC522_SCK;
Pin *RC522_MOSI;
Pin *RC522_MISO;
Pin *RC522_RST;
Pin *LED_BUILTIN;

const RC522_Module RFID = {
    .init = RC522_Init,
    .reset = RC522_Reset,
    .antennaOn = RC522_AntennaOn,
    .writeRegister = RC522_WriteRegister,
    .readRegister = RC522_ReadRegister,
    .request = RC522_Request,
    .anticoll = RC522_Anticoll,
    .clearRegisterMask = RC522_ClearRegisterMask,
    .setRegisterMask = RC522_SetRegisterMask,
    .select = RC522_Select,
    .haltA = RC522_HaltA,
    .read = RC522_Read,
    .calculateCRC = RC522_CalculateCRC,
    .communicateWithPICC = RC522_CommunicateWithPICC,
};

void RC522_Init(void)
{
    // enable peripheral SPI clock
    CLK.peripheralEnable(CLK_PERIPHERAL_SPI);

    GPIO.externalPullup(GPIOC, (PIN_7 | PIN_6 | PIN_5), ENABLE);

    // configure pinouts
    RC522_SDA = GPIO.init(GPIOC, PIN_4, OUTPUT_PP_HIGH_SLOW);
    RC522_RST = GPIO.init(GPIOC, PIN_3, OUTPUT_PP_HIGH_SLOW);

    // configure SPI for RC522
    SPI.deinit();
    SPI.init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
    delay.ms(500);
    SPI.enable();

    // soft reset
    RFID.reset();

    RFID.writeRegister(TModeReg, 0x80);      // TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    RFID.writeRegister(TPrescalerReg, 0xA9); // TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25μs.
    RFID.writeRegister(TReloadRegH, 0x03);   // Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
    RFID.writeRegister(TReloadRegL, 0xE8);

    RFID.writeRegister(TxASKReg, 0x40); // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
    RFID.writeRegister(ModeReg, 0x3D);  // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
    RFID.antennaOn();
}

void RC522_Reset(void)
{
    GPIO.writeHigh(RC522_RST);
    nop();
    GPIO.writeLow(RC522_RST);
    nop();
    GPIO.writeHigh(RC522_RST);
    nop();
    RFID.writeRegister(CommandReg, PCD_SoftReset);
    nop();
}

void RC522_AntennaOn(void)
{
    uint8_t value = RFID.readRegister(TxControlReg);

    if ((value & 0x03) != 0x03)
    {
        RFID.writeRegister(TxControlReg, value | 0x03);
    }
}

void RC522_WriteRegister(uint8_t reg, uint8_t value)
{
    GPIO.writeLow(RC522_SDA);

    SPI.sendData((reg << 1) & 0x7E);

    while (!SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    SPI.sendData(value);

    while (!SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    GPIO.writeHigh(RC522_SDA);
}

uint8_t RC522_ReadRegister(uint8_t reg)
{
    uint8_t data;

    GPIO.writeLow(RC522_SDA);
    SPI.sendData(((reg << 1) & 0x7E) | 0x80);

    while (!SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    SPI.sendData(0x00);

    while (!SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    data = SPI.receiveData();
    GPIO.writeHigh(RC522_SDA);

    return data;
}

void RC522_ClearRegisterMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = RFID.readRegister(reg);
    RFID.writeRegister(reg, tmp & (~mask));
}

void RC522_SetRegisterMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = RFID.readRegister(reg);
    RFID.writeRegister(reg, tmp | mask);
}

#define MAXRLEN 18

uint8_t RC522_Request(uint8_t reg, uint8_t *piccType)
{
    uint8_t status;
    uint8_t len;
    uint8_t buffer[MAXRLEN];

    RFID.clearRegisterMask(Status2Reg, 0x08);
    RFID.writeRegister(BitFramingReg, 0x07);
    RFID.setRegisterMask(TxControlReg, 0x03);

    buffer[0] = reg;

    status = RFID.communicateWithPICC(PCD_Transceive, buffer, 1, buffer, &len);

    if ((status == MI_OK) && (len == 0x10))
    {
        *piccType = buffer[0];
        *(piccType + 1) = buffer[1];
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

uint8_t RC522_Anticoll(uint8_t *piccType)
{
    uint8_t status;
    uint8_t i, snr_check = 0;
    uint8_t len;
    uint8_t buffer[MAXRLEN];

    RFID.clearRegisterMask(Status2Reg, 0x08);
    RFID.writeRegister(BitFramingReg, 0x00);
    RFID.clearRegisterMask(CollReg, 0x80);

    buffer[0] = PICC_CMD_SEL_CL1;
    buffer[1] = 0x20;

    status = RFID.communicateWithPICC(PCD_Transceive, buffer, 2, buffer, &len);

    if (status == MI_OK)
    {
        for (i = 0; i < 4; i++)
        {
            *(piccType + i) = buffer[i];
            snr_check ^= buffer[i];
        }
        if (snr_check != buffer[i])
        {
            status = MI_ERR;
        }
    }

    RFID.setRegisterMask(CollReg, 0x80);
    return status;
}

uint8_t RC522_Select(uint8_t *piccType)
{
    uint8_t status;
    uint8_t i;
    uint8_t len;
    uint8_t buffer[MAXRLEN];

    buffer[0] = PICC_CMD_SEL_CL1;
    buffer[1] = 0x70;
    buffer[6] = 0;

    for (i = 0; i < 4; i++)
    {
        buffer[i + 2] = *(piccType + i);
        buffer[6] ^= *(piccType + i);
    }
    RFID.calculateCRC(buffer, 7, &buffer[7]);

    RFID.clearRegisterMask(Status2Reg, 0x08);

    status = RFID.communicateWithPICC(PCD_Transceive, buffer, 9, buffer, &len);

    if ((status == MI_OK) && (len == 0x18))
    {
        status = MI_OK;
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

uint8_t RC522_HaltA()
{
    uint8_t status;
    uint8_t len;
    uint8_t buffer[MAXRLEN];

    buffer[0] = PICC_CMD_HLTA;
    buffer[1] = 0;
    RFID.calculateCRC(buffer, 2, &buffer[2]);

    status = RFID.communicateWithPICC(PCD_Transceive, buffer, 4, buffer, &len);

    return MI_OK;
}

uint8_t RC522_Read(uint8_t blockAddr, uint8_t *data)
{
    uint8_t status;
    uint8_t len;
    uint8_t i, buffer[MAXRLEN];

    buffer[0] = PICC_CMD_MF_READ;
    buffer[1] = blockAddr;
    RFID.calculateCRC(buffer, 2, &buffer[2]);

    status = RFID.communicateWithPICC(PCD_Transceive, buffer, 4, buffer, &len);
    if ((status == MI_OK) && (len == 0x90))
    //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i = 0; i < 16; i++)
        {
            *(data + i) = buffer[i];
        }
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

void RC522_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result)
{
    uint8_t i, n;
    RFID.clearRegisterMask(DivIrqReg, 0x04);
    RFID.writeRegister(CommandReg, PCD_Idle);
    RFID.setRegisterMask(FIFOLevelReg, 0x80);

    for (i = 0; i < length; i++)
    {
        RFID.writeRegister(FIFODataReg, *(data + i));
    }
    RFID.writeRegister(CommandReg, PCD_CalcCRC);
    i = 0xFF;
    do
    {
        n = RFID.readRegister(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));
    result[0] = RFID.readRegister(CRCResultRegL);
    result[1] = RFID.readRegister(CRCResultRegH);
}

uint8_t RC522_CommunicateWithPICC(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitFor = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint8_t i;
    switch (command)
    {
    case PCD_MFAuth:
        irqEn = 0x12;
        waitFor = 0x10;
        break;
    case PCD_Transceive:
        irqEn = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }

    RFID.writeRegister(ComIEnReg, irqEn | 0x80);
    RFID.clearRegisterMask(ComIrqReg, 0x80);
    RFID.writeRegister(CommandReg, PCD_Idle);
    RFID.setRegisterMask(FIFOLevelReg, 0x80);

    for (i = 0; i < sendLen; i++)
    {
        RFID.writeRegister(FIFODataReg, sendData[i]);
    }
    RFID.writeRegister(CommandReg, command);

    if (command == PCD_Transceive)
    {
        RFID.setRegisterMask(BitFramingReg, 0x80);
    }

    i = 600;
    do
    {
        n = RFID.readRegister(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));
    RFID.clearRegisterMask(BitFramingReg, 0x80);

    if (i != 0)
    {
        if (!(RFID.readRegister(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (command == PCD_Transceive)
            {
                n = RFID.readRegister(FIFOLevelReg);
                lastBits = RFID.readRegister(ControlReg) & 0x07;
                if (lastBits)
                {
                    *backLen = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *backLen = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    backData[i] = RFID.readRegister(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

    RFID.clearRegisterMask(ControlReg, 0x80);
    RFID.writeRegister(CommandReg, PCD_Idle);
    return status;
}