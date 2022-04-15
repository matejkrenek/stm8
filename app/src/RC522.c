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
    .readRegister2 = RC522_ReadRegister2,
    .antennaOn = RC522_AntennaOn,
    .requestA = RC522_RequestA,
    .REQA_or_WUPA = RC522_REQA_or_WUPA,
    .transceiveData = RC522_TransceiveData,
    .communicateWithPICC = RC522_CommunicateWithPICC,
    .clearBitMask = RC522_ClearBitMask,
    .setBitMask = RC522_SetBitMask,
    .select = RC522_Select,
    .haltA = RC522_HaltA,
    .calculateCRC = RC522_CalculateCRC,
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

void RC522_ReadRegister2(uint8_t address, uint8_t count, uint8_t *values, uint8_t rxAlign)
{
    if (count == 0)
    {
        return;
    }
    uint8_t index = 0;
    GPIO.writeLow(RC522_SDA);
    count--;
    SPI.sendData((address << 1) | 0x80);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    if (rxAlign)
    { // Only update bit positions rxAlign..7 in values[0]
        // Create bit mask for bit positions rxAlign..7
        uint8_t mask = (0xFF << rxAlign) & 0xFF;
        // Read value and tell that we want to read the same address again.
        SPI.sendData((address << 1) | 0x80);
        while (SPI.getFlag(SPI_FLAG_TXE))
            ;
        while (!SPI.getFlag(SPI_FLAG_RXNE))
            ;
        uint8_t value = SPI.receiveData();
        // Apply mask to both current value of values[0] and the new data in value.
        values[0] = (values[0] & ~mask) | (value & mask);
        index++;
    }
    while (index < count)
    {
        SPI.sendData((address << 1) | 0x80);
        while (SPI.getFlag(SPI_FLAG_TXE))
            ;
        while (!SPI.getFlag(SPI_FLAG_RXNE))
            ;
        values[index] = SPI.receiveData();
        index++;
    }

    SPI.sendData(0x00);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    values[index] = SPI.receiveData();

    GPIO.writeHigh(RC522_SDA);
}

void RC522_AntennaOn(void)
{
    uint8_t value = RFID.readRegister(TxControlReg);

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

RC522_StatusCode RC522_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign)
{
    uint8_t txLastBits = validBits ? *validBits : 0;
    uint8_t bitFraming = (0x00 << 4) + txLastBits;

    RFID.writeRegister(CommandReg, PCD_Idle); // Stop any active command.
    RFID.writeRegister(ComIrqReg, 0x7F);      // Clear all seven interrupt request bits
    RFID.writeRegister(FIFOLevelReg, 0x80);   // FlushBuffer = 1, FIFO initialization
    for (uint8_t i = 0; i < sendLen; i++)
    {
        RFID.writeRegister(FIFODataReg, sendData[i]); // Write sendData to the FIFO
    }

    RFID.writeRegister(BitFramingReg, bitFraming); // Bit adjustments
    RFID.writeRegister(CommandReg, command);       // Execute the command

    if (command == PCD_Transceive)
    {
        RFID.setBitMask(BitFramingReg, 0x80);
    }

    const uint32_t deadline = milis.get() + 36;
    bool completed = FALSE;

    do
    {
        uint8_t interrupt_n = RFID.readRegister(ComIrqReg);

        if (interrupt_n & waitIRq)
        {
            completed = TRUE;
            break;
        }
        if (interrupt_n & 0x01)
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
        uint8_t fifo_cap_n = RFID.readRegister(FIFOLevelReg); // Number of bytes in the FIFO
        if (fifo_cap_n > *backLen)
        {
            return STATUS_NO_ROOM;
        }
        *backLen = fifo_cap_n;                                          // Number of bytes returned
        RFID.readRegister2(FIFODataReg, fifo_cap_n, backData, rxAlign); // Get received data from FIFO
        _validBits = RFID.readRegister(ControlReg) & 0x07;              // RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
        if (validBits)
        {
            *validBits = _validBits;
        }
    }

    // Tell about collisions
    if (errorRegValue & 0x08)
    { // CollErr
        return STATUS_COLLISION;
    }

    // Perform CRC_A validation if requested.
    if (backData && backLen)
    {
        // In this case a MIFARE Classic NAK is not OK.
        if (*backLen == 1 && _validBits == 4)
        {
            return STATUS_MIFARE_NACK;
        }
        // We need at least the CRC_A value and all 8 bits of the last byte must be received.
        if (*backLen < 2 || _validBits != 0)
        {
            return STATUS_CRC_WRONG;
        }
        // Verify CRC_A - do our own calculation and store the control in controlBuffer.
        uint8_t controlBuffer[2];
        RC522_StatusCode status = RFID.calculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
        if (status != STATUS_OK)
        {
            return status;
        }
        if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1]))
        {
            return STATUS_CRC_WRONG;
        }
    }

    return STATUS_OK;
}

RC522_StatusCode RC522_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign)
{
    uint8_t waitIrq = 0x30;
    return RFID.communicateWithPICC(PCD_Transceive, waitIrq, sendData, sendLen, backData, backLen, validBits, rxAlign);
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
    status = RFID.transceiveData(&command, 1, bufferATQA, bufferSize, &validBits, 0);
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

RC522_StatusCode RC522_Select(RC522_UID *uid)
{
    bool uidComplete;
    bool selectDone;
    bool useCascadeTag;
    uint8_t cascadeLevel = 1;
    RC522_StatusCode result;
    uint8_t count;
    uint8_t checkBit;
    uint8_t index;
    uint8_t uidIndex;
    uint8_t currentLevelKnownBits;
    uint8_t buffer[9];
    uint8_t bufferUsed;
    uint8_t rxAlign;
    uint8_t txLastBits;
    uint8_t *responseBuffer;
    uint8_t responseLength;
    uint8_t validBits = 0x00;

    if (validBits > 80)
    {
        return STATUS_INVALID;
    }

    RFID.clearBitMask(CollReg, 0x80);

    uidComplete = FALSE;

    while (!uidComplete)
    {
        switch (cascadeLevel)
        {
        case 1:
            buffer[0] = PICC_CMD_SEL_CL1;
            uidIndex = 0;
            useCascadeTag = validBits && uid->size > 4; // When we know that the UID has more than 4 bytes
            break;

        case 2:
            buffer[0] = PICC_CMD_SEL_CL2;
            uidIndex = 3;
            useCascadeTag = validBits && uid->size > 7; // When we know that the UID has more than 7 bytes
            break;

        case 3:
            buffer[0] = PICC_CMD_SEL_CL3;
            uidIndex = 6;
            useCascadeTag = FALSE; // Never used in CL3.
            break;

        default:
            return STATUS_INTERNAL_ERROR;
            break;
        }

        currentLevelKnownBits = validBits - (8 * uidIndex);
        if (currentLevelKnownBits < 0)
        {
            currentLevelKnownBits = 0;
        }
        // Copy the known bits from uid->uidByte[] to buffer[]
        index = 2; // destination index in buffer[]
        if (useCascadeTag)
        {
            buffer[index++] = PICC_CMD_CT;
        }
        uint8_t bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0); // The number of bytes needed to represent the known bits for this level.
        if (bytesToCopy)
        {
            uint8_t maxBytes = useCascadeTag ? 3 : 4; // Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag
            if (bytesToCopy > maxBytes)
            {
                bytesToCopy = maxBytes;
            }
            for (count = 0; count < bytesToCopy; count++)
            {
                buffer[index++] = uid->uidByte[uidIndex + count];
            }
        }
        // Now that the data has been copied we need to include the 8 bits in CT in currentLevelKnownBits
        if (useCascadeTag)
        {
            currentLevelKnownBits += 8;
        }

        selectDone = FALSE;
        while (!selectDone)
        {
            if (currentLevelKnownBits >= 32)
            { // All UID bits in this Cascade Level are known. This is a SELECT.
                // Serial.print(F("SELECT: currentLevelKnownBits=")); Serial.println(currentLevelKnownBits, DEC);
                buffer[1] = 0x70; // NVB - Number of Valid Bits: Seven whole bytes
                // Calculate BCC - Block Check Character
                buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
                // Calculate CRC_A
                result = RFID.calculateCRC(buffer, 7, &buffer[7]);
                if (result != STATUS_OK)
                {
                    return result;
                }
                txLastBits = 0; // 0 => All 8 bits are valid.
                bufferUsed = 9;
                // Store response in the last 3 bytes of buffer (BCC and CRC_A - not needed after tx)
                responseBuffer = &buffer[6];
                responseLength = 3;
            }
            else
            { // This is an ANTICOLLISION.
                // Serial.print(F("ANTICOLLISION: currentLevelKnownBits=")); Serial.println(currentLevelKnownBits, DEC);
                txLastBits = currentLevelKnownBits % 8;
                count = currentLevelKnownBits / 8;     // Number of whole bytes in the UID part.
                index = 2 + count;                     // Number of whole bytes: SEL + NVB + UIDs
                buffer[1] = (index << 4) + txLastBits; // NVB - Number of Valid Bits
                bufferUsed = index + (txLastBits ? 1 : 0);
                // Store response in the unused part of buffer
                responseBuffer = &buffer[index];
                responseLength = sizeof(buffer) - index;
            }

            // Set bit adjustments
            rxAlign = txLastBits;                                           // Having a separate variable is overkill. But it makes the next line easier to read.
            RFID.writeRegister(BitFramingReg, (rxAlign << 4) + txLastBits); // RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]

            result = RFID.transceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign);
            if (result == STATUS_COLLISION)
            {                                                        // More than one PICC in the field => collision.
                uint8_t valueOfCollReg = RFID.readRegister(CollReg); // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]
                if (valueOfCollReg & 0x20)
                {                            // CollPosNotValid
                    return STATUS_COLLISION; // Without a valid collision position we cannot continue
                }
                uint8_t collisionPos = valueOfCollReg & 0x1F; // Values 0-31, 0 means bit 32.
                if (collisionPos == 0)
                {
                    collisionPos = 32;
                }
                if (collisionPos <= currentLevelKnownBits)
                { // No progress - should not happen
                    return STATUS_INTERNAL_ERROR;
                }
                // Choose the PICC with the bit set.
                currentLevelKnownBits = collisionPos;
                count = currentLevelKnownBits % 8; // The bit to modify
                checkBit = (currentLevelKnownBits - 1) % 8;
                index = 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First byte is index 0.
                buffer[index] |= (1 << checkBit);
            }
            else if (result != STATUS_OK)
            {
                return result;
            }
            else
            { // STATUS_OK
                if (currentLevelKnownBits >= 32)
                {                      // This was a SELECT.
                    selectDone = TRUE; // No more anticollision
                                       // We continue below outside the while.
                }
                else
                { // This was an ANTICOLLISION.
                    // We now have all 32 bits of the UID in this Cascade Level
                    currentLevelKnownBits = 32;
                    // Run loop again to do the SELECT.
                }
            }
        }
        // We do not check the CBB - it was constructed by us above.

        // Copy the found UID bytes from buffer[] to uid->uidByte[]
        index = (buffer[2] == PICC_CMD_CT) ? 3 : 2; // source index in buffer[]
        bytesToCopy = (buffer[2] == PICC_CMD_CT) ? 3 : 4;
        for (count = 0; count < bytesToCopy; count++)
        {
            uid->uidByte[uidIndex + count] = buffer[index++];
        }

        // Check response SAK (Select Acknowledge)
        if (responseLength != 3 || txLastBits != 0)
        { // SAK must be exactly 24 bits (1 byte + CRC_A).
            return STATUS_ERROR;
        }
        // Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those bytes are not needed anymore.
        result = RFID.calculateCRC(responseBuffer, 1, &buffer[2]);
        if (result != STATUS_OK)
        {
            return result;
        }
        if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2]))
        {
            return STATUS_CRC_WRONG;
        }
        if (responseBuffer[0] & 0x04)
        { // Cascade bit set - UID not complete yes
            cascadeLevel++;
        }
        else
        {
            uidComplete = TRUE;
            uid->sak = responseBuffer[0];
        }
    }

    uid->size = 3 * cascadeLevel + 1;

    return STATUS_OK;
}

RC522_StatusCode RC522_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result)
{
    RFID.writeRegister(CommandReg, PCD_Idle);      // Stop any active command.
    RFID.writeRegister(DivIrqReg, 0x04);           // Clear the CRCIRq interrupt request bit
    RFID.writeRegister(FIFOLevelReg, 0x80);        // FlushBuffer = 1, FIFO initialization
    RFID.writeRegister(FIFODataReg, length, data); // Write data to the FIFO
    RFID.writeRegister(CommandReg, PCD_CalcCRC);   // Start the calculation

    const uint32_t deadline = milis.get() + 89;

    do
    {
        // DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
        uint8_t n = RFID.readRegister(DivIrqReg);
        if (n & 0x04)
        {                                             // CRCIRq bit set - calculation done
            RFID.writeRegister(CommandReg, PCD_Idle); // Stop calculating CRC for new content in the FIFO.
            // Transfer the result from the registers to the result buffer
            result[0] = RFID.readRegister(CRCResultRegL);
            result[1] = RFID.readRegister(CRCResultRegH);
            return STATUS_OK;
        }

    } while (milis.get() < deadline);

    return STATUS_TIMEOUT;
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