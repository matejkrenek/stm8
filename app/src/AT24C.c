#include "AT24C.h"
#include "Serial.h"

uint8_t _at24c_address;
uint32_t _at24c_size;
Pin *AT24C_SDA;
Pin *AT24C_SCL;

AT24C_Module AT24C = {
    .init = AT24C_Init,
    .length = AT24C_Length,
    .read = AT24C_Read,
    .write = AT24C_Write,
    .update = AT24C_Update,
};

void AT24C_Init(uint8_t address, uint32_t size)
{
    _at24c_address = address << 1;
    _at24c_size = size;

    delay.ms(50);
}

uint32_t AT24C_Length()
{
    return _at24c_size * 1024;
}

uint8_t AT24C_Read(uint16_t address)
{
    uint8_t data;
    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
        ;

    I2C.start();

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    I2C.sendAddress(_at24c_address, I2C_DIRECTION_TX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    I2C.sendData(highByte(address));

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    I2C.sendData(lowByte(address));

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C.stop();

    I2C.start();

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    I2C.sendAddress(_at24c_address, I2C_DIRECTION_RX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;

    while (!I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED))
        ;

    data = I2C_ReceiveData();

    I2C.stop();

    return data;
}

void AT24C_Write(uint16_t address, uint8_t value)
{

    Serial.write("start");
    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
        ;
    Serial.write("I2C_FLAG_BUSBUSY");

    I2C.start();

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    Serial.write("I2C_EVENT_MASTER_MODE_SELECT");

    I2C.sendAddress(_at24c_address, I2C_DIRECTION_TX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    Serial.write("I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED");

    I2C.sendData(highByte(address));

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    Serial.write("I2C_EVENT_MASTER_BYTE_TRANSMITTED1");

    I2C.sendData(lowByte(address));

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    Serial.write("I2C_EVENT_MASTER_BYTE_TRANSMITTED2");

    I2C.sendData(value);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    Serial.write("I2C_EVENT_MASTER_BYTE_TRANSMITTED3");

    I2C.stop();
}

void AT24C_Update(uint16_t address, uint8_t value)
{
}