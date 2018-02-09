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


  /*
   * IOCON -- the general configuration.
   *
   * Here we define some enums to make code clear.  Each field of the
   * Adafruit_MCP23008::IOConfiguration used has a specific type, so set
   *
   *   intpol: 	ActiveLowInterrupt or ActiveHighInterrupt
   *   odr: 	ActiveDriver or OpenDrain
   *   haen:	HADisabled or HAEnabled (only makes sense with SPI version of chip)
   *   disslw:	SlewRateEnabled or SlewRateDisable
   *   seqop:	SeqOpEnabled or SeqOpDisabled
   *
   * you can instantiate a config object:
   *  Adafruit_MCP23008::IOConfiguration myconf;
   * and set all the values... e.g.
   *  myconf.intpol = Adafruit_MCP23008::ActiveHighInterrupt;
   * then call
   *  mcp.setConfiguration(myconf);
   * or use the utility methods, eg
   *  mcp.setInterruptPolarity(Adafruit_MCP23008::ActiveHighInterrupt);
   *
   */
  typedef enum {
	  CompareToPreviousValue = 0,
	  CompareToDefaultValue = 1
  } CompareTo;
  typedef enum {
	  ActiveLowInterrupt = 0,
	  ActiveHighInterrupt = 1,
  } InterruptPolarity;

  typedef enum {
	  ActiveDriver = 0,
	  OpenDrain = 1
  } InterruptOutput;
  typedef enum {
	  HADisabled = 0,
	  HAEnabled = 1
  } HardwareAddressEnable;
  typedef enum {
	  SlewRateEnabled = 0,
	  SlewRateDisable = 1
  } SlewRateControl;
  typedef enum {
	  SeqOpEnabled = 0,
	  SeqOpDisabled = 1
  } SequentialOperation;


  typedef union IOConfigUnion {
	  struct {
	  uint8_t res1: 1;
	  InterruptPolarity 		intpol: 1;
	  InterruptOutput			odr: 1;
	  HardwareAddressEnable		haen: 1;
	  SlewRateControl 			disslw: 1;
	  SequentialOperation 		seqop: 1;
	  uint8_t res2: 2;
	  };
	  uint8_t registerValue;

	  IOConfigUnion() : registerValue(0) {

	  }

  } IOConfiguration;

  /*
   * Configuration
   *
   * A single register that holds the general config for the IO expander,
   * including some control of the interrupt output pin (~INT)
   */
  void setConfiguration(const IOConfiguration & config);
  IOConfiguration configuration();

  void setInterruptPolarity(InterruptPolarity pol);
  InterruptPolarity interruptPolarity();

  void setInterruptOutput(InterruptOutput intout);
  InterruptOutput interruptOutput();




  // Interrupt control and query

  /*
   * interrupt-on-change
   * Will generate an interrupt when the value of a pin is different from either:
   * 	* the previous value (it has changed)
   * 	* the default value (it is no longer as specified in the DEFVAL register)
   *
   * So, to use:
   *   - set the default pin value, if that's what you wish to compare to
   *   - set the interrupt on change, along with what it should be compared to
   *   - deal with interrupts. See the example code/datasheet to get more deets.
   *   - clear the MCP interrupt flag, by either reading the GPIO or interruptCapture()
   */

  void setDefaultValue(uint8_t reg); // set the default values to compare to
  void setDefaultValuePin(uint8_t p, uint8_t v);

  uint8_t defaultValue();
  uint8_t defaultValuePin(uint8_t p);


  // setInterruptOnChangePin enable/disable interrupt-on-change for a pin.
  // you may specify a "compareto" value (either
  // 	* Adafruit_MCP23008::CompareToPreviousValue (the default setting)
  // 	* Adafruit_MCP23008::CompareToDefaultValue 	(compares to contents of DEFVAL register)
  void setInterruptOnChangePin(uint8_t p, bool enabled=true, CompareTo compareto=CompareToPreviousValue);
  void setInterruptOnChange(uint8_t reg);

  uint8_t interruptOnChange(); // returns the register contents
  bool interruptOnChangePin(uint8_t p); // returns whether enabled for a given pin

  // setInterruptCompare -- specify what pins are compared to in order to decide whether
  // to generate an interrupt (this is the INTCON register)
  void setInterruptCompare(uint8_t reg);
  void setInterruptComparePin(uint8_t p, CompareTo compareto=CompareToPreviousValue);


  /* interrupt flags and capture.
   *
   */
  uint8_t interruptFlags();
  bool interruptOnPin(uint8_t p);
  uint8_t interruptCapture();




 private:
  uint8_t i2caddr;
  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t data);
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
