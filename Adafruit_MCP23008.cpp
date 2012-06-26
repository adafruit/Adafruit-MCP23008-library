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

#include <Wire.h>
#include <avr/pgmspace.h>
#include "Adafruit_MCP23008.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// minihelper
static inline void wiresend(uint8_t x) {
#if ARDUINO >= 100
  Wire.write((uint8_t)x);
#else
  Wire.send(x);
#endif
}

static inline uint8_t wirerecv(void) {
#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}

////////////////////////////////////////////////////////////////////////////////

void Adafruit_MCP23008::begin(uint8_t addr) {
  if (addr > 7) {
    addr = 7;
  }
  i2caddr = addr;

  Wire.begin();

  // set defaults!
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(MCP23008_IODIR);
  wiresend(0xFF);  // all inputs on
  Wire.endTransmission();
}


void Adafruit_MCP23008::begin(void) {
  begin(0);
}

void Adafruit_MCP23008::pinMode(uint8_t p, uint8_t d) {
  uint8_t iodir;
  uint8_t iodiraddr;
 
  // only 8 bits!
  if (p > 7)
    return;

  iodiraddr = MCP23008_IODIR;

  // read the current IODIR
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(iodiraddr);	
  Wire.endTransmission();
  
  Wire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
  iodir = wirerecv();

  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p; 
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(iodiraddr);
  wiresend(iodir);	
  Wire.endTransmission();
}

uint8_t Adafruit_MCP23008::readGPIO() {
  uint8_t a;

  // read the current GPIO output latches
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(MCP23008_GPIO);	
  Wire.endTransmission();
  
  Wire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
  a = wirerecv();
  return a;
}

void Adafruit_MCP23008::writeGPIO(uint8_t a) {
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(MCP23008_GPIO);	
  wiresend(a & 0xFF);
  Wire.endTransmission();
}


void Adafruit_MCP23008::digitalWrite(uint8_t p, uint8_t d) {
  uint8_t gpio;
  uint8_t gpioaddr, olataddr;
  
  // only 8 bits!
  if (p > 7)
    return;

  olataddr = MCP23008_OLAT;
  gpioaddr = MCP23008_GPIO;

  // read the current GPIO output latches
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(olataddr);	
  Wire.endTransmission();
  
  Wire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
   gpio = wirerecv();

  // set the pin and direction
  if (d == HIGH) {
    gpio |= 1 << p; 
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(gpioaddr);
  wiresend(gpio);	
  Wire.endTransmission();
}

void Adafruit_MCP23008::pullUp(uint8_t p, uint8_t d) {
  uint8_t gppu;
  uint8_t gppuaddr;

  // only 8 bits!
  if (p > 7)
    return;

  gppuaddr = MCP23008_GPPU;

  // read the current pullup resistor set
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(gppuaddr);	
  Wire.endTransmission();
  
  Wire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
  gppu = wirerecv();


  // set the pin and direction
  if (d == HIGH) {
    gppu |= 1 << p; 
  } else {
    gppu &= ~(1 << p);
  }

  // write the new GPIO
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(gppuaddr);
  wiresend(gppu);	
  Wire.endTransmission();
}

uint8_t Adafruit_MCP23008::digitalRead(uint8_t p) {
  uint8_t gpioaddr;

  // only 8 bits!
  if (p > 7)
    return 0;

  gpioaddr = MCP23008_GPIO;

  // read the current GPIO
  Wire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  wiresend(gpioaddr);	
  Wire.endTransmission();
  
  Wire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
  return (wirerecv() >> p) & 0x1;
}
