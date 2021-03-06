# HOW TO INCLUDE IN ESP-IDF PROJECT?

Simply write `set(EXTRA_COMPONENT_DIRS "/path/to/the/folder/where/is/lcd_i2c/folder")` (/path/to/the/folder/where/is/lcd_i2c/folder) = parent folder of lcd_i2c lbrary ---- to the CMakeLists.txt

### BE CAREFUL! Write it AFTER `cmake_minimum_required(VERSION x.x)` but BEFORE `include($ENV{IDF_PATH}/tools/cmake/project.cmake)`

Then add
```c
#include "lcd_i2c.h"
```
in the project main.c or wherever you need to use it

## HOW TO USE
#### 1. Init I2C
```c
lcd1602_i2c_init(18, 19, 0);  //(SDA, SCL, I2C_Channel)
```
#### 2. Create and initialize structure
```c
lcd1602_t lcdtmp = {0};
```
#### 3. Set address and backlight on/off switch
```c
lcdtmp.i2caddr = 0x27;  //(0x27) is the default of lcd1602
lcdtmp.backlight = 1;
```
#### 4. Set display on/off, cursor on/off, blink on/off
```c
lcd1602_dcb_set(&lcdtmp, 1, 0, 0);
```
#### 5. Init display (Send display init sequence)

```c
lcd1602_init(&lcdtmp);
```

#### 6. Use it!

## FUNCTION DESCRIPTIONS

```c
void lcd1602_send_string(lcd1602_t *lcd, char *str);
```
Sends string to the lcd specified by lcd1602_t struct

```c
void lcd1602_create_char(lcd1602_t *lcd, uint8_t location, uint8_t *charmap);
```
Used to create custom chars (Up to 8 memory space available when running 5x8 mode)
`*lcd` - pointer to lcd,
`location` - location in memory (0-7),
`*charmap` - uint8_t[8] array where each index is one row in character bank, (for example 0b010111 -> 0 == block off, 1 == block on), see example code

```c
void lcd1602_update_backlight(lcd1602_t *lcd);
```
Used for on/off backlight (before call, set lcd.backlight to either `true` or `false`)

```c
void lcd1602_dcb_set(lcd1602_t *lcd, bool displayon, bool cursoron, bool blinkon);
```
Sets the dcb bits in `lcd1602_t` structure, to apply changes call `lcd1602_dcb_update`

```c
void lcd1602_dcb_update(lcd1602_t *lcd);
```
Updates and applies changes from lcd->dcb to display

```c
void lcd1602_entry_set(lcd1602_t *lcd, bool direction, bool shift);
```
Sets the entry bits in lcd1602_t structure, `direction` -> 0 = right, 1 = left, `shift` -> 0 = Entry shift decrement, 1 = Entry shift increment

```c
void lcd1602_entry_update(lcd1602_t *lcd);
```
Updates and applies entry bits from lcd1602_t to display

```c
void lcd1602_jumptohome(lcd1602_t *lcd);
```
Jumps to home and sets cursor to the original position if shifted

```c
void lcd1602_clear(lcd1602_t *lcd);
```
Clears whole DDRAM (Whole display memory, so nothing will show up, even if shifted)

```c
void lcd1602_rl_shift(lcd1602_t *lcd, uint8_t cur_display, uint8_t direction);
```
Shifts display (showing of DDRAM contents) or cursor to either left or right
`lcd` - pointer to lcd1602_t struct
`cur_display` - cursor (`LCD1602_CURSORMOVE`) or display (`LCD1602_DISPLAYMOVE`) shift select
`direction` - direction select: left (`LCD1602_MOVELEFT`), right (`LCD1602_MOVERIGHT`)

```c
void lcd1602_set_pos(lcd1602_t *lcd, uint8_t row, uint8_t col);
```
Sets position of cursor in DDRAM space
`lcd` - pointer to the lcd1602_t struct
`row` - row 0 (first) or 1 (second)
`col` - column select, can be more than 16 because DDRAM is bigger than the display (80 * 8 bits register = 80 characters = 40 chars on one line)

## EXAMPLE PROJECT
Here is example project repo -> https://github.com/mvarchdev/lcd1602_test
