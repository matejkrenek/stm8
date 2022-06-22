#include "stm8s.h"
#include "Serial.h"
#include "DSRTC.h"
#include "AT24C.h"
#include "UI.h"
#include "Encoder.h"
#include "LCD.h"

uint8_t hours[20];
uint8_t minutes[20];
uint8_t seconds[20];
uint8_t month[20];
uint8_t dayOfMonth[20];
uint8_t year[20];

DateTime *datetime;

uint8_t route_home[][20] = {"Settings", "Back"};
uint8_t route_menu[][20] = {"LCD", "Datetime", "Alarm", "Back"};
uint8_t route_lcd[][20] = {"Backlight: ", "Cursor: ", "Blink: ", "Back"};
uint8_t route_datetime[][20] = {"Time: ", "Date: ", "Back"};
uint8_t route_alarm[][20] = {"Every: ", "Volume: ", "Freq: ", "Back"};

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
    if (UI.editing() == -1)
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
                    uint8ToString(hours, datetime->hours, TRUE);

                    LCD.setCursor(strlen(route_datetime[0]) + 1, UI.pointer());
                    LCD.print(hours);
                    LCD.setCursor(strlen(route_datetime[0]) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->minutes != 59)
                    {
                        datetime->minutes++;
                    }
                    uint8ToString(minutes, datetime->minutes, TRUE);

                    LCD.setCursor(strlen(route_datetime[0]) + 4, UI.pointer());
                    LCD.print(minutes);
                    LCD.setCursor(strlen(route_datetime[0]) + 5, UI.pointer());

                    break;
                case 2:
                    if (datetime->seconds != 59)
                    {
                        datetime->seconds++;
                    }
                    uint8ToString(seconds, datetime->seconds, TRUE);

                    LCD.setCursor(strlen(route_datetime[0]) + 7, UI.pointer());
                    LCD.print(seconds);
                    LCD.setCursor(strlen(route_datetime[0]) + 8, UI.pointer());

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
                    uint8ToString(dayOfMonth, datetime->dayOfMonth, TRUE);

                    LCD.setCursor(strlen(route_datetime[1]) + 1, UI.pointer());
                    LCD.print(dayOfMonth);
                    LCD.setCursor(strlen(route_datetime[1]) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->month != 12)
                    {
                        datetime->month++;
                    }
                    uint8ToString(month, datetime->month, TRUE);

                    LCD.setCursor(strlen(route_datetime[1]) + 4, UI.pointer());
                    LCD.print(month);
                    LCD.setCursor(strlen(route_datetime[1]) + 5, UI.pointer());

                    break;
                case 2:
                    if (datetime->year != 99)
                    {
                        datetime->year++;
                    }
                    uint8ToString(year, datetime->year, TRUE);

                    LCD.setCursor(strlen(route_datetime[1]) + 7, UI.pointer());
                    LCD.print(year);
                    LCD.setCursor(strlen(route_datetime[1]) + 8, UI.pointer());

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
    if (UI.editing() == -1)
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
                    uint8ToString(hours, datetime->hours, TRUE);

                    LCD.setCursor(strlen(route_datetime[0]) + 1, UI.pointer());
                    LCD.print(hours);
                    LCD.setCursor(strlen(route_datetime[0]) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->minutes != 0)
                    {
                        datetime->minutes--;
                    }
                    uint8ToString(minutes, datetime->minutes, TRUE);

                    LCD.setCursor(strlen(route_datetime[0]) + 4, UI.pointer());
                    LCD.print(minutes);
                    LCD.setCursor(strlen(route_datetime[0]) + 5, UI.pointer());
                    break;
                case 2:
                    if (datetime->seconds != 0)
                    {
                        datetime->seconds--;
                    }

                    uint8ToString(seconds, datetime->seconds, TRUE);

                    LCD.setCursor(strlen(route_datetime[0]) + 7, UI.pointer());
                    LCD.print(seconds);
                    LCD.setCursor(strlen(route_datetime[0]) + 8, UI.pointer());
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
                    uint8ToString(dayOfMonth, datetime->dayOfMonth, TRUE);

                    LCD.setCursor(strlen(route_datetime[1]) + 1, UI.pointer());
                    LCD.print(dayOfMonth);
                    LCD.setCursor(strlen(route_datetime[1]) + 2, UI.pointer());

                    break;
                case 1:
                    if (datetime->month != 0)
                    {
                        datetime->month--;
                    }
                    uint8ToString(month, datetime->month, TRUE);

                    LCD.setCursor(strlen(route_datetime[1]) + 4, UI.pointer());
                    LCD.print(month);
                    LCD.setCursor(strlen(route_datetime[1]) + 5, UI.pointer());
                    break;
                case 2:
                    if (datetime->year != 0)
                    {
                        datetime->year--;
                    }
                    uint8ToString(year, datetime->year, TRUE);

                    LCD.setCursor(strlen(route_datetime[1]) + 7, UI.pointer());
                    LCD.print(year);
                    LCD.setCursor(strlen(route_datetime[1]) + 8, UI.pointer());
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
        case 2:
            UI.redirect(5);
            break;
        default:
            UI.redirect(1);
            break;
        }
        break;
    case 3:
        Serial.write("franta");

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
            Serial.write("readed: %u", AT24C.read(0x0000));
            LCD.clear();
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
                LCD.setCursor(strlen(route_datetime[0]) + 2, UI.pointer());
                UI.edit(0);
                break;
            case 0:
                LCD.setCursor(strlen(route_datetime[0]) + 5, UI.pointer());
                UI.edit(1);
                break;
            case 1:
                LCD.setCursor(strlen(route_datetime[0]) + 8, UI.pointer());
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
                LCD.setCursor(strlen(route_datetime[1]) + 2, UI.pointer());

                UI.edit(0);
                break;
            case 0:
                LCD.setCursor(strlen(route_datetime[1]) + 5, UI.pointer());
                UI.edit(1);
                break;
            case 1:
                LCD.setCursor(strlen(route_datetime[1]) + 8, UI.pointer());

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
}

void main(void)
{
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    I2C.init(100000, 0x00, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK.getFrequency() / 1000000);
    I2C.enable();
    // LCD.init(0x26, 16, 2);
    // DSRTC.init(0x68);
    AT24C.init(0x50, 32);
    // Encoder.init(onScrollUp, onScrollDown, onClick);
    // UI.init();

    while (1)
    {
        Serial.write("read: %u", AT24C.read(0x0000));
        delay.ms(1000);
        // if (UI.Router.current() == 0 || UI.Router.current() != UI.Router.previous())
        // {
        //     UI.setWasRendered(FALSE);
        // }

        // if (!UI.wasRendered() || UI.rendered())
        // {

        //     if (UI.Router.current() == 0 || UI.Router.current() == 4)
        //     {
        //         datetime = DSRTC.dateTime();
        //         uint8ToString(hours, datetime->hours, TRUE);
        //         uint8ToString(minutes, datetime->minutes, TRUE);
        //         uint8ToString(seconds, datetime->seconds, TRUE);
        //         uint8ToString(month, datetime->month, TRUE);
        //         uint8ToString(dayOfMonth, datetime->dayOfMonth, TRUE);
        //         uint8ToString(year, datetime->year, TRUE);
        //     }

        //     switch (UI.Router.current())
        //     {
        //     case 0:

        //         if (!UI.rendered())
        //         {
        //             LCD.clear();
        //         }
        //         LCD.setCursor(0, 0);
        //         LCD.printChar(2);
        //         LCD.print(hours);
        //         LCD.print(":");
        //         LCD.print(minutes);
        //         LCD.print(":");
        //         LCD.print(seconds);
        //         if (!UI.rendered())
        //         {
        //             LCD.setCursor(0, 1);
        //             LCD.printChar(3);
        //             LCD.print(dayOfMonth);
        //             LCD.print("/");
        //             LCD.print(month);
        //             LCD.print("/");
        //             LCD.print(year);
        //         }
        //         UI.setRendered(TRUE);
        //         delay.ms(1000);

        //         break;

        //     case 1:
        //         UI.setRows(2);
        //         LCD.clear();
        //         for (int i = 0; i < 2; i++)
        //         {
        //             LCD.setCursor(1, i);
        //             LCD.print(route_home[i + UI.scrollbar()]);
        //         }

        //         LCD.setCursor(0, UI.pointer());
        //         LCD.printChar(1);

        //         UI.setWasRendered(TRUE);
        //         break;

        //     case 2:
        //         UI.setRows(4);
        //         LCD.clear();
        //         for (int i = 0; i < 2; i++)
        //         {
        //             LCD.setCursor(1, i);
        //             LCD.print(route_menu[i + UI.scrollbar()]);
        //         }

        //         LCD.setCursor(0, UI.pointer());

        //         LCD.printChar(1);

        //         UI.setWasRendered(TRUE);
        //         break;

        //     case 3:
        //         UI.setRows(4);
        //         LCD.clear();
        //         for (int i = 0; i < 2; i++)
        //         {
        //             LCD.setCursor(1, i);
        //             if (UI.rows() - UI.scrollbar() == 2 && i == 1)
        //             {
        //                 LCD.print(route_lcd[i + UI.scrollbar()]);
        //             }
        //             else
        //             {
        //                 switch (i + UI.scrollbar())
        //                 {
        //                 case 0:
        //                     UI.boolean(route_lcd[i + UI.scrollbar()], LCD.getBacklight());
        //                     break;
        //                 case 1:
        //                     UI.boolean(route_lcd[i + UI.scrollbar()], LCD.getCursor());
        //                     break;
        //                 case 2:
        //                     UI.boolean(route_lcd[i + UI.scrollbar()], LCD.getBlink());
        //                     break;

        //                 default:
        //                     break;
        //                 }
        //             }
        //         }
        //         LCD.setCursor(0, UI.pointer());
        //         LCD.printChar(1);

        //         LCD.setCursor(strlen(route_lcd[UI.pointer() + UI.scrollbar()]) + 1, UI.pointer());

        //         UI.setWasRendered(TRUE);
        //         break;

        //     case 4:
        //         UI.setRows(3);
        //         LCD.clear();

        //         for (int i = 0; i < 2; i++)
        //         {
        //             LCD.setCursor(1, i);
        //             if (UI.rows() - UI.scrollbar() == 2 && i == 1)
        //             {
        //                 LCD.print(route_datetime[i + UI.scrollbar()]);
        //             }
        //             else
        //             {
        //                 switch (i + UI.scrollbar())
        //                 {
        //                 case 0:
        //                     UI.timepicker(route_datetime[i + UI.scrollbar()], hours, minutes, seconds);
        //                     break;
        //                 case 1:
        //                     UI.datepicker(route_datetime[i + UI.scrollbar()], dayOfMonth, month, year);
        //                     break;
        //                 default:
        //                     break;
        //                 }
        //             }
        //         }

        //         LCD.setCursor(0, UI.pointer());
        //         LCD.printChar(1);

        //         UI.setWasRendered(TRUE);

        //         break;

        //     case 5:
        //         UI.setRows(4);
        //         LCD.clear();
        //         for (int i = 0; i < 2; i++)
        //         {
        //             LCD.setCursor(1, i);
        //             LCD.print(route_alarm[i + UI.scrollbar()]);
        //         }

        //         LCD.setCursor(0, UI.pointer());
        //         LCD.printChar(1);

        //         UI.setWasRendered(TRUE);

        //         break;
        //     default:
        //         break;
        //     };
        // }
    }
}