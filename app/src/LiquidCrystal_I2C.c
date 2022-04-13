#include "stm8s.h"
#include "LiquidCrystal_I2C.h"

uint8_t _lcd_address;
uint8_t _lcd_cols;
uint8_t _lcd_rows;
uint8_t _lcd_backlight;
uint8_t _lcd_displayfunction;
uint8_t _lcd_displaycontrol;
uint8_t _lcd_displaymode;
Pin *SDA;
Pin *SCL;

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
LiquidCrystal_I2C_Module LCD = {
    .init = LiquidCrystal_I2C_Init,
    .expanderWrite = LiquidCrystal_I2C_ExpanderWrite,
    .write = LiquidCrystal_I2C_Write,
    .write4bits = LiquidCrystal_I2C_Write4Bits,
    .pulseEnable = LiquidCrystal_I2C_PulseEnable,
    .command = LiquidCrystal_I2C_Command,
    .send = LiquidCrystal_I2C_Send,
    .display = LiquidCrystal_I2C_Display,
    .clear = LiquidCrystal_I2C_Clear,
    .home = LiquidCrystal_I2C_Home,
    .noBacklight = LiquidCrystal_I2C_NoBacklight,
    .backlight = LiquidCrystal_I2C_Backlight,
    .setCursor = LiquidCrystal_I2C_SetCursor,
    .print = LiquidCrystal_I2C_Print,
};

void LiquidCrystal_I2C_Init(uint8_t address, uint8_t cols, uint8_t rows)
{
    _lcd_address = address << 1;
    _lcd_cols = cols;
    _lcd_rows = rows;
    _lcd_backlight = LCD_BACKLIGHT;
    _lcd_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

    SDA = GPIO.init(GPIOE, PIN_2, OUTPUT_PP_HIGH_FAST);
    SCL = GPIO.init(GPIOE, PIN_1, OUTPUT_PP_HIGH_FAST);

    I2C.deinit();
    I2C.init(100000, _lcd_address, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
    I2C.enable();

    delay.ms(50);
    LCD.expanderWrite(_lcd_backlight);
    delay.ms(1000);
    LCD.write4bits(0x03 << 4);
    delay.us(4500);
    LCD.write4bits(0x03 << 4);
    delay.us(4500);
    LCD.write4bits(0x03 << 4);
    delay.us(150);
    LCD.write4bits(0x02 << 4);
    LCD.command(LCD_FUNCTIONSET | _lcd_displayfunction);
    _lcd_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    LCD.display();
    LCD.clear();
    _lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    LCD.command(LCD_ENTRYMODESET | _lcd_displaymode);
    LCD.home();
}

void LiquidCrystal_I2C_Write(uint8_t slave_address, uint8_t data)
{
    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
        ;

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
    LCD.write(_lcd_address, data | _lcd_backlight);
}

void LiquidCrystal_I2C_Write4Bits(uint8_t data)
{
    LCD.expanderWrite(data);
    LCD.pulseEnable(data);
}

void LiquidCrystal_I2C_PulseEnable(uint8_t data)
{
    LCD.expanderWrite(data | En);
    delay.us(1);

    LCD.expanderWrite(data & ~En);
    delay.us(50);
}

void LiquidCrystal_I2C_Command(uint8_t data)
{
    LCD.send(data, 0);
}

void LiquidCrystal_I2C_Send(uint8_t data, uint8_t mode)
{
    uint8_t highnib = data & 0xf0;
    uint8_t lownib = (data << 4) & 0xf0;
    LCD.write4bits(highnib | mode);
    LCD.write4bits(lownib | mode);
}

void LiquidCrystal_I2C_Display()
{
    _lcd_displaycontrol |= LCD_DISPLAYON;
    LCD.command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

void LiquidCrystal_I2C_Clear()
{
    LCD.command(LCD_CLEARDISPLAY);
    delay.us(2000);
}

void LiquidCrystal_I2C_Home()
{
    LCD.command(LCD_RETURNHOME);
    delay.us(2000);
}

void LiquidCrystal_I2C_NoBacklight()
{
    _lcd_backlight = LCD_NOBACKLIGHT;
    LCD.expanderWrite(0);
}

void LiquidCrystal_I2C_Backlight()
{
    _lcd_backlight = LCD_BACKLIGHT;
    LCD.expanderWrite(0);
}

void LiquidCrystal_I2C_SetCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};

    if (row > _lcd_rows)
    {
        row = _lcd_rows - 1;
    }

    LCD.command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LiquidCrystal_I2C_Print(uint8_t *string)
{
    while (*string)
    {
        LCD.send(*string, Rs);
        string++;
    }
}
