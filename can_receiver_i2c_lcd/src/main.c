#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>
#include "lcd.h"

LOG_MODULE_REGISTER(can_tx_app, LOG_LEVEL_INF);

#define CAN_ID_TX 0x35
#define CAN_ID_RX 0x30

const struct device *can_dev,*i2c_dev;
static volatile bool rx_flag = false;

void convert_into_string(int16_t temp,uint16_t hum,uint16_t press,uint16_t gas)
{
        uint8_t tempature[10],humidity[10],pressure[10],gass[10]; 
       
        tempature[0]=(temp/1000)+48;
        tempature[1]=((temp/100)%10)+48;
        tempature[2]='.';
        tempature[3]=((temp/10)%10)+48;
        tempature[4]=(temp%10)+48;
        tempature[5]='\0'; 
        
        lcd_send_cmd(i2c_dev,0x80);
        lcd_print(i2c_dev,"Temp: ");
        lcd_print(i2c_dev,tempature);
        lcd_send_data(i2c_dev,(uint8_t)(0xdf));
        lcd_send_data(i2c_dev,(uint8_t)('C'));

        humidity[0]=(hum/1000)+48;
        humidity[1]=((hum/100)%10)+48;
        humidity[2]='.';
        humidity[3]=((hum/10)%10)+48;
        humidity[4]=(hum%10)+48;
        humidity[5]='\0'; 
        
        lcd_send_cmd(i2c_dev,0xC0);
        lcd_print(i2c_dev,"HUM: ");
        lcd_print(i2c_dev,humidity);
        lcd_print(i2c_dev," %%");
        
        pressure[0]=(press/10000)+48;
        pressure[1]=((press/1000)%10)+48;
        pressure[2]=((press/100)%10)+48;
        pressure[3]=((press/10)%10)+48;
        pressure[4]='.';
        pressure[5]=(press%10)+48;
        pressure[6]='\0'; 
        
        lcd_send_cmd(i2c_dev,0x94);
        lcd_print(i2c_dev,"PRESS: ");
        lcd_print(i2c_dev,pressure);
        lcd_print(i2c_dev," hPa");

        gass[0]=(gas/10000)+48;
        gass[1]=((gas/1000)%10)+48;
        gass[2]=((gas/100)%10)+48;
        gass[3]=((gas/10)%10)+48;
        gass[4]=(gas%10)+48;
        gass[5]='\0'; 
        
        lcd_send_cmd(i2c_dev,0xD4);
        lcd_print(i2c_dev,"GAS: ");
        lcd_print(i2c_dev,gass);
        lcd_print(i2c_dev," Ohms");       
  
}

static void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{   
    if (frame->id == CAN_ID_RX)
    {
        rx_flag = true;
        
        printk("Received id: 0x%02X \n", frame->id);
        printk("Received data :- \n");

        int16_t temp = (frame->data[0] << 8) |frame->data[1];
        uint16_t hum = (frame->data[2] << 8) | frame->data[3];
        uint16_t press = (frame->data[4] << 8) | frame->data[5];
        uint16_t gas = (frame->data[6] << 8) | frame->data[7];
        
        printk("Temperature: %d.%02d °C\n", temp / 100, temp % 100);
        printk("Humidity: %d.%02d %%\n", hum / 100, hum % 100);
        printk("Pressure: %d.%01d hPa\n", press / 10, press % 10);
        printk("Gas Resistance: %d Ohms\n\n", gas);
        
        convert_into_string(temp,hum,press,gas);        
    }   
}

int main(void)
{
    can_dev = DEVICE_DT_GET(DT_NODELABEL(mcp2515));
    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device not ready");
        return 0;
    }

    LOG_INF("CAN device ready");
    
    
    i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
    
    if (!device_is_ready(i2c_dev)) {
        printk("I2C not ready!\n");
        return 0;
    }

    lcd_init(i2c_dev);

    if (can_start(can_dev) != 0) {
        LOG_ERR("CAN start failed");
        return 0;
    }

    struct can_filter filter = {
        .id = CAN_ID_RX,
        .mask = CAN_STD_ID_MASK,
        .flags = 0,
    };
    can_add_rx_filter(can_dev, can_rx_callback, NULL, &filter);

    struct can_frame tx_frame = {
        .id = CAN_ID_TX,
        .dlc = 8,
        .flags = 0
    };
    const char* msg = "RECEIVED";
    // Copy string into data buffer
    for (int i = 0; i < 8; i++) {
       tx_frame.data[i] = msg[i];  // Each character is stored as ASCII (uint8_t)
    }

    while (1) {
    rx_flag = false;
       
    if (can_send(can_dev, &tx_frame, K_MSEC(100), NULL, NULL) == 0) {
       printk("Sent id: 0x%02X \n", tx_frame.id);
       printk("Sent data : %s\n\n", msg);    
    } 
    else {
            LOG_ERR("Failed to send");
        }    
        int64_t start = k_uptime_get();
        while (!rx_flag && k_uptime_get() - start < 3000)
        {
            k_msleep(10);
        }

        if (!rx_flag) {
            LOG_WRN("No response received");
        }

        k_sleep(K_SECONDS(2));
    }
}


