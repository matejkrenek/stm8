#include "milis.h"

__IO uint32_t current_millis = 0; //--IO: volatile read/write

const MILIS_Module milis = {
    .init = milis_init,
    .get = milis_get,
};

void milis_init(void)
{
    CLK.HSI(HSIDIV1);
    TIM4.init(TIM4_PRESCALER_128, 124);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    ITC.enable();
    TIM4.enable();
}

uint32_t millis_get(void)
{
    return current_millis;
}

// Interupt event, happen every 1 ms
onInterrupt(TIM4_UPD_OVF_IRQHandler, 23)
{
    // increase 1, for millis() function
    current_millis++;

    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}