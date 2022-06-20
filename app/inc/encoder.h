#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm8s.h"

/**
 * @brief  Definition of the Encoder struct to work with rotatory encoder.
 */
typedef struct
{
    void (*init)(void()(), void()(), void()());
    bool (*changed)();
    void (*setChanged)(bool);
    uint8_t (*read)();
    void (*left)();
    void (*right)();
    void (*click)();
} Encoder_Module;

extern Pin *ENCODER_CLK;
extern Pin *ENCODER_BTN;
extern Pin *ENCODER_DT;
extern uint8_t ENCODER_COUNTER;
extern bool ENCODER_CHANGED;

void Encoder_Init(void (*left)(), void (*right)(), void (*click)());
uint8_t Encoder_Read();
bool Encoder_Changed();
void Encoder_SetChanged(bool changed);
extern Encoder_Module Encoder;

#endif