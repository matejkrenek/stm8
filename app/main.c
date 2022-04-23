#include "stm8s.h"
#include "Serial.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>

uint16_t ADC_Read(ADC2_Channel_TypeDef channel_number);
// 65536 -> 5V
// 65472 ->

void main(void)
{
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    LCD.init(0x26, 16, 2);

    LCD.print("Matej Krenek");

    while (1)
    {
        uint16_t volts = (ADC_Read(ADC2_CHANNEL_0) / (65536 / 100)) * 5;

        printf("potenciometr: %u V \n", volts);
        delay.ms(200);
    }
}

uint16_t ADC_Read(ADC2_Channel_TypeDef channel_number)
{
    uint16_t result = 0;

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
