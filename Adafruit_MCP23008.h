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
#ifdef __AVR_ATtiny85__
  #include <TinyWireM.h>
#else
  #include <Wire.h>
#endif

class Adafruit_MCP23008 {
public:
  void begin(uint8_t addr);
  void begin(void);

  void pinMode(uint8_t p, uint8_t d);
  void digitalWrite(uint8_t p, uint8_t d);
  void pullUp(uint8_t p, uint8_t d);
  uint8_t digitalRead(uint8_t p);
  uint8_t readGPIO(void);
  void writeGPIO(uint8_t);

  // Interrupt functions
  void setInterruptOutPinMode(uint8_t mode);
  uint8_t readInterrupts();
  void enableInterrupt(uint8_t pin, int mode=CHANGE);
  void disableInterrupt(uint8_t pin);

 private:
  uint8_t i2caddr;
  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t data);

  uint8_t interruptHandlers[8];
};

#define MCP23008_ADDRESS 0x20

// Interrupt pin modes
#define MCP23008_INT_OUT_DRAIN 0x00
#define MCP23008_INT_OUT_HIGH 0x01
#define MCP23008_INT_OUT_LOW 0x02

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
