#include "stm8s.h"
#include "Serial.h"
#include "DSRTC.h"
#include "UI.h"
#include "Encoder.h"
#include "LCD.h"
#include "str.h"
#include "uint8.h"

uint8_t hours[20];
uint8_t minutes[20];
uint8_t seconds[20];
uint8_t month[20];
uint8_t dayOfMonth[20];
uint8_t year[20];

DateTime *datetime;

void onScrollUp();
void onScrollDown();
void onClick();

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
        if (UI.Router.current() == 0)
        {
            UI.setWasRendered(FALSE);
        }

        if (!UI.wasRendered() || UI.rendered())
        {

            if (UI.Router.current() == 0 || UI.Router.current() == 4)
            {
                datetime = DSRTC.dateTime();
                uint8.toString(hours, datetime->hours);
                uint8.toString(minutes, datetime->minutes);
                uint8.toString(seconds, datetime->seconds);
                uint8.toString(month, datetime->month);
                uint8.toString(dayOfMonth, datetime->dayOfMonth);
                uint8.toString(year, datetime->year);
            }

            switch (UI.Router.current())
            {
            case 0:
                UI.home(hours, minutes, seconds, dayOfMonth, month, year);
                break;

            case 1:
                UI.menu();
                break;

            case 2:
                UI.settings();
                break;

            case 3:
                UI.lcd();
                break;

            case 4:
                UI.datetime(hours, minutes, seconds, dayOfMonth, month, year);
                break;
            default:
                break;
            };
        }
    }
}

void onScrollUp()
{
    if (UI.editing() < 0)
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
        UI.edit(-1);
        UI.setWasRendered(FALSE);
    }
    else
    {
        if (UI.Router.current() == 4)
        {
            switch (UI.pointer() + UI.scrollbar())
            {
            case 0:
                switch (UI.editing())
                {
                case 0:
                    if (datetime->hours != 23)
                    {
                        datetime->hours++;
                    }

                    uint8.toString(hours, datetime->hours);

                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 1, UI.pointer());
                    LCD.print(hours);
                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->minutes != 59)
                    {
                        datetime->minutes++;
                    }
                    uint8.toString(minutes, datetime->minutes);

                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 4, UI.pointer());
                    LCD.print(minutes);
                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 5, UI.pointer());

                    break;
                case 2:
                    if (datetime->seconds != 59)
                    {
                        datetime->seconds++;
                    }
                    uint8.toString(seconds, datetime->seconds);

                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 7, UI.pointer());
                    LCD.print(seconds);
                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 8, UI.pointer());

                    break;
                default:
                    break;
                }
                break;

            case 1:
                switch (UI.editing())
                {
                case 0:
                    if (datetime->dayOfMonth != 31)
                    {
                        datetime->dayOfMonth++;
                    }
                    uint8.toString(dayOfMonth, datetime->dayOfMonth);

                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 1, UI.pointer());
                    LCD.print(dayOfMonth);
                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->month != 12)
                    {
                        datetime->month++;
                    }
                    uint8.toString(month, datetime->month);

                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 4, UI.pointer());
                    LCD.print(month);
                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 5, UI.pointer());

                    break;
                case 2:
                    if (datetime->year != 99)
                    {
                        datetime->year++;
                    }
                    uint8.toString(year, datetime->year);

                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 7, UI.pointer());
                    LCD.print(year);
                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 8, UI.pointer());

                    break;
                default:
                    break;
                }
                break;

            default:
                break;
            }
        }
    }
}

void onScrollDown()
{
    if (UI.editing() < 0)
    {
        if (UI.pointer() == 0)
        {
            UI.setPointer(1);
        }
        else if (UI.rows() > 2)
        {
            if ((UI.rows() - UI.scrollbar()) != 2)
            {

                UI.setScrollbar(UI.scrollbar() + 1);
            }
        }
        UI.edit(-1);
        UI.setWasRendered(FALSE);
    }
    else
    {
        if (UI.Router.current() == 4)
        {
            switch (UI.pointer() + UI.scrollbar())
            {
            case 0:
                switch (UI.editing())
                {
                case 0:
                    if (datetime->hours != 0)
                    {
                        datetime->hours--;
                    }
                    uint8.toString(hours, datetime->hours);

                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 1, UI.pointer());
                    LCD.print(hours);
                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->minutes != 0)
                    {
                        datetime->minutes--;
                    }
                    uint8.toString(minutes, datetime->minutes);

                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 4, UI.pointer());
                    LCD.print(minutes);
                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 5, UI.pointer());
                    break;
                case 2:
                    if (datetime->seconds != 0)
                    {
                        datetime->seconds--;
                    }

                    uint8.toString(seconds, datetime->seconds);

                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 7, UI.pointer());
                    LCD.print(seconds);
                    LCD.setCursor(str.length(UI.datetimeLink(0)) + 8, UI.pointer());
                    break;
                default:
                    break;
                }
                break;

            case 1:
                switch (UI.editing())
                {
                case 0:
                    if (datetime->dayOfMonth != 0)
                    {
                        datetime->dayOfMonth--;
                    }
                    uint8.toString(dayOfMonth, datetime->dayOfMonth);

                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 1, UI.pointer());
                    LCD.print(dayOfMonth);
                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->month != 0)
                    {
                        datetime->month--;
                    }
                    uint8.toString(month, datetime->month);

                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 4, UI.pointer());
                    LCD.print(month);
                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 5, UI.pointer());
                    break;
                case 2:
                    if (datetime->year != 0)
                    {
                        datetime->year--;
                    }
                    uint8.toString(year, datetime->year);

                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 7, UI.pointer());
                    LCD.print(year);
                    LCD.setCursor(str.length(UI.datetimeLink(1)) + 8, UI.pointer());
                    break;
                default:
                    break;
                }
                break;

            default:
                break;
            }
        }
    }
}

void onClick()
{
    switch (UI.Router.current())
    {
    case 0:
        UI.redirect(1);
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
        default:
            UI.redirect(1);
            break;
        }
        break;
    case 3:

        switch (UI.pointer() + UI.scrollbar())
        {
        case 0:
            if (LCD.getBacklight())
            {
                LCD.noBacklight();
            }
            else
            {
                LCD.backlight();
            }

            break;
        case 1:

            if (LCD.getCursor())
            {
                LCD.noCursor();
            }
            else
            {
                LCD.cursor();
            }

            break;
        case 2:
            if (LCD.getBlink())
            {
                LCD.noBlink();
            }
            else
            {
                LCD.blink();
            }
            break;
        default:
            UI.redirect(2);
            break;
        }
        UI.setRendered(FALSE);
        UI.setWasRendered(FALSE);
        break;
    case 4:
        switch (UI.pointer() + UI.scrollbar())
        {
        case 0:
            LCD.cursor();
            switch (UI.editing())
            {
            case -1:
                LCD.setCursor(str.length(UI.datetimeLink(0)) + 2, UI.pointer());
                UI.edit(0);
                break;
            case 0:
                LCD.setCursor(str.length(UI.datetimeLink(0)) + 5, UI.pointer());
                UI.edit(1);
                break;
            case 1:
                LCD.setCursor(str.length(UI.datetimeLink(0)) + 8, UI.pointer());
                UI.edit(2);
                break;
            case 2:
                LCD.noCursor();
                UI.edit(-1);
                DSRTC.setDateTime(datetime->seconds, datetime->minutes, datetime->hours, 0, datetime->dayOfMonth, datetime->month, datetime->year);
                break;
            default:
                LCD.noCursor();
                break;
            }

            break;
        case 1:
            LCD.cursor();
            switch (UI.editing())
            {
            case -1:
                LCD.setCursor(str.length(UI.datetimeLink(1)) + 2, UI.pointer());

                UI.edit(0);
                break;
            case 0:
                LCD.setCursor(str.length(UI.datetimeLink(1)) + 5, UI.pointer());
                UI.edit(1);
                break;
            case 1:
                LCD.setCursor(str.length(UI.datetimeLink(1)) + 8, UI.pointer());

                UI.edit(2);
                break;
            case 2:
                LCD.noCursor();
                UI.edit(-1);
                DSRTC.setDateTime(datetime->seconds, datetime->minutes, datetime->hours, 0, datetime->dayOfMonth, datetime->month, datetime->year);
                break;
            default:
                LCD.noCursor();

                break;
            }
            break;
        default:
            UI.redirect(2);
            break;
        }
        break;
    default:
        break;
    }
}