/*!
 * @file Adafruit_MCP23008.h
 */

#ifndef _ADAFRUIT_MCP23008_H
#define _ADAFRUIT_MCP23008_H

#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <Arduino.h>

/*!
 * @brief Class that stores state and functions for interacting with MCP23008
 * chip
 */
class Adafruit_MCP23008 {
public:
  /*!
   * @brief Begins the i2c connection using specified address
   * @param addr i2c address of the MCP23008, defaults to 0x20
   * @param wire TwoWire interface, defaults to &Wire
   */
  bool begin(uint8_t addr=0x20, TwoWire *wire = &Wire);

  /*!
   * @brief Sets the pin mode
   * @param p Mode to set
   * @param d Pin to set the mode to
   */
  bool pinMode(uint8_t p, uint8_t d);
  /*!
   * @brief Sets the pin and direction
   * @param p Pin to set
   * @param d Direction to set the pin
   */
  bool digitalWrite(uint8_t p, uint8_t d);
  /*!
   * @brief Sets pull-up resistor on specified pin
   * @param p Pin to set
   * @param d Direction to set the pin
   */
  bool pullUp(uint8_t p, uint8_t d);
  /*!
   * @brief Reads the status of a gpio pin
   * @param p Pin to read
   * @return Returns the current gpio
   */
  uint8_t digitalRead(uint8_t p);
  /*!
   * @brief Reads the current GPIO input
   * @return Returns the current GPIO input
   */
  uint8_t readGPIO(void);
  /*!
   * @brief Writes to the GPIO
   * @param gpio what to write
   */
  bool writeGPIO(uint8_t);

protected:
  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface

  uint8_t _i2caddr;
  uint8_t read8(uint8_t addr);
  bool write8(uint8_t addr, uint8_t data);
};

#define MCP23008_ADDRESS 0x20 //!< MCP23008 serial address

// registers
#define MCP23008_IODIR 0x00   //!< I/O direction register
#define MCP23008_IPOL 0x01    //!< Input polarity register
#define MCP23008_GPINTEN 0x02 //!< Interrupt-on-change control register
#define MCP23008_DEFVAL                                                        \
  0x03 //!< Default compare register for interrupt-on-change
#define MCP23008_INTCON 0x04 //!< Interrupt control register
#define MCP23008_IOCON 0x05  //!< Configuration register
#define MCP23008_GPPU 0x06   //!< Pull-up resistor configuration register
#define MCP23008_INTF 0x07   //!< Interrupt flag register
#define MCP23008_INTCAP 0x08 //!< Interrupt capture register
#define MCP23008_GPIO 0x09   //!< Port register
#define MCP23008_OLAT 0x0A   //!< Output latch register

#endif
