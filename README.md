<img width="1588" height="1192" alt="image" src="https://github.com/user-attachments/assets/1b20de27-81ca-42b7-b5e2-a2e4747419ff" />

# ESP32 Digital Photo Frame

An DIY digital photo frame project using an **ESP32** and an **ILI9341 TFT display**. This project features a dual SPI bus configuration to eliminate data interference and maximize image loading speeds.

## Key Features
* **Dual SPI Architecture:** Uses separate buses for the Display (VSPI) and the SD Card (HSPI) for stable, high-speed performance.
* **Buffer-Optimized Rendering:** Fast BMP processing using memory buffering.
* **Random Start:** Picks a completely random image from the SD card every time it boots up.
* **Auto-Slideshow (Default):** Advances to the next image every 5 seconds automatically.
* **Manual Control:** Dedicated buttons for "Next" and "Previous" navigation.
* **Dynamic Mode Toggle:** Switch between Manual and Auto modes by holding both buttons for 3 seconds.

## Hardware Requirements
* ESP32 DevKit V1
* ILI9341 TFT Display (2.8" or 2.4" SPI)
* Micro SD Card Module
* 2x Push Buttons
* Lithium Battery, Charger Module and toggle button (Optional)

## Wiring & Pinout
To avoid boot loops and "strapping pin" conflicts, the following pinout is used. 

> [!IMPORTANT]
> The display used for the proyect do not have a CS pin.
> All GND have to be connected to the same GND ESP32 pin.
> VERY CAREFULL with the polarity of the battery, charger pins and conexions with ESP32.
> Refer to the **Schematics.pdf** file in this repository for the full visual circuit diagram.

| Component   | Pin Name | ESP32 Pin | SPI Bus |
| :---------- | :------- | :-------- | :------ |
| **Display** | VCC      | 3v3       | -       |
|             | CLK      | GPIO 18   | VSPI    |
|             | MOSI     | GPIO 23   | VSPI    |
|             | RES      | GPIO 4    | -       |
|             | DC       | GPIO 2    | -       |
|             | BLK      | 3v3       | -       |
|             | MISO     | GPIO 19   | -       |
|             | GND      | GND       | -       |
| **SD Card** | CS       | GPIO 33   | HSPI    |
|             | MOSI     | GPIO 25   | HSPI    |
|             | CLK      | GPIO 22   | HSPI    |
|             | MISO     | GPIO 32   | HSPI    |
|             | 3V3      | 3v3       | -       |
|             | GND      | GND       | -       |
| **Buttons** | Next     | GPIO 27   | -       |
|             | Prev     | GPIO 26   | -       |
|             | GND      | GND       | -       |
| **Charger** | Out+     | VIN       | -       |
|             | Out-     | GND       | -       |

| Component   | Pin Name | Charger Pin |
| :---------- | :------- | :--------   |
| **Battery** | RED      | B+          |
|             | BLACK    | B-          |



## Image Preparation
To ensure the frame works correctly, your images must be formatted as follows:
* **Format:** BMP (Windows 24-bit / R8 G8 B8).
* **Resolution:** 320x240 pixels.
* **Storage:** Place images in the root directory of a FAT32 formatted Micro SD card.

## User Controls
* **Manual Navigation:** Press the "Next" (GPIO 27) or "Previous" (GPIO 26) button.
* **Mode Switch:** Press and hold **both buttons simultaneously for 3 seconds**. 
    * **Green Label:** Auto-mode active.
    * **Red Label:** Manual mode active.
* **On-off control:** Press the toggle button conected to the battery.


## Getting Started
1.  Clone this repository.
2.  Install `Adafruit_ILI9341` and `Adafruit_GFX` libraries in your Arduino IDE.
3.  Upload the `main.ino` sketch to your ESP32.
4.  Check the **Connection_Diagram.pdf** for wiring details.

---
*Code helped by AI*
*Created by Pablo Vazquez*
