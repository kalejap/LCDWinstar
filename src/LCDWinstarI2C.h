#ifndef LCDWinstarI2C_h
#define LCDWinstarI2C_h

#include <inttypes.h>
#include "Print.h"

// commands
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
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LCDWinstarI2C : public Print {
public:
    LCDWinstarI2C(uint8_t address);
        
    void init(uint8_t address);

    void begin(uint8_t cols, uint8_t rows,
        uint8_t dotSize = LCD_5x8DOTS, uint8_t initWire = true);

    void clear();
    void home();

    void noDisplay();
    void display();
    void noBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void leftToRight();
    void rightToLeft();
    void autoscroll();
    void noAutoscroll();

    void createChar(uint8_t, const uint8_t[8]);
    void createChar(uint8_t, const __FlashStringHelper *);
    void setCursor(uint8_t, uint8_t); 
    void command(uint8_t);

    // Print overrides
    size_t write(uint8_t) override;
    size_t write(const uint8_t *buffer, size_t size) override;
    
    // New method to write a string from program memory
    size_t write(const __FlashStringHelper *, size_t size = 0);

    using Print::write;
private:
    uint8_t send(uint8_t, uint8_t);

    uint8_t m_address;     // Address of I2C device
    uint8_t m_cols;        // Number of columns
    uint8_t m_rows;        // Number of rows
    uint8_t m_displayFunction;
    uint8_t m_displayControl;
    uint8_t m_displayMode;
};

#endif