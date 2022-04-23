#include "stm8s.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>

uint8_t ADC_Read(ADC2_Channel_TypeDef channel_number);
uint8_t potenciometr = 200;
char potenciometr_char[10];

void main(void)
{
    CLK.HSI(HSIDIV1);

    LCD.init(0x26, 16, 2);
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Ctecka karet");
    LCD.setCursor(0, 1);
    LCD.print("UUD:");

    while (1)
    {

        LCD.setCursor(4, 1);
        LCD.printChar(12);
        delay.ms(1);
    }
}

uint8_t ADC_Read(ADC2_Channel_TypeDef channel_number)
{
    uint8_t result = 0;

    ADC2_DeInit();

    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS, channel_number, ADC2_PRESSEL_FCPU_D18, ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_LEFT, ADC2_SCHMITTTRIG_ALL, DISABLE);

    ADC2_Cmd(ENABLE);

    ADC2_StartConversion();

    while (!ADC2_GetFlagStatus())
        ;

    result = ADC2_GetConversionValue();
    ADC2_ClearFlag();

    ADC2_DeInit();

    return result;
}