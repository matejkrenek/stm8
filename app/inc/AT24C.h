#ifndef __AT24C_H
#define __AT24C_H

#include "stm8s.h"

#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

typedef struct
{
    void (*init)(uint8_t, uint32_t);
    uint32_t (*length)();
    uint8_t (*read)(uint16_t);
    void (*write)(uint16_t, uint8_t);
    void (*update)(uint16_t, uint8_t);
} AT24C_Module;

extern uint8_t _at24c_address;
extern uint32_t _at24c_size;
extern Pin *AT24C_SDA;
extern Pin *AT24C_SCL;

void AT24C_Init(uint8_t address, uint32_t size);
uint32_t AT24C_Length();
uint8_t AT24C_Read(uint16_t address);
void AT24C_Write(uint16_t address, uint8_t value);
void AT24C_Update(uint16_t address, uint8_t value);
extern AT24C_Module AT24C;

#endif