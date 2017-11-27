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

void Adafruit_MCP23008::setInterruptOutPinMode(uint8_t mode) {
  // Set the interrupt pin open-drain
  uint8_t registerVal = read8(MCP23008_IOCON);

  if (mode == MCP23008_INT_OUT_DRAIN) {
    registerVal |= (1 << 2); // INTPOL = 1 (drain)
  } else {
    registerVal &= ~(1 << 2); // INTPOL = 0 (active driver)

    if (mode == MCP23008_INT_OUT_HIGH) {
      registerVal |= (1 << 1);
    } else {
      registerVal &= ~(1 << 1);
    }
  }

  write8(MCP23008_IOCON, registerVal);
}

uint8_t Adafruit_MCP23008::readInterrupts() {
  // Read which pins are active
  // intf is a mask of pins with pending interrupts
  uint8_t activeInterruptPins = read8(MCP23008_INTF);

  uint8_t validInterrupts = 0;

  // Read the captured interrupt mask which will also clear the interrupt
  uint8_t capturedInterruptMask = read8(MCP23008_INTCAP);

  uint8_t pinInterruptType;

  for (uint8_t pin = 0; pin < 8; pin++) {
    pinInterruptType = (capturedInterruptMask >> pin) & 1;

    // If the pin in the active pins is 0 then we've no interrupt for this pin
    if ((activeInterruptPins >> pin) & 1 == 0) {
      continue;
    }

    if (this->interruptHandlers[pin] == RISING) {
      if (pinInterruptType == 1) {
        validInterrupts |= (1 << pin);
      }
    } else if (this->interruptHandlers[pin] == FALLING) {
      if (pinInterruptType == 0) {
        validInterrupts |= (1 << pin);
      }
    } else if (this->interruptHandlers[pin] == CHANGE) {
      validInterrupts |= (1 << pin);
    }
  }

  return validInterrupts;
}

void Adafruit_MCP23008::enableInterrupt(uint8_t pin, int mode) {
  // Only 8 pins
  if (pin > 7) { return; }

  uint8_t registerVal;

  // Save the handler type
  interruptHandlers[pin] = mode;

  // Enable interrupt for pin (Enable interrupt on change)
  registerVal = read8(MCP23008_GPINTEN);
  registerVal |= (1 << pin);
  write8(MCP23008_GPINTEN, registerVal);

  // Interrupt compare mode
  // 0 = compare against previous
  // 1 = compare against reference
  registerVal = read8(MCP23008_INTCON);

  if (mode == CHANGE || mode == RISING || mode == FALLING) {
    registerVal &= ~(1 << pin);
  } else {
    registerVal |= (1 << pin);
  }

  write8(MCP23008_INTCON, registerVal);

  // If we're not doing on change; doing compare to value isntead
  // A interrupt will be triggered if the pin value IS NOT what this is set to
  // This only has an effect when INTCON for pin is set to 1
  registerVal = read8(MCP23008_DEFVAL);

  if (mode == ONHIGH) {
    registerVal &= ~(1 << pin);
  } else {
    registerVal |= (1 << pin);
  }

  write8(MCP23008_DEFVAL, registerVal);
}

void Adafruit_MCP23008::disableInterrupt(uint8_t pin) {
  // Only 8 pins
  if (pin > 7) { return; }

  uint8_t registerVal;

  // Turn off interrupt on change for the pin
  registerVal = read8(MCP23008_GPINTEN);
  registerVal &= ~(1 << pin);
  write8(MCP23008_GPINTEN, registerVal);
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

#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
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
