
#include <stdio.h>
#include "driver/i2c.h"
#include <unistd.h>
#include "lcd1602_lib.h"

#define I2C_MASTER_TX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ         100000            /*!< I2C master clock frequency */

#define I2C_ACK_CHECK_EN                       (0x1)              /*!< I2C master will check ack from slave*/
#define I2C_ACK_CHECK_DIS                      (0x0)              /*!< I2C master will not check ack from slave */

#define LCD1602_GPIO_SDA	18
#define LCD1602_GPIO_SCL	19

#define LCD1602_CLEARDISPLAY (0x01)
#define LCD1602_RETURNHOME (0x02)
#define LCD1602_ENTRYMODESET (0x04)
#define LCD1602_DISPLAYCONTROL (0x08)
#define LCD1602_CURSORSHIFT (0x10)
#define LCD1602_FUNCTIONSET (0x20)
#define LCD1602_SETCGRAMADDR (0x40)
#define LCD1602_SETDDRAMADDR (0x80)

// flags for display entry mode
#define LCD1602_ENTRYRIGHT (0x00)
#define LCD1602_ENTRYLEFT (0x02)
#define LCD1602_ENTRYSHIFTINCREMENT (0x01)
#define LCD1602_ENTRYSHIFTDECREMENT (0x00)

// flags for display on/off control
#define LCD1602_DISPLAYON (0x04)
#define LCD1602_DISPLAYOFF (0x00)
#define LCD1602_CURSORON (0x02)
#define LCD1602_CURSOROFF (0x00)
#define LCD1602_BLINKON (0x01)
#define LCD1602_BLINKOFF (0x00)

// flags for function set
#define LCD1602_8BITMODE (0x10)
#define LCD1602_4BITMODE (0x00)
#define LCD1602_2LINE (0x08)
#define LCD1602_1LINE (0x00)
#define LCD1602_5x10DOTS 0x04
#define LCD1602_5x8DOTS 0x00

// flags for backlight control
#define LCD1602_BACKLIGHT (0x08)
#define LCD1602_NOBACKLIGHT (0x00)

#define LCD1602_EN 0b00000100  // Enable bit
#define LCD1602_RW 0b00000010  // Read/Write bit
#define LCD1602_RS 0b00000001  // Register select bit


int lcd1602_exp_write_data(uint8_t addr, uint8_t data[], int len) //write bytes
{
    i2c_cmd_handle_t hCmd = i2c_cmd_link_create();
    i2c_master_start(hCmd);
    i2c_master_write_byte(hCmd, (addr << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN);
    i2c_master_write(hCmd, data, len, I2C_ACK_CHECK_EN);
    i2c_master_stop(hCmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, hCmd, 0);
    i2c_cmd_link_delete(hCmd);

    //for (int x = 0; x < len; x++)
    //    printf("lcd1602_exp_write_data: [0x%x]\n", data[x]);
    if (ret != ESP_OK)
        printf("lcd1602_write_data Error [0x%x]\n", data[0]);

    return ret;
}

int lcd1602_exp_write_byte(uint8_t addr, uint8_t byte)
{
    i2c_cmd_handle_t hCmd = i2c_cmd_link_create();
    i2c_master_start(hCmd);
    i2c_master_write_byte(hCmd, (addr << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN);
    i2c_master_write_byte(hCmd, byte, I2C_ACK_CHECK_EN);
    i2c_master_stop(hCmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, hCmd, 0);
    i2c_cmd_link_delete(hCmd);
    //printf("lcd1602_exp_write_byte: [0x%x]\n", byte);
    //if (ret != ESP_OK)
    //    printf("lcd1602_write_cmd Error [0x%x]\n", byte);

    return ret;
}

void lcd1602_backlight(lcd1602_t *lcd, uint8_t *sendb, uint8_t flags)
{
    *sendb = (*sendb) | (lcd->backlight * LCD1602_BACKLIGHT) | flags;
}

void lcd1602_exp_4bits_pulse(lcd1602_t *lcd, uint8_t sendb)
{ //need to shift data to high
    sendb = sendb & 0xf0;
    lcd1602_backlight(lcd, &sendb, 0);
    lcd1602_exp_write_byte(lcd->i2caddr, sendb | LCD1602_EN);
    lcd1602_exp_write_byte(lcd->i2caddr, sendb);
}

int lcd1602_send8_pulse(lcd1602_t *lcd, uint8_t data, uint8_t flags)
{
    int res;
    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;
    lcd1602_backlight(lcd, &up, flags);
    lcd1602_backlight(lcd, &lo, flags);
    uint8_t data_arr[4];
    data_arr[0] = up | LCD1602_EN;
    data_arr[1] = up;
    data_arr[2] = lo | LCD1602_EN;
    data_arr[3] = lo;

    res = lcd1602_exp_write_data(lcd->i2caddr, data_arr, 4);

    return res;
}

void lcd1602_send_command(lcd1602_t *lcd, uint8_t cmd)
{
    lcd1602_send8_pulse(lcd, cmd, 0);
}

void lcd1602_send_data(lcd1602_t *lcd, uint8_t data)
{
    lcd1602_send8_pulse(lcd, data, LCD1602_RS);
    usleep(48);
}

void lcd1602_SendString(lcd1602_t *lcd, char *str)
{
    while (*str)
    {
        lcd1602_send_data(lcd, (uint8_t)(*str));
        str++;
    }
}

void lcd1602_create_char(lcd1602_t *lcd, uint8_t location, uint8_t *charmap)
{
    location &= 0x7; // we only have 8 locations 0-7
    lcd1602_send_command(lcd, LCD1602_SETCGRAMADDR | (location << 3));
    usleep(45);
    for (int i = 0; i < 8; i++)
    {
        lcd1602_send_data(lcd, charmap[i]);
    }
}

void lcd1602_dcb_set(lcd1602_t *lcd, bool displayon, bool cursoron, bool blinkon)
{ //display, cursor, blinking cursor
    lcd->dcb = (displayon * LCD1602_DISPLAYON) | (cursoron * LCD1602_CURSORON) | (blinkon * LCD1602_BLINKON);
}

void lcd1602_set_pos(lcd1602_t *lcd, uint8_t row, uint8_t col)
{
    lcd1602_send_command(lcd, LCD1602_SETDDRAMADDR | ((row && 1) * 0x40) | (0x3f & col));
    usleep(45);
}

void lcd1602_entry_set(lcd1602_t *lcd, bool direction, bool shift)
{
    lcd->entrymode = (direction * LCD1602_ENTRYLEFT) | (shift * LCD1602_ENTRYSHIFTINCREMENT);
}

void lcd1602_entry_update(lcd1602_t *lcd)
{
    lcd1602_send8_pulse(lcd, lcd->entrymode | LCD1602_ENTRYMODESET, 0);
    usleep(45);
}

void lcd1602_dcb_update(lcd1602_t *lcd)
{
    lcd1602_send8_pulse(lcd, lcd->dcb | LCD1602_DISPLAYCONTROL, 0);
    usleep(45);
}

void lcd1602_jumptohome(lcd1602_t *lcd)
{
    lcd1602_send_command(lcd, 0x02);
    usleep(2000);
}

void lcd1602_clear(lcd1602_t *lcd)
{
    lcd1602_send_command(lcd, 0x01);
    usleep(2000);
}

void lcd1602_rl_shift(lcd1602_t *lcd, uint8_t cur_display, uint8_t direction)
{
    lcd1602_send8_pulse(lcd, (cur_display & 0x08) | (direction & 0x04), LCD1602_CURSORSHIFT);
    usleep(45);
}

void lcd1602_init(lcd1602_t *lcd)
{
    lcd1602_exp_4bits_pulse(lcd, 0x03 << 4);
    usleep(4200); // wait min 4.1ms

    // second try
    lcd1602_exp_4bits_pulse(lcd, 0x03 << 4);
    usleep(130);

    // third go!
    lcd1602_exp_4bits_pulse(lcd, 0x03 << 4);
    usleep(130);

    // finally, set to 4-bit interface
    lcd1602_exp_4bits_pulse(lcd, 0x02 << 4);
    usleep(45);
    // display & cursor home (keep this!)
    lcd1602_jumptohome(lcd);
    lcd1602_dcb_update(lcd);
    lcd1602_entry_update(lcd);
    // clear display (optional here)
    lcd1602_clear(lcd);
    lcd1602_set_pos(lcd, 0, 0);
}

void i2c_scan(void)
{
    int res;
    char info[] = "Scanning I2C bus...\r\n";

    printf("%s", info);

    for (uint16_t i = 0; i < 128; i++)
    {
        res = lcd1602_exp_write_byte(i, 0x00);
        if (res == ESP_OK)
        {
            char msg[64];

            snprintf(msg, sizeof(msg), "0x%02X", i);
            printf("%s", msg);
        }
        else
        {
            printf("%s", ".");
        }
    }

    printf("%s", "\r\n");
}

void lcd1602_i2c_init(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = LCD1602_GPIO_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = LCD1602_GPIO_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
}