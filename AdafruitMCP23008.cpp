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

static uint8_t withValueAtPositionToggled(uint8_t original, uint8_t position, uint8_t shouldSet) {
  const uint8_t bitMask = 1 << position;
  return shouldSet ? original |= bitMask : original &= ~bitMask;
}

static uint8_t setIfInput(uint8_t original, uint8_t position, uint8_t direction) {
  return withValueAtPositionToggled(original, position, direction == INPUT);
}

static uint8_t setIfHigh(uint8_t original, uint8_t position, uint8_t writeValue) {
  return withValueAtPositionToggled(original, position, writeValue == HIGH);
}

void AdafruitMCP23008::pinMode(uint8_t portNumber, uint8_t direction) {
  CHECK_8BITS(portNumber);
  
  write8(MCP23008_IODIR, setIfInput(read8(MCP23008_IODIR), portNumber, direction));
}

// read the current GPIO value
uint8_t AdafruitMCP23008::readGPIO(void) {
  return read8(MCP23008_GPIO);
}

void AdafruitMCP23008::writeGPIO(uint8_t gpio) {
  write8(MCP23008_GPIO, gpio);
}


void AdafruitMCP23008::digitalWrite(uint8_t portNumber, uint8_t writeValue) {
  CHECK_8BITS(portNumber);

  writeGPIO(setIfHigh(readGPIO(), portNumber, writeValue));
}

void AdafruitMCP23008::pullUp(uint8_t portNumber, uint8_t writeValue) {
  CHECK_8BITS(portNumber);

  write8(MCP23008_GPPU, setIfHigh(read8(MCP23008_GPPU), portNumber, writeValue));
}
void AdafruitMCP23008::inputPolarity(uint8_t portNumber, bool inverted) {
  CHECK_8BITS(portNumber);

  write8(MCP23008_IPOL, 
         withValueAtPositionToggled(read8(MCP23008_IPOL), portNumber, inverted));
}

uint8_t AdafruitMCP23008::digitalRead(uint8_t portNumber) {
  if (portNumber > 7)
    return 0;

  return (readGPIO() >> portNumber) & 0x1;
}

uint8_t AdafruitMCP23008::read8(uint8_t addr) {
  Wire.beginTransmission(i2caddr);
  WireWrite(addr);	
  Wire.endTransmission();
  Wire.requestFrom(i2caddr, (uint8_t)1);
  return WireRead();
}


void AdafruitMCP23008::write8(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(i2caddr);
  WireWrite(addr);
  WireWrite(data);
  Wire.endTransmission();
}
