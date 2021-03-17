# HOW TO INCLUDE IN ESP-IDF PROJECT?

Simply write `set(EXTRA_COMPONENT_DIRS "/path/to/the/folder/where/is/lcd_i2c/folder")` (/path/to/the/folder/where/is/lcd_i2c/folder) = parent folder of lcd_i2c lbrary

### BE CAREFUL! Write it AFTER cmake_minimum_required(VERSION x.x) but BEFORE include($ENV{IDF_PATH}/tools/cmake/project.cmake)

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
#### 5. Use it!

## FUNCTION DESCRIPTIONS
