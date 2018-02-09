/*
 * Configuration and Interrupt handling sample
 * Copyright (C) 2018 Pat Deegan, https://flyingcarsandstuff.com
 * 
 * Released as part of the Adafruit MCP23008 library, under the same
 * BSD license.
 * 
 * 
 * Hookup:
 *
 * Tie i2c lines (i.e. analog 4=SDA/DATA and analog 5=SCL/CLOCK on a Uno), 
 * using 2-5k pullups as required.
 * 
 * Power the unit and tie INTERRUPT_PIN to the MCP's interrupt output (defined below).
 * This pin must actually support interrupts (e.g. pin 2 or 3 on a Uno). 
 * 
 * 
 */

#include <Wire.h>
#include <Adafruit_MCP23008.h>

/*
   Defines used below for Serial, baudrate and interrupt pin to monitor
*/

#define SERIAL_TO_USE			Serial
#define SERIALPORT_BAUDRATE		57600
#define INTERRUPT_PIN			2



// Our global MCP object
Adafruit_MCP23008 mcp;


// IOPinsOfInterest -- list of GPIO pins we'll be
// monitoring... terminate list with any value > 7...
uint8_t IOPinsOfInterest[] = {
  0, 1, 2, 3, 4,
  0xff // terminator
};

#define DEBUG_BEGIN(rate)		SERIAL_TO_USE.begin(rate)
#define DEBUG_OUT(...)			SERIAL_TO_USE.print(__VA_ARGS__)
#define DEBUG_OUTLN(...)		SERIAL_TO_USE.println(__VA_ARGS__)
#define DEBUG_NEWLINE()			DEBUG_OUTLN(' ')




/*
   Our interrupt flag / service routine.
*/
volatile bool inputsChanged = false;
void inputsHaveChanged() {
  inputsChanged = true;
}


void setup() {

  DEBUG_BEGIN(SERIALPORT_BAUDRATE);
  DEBUG_OUT(F("Starting up MCP..."));
  mcp.begin();      // use default address 0
  
  /*
     We want to configure interrupts a bit, just to
     show how it's done (though this is the default IOCONF setup anyway)
  */

  // IOConfiguration acts like a struct with the same fields
  // as the IOCON register.  Each field uses an enum for
  // clear and error free settings.  See the
  Adafruit_MCP23008::IOConfiguration config = mcp.configuration();
  config.intpol = Adafruit_MCP23008::ActiveLowInterrupt; // (or ActiveHighInterrupt)
  config.odr = Adafruit_MCP23008::ActiveDriver; // (or Adafruit_MCP23008::OpenDrain)

  DEBUG_OUT(F("Setting Config..."));
  mcp.setConfiguration(config);
  /*
      Could also use individual setters, e.g.
      mcp.setInterruptPolarity(Adafruit_MCP23008::ActiveLowInterrupt);
      mcp.setInterruptOutput(Adafruit_MCP23008::ActiveDriver);
      etc
      less efficient, maybe clearer...
   */
  
  
  config = mcp.configuration();
  DEBUG_OUT(F(" is now: 0x"));
  DEBUG_OUTLN(config.registerValue, HEX);
  
  mcp.setInterruptOnChange(0); // clear all interrupts-on-change
  // you could mcp.setDefaultValue(0xff) and compare to that, but we'll 
  // just compare to the previous value.

 

  for (uint8_t i=0; i<8; i++) {
    // everybody an input, no pullup
    mcp.pinMode(i, INPUT);
    mcp.pullUp(i, LOW);
    mcp.setInterruptOnChangePin(i, false);
  }

  DEBUG_OUTLN(F("Interrupt for "));
  for (uint8_t i=0; i<8; i++) {
    DEBUG_OUT(F("Pin "));
    DEBUG_OUT((int)i);
    DEBUG_OUT(':');
    DEBUG_OUTLN(mcp.interruptOnChangePin(i) ? F("ON") : F("OFF"));
    
  }
  
  
  
  uint8_t idx = 0;
  DEBUG_OUT(F("Setup inputs I/O on "));
  while (IOPinsOfInterest[idx] < 8) {
    DEBUG_OUT((int)IOPinsOfInterest[idx]);
    DEBUG_OUT(' ');
    mcp.pullUp(IOPinsOfInterest[idx], HIGH);
    mcp.setInterruptOnChangePin(IOPinsOfInterest[idx], true, Adafruit_MCP23008::CompareToPreviousValue);
    idx++;
  }
  DEBUG_NEWLINE();

  DEBUG_OUTLN(F("Current interrupt now:"));
  for (uint8_t i=0; i<8; i++) {
    DEBUG_OUT(F("Pin "));
    DEBUG_OUT((int)i);
    DEBUG_OUT(':');
    DEBUG_OUTLN(mcp.interruptOnChangePin(i) ? F("ON") : F("OFF"));
    
  }


  /* Setup our local interrupt handler */
  DEBUG_OUT(F("Config interrupt handler... "));
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), inputsHaveChanged, LOW);

  DEBUG_OUTLN(F("Setup complete."));

}




uint8_t lc = 0;
void loop() {
  if (! inputsChanged ) {
    // nothing going on...
    delay(50);
    if (lc++ > 100) {
      lc = 0;
      
      DEBUG_OUT('.');
    }
    return;
  }


  // inputs have changed! exciting!

  // Read all values at once, 'cause we're efficient that way...
  
  // could use mcp.interruptCapture() to clear flags, but readGPIO() returns
  // current state rather than the event that triggered the interrupt
  uint8_t val = mcp.readGPIO(); // clears the interrupt flag on the MCP (INT pin goes high)
  
  inputsChanged = false; // we clear our own flag

  DEBUG_OUTLN(F("Change detected!"));

  // Loop around to get some output
  // first the "header"
  uint8_t idx = 0;
  DEBUG_OUT(F("PIN:\t"));
  while (IOPinsOfInterest[idx] < 8) {
    DEBUG_OUT((int)IOPinsOfInterest[idx]);
    DEBUG_OUT('\t');
    idx ++;
  }
  DEBUG_NEWLINE();
  DEBUG_OUT(F("VAL:\t"));
  idx = 0;
  while (IOPinsOfInterest[idx] < 8) {
    if (val & (1 << IOPinsOfInterest[idx])) {
      DEBUG_OUT(F("HI"));
    } else {
      DEBUG_OUT(F("LO"));
    }
    DEBUG_OUT('\t');
    idx++;
  }
  DEBUG_NEWLINE();

}