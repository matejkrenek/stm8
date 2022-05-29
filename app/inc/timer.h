#ifndef _TIMER_H_
#define _TIMER_H_ 1

// #define MILIS_16BIT
#include "stm8s.h"

typedef struct
{
    void (*init)();
    uint16_t (*milis)();
    uint32_t (*micros)();
} Timer_Module;

void timer_init();
uint16_t milis_get();
uint32_t micros_get();
extern const Timer_Module timer;

#endif