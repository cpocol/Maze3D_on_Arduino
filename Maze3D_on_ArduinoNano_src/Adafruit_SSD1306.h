#pragma once

#include <Wire.h>

#define SSD1306_MEMORYMODE 0x20           ///< See datasheet
#define SSD1306_COLUMNADDR 0x21           ///< See datasheet
#define SSD1306_PAGEADDR 0x22             ///< See datasheet
#define SSD1306_SETCONTRAST 0x81          ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D           ///< See datasheet
#define SSD1306_SEGREMAP 0xA0             ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4  ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5         ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6        ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7        ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8         ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE           ///< See datasheet
#define SSD1306_DISPLAYON 0xAF            ///< See datasheet
#define SSD1306_COMSCANINC 0xC0           ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8           ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3     ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5   ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9         ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA           ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB        ///< See datasheet

#define SSD1306_SETSTARTLINE 0x40   ///< See datasheet

#define SSD1306_EXTERNALVCC 0x01   ///< External display voltage source
#define SSD1306_SWITCHCAPVCC 0x02  ///< Gen. display voltage from 3.3V

#define SSD1306_DEACTIVATE_SCROLL 0x2E                     ///< Stop scroll

class Adafruit_SSD1306 {
public:
  Adafruit_SSD1306(uint8_t w_, uint8_t h_);

  void setBuffer(uint8_t *buffer_) { buffer = buffer_; }

  bool begin(uint8_t i2caddr = 0);
  void flush(void);

protected:
  void ssd1306_command1(uint8_t c);
  void ssd1306_commandList(const uint8_t *c, uint8_t n, bool fromPROGMEM = true);  // fromPROGMEM = false not working

  TwoWire *wire;    ///< Initialized during construction when using I2C. See Wire.cpp, Wire.h
  uint8_t *buffer;  ///< Buffer data used for display buffer. Allocated when begin method is called.
  uint8_t w, h;
  int8_t i2caddr;   ///< I2C address initialized when begin method is called.

  friend void setup();
};
