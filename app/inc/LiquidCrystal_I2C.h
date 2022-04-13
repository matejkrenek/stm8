#ifndef __LiquidCrystal_I2C_H
#define __LiquidCrystal_I2C_H

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04 // Enable bit
#define Rw 0x02 // Read/Write bit
#define Rs 0x01 // Register select bit

typedef struct
{
    void (*init)(uint8_t, uint8_t, uint8_t);
    void (*expanderWrite)(uint8_t);
    void (*write)(uint8_t, uint8_t);
    void (*write4bits)(uint8_t);
    void (*pulseEnable)(uint8_t);
    void (*command)(uint8_t);
    void (*send)(uint8_t, uint8_t);
    void (*display)();
    void (*clear)();
    void (*home)();
    void (*noBacklight)();
    void (*backlight)();
    void (*setCursor)(uint8_t, uint8_t);
    void (*print)(uint8_t *);

} LiquidCrystal_I2C_Module;

extern uint8_t _lcd_address;
extern uint8_t _lcd_cols;
extern uint8_t _lcd_rows;
extern uint8_t _lcd_backlight;
extern uint8_t _lcd_displayfunction;

void LiquidCrystal_I2C_Init(uint8_t address, uint8_t cols, uint8_t rows);
void LiquidCrystal_I2C_ExpanderWrite(uint8_t data);
void LiquidCrystal_I2C_Write(uint8_t slave_address, uint8_t data);
void LiquidCrystal_I2C_Write4Bits(uint8_t data);
void LiquidCrystal_I2C_PulseEnable(uint8_t data);
void LiquidCrystal_I2C_Command(uint8_t data);
void LiquidCrystal_I2C_Send(uint8_t data, uint8_t mode);
void LiquidCrystal_I2C_Display();
void LiquidCrystal_I2C_Clear();
void LiquidCrystal_I2C_Home();
void LiquidCrystal_I2C_NoBacklight();
void LiquidCrystal_I2C_Backlight();
void LiquidCrystal_I2C_SetCursor(uint8_t col, uint8_t row);
void LiquidCrystal_I2C_Print(uint8_t *string);
extern const LiquidCrystal_I2C_Module LCD;

#endif