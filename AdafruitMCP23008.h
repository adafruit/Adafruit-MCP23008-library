/*************************************************** 
  This is a library for the MCP23008 i2c port expander

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ADAFRUIT_MCP23008_H
#define _ADAFRUIT_MCP23008_H
// Don't forget the Wire library

class AdafruitMCP23008 {
public:
  void begin(uint8_t addr);
  void begin(void);
  void reset();

  void pinMode(uint8_t p, uint8_t d);
  void digitalWrite(const uint8_t portNumber, const uint8_t writeValue);
  void pullUp(uint8_t p, uint8_t d);
  void inputPolarity(uint8_t portNumber, bool inverted);

  uint8_t digitalRead(uint8_t p);
  uint8_t readGPIO(void);
  void writeGPIO(uint8_t);


  void interruptWhenValueSwitchesAt(uint8_t portNumber, bool enabled);

  // -1 for not interrupted, 0 for interrupted with low value, 1 for interrupted with high value
  int8_t interruptValueAt(uint8_t portNumber);
  
  uint8_t read8(const uint8_t addr);
 private:
  uint8_t i2caddr;
  void write8(const uint8_t addr, const uint8_t data);
  void updateRegister(const uint8_t registerId, const uint8_t offset, const bool bitValue);
};

#define MCP23008_ADDRESS 0x20

// registers
#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

#endif
