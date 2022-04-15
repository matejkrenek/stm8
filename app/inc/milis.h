#ifndef MILLIS_H_
#define MILLIS_H_

#include "stm8s.h"

typedef struct
{
    void (*init)(void);
    uint32_t (*get)(void);
} MILIS_Module;

void milis_init(void);
uint32_t milis_get(void);
extern const MILIS_Module milis;

#endif