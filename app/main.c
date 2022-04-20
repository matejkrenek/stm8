#include "setup.h"
#include "stm8s.h"
#include "LiquidCrystal_I2C.h"
#include "RC522.h"

char MasterTag[5] = {
    0x43, 0xdc, 0x52, 0xb6, 0x7b};
char TagID[5];

void setup(void)
{

    CLK.HSI(HSIDIV1);
    CLK.SYS(CPUDIV1);

    LCD.init(0x26, 16, 2);

    LCD.clear();
    LCD.print(" Access Control ");
    LCD.setCursor(0, 1);
    LCD.print("<<< Scan Card");
}

void loop(void)
{
}