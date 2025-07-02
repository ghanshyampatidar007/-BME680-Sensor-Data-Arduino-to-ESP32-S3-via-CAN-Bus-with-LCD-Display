#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

Adafruit_BME680 bme; 

struct can_frame rx_frame;
struct can_frame tx_frame;

MCP2515 mcp2515(10);  // CS pin (use pin 10)

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize I2C
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms



  // Initialize SPI for MCP2515: SCK=13, MISO=12, MOSI=11, CS=10 (Arduino Uno default)
  SPI.begin();

  if (mcp2515.reset() != MCP2515::ERROR_OK) {
    Serial.println("MCP2515 Reset FAILED");
    while (1);
  }

  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // Match ESP32-S3 config
  mcp2515.setNormalMode();  // Set MCP2515 to normal mode

  Serial.println("CAN Receiver Ready");
}

void loop() {

  // Perform measurement
  if (!bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }


  if (mcp2515.readMessage(&rx_frame) == MCP2515::ERROR_OK)
  {
    Serial.print("Received CAN ID: 0x");
    Serial.println(rx_frame.can_id, HEX);

    Serial.print("Received Data: ");
    for (int i = 0; i < rx_frame.can_dlc; i++) {
      Serial.print((char)rx_frame.data[i]);
    }
    Serial.println("\n");

    float temperatureC = bme.temperature;     // °C
    float humidity = bme.humidity;           // %
    float pressure = bme.pressure;           // hPa
    float gas = bme.gas_resistance;          // Ohms

    int16_t tempInt = (int16_t)(temperatureC * 100);   // e.g., 25.35°C → 2535
    uint16_t humInt = (uint16_t)(humidity * 100);      // e.g., 45.65% → 4565
    uint16_t pressInt = (uint16_t)(pressure * 10);     // e.g., 1013.2 hPa → 10132
    uint16_t gasInt = (uint16_t)(gas / 10);            // e.g., 52890 Ohms → 5289
  
    
    // Prepare 8-byte CAN frame
    tx_frame.can_id  = 0x030;
    tx_frame.can_dlc = 8;

    // Fill data bytes (2 bytes per value)
    tx_frame.data[0] = (tempInt >> 8) & 0xFF;
    tx_frame.data[1] = tempInt & 0xFF;

    tx_frame.data[2] = (humInt >> 8) & 0xFF;
    tx_frame.data[3] = humInt & 0xFF;

    tx_frame.data[4] = (pressInt >> 8) & 0xFF;
    tx_frame.data[5] = pressInt & 0xFF;

    tx_frame.data[6] = (gasInt >> 8) & 0xFF;
    tx_frame.data[7] = gasInt & 0xFF;

    if (mcp2515.sendMessage(&tx_frame) == MCP2515::ERROR_OK)
    {
          Serial.print("Sent ID: ");
          Serial.println(tx_frame.can_id, HEX);
          Serial.print("Sent data :- \n");
        
          Serial.print("Temperature: ");
          Serial.print(tempInt / 100);
          Serial.print(".");
          Serial.print(abs(tempInt % 100));
          Serial.println(" °C");

          Serial.print("Humidity: ");
          Serial.print(humInt / 100);
          Serial.print(".");
          Serial.print(humInt % 100);
          Serial.println(" %");

          Serial.print("Pressure: ");
          Serial.print(pressInt / 10);
          Serial.print(".");
          Serial.print(pressInt % 10);
          Serial.println(" hPa");

          Serial.print("Gas Resistance: ");
          Serial.print(gasInt);  // since gas was encoded as value / 10
          Serial.println(" Ohms");
          Serial.println("\n");
       
    }
    else
    {
      Serial.println("Failed to send ACK");
    }
    
 }

  delay(1000);

}

