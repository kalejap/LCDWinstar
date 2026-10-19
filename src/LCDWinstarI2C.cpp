// Based on the work by DFRobot

#include "LCDWinstarI2C.h"
#include <inttypes.h>

// States of A0, C0 bits of control byte
#define A0_CMD 0b00000000  // A0 bit is cleared: command (instruction register)
#define A0_RAM 0b01000000  // A0 bit is set: data (RAM write)

#define C0_ONLY_DATABYTE_FOLLOWS 0b00000000 // CO bit is cleared (only datat byte follows)
#define C0_NEXT_CW_FOLLOWS       0b10000000 // C0 bit is set (next control word follows)

// This class is intended for Winstar LCD with I2C. Bit DB4 (DL) must be set to 1
#define LCD_I2C_MODE 0x10

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#define WriteI2C(args) Wire.write(args)
inline size_t LCDWinstarI2C::write(uint8_t value)
{
    send(value, A0_RAM);
    return 1;
}

#else
#include "WProgram.h"

#define WriteI2C(args) Wire.send(args)
inline void LCDWinstarI2C::write(uint8_t value)
{
    send(value, A0_RAM);
}

#endif
#include "Wire.h"

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

LCDWinstarI2C::LCDWinstarI2C(uint8_t address)
{
    _address = address;
}

void LCDWinstarI2C::init(uint8_t address)
{
    _address = address;
}

void LCDWinstarI2C::begin(uint8_t cols, uint8_t rows,
    uint8_t dotSize, uint8_t initWire)
{
    _cols = cols;
    _rows = rows;

    _displayFunction = LCD_I2C_MODE | LCD_1LINE | LCD_5x8DOTS;

    // turn the display on with no cursor or blinking default
    _displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    
    // Initialize to default text direction (for roman languages)
    _displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    
    if (rows > 1)
        _displayFunction |= LCD_2LINE;
    
    // for some 1 line displays you can select a 10 pixel high font
    if ((dotSize != 0) && (rows == 1))
        _displayFunction |= LCD_5x10DOTS;

    if (initWire)
        Wire.begin();

    // See section 4.Initializing of LCM
    // Wait for more than 15 ms after VDD rises 4.5V (so we'll wait 20 ms)
    delay(20); 

    // set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayFunction);
    
    // Wait for more than 100 us (We'll wait 150 us)
    delayMicroseconds(150);

    display();

    // Wait for more than 100 us (We'll wait 150 us)
    delayMicroseconds(150);
    
    // clear it off
    clear();

    // Wait for more than 100 us (We'll wait 150 us)
    delayMicroseconds(150);
    
    // set the entry mode
    command(LCD_ENTRYMODESET | _displayMode);

    home();
}

/********** high level commands, for the user! */
void LCDWinstarI2C::clear()
{
    // Clear all the display data by writing "20H" (space code) to all DDRAM
    // address, and set DDRAM address to "00H" into AC (address counter).
    // Return cursor to the original status.
    command(LCD_CLEARDISPLAY);
    delayMicroseconds(800); // According to datasheet 0.76 ms (760 us)
}

void LCDWinstarI2C::home(){
    // Return Home is cursor return home instruction. Set DDRAM address to "00H"
    // into the address counter. Return cursor to its original site and return
    // display to its original status, if shifted. A content of DDRAM
    // does not change.
    command(LCD_RETURNHOME);
    delayMicroseconds(800); // According to datasheet 0.76 ms (760 us)
}

void LCDWinstarI2C::setCursor(uint8_t col, uint8_t row)
{
    uint8_t rowOffsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row >= _rows)
        row = _rows - 1;

    command(LCD_SETDDRAMADDR | (col + rowOffsets[row]));
}

// Turn the display on/off (quickly)
void LCDWinstarI2C::noDisplay()
{
    _displayControl &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

void LCDWinstarI2C::display()
{
    _displayControl |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

// Turns the underline cursor on/off
void LCDWinstarI2C::noCursor()
{
    _displayControl &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}
void LCDWinstarI2C::cursor()
{
    _displayControl |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

// Turn on and off the blinking cursor
void LCDWinstarI2C::noBlink()
{
    _displayControl &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

void LCDWinstarI2C::blink()
{
    _displayControl |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displayControl);
}

// These commands scroll the display without changing the RAM
void LCDWinstarI2C::scrollDisplayLeft(void)
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCDWinstarI2C::scrollDisplayRight(void)
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCDWinstarI2C::leftToRight(void)
{
    _displayMode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// This is for text that flows Right to Left
void LCDWinstarI2C::rightToLeft(void)
{
    _displayMode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// This will 'right justify' text from the cursor
void LCDWinstarI2C::autoscroll(void)
{
    _displayMode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// This will 'left justify' text from the cursor
void LCDWinstarI2C::noAutoscroll(void)
{
    _displayMode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displayMode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCDWinstarI2C::createChar(uint8_t location, const uint8_t charmap[8])
{
    location &= 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    
    write(charmap, 8);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters using data from program memory
void LCDWinstarI2C::createChar(uint8_t location, const __FlashStringHelper *ifsh)
{
    location &= 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    
    write(ifsh, 8);
}


/*********** mid level commands, for sending data/cmds */

inline void LCDWinstarI2C::command(uint8_t value)
{
    send(value, A0_CMD);
}


/************ low level function **********/

size_t LCDWinstarI2C::write(const uint8_t *buffer, size_t size)
{
    Wire.beginTransmission(_address);

    // Last word command
    WriteI2C(A0_RAM | C0_ONLY_DATABYTE_FOLLOWS);
    
    size_t written = 0;
    while (size--) 
    {
        WriteI2C(*(buffer++));
        written++;
    }
    
    Wire.endTransmission();

    delay(20);
    
    return written;
}

// Read data from program memory (flash) and write it to LCD RAM (DDRAM or
// CGRAM)
size_t LCDWinstarI2C::write(const __FlashStringHelper *ifsh, size_t size)
{
    Wire.beginTransmission(_address);

    // Last word command
    WriteI2C(A0_RAM | C0_ONLY_DATABYTE_FOLLOWS);
    
    PGM_P p = reinterpret_cast<PGM_P>(ifsh);
    size_t written = 0;
    if (size) 
    {
        while (size--) 
        {
            unsigned char c = pgm_read_byte(p++);
            WriteI2C(c);
            written++;
        }
    }
    else
    {
        while (1) {
            unsigned char c = pgm_read_byte(p++);
            if (c == 0)
                break;
            WriteI2C(c);
            written++;
        }
    }
    
    Wire.endTransmission();

    delay(20);
    
    return written;
}


// write either command or data
uint8_t LCDWinstarI2C::send(uint8_t value, uint8_t mode)
{
    Wire.beginTransmission(_address);

    // word command
    WriteI2C(mode | C0_ONLY_DATABYTE_FOLLOWS); // control byte
    WriteI2C(value); // data byte
    
    uint8_t stat = Wire.endTransmission();

    return stat;
}
