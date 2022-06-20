#ifndef __UI_H
#define __UI_H

#include "stm8s.h"
#include "LCD.h"

typedef struct
{
    void (*render)();
    union
    {
        uint8_t (*current)();
        uint8_t (*previous)();
        char *(*routes)();
    } Router;

} UI_Module;

uint8_t UI_Router_Current();
void UI_Router_Render();
uint8_t UI_Router_Previous();

extern UI_Module UI;

#endif