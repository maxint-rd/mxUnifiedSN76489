# mxUnifiedSN7648
mxUnifiedIO device library for Arduino. Device specific driver for the SN76489 sound chip. Use SPI shift registers or I2C I/O expanders via the [mxUnifiedIO](https://github.com/maxint-rd/mxUnifiedIO) API.

# Introduction
This is a library for the LCD display found in the SN76489 sound chip. The SN76489 sound chip is a low-cost 3+1 channel complex sound generator that provides three programmable tone generators and one noise generator, all with programmable attenuation (volume). All four channels can be active simultanously and are mixed to one output, allowing for three-voice music plus noise. The chip was quite popular in the early days of home computing and gaming, being used in consoles and computers such as the Sega Genesis, IBM PCjr and BBC Micro.
See [Wikipedia](https://en.wikipedia.org/wiki/Texas_Instruments_SN76489) and the [datasheet](documentation/datasheet%20SN76489AN.pdf) for more information

This library requires the the mxUnifiedIO base-library and a suitable mxUnifiedIO expander driver. Additionally it is supported by the MML Music library (to be released soon!). Instead of directly using MCU pins, the [mxUnifiedIO](https://github.com/maxint-rd/mxUnifiedIO) libraries allow the sound chip to be driven via the expanded pins of an I2C I/O expander or a shift-register.

# Pinouts & connections
The SN76489 sound generator requires power via VCC/GND, a 4MHZ clock signal, instructions via eight data pins and one control pin (write enable, /WE). The output enable pin (/OE) can be tied low. Audio out requires a mono amplifier to filter noise and amplyfy the signal for a speaker or headphones.

This fine piece of ASCII-art shows the pinout of the SN76489. Of course you can also review the [datasheet](documentation/datasheet%20SN76489AN.pdf) (PDF).
```
Pinout SN76489 sound generator:

     +--v--+          PINS 1-8       PINS 9-16
  1 -+     +- 16      1: D2          16: VCC
  2 -+     +- 15      2: D1          15: D3
  3 -+     +- 14      3: D0          14: Clock
  4 -+     +- 13      4: Ready       13: D4
  5 -+     +- 12      5: /WE         12: D5
  6 -+     +- 11      6: /OE         11: D6
  7 -+     +- 10      7: Audio Out   10: D7
  8 -+     +- 9       8: GND          9: N.C.
     +-----+
```
Note that D0 to D7 are in LSB-first order. This means that the proper connection to a shift-register is D7 to Qa, D6 to Qb, etc. Alternatively the output of the shift-register can be reversed using setBitOrder(LSB_FIRST);

# Features & limitations
- The current version of this library was tested using the Arduino 1.8.2 IDE, one 74HC595 shift-register and an ATtiny85 MCU. It should also work using two cascaded shiftregisters or the PCF8574/PCF8575 I2C I/O expanders, as well as using other Arduino IDE supported MCUs such as the ESP32/ESP8266 and the ATmega328 and ATmega168 as of found on the Arduino Uno and Nano.

# Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
