# NanoOS

NanoOS is the firmware of the NanoBot. It runs on the ESP32 and drives the robot through a RoboCup Junior Rescue Line course.

| Sketch | Board | Version |
|---|---|---|
| [`NanoOS`](NanoOS) | NanoBot REV2 | 1.15 (April 2023) |
| [`NanoOS-REV1`](NanoOS-REV1) | NanoBot REV1 | 0.80 (March 2023), last version with the REV1 pinout |

The code was written for my own robot and is meant as a starting point: all sensors and actuators of the board are wrapped in functions, so you can keep the hardware layer and replace the strategy.

## What it does

- **Line following** with calibrated phototransistors and a proportional correction (`FollowLine`, `FollowLineDiffFactor` in `Settings.h`)
- **Tile detection** (`Tiles.cpp`): left/right turns, green junction markers via the two TCS34725 colour sensors, dead ends, gaps, line search
- **Ramps and speed bumps** via the MPU6050 pitch angle, each with its own motor speed
- **Obstacles**: driving around them with the ToF sensors and the gyro (`ObstacleDir` in `Settings.h` sets the side)
- **Evacuation zone**: wall following with the ToF sensors to find the exit
- **OLED menu** with the rotary encoder: start, calibration of the light sensors and the green / red markers, sensor read-outs, system check. Calibration values are stored in the EEPROM.
- **Contest mode** (`CONTESTMODE`): starts the main program directly after power-on
- **OTA updates** over Wi-Fi (`ESP32_WiFi.cpp`)

The "Bluetooth" menu entry is a placeholder; the Bluetooth code was never finished.

## Build

1. Install the [ESP32 Arduino core](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html). Tested with **2.0.17**; core 3.x is not supported.
2. Install the libraries:

   | Library | Used for |
   |---|---|
   | [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library), [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306), [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO) | OLED |
   | [Adafruit TCS34725](https://github.com/adafruit/Adafruit_TCS34725) | colour sensors |
   | [MPU6050_light](https://github.com/rfetick/MPU6050_light) | gyro |
   | [CD74HC4067](https://github.com/waspinator/CD74HC4067) | analog multiplexer |
   | [ESP32Servo (jkb-git)](https://github.com/jkb-git/ESP32Servo) (`ESP32_Servo.h`) | servos |
   | [VL53L1X (Pololu)](https://github.com/pololu/vl53l1x-arduino) | ToF sensors, REV2 |
   | [VL53L0X (Pololu)](https://github.com/pololu/vl53l0x-arduino) | ToF sensors, REV1 |

3. Enter your Wi-Fi credentials and an OTA password in `Settings.h` if you want to update over the air.
4. Select **ESP32 Dev Module** with these settings:

   | Setting | Value |
   |---|---|
   | Upload Speed | 921600 |
   | CPU Frequency | 240 MHz (WiFi/BT) |
   | Flash Frequency | 80 MHz |
   | Flash Mode | QIO |
   | Flash Size | 16 MB (4 MB if the W25Q32 flash is fitted) |
   | Partition Scheme | Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS) |

Or with arduino-cli:

```bash
arduino-cli compile --upload -p /dev/ttyUSB0 \
  --fqbn esp32:esp32:esp32:PartitionScheme=min_spiffs,FlashSize=16M \
  firmware/NanoOS
```

If Windows does not detect the board, install the [CP210x driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).

## Flash a release binary

Every [release](https://github.com/marvinramm/NanoBot/releases) has two images:

- `*_full.bin` – bootloader, partition table and application in one file, flash to **`0x0`**
- `*.bin` – application only, flash to **`0x10000`**

```bash
esptool.py --chip esp32 --baud 921600 write_flash 0x0 NanoOS-v1.15-REV2_full.bin
```

On Windows you can also use the [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools). If the upload does not start, hold the BOOT button (or bridge the `BOOT` and `GND` pads) while connecting.

The release binaries use the default settings from `Settings.h` (REV2: contest mode on, REV1: menu at start; placeholder Wi-Fi credentials in both). Build the sketch yourself to change them.
