#include "UI.h"
#include "Serial.h"

int _ui_router_current = 0;
bool _ui_rendered = FALSE;
int _ui_router_previous = 0;
int _ui_scrollbar_current = 0;
int _ui_pointer_current = 0;
int _ui_rows_count = 0;
bool _ui_was_rendered = FALSE;

uint8_t chevron[8] = {
    0b10000,
    0b11000,
    0b11100,
    0b11110,
    0b11100,
    0b11000,
    0b10000,
    0b00000,
};

uint8_t clock[8] = {
    0b00000,
    0b01110,
    0b10001,
    0b10101,
    0b10011,
    0b01110,
    0b00000,
    0b00000,
};

uint8_t calendar[8] = {
    0b00000,
    0b11111,
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b11111,
    0b00000,
};

UI_Module UI = {
    .init = UI_Init,
    .rendered = UI_Rendered,
    .setRendered = UI_SetRendered,
    .wasRendered = UI_WasRendered,
    .setWasRendered = UI_SetWasRendered,
    .rows = UI_Rows,
    .setRows = UI_SetRows,
    .redirect = UI_Redirect,
    .scrollbar = UI_Scrollbar,
    .setScrollbar = UI_SetScrollbar,
    .pointer = UI_Pointer,
    .setPointer = UI_SetPointer,
    .Router.current = UI_Router_Current,
    .Router.previous = UI_Router_Previous,
};

void UI_Init()
{
    LCD.createChar(1, chevron);
    LCD.createChar(2, clock);
    LCD.createChar(3, calendar);
    UI.redirect(0);
    UI.setWasRendered(FALSE);
}

bool UI_Rendered()
{
    return _ui_rendered;
}

void UI_SetRendered(bool value)
{
    _ui_rendered = value;
}

bool UI_WasRendered()
{
    return _ui_was_rendered;
}

void UI_SetWasRendered(bool value)
{
    _ui_was_rendered = value;
}

void UI_SetRows(int rows)
{
    _ui_rows_count = rows;
}

int UI_Rows()
{
    return _ui_rows_count;
}

int UI_Pointer()
{
    return _ui_pointer_current;
}

int UI_Scrollbar()
{
    return _ui_scrollbar_current;
}

int UI_Router_Current()
{
    return _ui_router_current;
}

int UI_Router_Previous()
{
    return _ui_router_previous;
}

void UI_Redirect(int route)
{
    _ui_scrollbar_current = 0;
    _ui_pointer_current = 0;
    _ui_router_previous = _ui_router_current;
    _ui_router_current = route;
}

void UI_SetScrollbar(int value)
{
    _ui_scrollbar_current = value;
}
void UI_SetPointer(int value)
{
    _ui_pointer_current = value;
}