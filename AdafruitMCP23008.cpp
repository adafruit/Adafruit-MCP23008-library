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

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>
#include <avr/pgmspace.h>
#include "AdafruitMCP23008.h"

#if ARDUINO >= 100
#define WireWrite(b) Wire.write((byte)b)
#define WireRead() Wire.read()
#else
#define WireWrite(b) Wire.send(b)
#define WireRead() Wire.receive()
#endif

#define CHECK_8BITS(portNumber) if (portNumber > 7) return
static const uint8_t IOCON_INTPOL = 1;
static const uint8_t IOCON_ODR = 2;

void AdafruitMCP23008::begin(uint8_t addr) {
  i2caddr = MCP23008_ADDRESS | (addr > 7 ? 7 : addr);

  Wire.begin();

  reset();
}

void AdafruitMCP23008::begin(void) {
  begin(0);
}

void AdafruitMCP23008::reset() {
  Wire.beginTransmission(i2caddr);
  WireWrite(MCP23008_IODIR);
  WireWrite(0xFF);
  WireWrite(0x00);
  WireWrite(0x00);
  WireWrite(0x00);
  WireWrite(0x00);
  WireWrite(0x00);
  WireWrite(0x00);
  WireWrite(0x00);
  WireWrite(0x00);
  WireWrite(0x00);	
  Wire.endTransmission();
}

static uint8_t withValueAtPositionToggled(const uint8_t original, const uint8_t position, const boolean shouldSet) {
  return shouldSet ? original | bit(position) : original & ~bit(position);
}

static uint8_t setIfHigh(uint8_t original, uint8_t position, boolean writeValue) {
  return withValueAtPositionToggled(original, position, writeValue == HIGH);
}

void AdafruitMCP23008::pinMode(const uint8_t portNumber, const uint8_t direction) {
  CHECK_8BITS(portNumber);
  
  setRegisterBit(MCP23008_IODIR, portNumber, direction == INPUT); 
}

uint8_t AdafruitMCP23008::readGPIO(void) {
  return read8(MCP23008_GPIO);
}

void AdafruitMCP23008::writeGPIO(uint8_t gpio) {
  write8(MCP23008_GPIO, gpio);
}

void AdafruitMCP23008::clearInterrupts(void) {
  write8(MCP23008_INTCON, 0x00);
  write8(MCP23008_DEFVAL, 0x00);
  write8(MCP23008_GPINTEN, 0x00);
}

void AdafruitMCP23008::setInterruptPolarity(const uint8_t polarity) {
  setRegisterBit(MCP23008_IOCON, IOCON_ODR, false);
  if (polarity == HIGH) {
    setRegisterBit(MCP23008_IOCON, IOCON_INTPOL, true);
  } else {
    setRegisterBit(MCP23008_IOCON, IOCON_INTPOL, false);
  }
}

void AdafruitMCP23008::interruptWhenValueSwitchesAt(const uint8_t portNumber, const bool enabled) {
  CHECK_8BITS(portNumber);

  setRegisterBit(MCP23008_INTCON, portNumber, false);
  setRegisterBit(MCP23008_GPINTEN, portNumber, enabled);
}


uint8_t AdafruitMCP23008::wasInterruptedAt(const uint8_t portNumber) {
  if (portNumber > 7)
    return 0;

  return bitRead(read8(MCP23008_INTF), portNumber) ? true : false;
}

uint8_t AdafruitMCP23008::readINTCAP(void) {
  return read8(MCP23008_INTCAP);
}

uint8_t AdafruitMCP23008::readIOCON(void) {
  return read8(MCP23008_IOCON);
}

void AdafruitMCP23008::digitalWrite(const uint8_t portNumber, const boolean writeValue) {
  CHECK_8BITS(portNumber);

  setRegisterBit(MCP23008_GPIO, portNumber, writeValue);
}

void AdafruitMCP23008::pullUp(uint8_t portNumber, uint8_t writeValue) {
  CHECK_8BITS(portNumber);
  setRegisterBit(MCP23008_GPPU, portNumber, writeValue == HIGH);
}

void AdafruitMCP23008::inputPolarity(uint8_t portNumber, bool inverted) {
  CHECK_8BITS(portNumber);

  setRegisterBit(MCP23008_IPOL, portNumber, inverted);
}

uint8_t AdafruitMCP23008::digitalRead(uint8_t portNumber) {
  if (portNumber > 7)
    return 0;

  return bitRead(readGPIO(), portNumber);
}

void AdafruitMCP23008::setRegisterBit(const uint8_t registerId, const uint8_t offset, const bool bitValue) {
  uint8_t bits = read8(registerId);
  bitWrite(bits, offset, bitValue);
  write8(registerId, bits);
}

uint8_t AdafruitMCP23008::read8(const uint8_t addr) {
  Wire.beginTransmission(i2caddr);
  WireWrite(addr);	
  Wire.endTransmission();
  Wire.requestFrom(i2caddr, (uint8_t)1);
  return WireRead();
}


void AdafruitMCP23008::write8(const uint8_t addr, const uint8_t data) {
  Wire.beginTransmission(i2caddr);
  WireWrite(addr);
  WireWrite(data);
  Wire.endTransmission();
}
