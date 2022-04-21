#include "stm8s.h"
#include "RC522.h"
#include "LiquidCrystal_I2C.h"

void Dec2Hex(uint8_t dec);
char dec2hex[2];
uint8_t status;
uint8_t buffer[20];
uint8_t DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void main(void)
{
    CLK.HSI(HSIDIV1);
    CLK.SYS(CPUDIV1);

    LCD.init(0x26, 16, 2);
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Ctecka karet");
    LCD.setCursor(0, 1);
    LCD.print("UUD:");

    RFID.init();

    while (1)
    {
        status = RFID.request(PICC_CMD_REQA, buffer);

        if (status != MI_OK)
        {
            continue;
        }

        status = RFID.anticoll(buffer);

        if (status != MI_OK)
        {
            continue;
        }

        status = RFID.select(buffer);

        if (status != MI_OK)
        {
            continue;
        }

        Dec2Hex(buffer[0]);
        LCD.setCursor(4, 1);
        LCD.printChar(dec2hex[0]);
        LCD.setCursor(5, 1);
        LCD.printChar(dec2hex[1]);
        ////
        Dec2Hex(buffer[1]);
        LCD.setCursor(7, 1);
        LCD.printChar(dec2hex[0]);
        LCD.setCursor(8, 1);
        LCD.printChar(dec2hex[1]);
        //
        Dec2Hex(buffer[2]);
        LCD.setCursor(10, 1);
        LCD.printChar(dec2hex[0]);
        LCD.setCursor(11, 1);
        LCD.printChar(dec2hex[1]);
        ////
        Dec2Hex(buffer[3]);
        LCD.setCursor(13, 1);
        LCD.printChar(dec2hex[0]);
        LCD.setCursor(14, 1);
        LCD.printChar(dec2hex[1]);
        ////////////////////////////////////////////////////////////////////
        status = RFID.read(1, buffer);
        if (status != MI_OK)
        {
            continue;
        }
        RFID.haltA();
    }
}

void Dec2Hex(uint8_t dec)
{
    dec2hex[0] = dec / 16;
    if (dec2hex[0] > 9)
    {
        dec2hex[0] = dec2hex[0] + 55;
    }
    else
        dec2hex[0] = dec2hex[0] + 48;
    /////////////////
    dec2hex[1] = dec % 16;
    if (dec2hex[1] > 9)
    {
        dec2hex[1] = dec2hex[1] + 55;
    }
    else
        dec2hex[1] = dec2hex[1] + 48;
}