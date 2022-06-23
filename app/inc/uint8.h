#ifndef __uint8_H
#define __uint8_H

#include "stm8s.h"

/**
 * @brief  Definition of the Serial struct to communicate with serial monitor.
 */
typedef struct
{
    void (*toString)(char *, uint8_t);
} UINT8_Module;

void UINT8_ToString(char *to, uint8_t from);

extern UINT8_Module uint8;

#endif