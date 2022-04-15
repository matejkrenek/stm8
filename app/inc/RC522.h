#ifndef __RC522_H
#define __RC522_H

#define FIFO_SIZE 64 // FIFO size=64byte

// Page 0: Command and status
#define CommandReg 0x01    // starts and stops command execution
#define ComIEnReg 0x02     // enable and disable interrupt request control bits
#define DivIEnReg 0x03     // enable and disable interrupt request control bits
#define ComIrqReg 0x04     // interrupt request bits
#define DivIrqReg 0x05     // interrupt request bits
#define ErrorReg 0x06      // error bits showing the error status of the last command executed
#define Status1Reg 0x07    // communication status bits
#define Status2Reg 0x08    // receiver and transmitter status bits
#define FIFODataReg 0x09   // input and output of 64 byte FIFO buffer
#define FIFOLevelReg 0x0A  // number of bytes stored in the FIFO buffer
#define WaterLevelReg 0x0B // level for FIFO underflow and overflow warning
#define ControlReg 0x0C    // miscellaneous control registers
#define BitFramingReg 0x0D // adjustments for bit-oriented frames
#define CollReg 0x0E       // bit position of the first bit-collision detected on the RF interface

// Page 1: Command
#define ModeReg 0x11        // defines general modes for transmitting and receiving
#define TxModeReg 0x12      // defines transmission data rate and framing
#define RxModeReg 0x13      // defines reception data rate and framing
#define TxControlReg 0x14   // controls the logical behavior of the antenna driver pins TX1 and TX2
#define TxASKReg 0x15       // controls the setting of the transmission modulation
#define TxSelReg 0x16       // selects the internal sources for the antenna driver
#define RxSelReg 0x17       // selects internal receiver settings
#define RxThresholdReg 0x18 // selects thresholds for the bit decoder
#define DemodReg 0x19       // defines demodulator settings
#define MfTxReg 0x1C        // controls some MIFARE communication transmit parameters
#define MfRxReg 0x1D        // controls some MIFARE communication receive parameters
#define SerialSpeedReg 0x1F // selects the speed of the serial UART interface

// Page 2: Configuration
#define CRCResultRegH 0x21     // shows the MSB and LSB values of the CRC calculation
#define CRCResultRegL 0x22     // shows the MSB and LSB values of the CRC calculation
#define ModWidthReg 0x24       // controls the ModWidth setting?
#define RFCfgReg 0x26          // configures the receiver gain
#define GsNReg 0x27            // selects the conductance of the antenna driver pins TX1 and TX2 for modulation
#define CWGsPReg 0x28          // defines the conductance of the p-driver output during periods of no modulation
#define ModGsPReg 0x29         // defines the conductance of the p-driver output during periods of modulation
#define TModeReg 0x2A          // defines settings for the internal timer
#define TPrescalerReg 0x2B     // the lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg.
#define TReloadRegH 0x2C       // defines the 16-bit timer reload value
#define TReloadRegL 0x2D       // defines the 16-bit timer reload value
#define TCounterValueRegH 0x2E // shows the 16-bit timer value
#define TCounterValueRegL 0x2F // shows the 16-bit timer value

// Page 3: Test Registers
#define TestSel1Reg 0x31     // general test signal configuration
#define TestSel2Reg 0x32     // general test signal configuration
#define TestPinEnReg 0x33    // enables pin output driver on pins D1 to D7
#define TestPinValueReg 0x34 // defines the values for D1 to D7 when it is used as an I/O bus
#define TestBusReg 0x35      // shows the status of the internal test bus
#define AutoTestReg 0x36     // controls the digital self-test
#define VersionReg 0x37      // shows the software version
#define AnalogTestReg 0x38   // controls the pins AUX1 and AUX2
#define TestDAC1Reg 0x39     // defines the test value for TestDAC1
#define TestDAC2Reg 0x3A     // defines the test value for TestDAC2
#define TestADCReg 0x3B      // shows the value of ADC I and Q channels

// PCD commands.
#define PCD_Idle 0x00       // no action, cancels current command execution
#define PCD_CalcCRC 0x03    // activates the CRC coprocessor or performs a self-test
#define PCD_Transmit 0x04   // transmits data from the FIFO buffer
#define PCD_Receive 0x08    // activates the receiver circuits
#define PCD_Transceive 0x0C // transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define PCD_MFAuth 0x0E     // performs the MIFARE standard authentication as a reader
#define PCD_SoftReset 0x0F  // resets the MFRC522

// Commands sent to the PICC.
#define PICC_CMD_REQA 0x26 // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
#define PICC_CMD_WUPA 0x52 // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
#define PICC_CMD_CT 0x88
#define PICC_CMD_SEL_CL1 0x93 // Anti collision/Select, Cascade Level 1
#define PICC_CMD_SEL_CL2 0x95 // Anti collision/Select, Cascade Level 2
#define PICC_CMD_SEL_CL3 0x97 // Anti collision/Select, Cascade Level 3
#define PICC_CMD_HLTA 0x50    // HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.

#define PICC_CMD_MF_AUTH_KEY_A 0x60 // Perform authentication with Key A
#define PICC_CMD_MF_AUTH_KEY_B 0x61 // Perform authentication with Key B
#define PICC_CMD_MF_READ 0x30       // Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
#define PICC_CMD_MF_WRITE 0xA0      // Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
#define PICC_CMD_MF_DECREMENT 0xC0  // Decrements the contents of a block and stores the result in the internal data register.
#define PICC_CMD_MF_INCREMENT 0xC1  // Increments the contents of a block and stores the result in the internal data register.
#define PICC_CMD_MF_RESTORE 0xC2    // Reads the contents of a block into the internal data register.
#define PICC_CMD_MF_TRANSFER 0xB0   // Writes the contents of the internal data register to a block.

// Status
#define MI_OK 0
#define MI_NOTAGERR (-1)
#define MI_ERR (-2)

typedef struct
{
    uint8_t size;
    uint8_t uidByte[10];
    uint8_t sak;
} RC522_UID;

typedef enum
{
    STATUS_OK,
    STATUS_ERROR,
    STATUS_COLLISION,
    STATUS_TIMEOUT,
    STATUS_NO_ROOM,
    STATUS_INTERNAL_ERROR,
    STATUS_INVALID,
    STATUS_CRC_WRONG,
    STATUS_MIFARE_NACK = 0xff
} RC522_StatusCode;

typedef struct
{
    void (*init)();
    void (*reset)();
    void (*writeRegister)(uint8_t, uint8_t);
    uint8_t (*readRegister)(uint8_t);
    void (*readRegister2)(uint8_t, uint8_t, uint8_t *, uint8_t);
    void (*antennaOn)();
    RC522_StatusCode (*requestA)(uint8_t *, uint8_t *);
    RC522_StatusCode (*REQA_or_WUPA)(uint8_t, uint8_t *, uint8_t *);
    void (*clearBitMask)(uint8_t, uint8_t);
    void (*setBitMask)(uint8_t, uint8_t);
    RC522_StatusCode (*transceiveData)(uint8_t *, uint8_t, uint8_t *, uint8_t *, uint8_t *, uint8_t);
    RC522_StatusCode (*communicateWithPICC)(uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t *, uint8_t *, uint8_t *, uint8_t);
    RC522_StatusCode (*haltA)();
    RC522_StatusCode (*select)(RC522_UID *);
    RC522_StatusCode (*calculateCRC)(uint8_t *, uint8_t, uint8_t *);
    RC522_StatusCode (*isNewCardPresent)();
    RC522_StatusCode (*readCardSerial)();
} RC522_Module;

extern Pin *RC522_SDA;
extern Pin *RC522_SCK;
extern Pin *RC522_MOSI;
extern Pin *RC522_MISO;
extern Pin *RC522_RST;

void RC522_Init(void);
void RC522_Reset(void);
void RC522_WriteRegister(uint8_t address, uint8_t data);
uint8_t RC522_ReadRegister(uint8_t address);
void RC522_ReadRegister2(uint8_t address, uint8_t count, uint8_t *values, uint8_t rxAlign);
void RC522_AntennaOn(void);
RC522_StatusCode RC522_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize);
RC522_StatusCode RC522_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize);
void RC522_ClearBitMask(uint8_t address, uint8_t mask);
void RC522_SetBitMask(uint8_t address, uint8_t mask);
RC522_StatusCode RC522_Select(RC522_UID *uid);
RC522_StatusCode RC522_TransceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign);
RC522_StatusCode RC522_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign);
RC522_StatusCode RC522_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result);
RC522_StatusCode RC522_HaltA();
RC522_StatusCode RC522_IsNewCardPresent();
RC522_StatusCode RC522_ReadCardSerial();
extern const RC522_Module RFID;
extern const RC522_UID uid;

#endif