#include "setup.h"
#include "stm8s.h"
#include "LiquidCrystal_I2C.h"
#include "RC522.h"
#include "milis.h"

Pin *LED_GREEN;
Pin *LED_RED;
char MasterTag[] = "4759AC8";
char TagID[] = "";

void setup(void)
{
    CLK.HSI(HSIDIV1);
    CLK.SYS(CPUDIV1);
    milis.init();

    RFID.init();
    LCD.init(0x26, 16, 2);

    LED_GREEN = GPIO.init(GPIOB, PIN_0, OUTPUT_PP_LOW_SLOW);
    LED_RED = GPIO.init(GPIOB, PIN_1, OUTPUT_PP_LOW_SLOW);

    LCD.clear();
    LCD.print(" Access Control ");
    LCD.setCursor(0, 1);
    LCD.print("<<< Scan Card");
}

void loop(void)
{
    while (getID())
    {
        LCD.clear();
        LCD.setCursor(0, 0);

        if (TagID == MasterTag)
        {
            LCD.print("Access Granted!");
            GPIO.writeHigh(LED_GREEN);
        }
        else
        {
            LCD.print(" Access Denied!");
            GPIO.writeHigh(LED_RED);
        }

        LCD.setCursor(0, 1);
        LCD.print(" ID : ");
        LCD.print(TagID);

        delay.ms(2000);
        GPIO.writeLow(LED_GREEN);
        GPIO.writeLow(LED_RED);

        LCD.clear();
        LCD.print(" Access Control ");
        LCD.setCursor(0, 1);
        LCD.print("<<< Scan Card");
    }
}

bool getID()
{

    if (!RFID.isNewCardPresent())
    {
        return FALSE;
    }
    if (!RFID.readCardSerial())
    {
        return FALSE;
    }

    // store card id into TagID variable

    RFID.haltA();
    return TRUE;
}