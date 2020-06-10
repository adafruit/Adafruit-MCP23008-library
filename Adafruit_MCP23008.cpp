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

#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif
#include "Adafruit_MCP23008.h"

void Adafruit_MCP23008::setWire(TwoWire *wire) {
  m_wire = wire;
}

////////////////////////////////////////////////////////////////////////////////
// RTC_DS1307 implementation

void Adafruit_MCP23008::begin(uint8_t addr) {
  if (m_wire == nullptr) {
    m_wire = &Wire;
  }

  if (addr > 7) {
    addr = 7;
  }
  i2caddr = addr;

  m_wire->begin();

  // set defaults!
  m_wire->beginTransmission(MCP23008_ADDRESS | i2caddr);
#if ARDUINO >= 100
  m_wire->write((byte)MCP23008_IODIR);
  m_wire->write((byte)0xFF);  // all inputs
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
  m_wire->write((byte)0x00);
#else
  m_wire->send(MCP23008_IODIR);
  m_wire->send(0xFF);  // all inputs
  m_wire->send(0x00);
  m_wire->send(0x00);
  m_wire->send(0x00);
  m_wire->send(0x00);
  m_wire->send(0x00);
  m_wire->send(0x00);
  m_wire->send(0x00);
  m_wire->send(0x00);
  m_wire->send(0x00);
#endif
  m_wire->endTransmission();

}

void Adafruit_MCP23008::begin(void) {
  begin(0);
}

void Adafruit_MCP23008::pinMode(uint8_t p, uint8_t d) {
  uint8_t iodir;


  // only 8 bits!
  if (p > 7)
    return;

  iodir = read8(MCP23008_IODIR);

  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p;
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  write8(MCP23008_IODIR, iodir);
}

uint8_t Adafruit_MCP23008::readGPIO(void) {
  // read the current GPIO input
  return read8(MCP23008_GPIO);
}

void Adafruit_MCP23008::writeGPIO(uint8_t gpio) {
  write8(MCP23008_GPIO, gpio);
}


void Adafruit_MCP23008::digitalWrite(uint8_t p, uint8_t d) {
  uint8_t gpio;

  // only 8 bits!
  if (p > 7)
    return;

  // read the current GPIO output latches
  gpio = readGPIO();

  // set the pin and direction
  if (d == HIGH) {
    gpio |= 1 << p;
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  writeGPIO(gpio);
}

void Adafruit_MCP23008::pullUp(uint8_t p, uint8_t d) {
  uint8_t gppu;

  // only 8 bits!
  if (p > 7)
    return;

  gppu = read8(MCP23008_GPPU);
  // set the pin and direction
  if (d == HIGH) {
    gppu |= 1 << p;
  } else {
    gppu &= ~(1 << p);
  }
  // write the new GPIO
  write8(MCP23008_GPPU, gppu);
}

uint8_t Adafruit_MCP23008::digitalRead(uint8_t p) {
  // only 8 bits!
  if (p > 7)
    return 0;

  // read the current GPIO
  return (readGPIO() >> p) & 0x1;
}

uint8_t Adafruit_MCP23008::read8(uint8_t addr) {
  m_wire->beginTransmission(MCP23008_ADDRESS | i2caddr);
#if ARDUINO >= 100
  m_wire->write((byte)addr);
#else
  m_wire->send(addr);
#endif
  m_wire->endTransmission();
  m_wire->requestFrom(MCP23008_ADDRESS | i2caddr, 1);

#if ARDUINO >= 100
  return m_wire->read();
#else
  return m_wire->receive();
#endif
}


void Adafruit_MCP23008::write8(uint8_t addr, uint8_t data) {
  m_wire->beginTransmission(MCP23008_ADDRESS | i2caddr);
#if ARDUINO >= 100
  m_wire->write((byte)addr);
  m_wire->write((byte)data);
#else
  m_wire->send(addr);
  m_wire->send(data);
#endif
  m_wire->endTransmission();
}
