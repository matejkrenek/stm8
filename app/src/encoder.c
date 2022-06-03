#include "encoder.h"

Pin* ENCODER_CLK;
Pin* ENCODER_DT;
Pin* LED;
uint8_t ENCODER_COUNTER;

onInterrupt(EXTI_PORTB_IRQHandler, 4)
{
    if(GPIO.read(ENCODER_CLK)) {
        ENCODER_COUNTER++;
    } else if(GPIO.read(ENCODER_DT)) {
        ENCODER_COUNTER--;
    }
}

const Encoder_Module Encoder = {
    .init = Encoder_Init,
    .read = Encoder_Read,
};

void Encoder_Init(void) {
    ENCODER_CLK = GPIO.init(GPIOB, PIN_1, INPUT_FL_IT);
    ENCODER_DT = GPIO.init(GPIOB, PIN_0, INPUT_FL_IT);
    LED = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_FAST);
    EXTI.setSensitivity(EXTI_GPIOB, RISE_FALL);
    ITC.setPriority(ITC_IRQ_GPIOB, LEVEL_1);
    ITC.enable();
}

uint8_t Encoder_Read() {
    return ENCODER_COUNTER;
}

