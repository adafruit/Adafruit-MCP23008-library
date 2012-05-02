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
  pinMode(InterruptPin, INPUT);
  pinMode(PassPin, OUTPUT);
  pinMode(FailPin, OUTPUT);
}

boolean checkEqual(int pin, byte expected, byte actual, char * description) {
     if (expected != actual) {
       Serial.print("> FAIL> ");
       Serial.print(description);
       Serial.print(" Pin ");
       Serial.print(pin);
       Serial.print(" was ");       
       Serial.println(actual ? "HIGH" : "LOW");
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


boolean checkEachMcpToArduino(unsigned int pin, unsigned int arduinoPin) {
    writeMcp(pin, LOW);
    const boolean testsPassing = checkEqual(pin, LOW, digitalRead(arduinoPin), "mcp -> ard");
    
    writeMcp(pin, HIGH);
    return testsPassing && checkEqual(pin, HIGH, digitalRead(arduinoPin), "mcp -> ard");
}

boolean checkEachArduinoToMcp(unsigned int pin, unsigned int arduinoPin) {
    writeArduino(arduinoPin, LOW);
    const int testsPassing = checkEqual(pin, LOW, mcp.digitalRead(pin), "ard -> mcp");

    writeArduino(arduinoPin, HIGH);
    return testsPassing && checkEqual(pin, HIGH, mcp.digitalRead(pin), "ard -> mcp");
}

boolean forAllPins(boolean (*checker)(unsigned int, unsigned int)) {
  boolean testsPassing = true;
  for (int i = 0; i < PinCount; i++) {    
    testsPassing = testsPassing && checker(i, toArduinoPin(i));
  }
  return testsPassing;
}

void loop() {
  boolean testsPassing = true;
  
  Serial.println("MCP to Arduino"); 
  setMcpPinsMode(OUTPUT);
  setArduinoPinsMode(INPUT);  
  testsPassing = testsPassing && forAllPins(checkEachMcpToArduino);
  
  Serial.println("Arduino to MCP"); 
  setMcpPinsMode(INPUT);
  setArduinoPinsMode(OUTPUT);
  testsPassing = testsPassing && forAllPins(checkEachArduinoToMcp);
  
  /*
    configure interrupt pin ???
    set pin low
    set interrupt on change for pin
    check interrupt pin is low
    check INTCAP bit is low
    set pin high
    check interrupt pin is high
    check INTCAP bit is high (resets)
    check interrupt pin is low
  */
  writeArduino(ArduinoLowPin, HIGH);
  


  
  digitalWrite(PassPin, testsPassing ? HIGH  : LOW);
  digitalWrite(FailPin, testsPassing ? LOW : HIGH);
  delay(500);
}

