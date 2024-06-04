#include "I2C_Adafruit_SSD1306.h"

I2C_Adafruit_SSD1306::I2C_Adafruit_SSD1306(uint8_t w_, uint8_t h_)
    : wire(&Wire), buffer(NULL), w(w_), h(h_)
{
}

// @brief Issue single command to SSD1306, using I2C. This is a protected function, not exposed (see ssd1306_command() instead).
// @param c   the command character to send to the display. Refer to ssd1306 data sheet for commands
void I2C_Adafruit_SSD1306::ssd1306_command1(uint8_t c) {
    wire->beginTransmission(i2caddr);
    wire->write((uint8_t)0x00); // Co = 0, D/C = 0
    wire->write(c);
    wire->endTransmission();
}

// @brief Issue list of commands to SSD1306. This is a protected function, not exposed.
// @param c   pointer to list of commands
// @param n   number of commands in the list
void I2C_Adafruit_SSD1306::ssd1306_commandList(const uint8_t *c, uint8_t n, bool fromPROGMEM/* = true*/) {  // fromPROGMEM = false not working
    wire->beginTransmission(i2caddr);
    wire->write((uint8_t)0x00); // Co = 0, D/C = 0
    uint16_t bytesOut = 1;
    while (n--) {
        if (bytesOut >= BUFFER_LENGTH) {
            wire->endTransmission();
            wire->beginTransmission(i2caddr);
            wire->write((uint8_t)0x00); // Co = 0, D/C = 0
            bytesOut = 1;
        }
        if (fromPROGMEM)
            wire->write(pgm_read_byte(c++));
        else
            wire->write(*c++);
        bytesOut++;
    }
    wire->endTransmission();
}

// @param  addr   I2C address of corresponding SSD1306 display (or pass 0 to use default of 0x3C for 128x32 display, 0x3D for all others).
// @return true on successful allocation/init, false otherwise. Well-behaved code should check the return value before proceeding.
// @note   MUST call this function before any drawing or updates!
bool I2C_Adafruit_SSD1306::begin(uint8_t addr) {
    if (!buffer)
        return false; //call setBuffer() first

    // Setup pin directions
    // If I2C address is unspecified, use default
    // (0x3C for 32-pixel-tall displays, 0x3D for all others).
    i2caddr = addr ? addr : ((h == 32) ? 0x3C : 0x3C);
    // TwoWire begin() function might be already performed by the calling
    // function if it has unusual circumstances (e.g. TWI variants that
    // can accept different SDA/SCL pins, or if two SSD1306 instances
    // with different addresses -- only a single begin() is needed).
    wire->begin();
    wire->setClock(800000UL); // Maximum OK: 1 MHz, on some displays only 800 KHz

    // Init sequence
    static const uint8_t PROGMEM init1[] = {SSD1306_DISPLAYOFF,         // 0xAE
                                            SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
                                            0x80,                       // the suggested ratio 0x80
                                            SSD1306_SETMULTIPLEX};      // 0xA8
    ssd1306_commandList(init1, sizeof(init1));
    ssd1306_command1(h - 1);

    static const uint8_t PROGMEM init2[] = {SSD1306_SETDISPLAYOFFSET,   // 0xD3
                                            0x0,                        // no offset
                                            SSD1306_SETSTARTLINE | 0x0, // line #0
                                            SSD1306_CHARGEPUMP};        // 0x8D
    ssd1306_commandList(init2, sizeof(init2));

    ssd1306_command1(0x14);

    static const uint8_t PROGMEM init3[] = {SSD1306_MEMORYMODE,     // 0x20
                                            0x00,                   // 0x0 act like ks0108
                                            SSD1306_SEGREMAP | 0x1,
                                            SSD1306_COMSCANDEC};
    ssd1306_commandList(init3, sizeof(init3));

    uint8_t comPins = 0x02;
    uint8_t contrast = 0x8F;

    if ((w == 128) && (h == 32)) {
        comPins = 0x02;
        contrast = 0x8F;
    } else if ((w == 128) && (h == 64)) {
        comPins = 0x12;
        contrast = 0xCF;
    }

    ssd1306_command1(SSD1306_SETCOMPINS);
    ssd1306_command1(comPins);
    ssd1306_command1(SSD1306_SETCONTRAST);
    ssd1306_command1(contrast);

    ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9
    ssd1306_command1(0xF1);
    static const uint8_t PROGMEM init5[] = {
        SSD1306_SETVCOMDETECT,       // 0xDB
        0x40,
        SSD1306_DISPLAYALLON_RESUME, // 0xA4
        SSD1306_NORMALDISPLAY,       // 0xA6
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_DISPLAYON};          // Main screen turn on
    ssd1306_commandList(init5, sizeof(init5));

    return true; // Success
}

// @brief  Push data currently in RAM to SSD1306 display.
// @note   Drawing operations are not visible until this function is called. Call after each graphics command, or after a whole set
//         of graphics commands, as best needed by one's own application.
void I2C_Adafruit_SSD1306::flush(void) {
  static const uint8_t PROGMEM dlist1[] = {
      SSD1306_PAGEADDR,
      0,                      // Page start address
      0xFF,                   // Page end (not really, but works here)
      SSD1306_COLUMNADDR, 0}; // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1)); // fromPROGMEM = false not working
  ssd1306_command1(w - 1); // Column end address

  uint16_t count = w * ((h + 7) / 8);
  uint8_t *ptr = buffer;
  wire->beginTransmission(i2caddr);
  wire->write((uint8_t)0x40);
  uint16_t bytesOut = 1;
  while (count--) {
      if (bytesOut >= BUFFER_LENGTH) {
          wire->endTransmission();
          wire->beginTransmission(i2caddr);
          wire->write((uint8_t)0x40);
          bytesOut = 1;
      }
      wire->write(*ptr++);
      bytesOut++;
  }
  wire->endTransmission();
}
