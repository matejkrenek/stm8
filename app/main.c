#include "stm8s.h"
#include "Serial.h"
#include "DSRTC.h"
#include "UI.h"
#include "Encoder.h"
#include "LCD.h"

uint8_t hours[20];
uint8_t minutes[20];
uint8_t seconds[20];
uint8_t month[20];
uint8_t dayOfMonth[20];
uint8_t year[20];

void onScrollUp()
{
    if (UI.pointer() == 1)
    {
        UI.setPointer(0);
    }
    else if (UI.scrollbar() != 0)
    {

        if (UI.pointer() == 0)
        {
            UI.setScrollbar(UI.scrollbar() - 1);
        }
    }

    UI.render();

    return;
}

void onScrollDown()
{
    if (UI.pointer() == 0)
    {
        UI.setPointer(1);
    }
    else if (UI.linksCount() > 2)
    {
        if ((UI.linksCount() - UI.scrollbar()) != 2)
        {

            UI.setScrollbar(UI.scrollbar() + 1);
        }
    }

    UI.render();

    return;
}

void onClick()
{

    switch (UI.Router.current())
    {
    case 0:
        UI.redirect(1);
        UI.render();
        break;
    case 1:
        switch (UI.pointer() + UI.scrollbar())
        {
        case 0:
            UI.redirect(2);
            break;
        default:
            UI.redirect(0);
            break;
        }
        UI.render();
        break;
    case 2:
        switch (UI.pointer() + UI.scrollbar())
        {
        case 0:
            UI.redirect(3);
            break;
        case 1:
            UI.redirect(4);
            break;
        case 2:
            UI.redirect(5);
            break;
        default:
            UI.redirect(1);
            break;
        }
        UI.render();
        break;
    case 3:
        switch (UI.pointer() + UI.scrollbar())
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        default:
            UI.redirect(2);
            break;
        }
        UI.render();
        break;
    case 4:
        switch (UI.pointer() + UI.scrollbar())
        {
        case 0:
            break;
        case 1:
            break;
        default:
            UI.redirect(2);
            break;
        }
        UI.render();
        break;
    case 5:
        switch (UI.pointer() + UI.scrollbar())
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        default:
            UI.redirect(2);
            break;
        }
        UI.render();
        break;
    default:
        break;
    }
}

void main(void)
{
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    LCD.init(0x26, 16, 2);
    DSRTC.init(0x68);
    Encoder.init(onScrollUp, onScrollDown, onClick);
    UI.init();

    while (1)
    {
        if (UI.Router.current() == 4)
        {
            DateTime *dateTime = DSRTC.dateTime();
            sprintf(hours, "%u", dateTime->hours);
            sprintf(minutes, "%u", dateTime->minutes);
            sprintf(seconds, "%u", dateTime->seconds);
            sprintf(month, "%u", dateTime->month);
            sprintf(dayOfMonth, "%u", dateTime->dayOfMonth);
            sprintf(year, "%u", dateTime->year);
        }
    }
}