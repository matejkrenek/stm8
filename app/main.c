#include "stm8s.h"
#include "Serial.h"
#include "LiquidCrystal_I2C.h"

Pin *PIN_CLK;
Pin *PIN_DT;
Pin *PIN_LED;

uint8_t counter = 0;
char counter_str[8];
uint8_t currentStateCLK;
uint8_t lastStateCLK;
unsigned long lastButtonPress = 0;

void main(void)
{
    CLK.HSI(HSIDIV1);
    // Serial.begin(9600);
    LCD.init(0x26, 16, 2);
    PIN_CLK = GPIO.init(GPIOB, PIN_1, INPUT_FL_NO_IT);
    PIN_DT = GPIO.init(GPIOB, PIN_0, INPUT_FL_NO_IT);
    PIN_LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_FAST);

    lastStateCLK = GPIO.read(PIN_CLK); // GPIO_ReadInputPin

    while (1)
    {
        // Read the current state of CLK
        currentStateCLK = GPIO.read(PIN_CLK); // GPIO_ReadInputPin

        // If last and current state of CLK are different, then pulse occurred
        // React to only 1 state change to avoid double count
        if (currentStateCLK != lastStateCLK && currentStateCLK == 1)
        {

            // If the DT state is different than the CLK state then
            // the encoder is rotating CCW so decrement
            if (GPIO.read(PIN_DT) != currentStateCLK)
            {
                counter--;
            }
            else
            {
                // Encoder is rotating CW so increment
                counter++;
            }

            // Serial.write("Direction: ");
            // Serial.write(" | Counter: ");
            // Serial.write("%u", counter);
            // Serial.write("\n");
            LCD.setCursor(0, 0);
            sprintf(counter_str, "%u", counter);
            LCD.print(counter_str);

            GPIO.writeHigh(PIN_LED);
        }

        // Remember last CLK state
        lastStateCLK = currentStateCLK;

        // Put in a slight delay to help debounce the reading
        delay.ms(1);
    }
}