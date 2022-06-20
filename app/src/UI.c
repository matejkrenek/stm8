#include "UI.h"
#include "DSRTC.h"
#include "Serial.h"

int _ui_router_current = 0;
int _ui_router_previous = 0;
int _ui_scrollbar_current = 0;
int _ui_pointer_current = 0;
int _ui_links_count = 0;

char _ui1_links[][20] = {"Settings", "Back"};
char _ui2_links[][20] = {"LCD", "Datetime", "Alarm", "Back"};
char _ui3_links[][20] = {"Backlight: ", "Cursor: ", "Blink: ", "Back"};
char _ui4_links[][20] = {"Time: ", "Date: ", "Back"};
char _ui5_links[][20] = {"Every: ", "Volume: ", "Freq: ", "Back"};

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
    .pointer = UI_Pointer,
    .linksCount = UI_LinksCount,
    .setPointer = UI_SetPointer,
    .setScrollbar = UI_SetScrollbar,
    .scrollbar = UI_Scrollbar,
    .render = UI_Router_Render,
    .Router.current = UI_Router_Current,
    .Router.previous = UI_Router_Previous,
    .redirect = UI_Router_Redirect,
};

void UI_Init()
{
    LCD.createChar(1, chevron);
    LCD.createChar(2, clock);
    LCD.createChar(3, calendar);
    UI.render();
}

int UI_LinksCount()
{
    return _ui_links_count;
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

void UI_Router_Redirect(int route)
{
    _ui_scrollbar_current = 0;
    _ui_pointer_current = 0;
    _ui_router_previous = _ui_router_current;
    _ui_router_current = route;
}

void UI_Router_Render()
{
    switch (_ui_router_current)
    {
    case 0:
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.printChar(2);
        LCD.print(" 12:22:55");
        LCD.setCursor(0, 1);
        LCD.printChar(3);
        LCD.print(" 27.09.2022");
        break;

    case 1:
        _ui_links_count = 2;
        LCD.clear();
        for (int i = 0; i < 2; i++)
        {
            LCD.setCursor(1, i);
            LCD.print(_ui1_links[i + UI.scrollbar()]);
        }

        LCD.setCursor(0, UI.pointer());
        LCD.printChar(1);

        break;

    case 2:
        _ui_links_count = 4;
        LCD.clear();
        for (int i = 0; i < 2; i++)
        {
            LCD.setCursor(1, i);
            LCD.print(_ui2_links[i + UI.scrollbar()]);
        }

        LCD.setCursor(0, UI.pointer());
        LCD.printChar(1);

        break;

    case 3:
        _ui_links_count = 4;
        LCD.clear();
        for (int i = 0; i < 2; i++)
        {
            LCD.setCursor(1, i);
            LCD.print(_ui3_links[i + UI.scrollbar()]);
        }

        LCD.setCursor(0, UI.pointer());
        LCD.printChar(1);

        break;

    case 4:
        _ui_links_count = 3;
        LCD.clear();
        for (int i = 0; i < 2; i++)
        {
            LCD.setCursor(1, i);
            LCD.print(_ui4_links[i + UI.scrollbar()]);
        }

        LCD.setCursor(0, UI.pointer());
        LCD.printChar(1);

        break;

    case 5:
        _ui_links_count = 4;
        LCD.clear();
        for (int i = 0; i < 2; i++)
        {
            LCD.setCursor(1, i);
            LCD.print(_ui5_links[i + UI.scrollbar()]);
        }

        LCD.setCursor(0, UI.pointer());
        LCD.printChar(1);

        break;

    default:
        break;
    };
}

void UI_SetScrollbar(int value)
{
    _ui_scrollbar_current = value;
}
void UI_SetPointer(int value)
{
    _ui_pointer_current = value;
}