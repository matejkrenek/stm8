#include "setup.h"
#include "stm8s.h"
#include "LiquidCrystal_I2C.h"

void setup(void)
{
    GPIO_DeInit;
    CLK_DeInit();
    I2C_DeInit();
    CLK.HSI(HSIDIV1);
    CLK.SYS(CPUDIV1);
    LiquidCrystal_I2C.init(0x26, 16, 2);
}

void loop(void)
{
}