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
    .writeRegisterMany = RC522_WriteRegisterMany,
    .readRegister = RC522_ReadRegister,
    .readRegisterMany = RC522_ReadRegisterMany,
    .clearRegisterMask = RC522_ClearRegisterMask,
    .setRegisterMask = RC522_SetRegisterMask,
    .select = RC522_Select,
    .readCardSerial = RC522_ReadCardSerial,
    .haltA = RC522_HaltA,
    .calculateCRC = RC522_CalculateCRC,
    .transceiveData = RC522_TransceiveData,
    .communicateWithPICC = RC522_CommunicateWithPICC,
};

RC522_UID uid;

void RC522_Init(void)
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

    SPI.sendData((reg << 1));

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    SPI.sendData(value);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    GPIO.writeHigh(RC522_SDA);
}

void RC522_WriteRegisterMany(uint8_t reg, uint8_t count, uint8_t *values)
{
    GPIO.writeLow(RC522_SDA);

    SPI.sendData((reg << 1));

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;
    for (uint8_t i = 0; i < count; i++)
    {
        SPI.sendData(values[i]);
        while (SPI.getFlag(SPI_FLAG_TXE))
            ;
        while (!SPI.getFlag(SPI_FLAG_RXNE))
            ;
    }

    GPIO.writeHigh(RC522_SDA);
}

uint8_t RC522_ReadRegister(uint8_t reg)
{
    uint8_t data;

    GPIO.writeLow(RC522_SDA);
    SPI.sendData((reg << 1) | 0x80);

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

void RC522_ReadRegisterMany(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign)
{
    uint8_t index = 0;

    if (!count)
    {
        return;
    }

    GPIO.writeLow(RC522_SDA);
    count--;
    SPI.sendData((reg << 1) | 0x80);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    if (rxAlign)
    { // Only update bit positions rxAlign..7 in values[0]
        // Create bit mask for bit positions rxAlign..7
        uint8_t mask = (0xFF << rxAlign) & 0xFF;
        // Read value and tell that we want to read the same address again.
        SPI.sendData((reg << 1) | 0x80);

        while (SPI.getFlag(SPI_FLAG_TXE))
            ;
        while (!SPI.getFlag(SPI_FLAG_RXNE))
            ;
        uint8_t value = SPI.receiveData();
        // Apply mask to both current value of values[0] and the new data in value.
        values[0] = (values[0] & (~mask)) | (value & mask);
        index++;
    }

    while (index < count)
    {
        SPI.sendData((reg << 1) | 0x80);

        while (SPI.getFlag(SPI_FLAG_TXE))
            ;
        while (!SPI.getFlag(SPI_FLAG_RXNE))
            ;
        values[index] = SPI.receiveData(); // Read value and tell that we want to read the same address again.
        index++;
    }
    SPI.sendData(0);

    while (SPI.getFlag(SPI_FLAG_TXE))
        ;
    while (!SPI.getFlag(SPI_FLAG_RXNE))
        ;

    values[index] = SPI.receiveData(); // Read the final byte. Send 0 to stop reading.

    GPIO.writeHigh(RC522_SDA);
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

RC522_StatusCode RC522_Select(RC522_UID *uid)
{
    int8_t uidComplete, selectDone;
    uint8_t cascadeLevel = 1;
    uint8_t result;
    uint8_t count;
    uint8_t checkBit;
    uint8_t index;
    uint8_t uidIndex;             // The first index in uid->uidByte[] that is used in the current Cascade Level.
    int8_t currentLevelKnownBits; // The number of known UID bits in the current Cascade Level.
    uint8_t buffer[9];            // The SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A
    uint8_t bufferUsed;           // The number of bytes used in the buffer, ie the number of bytes to transfer to the FIFO.
    uint8_t rxAlign;              // Used in BitFramingReg. Defines the bit position for the first bit received.
    uint8_t txLastBits;           // Used in BitFramingReg. The number of valid bits in the last transmitted byte.
    uint8_t *responseBuffer;
    uint8_t responseLength;

    // Description of buffer structure:
    // Byte 0: SEL Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3
    // Byte 1: NVB Number of Valid Bits (in complete command, not just the UID): High nibble: complete bytes, Low nibble: Extra bits.
    // Byte 2: UID-data or CT See explanation below. CT means Cascade Tag.
    // Byte 3: UID-data
    // Byte 4: UID-data
    // Byte 5: UID-data
    // Byte 6: BCC Block Check Character - XOR of bytes 2-5
    // Byte 7: CRC_A
    // Byte 8: CRC_A
    // The BCC and CRC_A are only transmitted if we know all the UID bits of the current Cascade Level.
    //
    // Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
    // UID size	Cascade level	Byte2	Byte3	Byte4	Byte5
    // ========	=============	=====	=====	=====	=====
    // 4 bytes 1 uid0	uid1	uid2	uid3
    // 7 bytes 1 CT uid0	uid1	uid2
    // 2 uid3	uid4	uid5	uid6
    // 10 bytes 1 CT uid0	uid1	uid2
    // 2 CT uid3	uid4	uid5
    // 3 uid6	uid7	uid8	uid9

    // Prepare MFRC522
    RFID.clearRegisterMask(CollReg, 0x80);

    uidComplete = 0;

    do
    {
        switch (cascadeLevel)
        {
        case 1:
            buffer[0] = PICC_CMD_SEL_CL1;
            uidIndex = 0;
            break;
        case 2:
            buffer[0] = PICC_CMD_SEL_CL2;
            uidIndex = 3;
            break;
        case 3:
            buffer[0] = PICC_CMD_SEL_CL3;
            uidIndex = 6;
            break;

        default:
            return STATUS_INTERNAL_ERROR;
        }

        // How many UID bits are known in this Cascade Level?
        currentLevelKnownBits = 0;

        // Copy the known bits from uid->uidByte[] to buffer[]
        uint8_t bytesToCopy = 0;

        // Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
        selectDone = 0;

        do
        {
            if (currentLevelKnownBits >= 32)
            {                      // All UID bits in this Cascade Level are known. This is a SELECT.
                buffer[1] = 0x70u; // NVB - Number of Valid Bits: Seven whole bytes
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
            rxAlign = txLastBits; // Having a separate variable is overkill. But it makes the next line easier to read.
            // RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
            RFID.writeRegister(BitFramingReg, (rxAlign << 4) + txLastBits);

            // Transmit the buffer and receive the response.
            result = RFID.transceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign, 0);
            if (result == STATUS_COLLISION)
            { // More than one PICC in the field => collision.
                // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]
                uint8_t valueOfCollReg = RFID.readRegister(CollReg);
                if (valueOfCollReg & 0x20)
                {                            // CollPosNotValid
                    return STATUS_COLLISION; // Without a valid collision position we cannot continue
                }
                int8_t collisionPos = (int8_t)(valueOfCollReg & 0x1F); // Values 0-31, 0 means bit 32.
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
                buffer[index] |= (1u << checkBit);
            }
            else if (result != STATUS_OK)
            {
                return result;
            }
            else
            { // STATUS_OK
                if (currentLevelKnownBits >= 32)
                {                   // This was a SELECT.
                    selectDone = 1; // No more anticollision
                    // We continue below outside the while.
                }
                else
                { // This was an ANTICOLLISION.
                    // We now have all 32 bits of the UID in this Cascade Level
                    currentLevelKnownBits = 32;
                    // Run loop again to do the SELECT.
                }
            }
        } while (!selectDone);
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
        if (responseBuffer[0] & 0x04u)
        { // Cascade bit set - UID not complete yes
            cascadeLevel++;
        }
        else
        {
            uidComplete = 1;
            // uid->sak = responseBuffer[0]; let's save some bytes
        }
    } while (!uidComplete);

    uid->size = 3 * cascadeLevel + 1;

    return STATUS_OK;
}

RC522_StatusCode RC522_ReadCardSerial()
{
    RC522_StatusCode result = RFID.select(&uid);
    return (result == STATUS_OK);
}

RC522_StatusCode RC522_HaltA()
{
    uint8_t result;
    uint8_t buffer[4];

    // Build command buffer
    buffer[0] = PICC_CMD_HLTA;
    buffer[1] = 0;
    // Calculate CRC_A
    result = RFID.calculateCRC(buffer, 2, &buffer[2]);
    if (result != STATUS_OK)
    {
        return result;
    }

    // Send the command.
    // The standard says:
    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    // We interpret that this way: Only STATUS_TIMEOUT is a success.
    result = RFID.transceiveData(buffer, sizeof(buffer), 0, 0, 0, 0, 0);
    if (result == STATUS_TIMEOUT)
    {
        return STATUS_OK;
    }
    if (result == STATUS_OK)
    { // That is ironically NOT ok in this case ;-)
        return STATUS_ERROR;
    }
    return result;
}

RC522_StatusCode RC522_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result)
{
    RFID.writeRegister(CommandReg, PCD_Idle);          // Stop any active command.
    RFID.writeRegister(DivIrqReg, 0x04);               // Clear the CRCIRq interrupt request bit
    RFID.writeRegister(FIFOLevelReg, 0x80);            // FlushBuffer = 1, FIFO initialization
    RFID.writeRegisterMany(FIFODataReg, length, data); // Write data to the FIFO
    RFID.writeRegister(CommandReg, PCD_CalcCRC);       // Start the calculation

    // Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73μs.

    for (uint16_t i = 5000; i > 0; i--)
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
    }
    // 89ms passed and nothing happend. Communication with the MFRC522 might be down.
    return STATUS_TIMEOUT;
}

RC522_StatusCode RC522_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, int8_t checkCRC)
{
    return RFID.communicateWithPICC(PCD_Transceive, 0x30, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
}

RC522_StatusCode RC522_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, int8_t checkCRC)
{
    uint8_t txLastBits = validBits ? *validBits : 0;
    uint8_t bitFraming = (rxAlign << 4) + txLastBits;

    RFID.writeRegister(CommandReg, PCD_Idle);               // Stop any active command.
    RFID.writeRegister(ComIrqReg, 0x7F);                    // Clear all seven interrupt request bits
    RFID.writeRegister(FIFOLevelReg, 0x80);                 // FlushBuffer = 1, FIFO initialization
    RFID.writeRegisterMany(FIFODataReg, sendLen, sendData); // Write sendData to the FIFO
    RFID.writeRegister(BitFramingReg, bitFraming);          // Bit adjustments
    RFID.writeRegister(CommandReg, command);                // Execute the command
    if (command == PCD_Transceive)
    {
        RFID.setRegisterMask(BitFramingReg, 0x80); // StartSend=1, transmission of data starts
    }

    // Wait for the command to complete.
    // In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
    // Each iteration of the do-while-loop takes 17.86μs.

    uint16_t i;
    for (i = 20000; i > 0; i--)
    {
        uint8_t n = RFID.readRegister(ComIrqReg); // ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
        if (n & waitIRq)
        { // One of the interrupts that signal success has been set.
            break;
        }
        if (n & 0x01u)
        { // Timer interrupt - nothing received in 25ms
            return STATUS_TIMEOUT;
        }
    }
    // 35.7ms and nothing happend. Communication with the MFRC522 might be down.
    if (!i)
    {
        return STATUS_TIMEOUT;
    }

    // Stop now if any errors except collisions were detected.
    // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
    uint8_t errorRegValue = RFID.readRegister(ErrorReg);
    if (errorRegValue & 0x13)
    { // BufferOvfl ParityErr ProtocolErr
        return STATUS_ERROR;
    }

    uint8_t _validBits = 0;

    // If the caller wants data back, get it from the MFRC522.
    if (backData && backLen)
    {
        uint8_t n = RFID.readRegister(FIFOLevelReg); // Number of bytes in the FIFO

        if (n > *backLen)
        {
            return STATUS_NO_ROOM;
        }

        *backLen = n;                                             // Number of bytes returned
        RFID.readRegisterMany(FIFODataReg, n, backData, rxAlign); // Get received data from FIFO
        // RxLastBits[2:0] indicates the number of valid bits in the last received uint8_t. If this value is 000b, the whole uint8_t is valid.
        _validBits = RFID.readRegister(ControlReg) & 0x07;
        if (validBits)
        {
            *validBits = _validBits;
        }
    }

    // Tell about collisions
    if (errorRegValue & 0x08u)
    { // CollErr
        return STATUS_COLLISION;
    }

#ifndef MFRC522_DISABLE_CRC_CHECK_IN_COMMUNICATEWITHPICC
    // Perform CRC_A validation if requested.
    if (backData && backLen && checkCRC)
    {
        // In this case a MIFARE Classic NAK is not OK.
        if (*backLen == 1 && _validBits == 4)
        {
            return STATUS_MIFARE_NACK;
        }
        // We need at least the CRC_A value and all 8 bits of the last uint8_t must be received.
        if (*backLen < 2 || _validBits != 0)
        {
            return STATUS_CRC_WRONG;
        }
        // Verify CRC_A - do our own calculation and store the control in controlBuffer.
        uint8_t controlBuffer[2];
        uint8_t status = RFID.calculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
        if (status != STATUS_OK)
        {
            return status;
        }
        if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1]))
        {
            return STATUS_CRC_WRONG;
        }
    }
#endif

    return STATUS_OK;
}