#include "stm8s.h"
#include "Serial.h"
#include "LiquidCrystal_I2C.h"
#include "DHT.h"
#include <stdio.h>

uint8_t bl_state;
uint8_t data_value;
uint8_t values[5];
const unsigned char symbol[8] =
{
   0x00, 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00
};
void clock_setup(void);
void GPIO_setup(void);
unsigned int make_word(unsigned char HB, unsigned char LB);

void main(void)
{
    unsigned char state = 0x00;
    unsigned int RH = 0x0000;
    unsigned int t = 0x0000;
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    LCD.init(0x26, 16, 2);
    DHT11_init();

    while (1)
    {
        state = get_data();
        LCD.setCursor(0, 0);
        RH = make_word(values[0], values[1]);
        LCD.setCursor(14, 0);
        LCD.print(values[0]);
        LCD.setCursor(0, 1);
        LCD.print("Tmp/");
        LCD.setCursor(5, 1);
        LCD.print("C:");
        t = make_word(values[2], values[3]);
        if((values[2] & 0x80) == 1)
        {
                LCD.setCursor(13, 1);
                LCD.print("-");
        }
        else
        {
                LCD.setCursor(13, 1);
                LCD.print(" ");
        }
        LCD.setCursor(14, 1);
        LCD.print(values[2]);
        delay.ms(1000);
    }
}

unsigned int make_word(unsigned char HB, unsigned char LB)
{
  unsigned int value = 0x0000;
  value = HB;
  value <<= 8;
  value |= LB;
  return value;
}