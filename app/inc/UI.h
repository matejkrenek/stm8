#ifndef __UI_H
#define __UI_H

#include "stm8s.h"
#include "LCD.h"

typedef struct
{
    void (*init)();
    void (*render)();
    int (*linksCount)();
    void (*redirect)(int);
    int (*scrollbar)();
    void (*setScrollbar)(int);
    int (*pointer)();
    void (*setPointer)(int);
    union
    {
        int (*current)();
        int (*previous)();
        char *(*routes)();
    } Router;

} UI_Module;

void UI_Init();
int UI_Scrollbar();
int UI_LinksCount();
int UI_Pointer();
void UI_SetScrollbar(int value);
void UI_SetPointer(int value);
int UI_Router_Current();
void UI_Router_Render();
void UI_Router_Redirect(int route);
int UI_Router_Previous();

extern UI_Module UI;

#endif