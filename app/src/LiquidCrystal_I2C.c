#include "stm8s.h"
#include "LiquidCrystal_I2C.h"

uint8_t _lcd_address;
uint8_t _lcd_cols;
uint8_t _lcd_rows;
uint8_t _lcd_backlight;
uint8_t _lcd_displayfunction;
Pin *SDA;
Pin *SCL;
Pin *LED_BUILTIN;

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

const LiquidCrystal_I2C_Module LiquidCrystal_I2C = {
    .init = LiquidCrystal_I2C_Init,
    .expanderWrite = LiquidCrystal_I2C_ExpanderWrite,
    .write = LiquidCrystal_I2C_Write};

void LiquidCrystal_I2C_Init(uint8_t address, uint8_t cols, uint8_t rows)
{
    SDA = GPIO.init(GPIOE, PIN_2, OUTPUT_PP_HIGH_FAST);
    SCL = GPIO.init(GPIOE, PIN_1, OUTPUT_PP_HIGH_FAST);
    LED_BUILTIN = GPIO.init(GPIOC, PIN_5, OUTPUT_PP_LOW_SLOW);

    _lcd_address = address;
    _lcd_cols = cols;
    _lcd_rows = rows;
    _lcd_backlight = LCD_BACKLIGHT;
    _lcd_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    uint8_t clock_freq = CLK_GetClockFreq() / 1000000;
    I2C_Init(100000, _lcd_address, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, clock_freq);

    LiquidCrystal_I2C.write(_lcd_address, _lcd_backlight);
}

void LiquidCrystal_I2C_Write(uint8_t slave_address, uint8_t data)
{
    I2C_GenerateSTART(ENABLE);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    I2C_Send7bitAddress(slave_address, I2C_DIRECTION_TX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(data);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    I2C_GenerateSTOP(ENABLE);
}

void LiquidCrystal_I2C_ExpanderWrite(uint8_t data)
{
    return;
}