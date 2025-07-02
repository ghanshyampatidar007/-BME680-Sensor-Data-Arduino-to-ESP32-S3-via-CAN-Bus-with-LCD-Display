#ifndef LCD_H
#define LCD_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <string.h>

void lcd_init(const struct device *i2c_dev);
void lcd_send_cmd(const struct device *i2c_dev,uint8_t cmd);
void lcd_send_data(const struct device *i2c_dev,uint8_t data);
void lcd_print(const struct device *i2c_dev,char *str);

#endif
