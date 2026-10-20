# LCDWinstar

Arduino library for Winstar I2C text LCD displays.

Implements the Winstar serial I2C protocol (control-byte + data-byte), which
differs from the PCF8574-based 4-bit parallel approach used by `LiquidCrystal_I2C`.
The public API mirrors `LiquidCrystal_I2C` so existing code requires minimal changes.

---

## I2C protocol

Winstar I2C LCDs use a direct serial interface.  Each transfer consists of a
**control byte** followed by one or more **data bytes**:

| Bit | Name | 0 | 1 |
|-----|------|---|---|
| 7   | C0   | Only one data byte follows | Next control word follows |
| 6   | A0   | Command (instruction register) | Data (DDRAM / CGRAM write) |

The display also requires `DB4 (DL) = 1` in the Function Set command to enable
the I2C interface (`LCD_I2C_MODE 0x10`).

---

## Contents

| Class | File | Description |
|---|---|---|
| `LCDWinstarI2C` | `src/LCDWinstarI2C.h` / `.cpp` | Low-level driver — mirrors the `LiquidCrystal_I2C` API |

---

## Usage

```cpp
#include <LCDWinstarI2C.h>

LCDWinstarI2C lcd(0x3C, 16, 2);  // address, cols, rows

void setup()
{
    lcd.init();             // initialises Wire and the display
    lcd.setCursor(0, 0);
    lcd.print("Hello, World!");
}

void loop() {}
```

### Constructor

```cpp
LCDWinstarI2C(uint8_t address, uint8_t cols, uint8_t rows);
```

`address` — 7-bit I2C address (typically `0x3C` for Winstar modules).

### init() / begin()

```cpp
void init();   // calls Wire.begin() then begin(cols, rows)

void begin(uint8_t cols, uint8_t rows,
           uint8_t dotSize = LCD_5x8DOTS,
           uint8_t initWire = true);
```

Use `init()` for normal setup (mirrors `LiquidCrystal_I2C`).
Use `begin()` directly when you need to override cols/rows or skip `Wire.begin()`
by passing `initWire = false`.

---

## API

| Method | Description |
|---|---|
| `clear()` | Clear display and return cursor home |
| `home()` | Return cursor to (0, 0) without clearing |
| `setCursor(col, row)` | Move cursor to column / row |
| `display()` / `noDisplay()` | Turn display on/off |
| `cursor()` / `noCursor()` | Show/hide underline cursor |
| `blink()` / `noBlink()` | Enable/disable blinking block cursor |
| `scrollDisplayLeft()` / `scrollDisplayRight()` | Scroll display contents |
| `leftToRight()` / `rightToLeft()` | Set text direction |
| `autoscroll()` / `noAutoscroll()` | Auto-shift display on each character write |
| `createChar(index, charmap[8])` | Define custom character (0–7) from SRAM |
| `createChar(index, F("..."))` | Define custom character from PROGMEM |
| `write(uint8_t)` | Write a single character (implements `Print`) |
| `command(uint8_t)` | Send a raw instruction byte |

---

## License

LGPL-2.1 — see [LICENSE](LICENSE).
