#include "DSRTC.h"
#include "LCD.h"
#include "Serial.h"

uint8_t _dsrtc_address;
DateTime _dsrtc_datetime;
Pin *DSRTC_SDA;
Pin *DSRTC_SCL;

uint8_t decToBcd(uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

// Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}

DSRTC_Module DSRTC = {
    .init = DSRTC_Init,
    .read = DSRTC_Read,
    .write = DSRTC_Write,
    .dateTime = DSRTC_DateTime,
    .setDateTime = DSRTC_SetDateTime,
};

void DSRTC_Init(uint8_t address)
{
    _dsrtc_address = address << 1;

    DSRTC_SDA = GPIO.init(GPIOE, PIN_2, OUTPUT_PP_LOW_FAST);
    DSRTC_SCL = GPIO.init(GPIOE, PIN_1, OUTPUT_PP_LOW_FAST);

    I2C.init(100000, 0x00, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK.getFrequency() / 1000000);
    I2C.enable();

    delay.ms(50);
}

void DSRTC_Read(uint8_t *buffer)
{
    uint8_t bytesToRead = 7;

    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
        ;

    I2C.start();

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    I2C.sendAddress(_dsrtc_address, I2C_DIRECTION_TX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    I2C.sendData(0x00);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    I2C.start();

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    I2C.sendAddress(_dsrtc_address, I2C_DIRECTION_RX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;

    while (bytesToRead)
    {
        if (bytesToRead != 3) /* Receive bytes from first byte until byte N-3 */
        {
            while (!I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED))
                ; /* Poll on RxNE */

            /* Read a byte from the Slave */
            *buffer = I2C_ReceiveData();

            /* Point to the next location where the byte read will be saved */
            buffer++;

            /* Decrement the read bytes counter */
            bytesToRead--;
        }
        if (bytesToRead == 3) /* it remains to read three data: data N-2, data N-1, Data N */
        {
            /* Data N-2 in DR and data N -1 in shift register */

            /* Poll on BTF */
            while (!I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED))
                ; /* Poll on RxNE */

            /* Clear ACK */
            I2C_AcknowledgeConfig(I2C_ACK_NONE);

            /* Disable general interrupts */
            disableInterrupts();

            /* Read Data N-2 */
            *buffer = I2C_ReceiveData();

            /* Point to the next location where the byte read will be saved */
            buffer++;

            /* Program the STOP */
            I2C.stop();

            /* Read DataN-1 */
            *buffer = I2C_ReceiveData();

            /* Enable General interrupts */
            enableInterrupts();

            /* Point to the next location where the byte read will be saved */
            buffer++;

            while (!I2C_GetFlagStatus(I2C_FLAG_RXNOTEMPTY))
                ; /* Poll on RxNE */

            /* Read DataN */
            *buffer = I2C_ReceiveData();

            /* Reset the number of bytes to be read by master */
            bytesToRead = 0;
        }
    }

    I2C_AcknowledgeConfig(I2C_ACK_CURR);
}

void DSRTC_Write()
{
    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
        ;

    I2C.start();

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    I2C.sendAddress(_dsrtc_address, I2C_DIRECTION_TX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    I2C.sendData(0x00);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.sendData(_dsrtc_datetime.seconds);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.sendData(_dsrtc_datetime.minutes);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.sendData(_dsrtc_datetime.hours);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.sendData(_dsrtc_datetime.dayOfWeek);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.sendData(_dsrtc_datetime.dayOfMonth);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.sendData(_dsrtc_datetime.month);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.sendData(_dsrtc_datetime.year);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    I2C.stop();
}

DateTime *DSRTC_DateTime()
{
    uint8_t buffer[200];
    DSRTC.read(buffer);

    _dsrtc_datetime.seconds = bcdToDec(buffer[0]);
    _dsrtc_datetime.minutes = bcdToDec(buffer[1]);
    _dsrtc_datetime.hours = bcdToDec(buffer[2]);
    _dsrtc_datetime.dayOfWeek = bcdToDec(buffer[3]);
    _dsrtc_datetime.dayOfMonth = bcdToDec(buffer[4]);
    _dsrtc_datetime.month = bcdToDec(buffer[5]);
    _dsrtc_datetime.year = bcdToDec(buffer[6]);

    return &_dsrtc_datetime;
}

void DSRTC_SetDateTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint8_t year)
{
    _dsrtc_datetime.seconds = decToBcd(seconds);
    _dsrtc_datetime.minutes = decToBcd(minutes);
    _dsrtc_datetime.hours = decToBcd(hours);
    _dsrtc_datetime.dayOfWeek = decToBcd(dayOfWeek);
    _dsrtc_datetime.dayOfMonth = decToBcd(dayOfMonth);
    _dsrtc_datetime.month = decToBcd(month);
    _dsrtc_datetime.year = decToBcd(year);

    DSRTC.write();
}
