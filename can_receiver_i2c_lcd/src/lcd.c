#include "lcd.h"

#define LCD_I2C_ADDR 0x27

void lcd_send_cmd(const struct device *i2c_dev,uint8_t cmd) {
    uint8_t data_u, data_l;
    uint8_t data_t[4];
    data_u = cmd & 0xF0;
    data_l = (cmd << 4) & 0xF0;
    data_t[0] = data_u | 0x0C;  // EN=1, RS=0
    data_t[1] = data_u | 0x08;  // EN=0, RS=0
    data_t[2] = data_l | 0x0C;
    data_t[3] = data_l | 0x08;
    i2c_write(i2c_dev, data_t, 4, LCD_I2C_ADDR);
}

void lcd_send_data(const struct device *i2c_dev,uint8_t data) {
    uint8_t data_u, data_l;
    uint8_t data_t[4];
    data_u = data & 0xF0;
    data_l = (data << 4) & 0xF0;
    data_t[0] = data_u | 0x0D;  // EN=1, RS=1
    data_t[1] = data_u | 0x09;  // EN=0, RS=1
    data_t[2] = data_l | 0x0D;
    data_t[3] = data_l | 0x09;
    i2c_write(i2c_dev, data_t, 4, LCD_I2C_ADDR);
}

void lcd_init(const struct device *i2c_dev) {
    k_sleep(K_MSEC(50));
    lcd_send_cmd(i2c_dev,0x33);
    lcd_send_cmd(i2c_dev,0x32);
    lcd_send_cmd(i2c_dev,0x28);
    lcd_send_cmd(i2c_dev,0x0C);
    lcd_send_cmd(i2c_dev,0x06);
    lcd_send_cmd(i2c_dev,0x01);
    k_sleep(K_MSEC(5));
}

void lcd_print(const struct device *i2c_dev,char *str) {
    while (*str) {
        lcd_send_data(i2c_dev,(uint8_t)(*str));
        str++;
    }
}
