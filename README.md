# 🌡️ BME680 Sensor Data Arduino to ESP32-S3 via CAN Bus with LCD Display
------

📌 Project Overview
------
1. This project demonstrates how to:

2. Read environmental data (temperature, pressure, humidity, and gas resistance) from a BME680 sensor using Arduino UNO.

3. Send the sensor data over the CAN Bus using an MCP2515 CAN controller module.

4. Receive the data on an ESP32-S3 microcontroller using another MCP2515 over SPI.

5. Display the data on both:

6. The ESP32-S3 serial monitor.

7. A 20x4 I2C LCD display connected to ESP32-S3.

8. Send a confirmation "RECEIVED" message back from ESP32-S3 to Arduino, and print it on the Arduino serial monitor.

-----
⚙️ Hardware Used
----
Component	Quantity
Arduino UNO	1
ESP32-S3 DevKitC	1
MCP2515 CAN Module	2
BME680 Sensor (I2C)	1
I2C 20x4 LCD	1
Jumper Wires	-
Power Source	1

-----
🧩 Communication Overview
-----

I2C:

  Arduino UNO ←→ BME680

  ESP32-S3 ←→ I2C LCD Display

SPI:

  Arduino UNO ←→ MCP2515 (CAN TX)

  ESP32-S3 ←→ MCP2515 (CAN RX)

CAN Bus:

  MCP2515 (TX) ←→ MCP2515 (RX)

-----
🔄 Data Flow
-----

1. Arduino reads sensor data from BME680 (I2C address 0x76).

2. It sends 8 bytes (2 bytes each for Temp, Pressure, Humidity, Gas) via CAN bus.

3. ESP32-S3 receives the CAN frame, parses it, and:

4. Prints sensor values on the serial monitor.

5. Displays the values on the I2C LCD.

6. ESP32-S3 sends back the ASCII string "RECEIVED" via CAN.

7. Arduino prints the "RECEIVED" message to the serial monitor.

-----
📋 Arduino Responsibilities
-----

1. Read data from BME680.

2. Print temperature, pressure, humidity, and gas values to the serial monitor.

3. Send the sensor data (8 bytes) via MCP2515 over CAN.

4. Wait for a response message.

5. Print "RECEIVED" if the ESP32-S3 responds.

-----
📋 ESP32-S3 Responsibilities
----
1. Use Zephyr RTOS to configure CAN and I2C peripherals.

2. Receive CAN frame from Arduino.

3. Parse sensor values and:

4. Print to serial monitor.

5. Display on I2C LCD.

6. Send "RECEIVED" string (8 ASCII bytes) via CAN to Arduino.

------
# 🔌 Wiring Summary


------
 🟦 Arduino UNO + MCP2515 + BME680
-----

| **MCP2515 Pin** | **Connects To (UNO Pin)** | **Description**         |
|------------------|----------------------------|--------------------------|
| VCC              | 5V                         | Power supply             |
| GND              | GND                        | Ground                   |
| CS               | D10                        | SPI Chip Select          |
| SO (MISO)        | D12                        | SPI MISO                 |
| SI (MOSI)        | D11                        | SPI MOSI                 |
| SCK              | D13                        | SPI Clock                |
| INT              | D2                         | Interrupt Pin            |

| **BME680 Pin** | **Connects To (UNO Pin)** | **Description**         |
|----------------|----------------------------|--------------------------|
| VCC            | 3.3V or 5V (depends on module) | Power supply         |
| GND            | GND                        | Ground                   |
| SDA            | A4                         | I2C Data                 |
| SCL            | A5                         | I2C Clock                |

---
 🟩 ESP32-S3 + MCP2515 + I2C LCD (20x4)
---

| **MCP2515 Pin** | **Connects To (ESP32-S3 Pin)** | **Description**       |
|------------------|-------------------------------|------------------------|
| VCC              | 3.3V or 5V                    | Power supply           |
| GND              | GND                           | Ground                 |
| CS               | GPIO10                        | SPI Chip Select        |
| SO (MISO)        | GPIO13                        | SPI MISO               |
| SI (MOSI)        | GPIO11                        | SPI MOSI               |
| SCK              | GPIO12                        | SPI Clock              |
| INT              | GPIO9                         | Interrupt Pin          |

| **I2C LCD Pin** | **Connects To (ESP32-S3 Pin)** | **Description**        |
|------------------|-------------------------------|-------------------------|
| VCC              | 3.3V or 5V                    | Power supply            |
| GND              | GND                           | Ground                  |
| SDA              | GPIO1                         | I2C Data                |
| SCL              | GPIO2                         | I2C Clock               |
"""

----
🧪 Expected Flow
----

1. Arduino reads data from BME680.

2. Sends it over CAN using MCP2515.

3. ESP32-S3 receives and displays it on LCD & serial.

4. ESP32-S3 sends ACK ("RECEIVED") back.

5. Arduino displays ACK in serial monitor.

----
📌 Note
----

1. Both MCP2515 modules must have 8 MHz crystal and should be set to the same bitrate (typically 500 kbps).

2. Make sure CAN_H and CAN_L are connected properly with a 120Ω termination resistor at each end of the CAN bus.

3. Use the correct INT pin from MCP2515 and define it in Zephyr overlay.

-------
