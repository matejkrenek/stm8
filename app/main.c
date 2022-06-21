#include "stm8s.h"
#include "Serial.h"
#include "DSRTC.h"
#include "UI.h"
#include "Encoder.h"
#include "LCD.h"

char hours[20];
char minutes[20];
char seconds[20];
char month[20];
uint8_t dayOfWeek;
char dayOfMonth[20];
char year[20];
char days[][20] = {
    "Ne",
    "Po",
    "Ut",
    "St",
    "Ct",
    "Pa",
    "So",
};

char route_home[][20] = {"Settings", "Back"};
char route_menu[][20] = {"LCD", "Datetime", "Alarm", "Back"};
char route_lcd[][20] = {"Backlight: ", "Cursor: ", "Blink: ", "Back"};
char route_datetime[][20] = {"Time: ", "Date: ", "Back"};
char route_alarm[][20] = {"Every: ", "Volume: ", "Freq: ", "Back"};

void prepend(char *s, const char *t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

void uint8ToString(char *to, uint8_t from, bool zero_prefix)
{
    sprintf(to, "%u", from);

    if (zero_prefix && from < 10)
    {
        prepend(to, "0");
    }
}

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

    UI.setWasRendered(FALSE);
}

void onScrollDown()
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

    UI.setWasRendered(FALSE);
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
        case 2:
            UI.redirect(5);
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
            break;
        case 1:
            break;
        case 2:
            break;
        default:
            UI.redirect(2);
            break;
        }
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
        break;
    default:
        break;
    }

    UI.setRendered(FALSE);
    UI.setWasRendered(FALSE);
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
        if (UI.Router.current() == 0 || UI.Router.current() != UI.Router.previous())
        {
            UI.setWasRendered(FALSE);
        }

        if (!UI.wasRendered() || UI.rendered())
        {

            if (UI.Router.current() == 0 || UI.Router.current() == 4)
            {
                DateTime *datetime = DSRTC.dateTime();
                uint8ToString(hours, datetime->hours, TRUE);
                uint8ToString(minutes, datetime->minutes, TRUE);
                uint8ToString(seconds, datetime->seconds, TRUE);
                uint8ToString(month, datetime->month, TRUE);
                dayOfWeek = datetime->dayOfWeek;
                uint8ToString(dayOfMonth, datetime->dayOfMonth, TRUE);
                uint8ToString(year, datetime->year, TRUE);
            }

            switch (UI.Router.current())
            {
            case 0:

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
                    LCD.print(".");
                    LCD.print(month);
                    LCD.print(".");
                    LCD.print(year);
                    LCD.print(",");
                    LCD.print(days[dayOfWeek]);
                }
                UI.setRendered(TRUE);
                delay.ms(1000);
                break;

            case 1:
                UI.setRows(2);
                LCD.clear();
                for (int i = 0; i < 2; i++)
                {
                    LCD.setCursor(1, i);
                    LCD.print(route_home[i + UI.scrollbar()]);
                }

                LCD.setCursor(0, UI.pointer());
                LCD.printChar(1);

                break;

            case 2:
                UI.setRows(4);
                LCD.clear();
                for (int i = 0; i < 2; i++)
                {
                    LCD.setCursor(1, i);
                    LCD.print(route_menu[i + UI.scrollbar()]);
                }

                LCD.setCursor(0, UI.pointer());
                LCD.printChar(1);

                break;

            case 3:
                UI.setRows(4);
                LCD.clear();
                for (int i = 0; i < 2; i++)
                {
                    LCD.setCursor(1, i);
                    LCD.print(route_lcd[i + UI.scrollbar()]);
                }

                LCD.setCursor(0, UI.pointer());
                LCD.printChar(1);

                break;

            case 4:
                UI.setRows(3);
                LCD.clear();

                for (int i = 0; i < 2; i++)
                {
                    LCD.setCursor(1, i);
                    LCD.print(route_datetime[i + UI.scrollbar()]);
                }

                LCD.setCursor(0, UI.pointer());
                LCD.printChar(1);

                break;

            case 5:
                UI.setRows(4);
                LCD.clear();
                for (int i = 0; i < 2; i++)
                {
                    LCD.setCursor(1, i);
                    LCD.print(route_alarm[i + UI.scrollbar()]);
                }

                LCD.setCursor(0, UI.pointer());
                LCD.printChar(1);

                break;
            default:
                break;
            };
            UI.setWasRendered(TRUE);
        }
    }
}