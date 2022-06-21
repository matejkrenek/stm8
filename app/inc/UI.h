#ifndef __UI_H
#define __UI_H

#include "stm8s.h"
#include "LCD.h"

typedef struct
{
    void (*init)();
    bool (*rendered)();
    void (*setRendered)(bool);
    bool (*wasRendered)();
    void (*setWasRendered)(bool);
    int (*rows)();
    void (*setRows)(int);
    void (*redirect)(int);
    int (*scrollbar)();
    void (*setScrollbar)(int);
    int (*pointer)();
    void (*setPointer)(int);
    union
    {
        int (*current)();
        int (*previous)();
    } Router;

} UI_Module;

void UI_Init();
bool UI_Rendered();
void UI_SetRendered(bool value);
int UI_Scrollbar();
int UI_Rows();
int UI_Pointer();
void UI_SetScrollbar(int value);
void UI_SetPointer(int value);
bool UI_WasRendered();
void UI_SetRows(int rows);
void UI_SetWasRendered(bool value);
void UI_Redirect(int route);
int UI_Router_Current();
int UI_Router_Previous();

extern UI_Module UI;

#endif