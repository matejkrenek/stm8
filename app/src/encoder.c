#include "encoder.h"
#include "Serial.h"

Pin *ENCODER_CLK;
Pin *ENCODER_BTN;
Pin *ENCODER_DT;
Pin *LED;
uint8_t ENCODER_COUNTER;
bool ENCODER_CHANGED;

bool PREV_STATE;
bool CURR_STATE;
bool BUTTON_PREV_STATE;
bool BUTTON_CURR_STATE;

Encoder_Module Encoder = {
    .init = Encoder_Init,
    .changed = Encoder_Changed,
    .setChanged = Encoder_SetChanged,
    .read = Encoder_Read,
};

onInterrupt(EXTI_PORTD_IRQHandler, 6)
{
    Serial.write("pressed\n");
}

onInterrupt(EXTI_PORTC_IRQHandler, 5)
{
    CURR_STATE = GPIO.read(ENCODER_CLK);

    if (CURR_STATE != PREV_STATE && CURR_STATE)
    {
        if (GPIO.read(ENCODER_DT) != CURR_STATE)
        {
            Encoder.left();
        }
        else
        {
            Encoder.right();
        }
    }

    PREV_STATE = CURR_STATE;

    delay.ms(1);
}

void Encoder_Init(void (*left)(), void (*right)(), void (*click)())
{
    ENCODER_CLK = GPIO.init(GPIOC, PIN_1, INPUT_FL_IT);
    ENCODER_BTN = GPIO.init(GPIOD, PIN_1, INPUT_FL_IT);
    ENCODER_DT = GPIO.init(GPIOC, PIN_2, INPUT_FL_IT);
    EXTI.setSensitivity(EXTI_GPIOC, RISE_FALL);
    EXTI.setSensitivity(EXTI_GPIOD, RISE_ONLY);
    ITC.setPriority(ITC_IRQ_GPIOC, LEVEL_1);
    ITC.setPriority(ITC_IRQ_GPIOD, LEVEL_1);
    ITC.enable();

    PREV_STATE = GPIO.read(ENCODER_CLK);
    Encoder.left = left;
    Encoder.right = right;
    Encoder.click = click;
}

bool Encoder_Changed()
{
    bool changed = ENCODER_CHANGED;

    Encoder.setChanged(FALSE);

    return changed;
}

void Encoder_SetChanged(bool changed)
{
    ENCODER_CHANGED = changed;
}

uint8_t Encoder_Read()
{
    return ENCODER_COUNTER;
}