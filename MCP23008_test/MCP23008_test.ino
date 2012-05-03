#include <Wire.h>
#include <AdafruitMCP23008.h>

// Test i/o expansion. 
// Checks all 8 pins of MCP23008 as outputs then as inputs
// Does not yet test pull ups or interrupts

// Connect pin #1 of the expander to Analog 5 (i2c clock)
// Connect pin #2 of the expander to Analog 4 (i2c data)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)

// MCP channels #0 to #7 (pins 10-17) connected to Arduino digital pins 5 to 12

// If you add an LED to pin PassPin, it will light up while the tests are passing. 
// If any test fails, it will turn off until the next time the tests are run and FailPin will turn on

AdafruitMCP23008 mcp;

const int PassPin = 13;
const int FailPin = 2;

const int ArduinoLowPin = 5;
const int PinCount = 8;
const int InterruptPin = 3;

void setup() { 
  Serial.begin(9600); 
  mcp.begin();      // use default address 0
  pinMode(PassPin, OUTPUT);
  pinMode(FailPin, OUTPUT);
}

boolean checkEqual(unsigned int pin, int expected, int actual, char * description) {
     if (expected != actual) {
       Serial.print("> FAIL> ");
       Serial.print(description);
       Serial.print(" Pin ");
       Serial.print(pin);
       Serial.print(" was ");
       switch(actual) {
         case 1: Serial.print("HIGH"); break;
         case 0: Serial.print("LOW"); break;
         default: Serial.print(actual); break;
       }
       Serial.println();
       return false;
     }
     return true;
}

void setMcpPinsMode(byte mode) {
  for (int i = 0; i < PinCount; i++) {
    mcp.pinMode(i, mode);
  }
}

int toArduinoPin(int pin) {
  return ArduinoLowPin + pin;
}

void setArduinoPinsMode(int mode) {
  for (int i = 0; i < PinCount; i++) {
    pinMode(toArduinoPin(i), mode);
  }
}

void writeArduino(int arduinoPin, byte signal) {
    digitalWrite(arduinoPin, signal);
    delay(1);
}

void writeMcp(int pin, byte signal) {
    mcp.digitalWrite(pin, signal);
    delay(1);
}

boolean checkInterruptValueAt(unsigned int pin, int expectedValue) {
  return checkEqual(pin, expectedValue, bitRead(mcp.readINTCAP(), pin), "interrupt value");
}

boolean checkInterruptionAt(unsigned int pin, boolean wasInterrupted, int interruptSignal, char* message) {
    return checkEqual(pin, wasInterrupted, mcp.wasInterruptedAt(pin), message)
        && checkEqual(InterruptPin, interruptSignal, digitalRead(InterruptPin), message);
}

boolean checkEachMcpSwitchedInterrupt(unsigned int pin, unsigned int arduinoPin, int interruptPolarity) {
  const int notInterrupted = interruptPolarity == HIGH ? LOW : HIGH;
  
  writeArduino(arduinoPin, LOW);
  mcp.clearInterrupts();
    
  mcp.interruptWhenValueSwitchesAt(pin, true);
  boolean passed = checkInterruptionAt(pin, false, notInterrupted, "before interrupt");  
  
  writeArduino(arduinoPin, HIGH);
  return passed && checkInterruptionAt(pin, true, interruptPolarity, "interrupted")
                && checkInterruptValueAt(pin, HIGH)
                && checkInterruptionAt(pin, false, notInterrupted, "after clear");
}

boolean checkEachMcpToArduino(unsigned int pin, unsigned int arduinoPin, int value) {
    writeMcp(pin, value);
    return checkEqual(pin, value, digitalRead(arduinoPin), "mcp -> ard");
}

boolean checkEachArduinoToMcp(unsigned int pin, unsigned int arduinoPin, int value) {
    writeArduino(arduinoPin, value);
    return checkEqual(pin, value, mcp.digitalRead(pin), "ard -> mcp");
}

boolean forAllPins(boolean (*checker)(unsigned int, unsigned int, int), int value) {
  boolean passed = true;
  for (int i = 0; i < PinCount; i++) {    
    passed = passed && checker(i, toArduinoPin(i), value);
  }
  return passed;
}

void loop() {
  digitalWrite(PassPin, LOW);
  digitalWrite(FailPin, LOW);
  boolean passed = true;
  
  Serial.println("MCP to Arduino"); 
  setMcpPinsMode(OUTPUT);
  setArduinoPinsMode(INPUT);  
  passed = passed 
           && forAllPins(checkEachMcpToArduino, HIGH)
           && forAllPins(checkEachMcpToArduino, LOW);
  
  Serial.println("Arduino to MCP"); 
  setMcpPinsMode(INPUT);
  setArduinoPinsMode(OUTPUT);
  passed = passed 
           && forAllPins(checkEachArduinoToMcp, HIGH)
           && forAllPins(checkEachArduinoToMcp, LOW);
           
  Serial.println("Interrupts"); 
  setMcpPinsMode(INPUT);
  pinMode(InterruptPin, INPUT);

  mcp.useActiveInterrupts(LOW);  
  passed = passed && forAllPins(checkEachMcpSwitchedInterrupt, LOW);

  mcp.useActiveInterrupts(HIGH);  
  passed = passed && forAllPins(checkEachMcpSwitchedInterrupt, HIGH);

  digitalWrite(passed ? PassPin : FailPin, HIGH);
  delay(500);
}

