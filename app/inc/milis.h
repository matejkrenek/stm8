#ifndef MILLIS_H_
#define MILLIS_H_

#include "stm8s.h"

typedef struct
{
    void (*init)();
    uint32_t (*get)();
} MILIS_Module;

void milis_init(void);
void milis_get(void);
extern const MILIS_Module milis;

#endif