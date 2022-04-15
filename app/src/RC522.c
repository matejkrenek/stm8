#include "stm8s.h"
#include "RC522.h"
#include "milis.h"

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
    .requestA = RC522_RequestA,
    .REQA_or_WUPA = RC522_REQA_or_WUPA,
    .transceiveData = RC522_TransceiveData,
    .communicateWithPICC = RC522_CommunicateWithPICC,
    .clearBitMast = RC522_ClearBitMask,
    .setBitMask = RC522_SetBitMask,
    .isNewCardPresent = RC522_IsNewCardPresent,
    .readCardSerial = RC522_ReadCardSerial,
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

RC522_StatusCode RC522_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits)
{
    uint8_t txLastBits = validBits ? *validBits : 0;
    uint8_t bitFraming = (0x00 << 4) + txLastBits;

    RFID.writeRegister(CommandReg, PCD_Idle);           // Stop any active command.
    RFID.writeRegister(ComIrqReg, 0x7F);                // Clear all seven interrupt request bits
    RFID.writeRegister(FIFOLevelReg, 0x80);             // FlushBuffer = 1, FIFO initialization
    RFID.writeRegister(FIFODataReg, sendLen, sendData); // Write sendData to the FIFO
    RFID.writeRegister(BitFramingReg, bitFraming);      // Bit adjustments
    RFID.writeRegister(CommandReg, command);            // Execute the command

    if (command == PCD_Transceive)
    {
        RFID.setBitMask(BitFramingReg, 0x80);
    }

    const uint32_t deadline = milis.get() + 36;
    bool completed = FALSE;

    do
    {
        uint8_t n = RFID.readRegister(ComIrqReg);

        if (n & waitIRq)
        {
            completed = TRUE;
            break;
        }
        if (n & 0x01)
        {
            return STATUS_TIMEOUT;
        }

    } while (milis.get() < deadline);

    if (!completed)
    {
        return STATUS_TIMEOUT;
    }

    uint8_t errorRegValue = RFID.readRegister(ErrorReg);

    if (errorRegValue & 0x13)
    {
        return STATUS_ERROR;
    }

    uint8_t _validBits = 0;

    if (backData && backLen)
    {
        uint8_t n = PCD_ReadRegister(FIFOLevelReg); // Number of bytes in the FIFO
        if (n > *backLen)
        {
            return STATUS_NO_ROOM;
        }
        *backLen = n;                                        // Number of bytes returned
        PCD_ReadRegister(FIFODataReg, n, backData, rxAlign); // Get received data from FIFO
        _validBits = PCD_ReadRegister(ControlReg) & 0x07;    // RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
        if (validBits)
        {
            *validBits = _validBits;
        }
    }
}

RC522_StatusCode RC522_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t backLen, uint8_t *validBits)
{
    uint8_t waitIrq = 0x30;
    return RFID.communicateWithPICC(PCD_Transceive, waitIrq, sendData, sendLen, backData, backLen, validBits);
}

RC522_StatusCode RC522_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize)
{
    return RFID.REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
}

RC522_StatusCode RC522_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize)
{
    uint8_t validBits;
    RC522_StatusCode status;

    if (bufferATQA == NULL || *bufferSize < 2)
    {
        return STATUS_NO_ROOM;
    }

    RFID.clearBitMask(CollReg, 0x80);
    validBits = 7;
    status = RFID.transceiveData(&command, 1, bufferATQA, bufferSize, &validBits);
    if (status != STATUS_OK)
    {
        return status;
    }

    if (*bufferSize != 2 || validBits != 0)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

RC522_StatusCode RC522_IsNewCardPresent()
{
    uint8_t bufferATQA[2];
    uint8_t bufferSize = sizeof(bufferATQA);

    RFID.writeRegister(TxModeReg, 0x00);
    RFID.writeRegister(RxModeReg, 0x00);
    RFID.writeRegister(ModWidthReg, 0x26);

    RC522_StatusCode result = RFID.requestA(bufferATQA, &bufferSize);

    return (result == STATUS_OK || result == STATUS_COLLISION);
}

RC522_StatusCode RC522_ReadCardSerial()
{
    RC522_StatusCode result = RFID.select(&uid);
    return (result == STATUS_OK);
}