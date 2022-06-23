#include "UI.h"
#include "LCD.h"
#include "Serial.h"
#include "str.h"

int _ui_router_current = 0;
bool _ui_rendered = FALSE;
int _ui_router_previous = 0;
int _ui_scrollbar_current = 0;
int _ui_pointer_current = 0;
int _ui_rows_count = 0;
bool _ui_was_rendered = FALSE;
int _ui_editing = -1;
uint8_t route_menu[][20] = {"Settings", "Back"};
uint8_t route_settings[][20] = {"LCD", "Datetime", "Back"};
uint8_t route_lcd[][20] = {"Backlight: ", "Cursor: ", "Blink: ", "Back"};
uint8_t route_datetime[][20] = {"Time: ", "Date: ", "Back"};

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
    .boolean = UI_Boolean,
    .timepicker = UI_Timepicker,
    .datepicker = UI_Datepicker,
    .edit = UI_Edit,
    .editing = UI_Editing,
    .home = UI_Home,
    .menu = UI_Menu,
    .settings = UI_Settings,
    .lcd = UI_LCD,
    .datetime = UI_Datetime,
    .datetimeLink = UI_DatetimeLink,
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
    UI.setRendered(FALSE);
    UI.setWasRendered(FALSE);
}

void UI_SetScrollbar(int value)
{
    _ui_scrollbar_current = value;
}
void UI_SetPointer(int value)
{
    _ui_pointer_current = value;
}

// Inputs
void UI_Boolean(uint8_t *label, bool value)
{
    LCD.print(label);

    if (value)
    {
        LCD.print("ON");
        return;
    }

    LCD.print("OFF");
}

void UI_Timepicker(uint8_t *label, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    LCD.print(label);
    LCD.print(hours);
    LCD.print(":");
    LCD.print(minutes);
    LCD.print(":");
    LCD.print(seconds);
}

void UI_Datepicker(uint8_t *label, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year)
{
    LCD.print(label);
    LCD.print(dayOfMonth);
    LCD.print("/");
    LCD.print(month);
    LCD.print("/");
    LCD.print(year);
}

void UI_Edit(int id)
{
    _ui_editing = id;
}
int UI_Editing()
{
    return _ui_editing;
}

void UI_Home(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year)
{
    if (!UI.rendered())
    {
        LCD.clear();
    }
    LCD.setCursor(0, 0);
    LCD.printChar(2);
    LCD.print(hours);
    LCD.print(":");
    LCD.print(minutes);
    LCD.print(":");
    LCD.print(seconds);
    if (!UI.rendered())
    {
        LCD.setCursor(0, 1);
        LCD.printChar(3);
        LCD.print(dayOfMonth);
        LCD.print("/");
        LCD.print(month);
        LCD.print("/");
        LCD.print(year);
    }
    UI.setRendered(TRUE);
    delay.ms(1000);
}

void UI_Menu()
{
    UI.setRows(2);
    LCD.clear();
    for (int i = 0; i < 2; i++)
    {
        LCD.setCursor(1, i);
        LCD.print(route_menu[i + UI.scrollbar()]);
    }

    LCD.setCursor(0, UI.pointer());
    LCD.printChar(1);

    UI.setWasRendered(TRUE);
}
void UI_Settings()
{
    UI.setRows(3);
    LCD.clear();
    for (int i = 0; i < 2; i++)
    {
        LCD.setCursor(1, i);
        LCD.print(route_settings[i + UI.scrollbar()]);
    }

    LCD.setCursor(0, UI.pointer());

    LCD.printChar(1);

    UI.setWasRendered(TRUE);
}
void UI_LCD()
{
    UI.setRows(4);
    LCD.clear();
    for (int i = 0; i < 2; i++)
    {
        LCD.setCursor(1, i);
        if (UI.rows() - UI.scrollbar() == 2 && i == 1)
        {
            LCD.print(route_lcd[i + UI.scrollbar()]);
        }
        else
        {
            switch (i + UI.scrollbar())
            {
            case 0:
                UI.boolean(route_lcd[i + UI.scrollbar()], LCD.getBacklight());
                break;
            case 1:
                UI.boolean(route_lcd[i + UI.scrollbar()], LCD.getCursor());
                break;
            case 2:
                UI.boolean(route_lcd[i + UI.scrollbar()], LCD.getBlink());
                break;

            default:
                break;
            }
        }
    }
    LCD.setCursor(0, UI.pointer());
    LCD.printChar(1);

    LCD.setCursor(str.length(route_lcd[UI.pointer() + UI.scrollbar()]) + 1, UI.pointer());

    UI.setWasRendered(TRUE);
}
void UI_Datetime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year)
{
    UI.setRows(3);
    LCD.clear();

    for (int i = 0; i < 2; i++)
    {
        LCD.setCursor(1, i);
        if (UI.rows() - UI.scrollbar() == 2 && i == 1)
        {
            LCD.print(route_datetime[i + UI.scrollbar()]);
        }
        else
        {
            switch (i + UI.scrollbar())
            {
            case 0:
                UI.timepicker(route_datetime[i + UI.scrollbar()], hours, minutes, seconds);
                break;
            case 1:
                UI.datepicker(route_datetime[i + UI.scrollbar()], dayOfMonth, month, year);
                break;
            default:
                break;
            }
        }
    }

    LCD.setCursor(0, UI.pointer());
    LCD.printChar(1);

    UI.setWasRendered(TRUE);
}

uint8_t *UI_DatetimeLink(int index)
{
    return route_datetime[index];
}