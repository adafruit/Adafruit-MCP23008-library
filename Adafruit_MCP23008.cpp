/*************************************************** 
  This is a library for the MCP23008 i2c port expander

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  IOCONF (configuration) and interrupt related code by
  Pat Deegan, https://flyingcarsandstuff.com/ 2018-02-09
  Release as part of the Adafruit library, under the same license.
 ****************************************************/

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#ifdef __AVR_ATtiny85__
  #include <TinyWireM.h>
  #define Wire TinyWireM
#else
  #include <Wire.h>
#endif

#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif
#include "Adafruit_MCP23008.h"


#define SETREGBIT(r, pos)		r |= (1 << pos)
#define CLEARREGBIT(r, pos)		r &= ~(1 << pos)

////////////////////////////////////////////////////////////////////////////////
// RTC_DS1307 implementation

void Adafruit_MCP23008::begin(uint8_t addr) {
  if (addr > 7) {
    addr = 7;
  }
  i2caddr = addr;

  Wire.begin();

  // set defaults!
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
#if ARDUINO >= 100
  Wire.write((byte)MCP23008_IODIR);
  Wire.write((byte)0xFF);  // all inputs

  Wire.write((byte)0x00);
  Wire.write((byte)0x00);
  Wire.write((byte)0x00);
  Wire.write((byte)0x00);
  Wire.write((byte)0x00);
  Wire.write((byte)0x00);
  Wire.write((byte)0x00);
  Wire.write((byte)0x00);
  Wire.write((byte)0x00);	

#else
  Wire.send(MCP23008_IODIR);
  Wire.send(0xFF);  // all inputs
  Wire.send(0x00);
  Wire.send(0x00);
  Wire.send(0x00);
  Wire.send(0x00);
  Wire.send(0x00);
  Wire.send(0x00);
  Wire.send(0x00);
  Wire.send(0x00);
  Wire.send(0x00);	
#endif
  Wire.endTransmission();

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
    SETREGBIT(iodir, p);
  } else {
    CLEARREGBIT(iodir, p);
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
    SETREGBIT(gpio, p);
  } else {
    CLEARREGBIT(gpio, p);
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
	SETREGBIT(gppu, p);
  } else {
    CLEARREGBIT(gppu, p);
  }
  // write the new GPPU
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
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
#if ARDUINO >= 100
  Wire.write((byte)addr);	
#else
  Wire.send(addr);	
#endif
  Wire.endTransmission();

  Wire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
  uint8_t val = 0;
#if ARDUINO >= 100
  // some platforms (e.g. xmega) NEED to check available
  // to have time to receive all bytes
  if (Wire.available()) {
	  val = Wire.read();
  }
#else
  val = Wire.receive();
#endif

  return val;
}


void Adafruit_MCP23008::write8(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
#if ARDUINO >= 100
  Wire.write((byte)addr);
  Wire.write((byte)data);
#else
  Wire.send(addr);	
  Wire.send(data);
#endif
  Wire.endTransmission();
}



void Adafruit_MCP23008::setDefaultValue(uint8_t reg) {
  write8(MCP23008_DEFVAL, reg);
}
void Adafruit_MCP23008::setDefaultValuePin(uint8_t p, uint8_t v) {
  uint8_t reg = defaultValue();
  if (v) {
    SETREGBIT(reg, p);
  } else {
    CLEARREGBIT(reg, p);
  }
  setDefaultValue(reg);
}


uint8_t Adafruit_MCP23008::defaultValue() {
  return read8(MCP23008_DEFVAL);
}

uint8_t Adafruit_MCP23008::defaultValuePin(uint8_t p) {
  uint8_t reg = defaultValue();
  return (reg & (1 << p));
}


void Adafruit_MCP23008::setInterruptOnChange(uint8_t reg) {
  write8(MCP23008_GPINTEN, reg);
}

void Adafruit_MCP23008::setInterruptOnChangePin(uint8_t p, bool enabled, CompareTo compareto) {
  if (p > 7) {
    return;
  }
  uint8_t reg = interruptOnChange();
  if (enabled) {
    SETREGBIT(reg, p);
    setInterruptComparePin(p, compareto);
  } else {
    // disabled.
	CLEARREGBIT(reg, p);
  }
  setInterruptOnChange(reg);
}

uint8_t Adafruit_MCP23008::interruptOnChange() {
  return read8(MCP23008_GPINTEN);
}

bool Adafruit_MCP23008::interruptOnChangePin(uint8_t p) {
  if (p > 7) {
    return false;
  }
  return (interruptOnChange() & (1 << p));
}

void Adafruit_MCP23008::setInterruptCompare(uint8_t reg) {
  write8(MCP23008_INTCON, reg);
}



void Adafruit_MCP23008::setInterruptComparePin(uint8_t p, CompareTo compareto) {
  uint8_t reg = read8(MCP23008_INTCON);
  if ((uint8_t)compareto) {
    SETREGBIT(reg, p);
  } else {
    CLEARREGBIT(reg, p);
  }
  setInterruptCompare(reg);
}

uint8_t Adafruit_MCP23008::interruptFlags() {
  return read8(MCP23008_INTF);
}

uint8_t Adafruit_MCP23008::interruptCapture() {
  return read8(MCP23008_INTCAP);
}

bool Adafruit_MCP23008::interruptOnPin(uint8_t p) {
  uint8_t reg = interruptFlags();
  return (reg & (1 << p));
}


void Adafruit_MCP23008::setConfiguration(const IOConfiguration& config) {
  write8(MCP23008_IOCON, config.registerValue);
}

Adafruit_MCP23008::IOConfiguration Adafruit_MCP23008::configuration() {
  Adafruit_MCP23008::IOConfiguration conf;
  conf.registerValue = read8(MCP23008_IOCON);
  return conf;
}

void Adafruit_MCP23008::setInterruptPolarity(Adafruit_MCP23008::InterruptPolarity pol) {
  Adafruit_MCP23008::IOConfiguration conf = configuration();
  conf.intpol = pol;
  setConfiguration(conf);
}

Adafruit_MCP23008::InterruptPolarity Adafruit_MCP23008::interruptPolarity() {
  Adafruit_MCP23008::IOConfiguration conf = configuration();
  return conf.intpol;
}

void Adafruit_MCP23008::setInterruptOutput(Adafruit_MCP23008::InterruptOutput intout) {
  Adafruit_MCP23008::IOConfiguration conf = configuration();
  conf.odr = intout;
  setConfiguration(conf);
}

Adafruit_MCP23008::InterruptOutput Adafruit_MCP23008::interruptOutput() {
  Adafruit_MCP23008::IOConfiguration conf = configuration();
  return conf.odr;
}
