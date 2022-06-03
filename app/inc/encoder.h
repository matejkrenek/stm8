#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm8s.h"

typedef struct
{
    void (*init)();
    uint8_t (*read)();
} Encoder_Module;

extern Pin* ENCODER_CLK;
extern Pin* ENCODER_DT;
extern uint8_t ENCODER_COUNTER;

void Encoder_Init();
uint8_t Encoder_Read();
const extern Encoder_Module Encoder;

#endif