#include "stm8s.h"
#include "Serial.h"
#include "DSRTC.h"
#include "UI.h"
#include "Encoder.h"
#include "LCD.h"

void up()
{
    Serial.write("up\n");
}

void down()
{
    Serial.write("down\n");
}

void click()
{
    Serial.write("clicker\n");
}

void main(void)
{
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    LCD.init(0x26, 16, 2);
    Encoder.init(up, down, click);
    UI.render();

    while (1)
    {
    }
}