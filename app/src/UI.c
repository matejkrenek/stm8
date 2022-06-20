#include "UI.h"
#include "Serial.h"

char _ui_router[][20] = {"456h", "menu"};
uint8_t _ui_router_current = 0;
uint8_t _ui_router_previous = 0;

UI_Module UI = {
    .render = UI_Router_Render,
    .Router.current = UI_Router_Current,
    .Router.previous = UI_Router_Previous,
};

uint8_t UI_Router_Current()
{
    return _ui_router_current;
}

uint8_t UI_Router_Previous()
{
    return _ui_router_previous;
}

void UI_Router_Render()
{
    switch (_ui_router_current)
    {
    case 0:
        LCD.clear();
        LCD.print("Home");
        break;

    case 1:
        LCD.clear();
        LCD.print("Menu");
        break;

    default:
        break;
    };
}