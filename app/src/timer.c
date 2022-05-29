#include "timer.h"

volatile uint32_t tick = 0;

const Timer_Module timer = {
    .init = timer_init,
    .milis = milis_get,
    .micros = micros_get,
};

void timer_init(void)
{
    CLK.HSI(HSIDIV1);
    TIM4.init(TIM4_PRESCALER_16, 250);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    ITC.enable();
    TIM4.enable();
}

uint16_t milis_get(void)
{
    return ((uint16_t)(tick >> 2));
}

uint32_t micros_get(void)
{
    return (tick * 250 + TIM4_GetCounter());
}

// Interupt event, happen every 1 ms
onInterrupt(TIM4_UPD_OVF_IRQHandler, 23)
{
    // increase 1, for millis() function
    tick++;

    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}