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
    void (*boolean)(uint8_t *, bool);
    void (*timepicker)(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
    void (*datepicker)(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
    void (*edit)(int);
    int (*editing)();
    union
    {
        int (*current)();
        int (*previous)();
    } Router;
    void (*home)(uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
    void (*menu)();
    void (*settings)();
    void (*lcd)();
    void (*datetime)(uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
    uint8_t *(*datetimeLink)(int);

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

void UI_Boolean(uint8_t *label, bool value);
void UI_Timepicker(uint8_t *label, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
void UI_Datepicker(uint8_t *label, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year);

void UI_Edit(int id);
int UI_Editing();

void UI_Home(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year);
void UI_Menu();
void UI_Settings();
void UI_LCD();
void UI_Datetime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year);
uint8_t *UI_DatetimeLink(int index);

extern UI_Module UI;

#endif