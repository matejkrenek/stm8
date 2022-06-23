#include "LCD.h"
#include "Serial.h"

uint8_t _lcd_address;
uint8_t _lcd_cols;
uint8_t _lcd_rows;
uint8_t _lcd_backlight;
uint8_t _lcd_cursor;
uint8_t _lcd_blink;
uint8_t _lcd_displayfunction;
uint8_t _lcd_displaycontrol;
uint8_t _lcd_displaymode;
Pin *SDA;
Pin *SCL;

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
    .getBacklight = LiquidCrystal_I2C_GetBacklight,
    .createChar = LiquidCrystal_I2C_CreateChar,
    .setCursor = LiquidCrystal_I2C_SetCursor,
    .getCursor = LiquidCrystal_I2C_GetCursor,
    .noCursor = LiquidCrystal_I2C_NoCursor,
    .cursor = LiquidCrystal_I2C_Cursor,
    .getBlink = LiquidCrystal_I2C_GetBlink,
    .noBlink = LiquidCrystal_I2C_NoBlink,
    .blink = LiquidCrystal_I2C_Blink,
    .print = LiquidCrystal_I2C_Print,
    .printChar = LiquidCrystal_I2C_PrintChar,
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

    I2C.init(100000, 0x00, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK.getFrequency() / 1000000);
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
    _lcd_displaycontrol = LCD_DISPLAYON;
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

    I2C.start();

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    I2C.sendAddress(slave_address, I2C_DIRECTION_TX);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    I2C.sendData(data);

    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;

    I2C.stop();
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

bool LiquidCrystal_I2C_GetBacklight()
{
    return _lcd_backlight == LCD_BACKLIGHT;
}

void LiquidCrystal_I2C_CreateChar(uint8_t location, uint8_t charmap[])
{

    location &= 0x7;
    LCD.command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++)
    {
        LCD.send(charmap[i], Rs);
    }
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

bool LiquidCrystal_I2C_GetCursor()
{
    return _lcd_cursor == LCD_CURSORON;
}

void LiquidCrystal_I2C_NoCursor()
{
    _lcd_cursor = LCD_CURSOROFF;
    _lcd_displaycontrol &= ~LCD_CURSORON;
    LCD.command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

void LiquidCrystal_I2C_Cursor()
{
    _lcd_cursor = LCD_CURSORON;
    _lcd_displaycontrol |= _lcd_cursor;
    LCD.command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

bool LiquidCrystal_I2C_GetBlink()
{
    return _lcd_blink == LCD_BLINKON;
}

void LiquidCrystal_I2C_NoBlink()
{
    _lcd_blink = LCD_BLINKOFF;
    _lcd_displaycontrol &= ~LCD_BLINKON;
    LCD.command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

void LiquidCrystal_I2C_Blink()
{
    _lcd_blink = LCD_BLINKON;
    _lcd_displaycontrol |= _lcd_blink;
    LCD.command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

void LiquidCrystal_I2C_Print(char *string)
{
    while (*string)
    {
        LCD.send(*string, Rs);
        string++;
    }
}

void LiquidCrystal_I2C_PrintChar(uint8_t character)
{
    LCD.send(character, Rs);
}
