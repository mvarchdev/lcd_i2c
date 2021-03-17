HOW TO INCLUDE IN ESP-IDF PROJECT?

Simply write `set(EXTRA_COMPONENT_DIRS "/path/to/the/folder/where/is/lcd_i2c/folder")` (/path/to/the/folder/where/is/lcd_i2c/folder) = parent folder of lcd_i2c lbrary

BE CAREFUL! Write it AFTER cmake_minimum_required(VERSION x.x) but BEFORE include($ENV{IDF_PATH}/tools/cmake/project.cmake)

Then add
```cpp
#include "lcd_i2c.h"
```
in the project main.c or wherever you need to use it
