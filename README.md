# Vita-Watch
Smart health monitoring watch built using ESP32 to track vital parameters such as heart rate, body temperature, and activity, with an emphasis on low power consumption and wearable design.
## Features
- Real-time heart rate monitoring
- Body temperature and humidity measurement
- OLED display for live health data
- Touch-based power ON/OFF control
- Rechargeable Li-Po battery support
- Low power consumption wearable design
- ## Hardware Components
- ESP32-S3 Zero (Waveshare)
- 0.96" OLED Display (I2C)
- MAX30102 Pulse Oximeter & Heart Rate Sensor
- Temperature and Humidity Sensor (BME280)
- TTP223 Capacitive Touch Sensor
- TP4056 Li-Po Battery Charging Module
- 220 mAh Li-Po Battery
- Slide Power Switch
- ## Working
VitaWatch functions as a wearable health monitoring system using the ESP32 microcontroller as the central processing unit. The MAX30102 sensor measures the user’s heart rate by detecting variations in blood flow using optical sensing. The BME280 sensor monitors both body temperature and ambient humidity in real time.

All sensor data is processed by the ESP32 and displayed on a 0.96-inch OLED screen, allowing users to view health parameters instantly. The device is powered by a rechargeable Li-Po battery, with charging handled by the TP4056 module.

Power efficiency is achieved using a TTP223 capacitive touch sensor for ON/OFF control along with optimized power management techniques, ensuring extended battery life for wearable operation.
## Communication Protocol
VitaWatch uses the I2C communication protocol to interface with multiple sensors and the OLED display. The MAX30102 sensor, BME280 sensor, and OLED display share the same I2C bus, allowing efficient data transfer using only two communication lines (SDA and SCL). This reduces wiring complexity and makes the design suitable for compact wearable applications.
## Pin Connections
| Component        | ESP32 Pin |
|------------------|-----------|
| OLED (SDA)       | GPIO 8    |
| OLED (SCL)       | GPIO 9    |
| MAX30102 (SDA)   | GPIO 8    |
| MAX30102 (SCL)   | GPIO 9    |
| BME280 (SDA)     | GPIO 8    |
| BME280 (SCL)     | GPIO 9    |
| TTP223 Touch     | GPIO 3    |
## Power Management
VitaWatch is powered by a 220 mAh Li-Po battery, making it suitable for wearable applications. A TP4056 charging module is used to safely charge the battery via USB while providing protection against overcharging and over-discharging.

To improve battery life, the system uses a TTP223 capacitive touch sensor for power control, allowing the user to turn the device ON or OFF when required. Efficient power handling ensures extended operation time for continuous health monitoring.
## Block Diagram

[BME280 Sensor] ─┐
                 ├── I2C ──> [ESP32-S3 Controller] ──> [OLED Display]
[MAX30102 Sensor]┘                     │
                                       │
                               [TTP223 Touch Sensor]
                                       │
                               Power ON / OFF Control
                                       │
                         [Li-Po Battery + TP4056 Module]
 ## Block Diagram Explanation
VitaWatch collects heart rate via the MAX30102 sensor and temperature/humidity via the BME280 sensor. The ESP32-S3 controller processes this data and displays it on a 0.96" OLED screen. Power is supplied by a 220 mAh Li-Po battery with TP4056 charging module, and a TTP223 touch sensor is used for ON/OFF control, ensuring efficient wearable operation.
## Applications
- Personal health monitoring
- Fitness and activity tracking
- Elderly care and health supervision
- Preventive healthcare awareness
## Conclusion
VitaWatch is a compact and efficient wearable health monitoring system that tracks vital parameters in real time. It provides a strong foundation for future intelligent health monitoring applications.
