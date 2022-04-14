#include "stm8s.h"
#include "RC522.h"

Pin *RC522_SDA;
Pin *RC522_SCK;
Pin *RC522_MOSI;
Pin *RC522_MISO;
Pin *RC522_RST;

const RC522_Module RFID = {
    .init = RC522_Init,
    .reset = RC522_Reset,
    .writeRegister = RC522_WriteRegister,
    .readRegister = RC522_ReadRegister,
    .antennaOn = RC522_AntennaOn,
    .request = RC522_Request,
    .communicateWithPICC = RC522_CommunicateWithPICC,
    .clearBitMast = RC522_ClearBitMask,
    .setBitMask = RC522_SetBitMask,
};

void RC522_PCD_Init(void)
{
    // configure pinouts
    RC522_SDA = GPIO.init(GPIOC, PIN_4, OUTPUT_PP_HIGH_SLOW);
    RC522_SCK = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_HIGH_SLOW);
    RC522_MOSI = GPIO.init(GPIOC, PIN_6, OUTPUT_PP_HIGH_SLOW);
    RC522_MISO = GPIO.init(GPIOC, PIN_7, INPUT_PU_NO_IT);
    RC522_RST = GPIO.init(GPIOC, PIN_3, OUTPUT_PP_HIGH_SLOW);

    // enable peripheral SPI clock
    CLK.peripheralEnable(CLK_PERIPHERAL_SPI);

    // configure SPI for RC522
    SPI.deinit();
    SPI.init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_4, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
    delay.ms(500);
    SPI.enable();

    // soft reset
    RFID.reset();

    // reset baud rates
    RFID.writeRegister(TxModeReg, 0x00);
    RFID.writeRegister(RxModeReg, 0x00);
    RFID.writeRegister(ModWidthReg, 0x26);

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
    RFID.writeRegister(CommandReg, PCD_SoftReset);

    uint8_t count = 0;
    do
    {
        delay.ms(50);
    } while ((RFID.readRegister(CommandReg) & (1 << 4)) && (++count) < 3);
}

void RC522_WriteRegister(uint8_t address, uint8_t data)
{
    GPIO.writeLow(RC522_SDA);

    SPI.sendData(address << 1);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    SPI.sendData(data);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    GPIO.writeHigh(RC522_SDA);
}

uint8_t RC522_ReadRegister(uint8_t address)
{
    uint8_t data;

    GPIO.writeLow(RC522_SDA);
    SPI.sendData((address << 1) | 0x80);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    SPI.sendData(0x00);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    data = SPI.receiveData();
    GPIO.writeHigh(RC522_SDA);

    return data;
}

void RC522_AntennaOn(void)
{
    uint8_t value = RFID.readReagister(TxControlReg);

    if ((value & 0x03) != 0x03)
    {
        RFID.writeRegister(TxControlReg, value | 0x03);
    }
}

#define MAXRLEN 18

char RC522_Request(uint8_t *buffer, uint8_t *bufferSize)
{
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ClearBitMask(Status2Reg, 0x08);
    WriteRawRC(BitFramingReg, 0x07);
    SetBitMask(TxControlReg, 0x03);

    ucComMF522Buf[0] = req_code;

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x10))
    {
        *pTagType = ucComMF522Buf[0];
        *(pTagType + 1) = ucComMF522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

void RC522_SetBitMask(uint8_t address, uint8_t mask)
{
    uint8_t tmp;
    tmp = RFID.readRegister(address);
    RFID.writeRegister(address, tmp | mask);
}

void RC522_ClearBitMask(uint8_t address, uint8_t mask)
{
    uint8_t tmp;
    tmp = RFID.readRegister(address);
    RFID.writeRegister(address, tmp & (~mask));
}

bool RC522_CommunicateWithPICC(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen)
{
    bool status = MI_ERR;
    uint8_t waitFor = (command == PCD_MFAuth ? 0x10 : 0x30);
    uint8_t lastBits;
    uint8_t n;
    uint8_t i;

    RFID.writeRegister(CommandReg, PCD_Idle);
    RFID.writeRegister(ComIrqReg, 0x7F);
    RFID.writeRegister(FIFOLevelReg, 0x80);
    for (i = 0; i < sendLen; i++)
    {
        RFID.writeRegister(FIFODataReg, sendData[i]);
    }
    RFID.writeRegister(CommandReg, command);

    if (command == PCD_Transceive)
    {
        RFID.setBitMask(BitFramingReg, 0x80);
    }

    i = 600;
    do
    {
        n = RFID.readRegister(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));

    if (i != 0)
    {
        if (!(RFID.readRegister(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if (n & 0x01)
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

    RFID.setBitMask(ControlReg, 0x80);
    RFID.writeRegister(CommandReg, PCD_Idle);
    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:???
//????: pSnr[OUT]:?????,4??
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i, snr_check = 0;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ClearBitMask(Status2Reg, 0x08);
    WriteRawRC(BitFramingReg, 0x00);
    ClearBitMask(CollReg, 0x80);

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);

    if (status == MI_OK)
    {
        for (i = 0; i < 4; i++)
        {
            *(pSnr + i) = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i])
        {
            status = MI_ERR;
        }
    }

    SetBitMask(CollReg, 0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:????
//????: pSnr[IN]:?????,4??
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i = 0; i < 4; i++)
    {
        ucComMF522Buf[i + 2] = *(pSnr + i);
        ucComMF522Buf[6] ^= *(pSnr + i);
    }
    CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);

    ClearBitMask(Status2Reg, 0x08);

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {
        status = MI_OK;
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:??????
//????: auth_mode[IN]: ??????
//                 0x60 = ??A??
//                 0x61 = ??B??
//          addr[IN]:???
//          pKey[IN]:??
//          pSnr[IN]:?????,4??
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(unsigned char auth_mode, unsigned char addr, unsigned char *pKey, unsigned char *pSnr)
{
    char status;
    unsigned int unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i = 0; i < 6; i++)
    {
        ucComMF522Buf[i + 2] = *(pKey + i);
    }
    for (i = 0; i < 6; i++)
    {
        ucComMF522Buf[i + 8] = *(pSnr + i);
    }
    //   memcpy(&ucComMF522Buf[2], pKey, 6);
    //   memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:??M1?????
//????: addr[IN]:???
//          pData[OUT]:?????,16??
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(unsigned char addr, unsigned char *pData)
{
    char status;
    unsigned int unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i = 0; i < 16; i++)
        {
            *(pData + i) = ucComMF522Buf[i];
        }
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:????M1???
//????: addr[IN]:???
//          pData[IN]:?????,16??
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(unsigned char addr, unsigned char *pData)
{
    char status;
    unsigned int unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        // memcpy(ucComMF522Buf, pData, 16);
        for (i = 0; i < 16; i++)
        {
            ucComMF522Buf[i] = *(pData + i);
        }
        CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:?????
//????: dd_mode[IN]:???
//               0xC0 = ??
//               0xC1 = ??
//          addr[IN]:????
//          pValue[IN]:4???(?)?,????
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdValue(unsigned char dd_mode, unsigned char addr, unsigned char *pValue)
{
    char status;
    unsigned int unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        // memcpy(ucComMF522Buf, pValue, 4);
        for (i = 0; i < 16; i++)
        {
            ucComMF522Buf[i] = *(pValue + i);
        }
        CalulateCRC(ucComMF522Buf, 4, &ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

        status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:????
//????: sourceaddr[IN]:???
//          goaladdr[IN]:????
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf, 4, &ucComMF522Buf[4]);

        status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status != MI_OK)
    {
        return MI_ERR;
    }

    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//?    ?:??????????
//?    ?: ????MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//?MF522??CRC16??
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
    unsigned char i, n;
    ClearBitMask(DivIrqReg, 0x04);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg, 0x80);
    for (i = 0; i < len; i++)
    {
        WriteRawRC(FIFODataReg, *(pIndata + i));
    }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}
