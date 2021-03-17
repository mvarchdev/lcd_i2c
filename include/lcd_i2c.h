// flags for display/cursor shift
#define LCD1602_DISPLAYMOVE (0x08)
#define LCD1602_CURSORMOVE (0x00)
#define LCD1602_MOVERIGHT (0x04)
#define LCD1602_MOVELEFT (0x00)

typedef struct {
    bool backlight;
    uint8_t dcb;    //Display on/off, cursor on/off, blinkng cursor on/off
    uint8_t entrymode;   //moving direction, entire display shift
    uint8_t i2caddr;    //i2c address
    uint8_t i2cchan;    //i2c channel
} lcd1602_t;

void lcd1602_create_char(lcd1602_t *lcd, uint8_t location, uint8_t *charmap);
int lcd1602_send8_pulse(lcd1602_t *lcd, uint8_t data, uint8_t flags);
void lcd1602_send_command(lcd1602_t *lcd, uint8_t cmd);
void lcd1602_send_data(lcd1602_t *lcd, uint8_t data);
void lcd1602_SendString(lcd1602_t *lcd, char *str);

void lcd1602_init(lcd1602_t *lcd);

void i2c_scan(void);
void lcd1602_i2c_init(uint8_t sda, uint8_t scl, uint8_t i2csel);

void lcd1602_dcb_set(lcd1602_t *lcd, bool displayon, bool cursoron, bool blinkon);
void lcd1602_entry_set(lcd1602_t *lcd, bool direction, bool shift);
void lcd1602_entry_update(lcd1602_t *lcd);
void lcd1602_dcb_update(lcd1602_t *lcd);
void lcd1602_jumptohome(lcd1602_t *lcd);
void lcd1602_clear(lcd1602_t *lcd);
void lcd1602_rl_shift(lcd1602_t *lcd, uint8_t cur_display, uint8_t direction);
void lcd1602_backlight(lcd1602_t *lcd, uint8_t *sendb, uint8_t flags);
void lcd1602_set_pos(lcd1602_t *lcd, uint8_t row, uint8_t col);