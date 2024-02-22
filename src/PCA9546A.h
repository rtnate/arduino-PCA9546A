//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//!  @file PCA9546A.h 
//!  @brief PCA9546 library header file
//!
//!  @author Nate Taylor 

//!  Contact: nate@rtelectronix.com
//!  @copyright (C) 2024  Nate Taylor - All Rights Reserved.
//
//    |-----------------------------------------------------------------------|
//    |                                                                       |
//    |         MMMMMMMMMMMMMMMMMMMMMM   NNNNNNNNNNNNNNNNNN                   |
//    |         MMMMMMMMMMMMMMMMMMMMMM   NNNNNNNNNNNNNNNNNN                   |
//    |        MMMMMMMMM    MMMMMMMMMM       NNNNNMNNN                        |
//    |        MMMMMMMM:    MMMMMMMMMM       NNNNNNNN                         |
//    |       MMMMMMMMMMMMMMMMMMMMMMM       NNNNNNNNN                         |
//    |      MMMMMMMMMMMMMMMMMMMMMM         NNNNNNNN                          |
//    |      MMMMMMMM     MMMMMMM          NNNNNNNN                           |
//    |     MMMMMMMMM    MMMMMMMM         NNNNNNNNN                           |
//    |     MMMMMMMM     MMMMMMM          NNNNNNNN                            |
//    |    MMMMMMMM     MMMMMMM          NNNNNNNNN                            |
//    |                MMMMMMMM        NNNNNNNNNN                             |
//    |               MMMMMMMMM       NNNNNNNNNNN                             |
//    |               MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM         |
//    |             MMMMMMM      E L E C T R O N I X         MMMMMM           |
//    |              MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM             |
//    |                                                                       |
//    |-----------------------------------------------------------------------|
//
//    |------------------------------------------------------------------------|
//    |                                                                        |
//    |    [MIT License]                                                       |
//    |                                                                        |
//    |    Copyright (c) 2024 Nathaniel Taylor                                 |
//    |                                                                        |
//    |    Permission is hereby granted, free of charge, to any person         |
//    |    obtaining a copy of this software and associated documentation      |
//    |    files (the "Software"), to deal in the Software without             |
//    |    restriction, including without limitation the rights to use,        |
//    |    copy, modify, merge, publish, distribute, sublicense, and/or sell   |
//    |    copies of the Software, and to permit persons to whom the Software  |
//    |    is furnished to do so, subject to the following conditions:         |
//    |                                                                        |
//    |    The above copyright notice and this permission notice shall be      |
//    |    included in all copies or substantial portions of the Software.     |
//    |                                                                        |
//    |    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,     |
//    |    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES     |
//    |    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND            |
//    |    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS |
//    |    BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN     |
//    |    AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF      |
//    |    OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS     |
//    |    IN THE SOFTWARE.                                                    |
//    |                                                                        |
//    |------------------------------------------------------------------------|
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef PCA9546A_LIB_H
#define PCA9546A_LIB_H

#include <Arduino.h>
#include <Wire.h>
#include <I2CDevice.h>


class PCA9546A: public HasI2CDevice
{
    /**
     * @brief The default I2C Address if all address pins are low
     */
    static constexpr uint8_t I2C_ADDR_DEFAULT = 0x70;

    /**
     * @brief Bit mask for the control register limiting to channels 
     *        that exist
     */
    static constexpr uint8_t CHANNEL_MASK = 0x0F;

    /**
     * @brief Value to use if device transmission fails
     */
    static constexpr uint8_t INVALID_DATA = 0xFF;

public:
    /**
     * @brief Construct a new PCA9546A object with the supplied 7-bit address
     * 
     * @param address The 7 bit I2C device address. Defaults to 0x70, 
     *                the address of a PCA9546A with all pin slow
     */
    PCA9546A(uint8_t address = I2C_ADDR_DEFAULT): PCA9546A(Wire, address){};

    /**
     * @brief Construct a new PCA9546A object with address calculated from 
     *        pin states.
     * 
     * @param a0 A0 pin state, false for LOW, true for HIGH
     * @param a1 A1 pin state, false for LOW, true for HIGH
     * @param a2 A2 pin state, false for LOW, true for HIGH
     */
    PCA9546A(bool a0, bool a1, bool a2): PCA9546A(Wire, 
        CalculateAddressFromPins(a0, a1, a2)){};

    /**
     * @brief Construct a new PCA9546A object with the supplied TwoWire instance 
     *        and 7-bit I2C address
     * 
     * @param tw  The TwoWire instance to use (useful for devices with multiple 
     *            I2C instances), passed by reference
     * @param address The 7 bit I2C device address.
     */
    PCA9546A(TwoWire& tw, uint8_t address):
        HasI2CDevice(tw, address){};


    /**
     * @brief Gets the memory address of the underlying TwoWire instance 
     *        used for transmission to this device 
     * 
     *        Was necessary for debugging at one point, and is left in 
     *        case it is useful later
     * 
     * @return The memory address of the TwoWire instance used 
     *         for transmission
     */
    inline uintptr_t getWireHardwareAddress() const {
        return bus.getWireHardwareAddress();
    }

    /**
     * @brief Gets the locally stored copy of the contents of the 
     *        PCA9546A control register
     * 
     * @return The control register contents 
     */
    inline uint8_t getControlRegister() const {
        return m_control_reg;
    }

    /**
     * @brief Check whether the supplied channel is enabled
     * 
     * @param ch The channel to check (0-3)
     * @return true if enabled, false if not
     */
    inline bool channelEnabled(uint8_t ch) const {
        if (ch > 3) return false;
        uint8_t enabledChannels = m_control_reg & CHANNEL_MASK;
        return (enabledChannels & (1 << ch));
    };

    /**
     * @brief Refreshes the locally stored copy of the control register 
     *        with the actual device
     * 
     */
    inline void refresh() {
        refreshDeviceStatus();
    };

    /**
     * @brief Selects the supplied channel as active, disabling all
     *        other channels
     * 
     * @param ch The channel to enable (0-3)
     */
    inline void selectChannelExclusive(const uint8_t ch) {
        if (ch > 3) return;
        uint8_t ctl = (0x1 << ch);
        setControlRegister(ctl);
    };

    /**
     * @brief Enables the supplied channel (does not effect other channels)
     * 
     * @param ch The channel to enable (0-3)
     */
    inline void enableChannel(const uint8_t ch) {
        if (ch > 3) return;
        uint8_t ctl = m_control_reg;
        ctl |= 0x1 << ch;
        setControlRegister(ctl);
    };

    /**
     * @brief Disables the supplied channel (does not effect other channels)
     * 
     * @param ch The channel to enable (0-3)
     */
    inline void disableChannel(const uint8_t ch) {
        if (ch > 3) return;
        uint8_t ctl = m_control_reg;
        bitClear(ctl, ch);
        setControlRegister(ctl);
    }

    /**
     * @brief Selectively enables or disables the 4 bus channels the device controls
     * 
     * @param ch0 true enables channel 0, false disables channel 0
     * @param ch1 true enables channel 1, false disables channel 1
     * @param ch2 true enables channel 2, false disables channel 2
     * @param ch3 true enables channel 3, false disables channel 3
     */
    inline void enableChannels(const bool ch0, const bool ch1, const bool ch2, 
                        const bool ch3) {
        uint8_t ctl = 0;
        if (ch0) ctl |= (0x1);
        if (ch1) ctl |= (0x1 << 1);
        if (ch2) ctl |= (0x1 << 2);
        if (ch3) ctl |= (0x1 << 3);
        setControlRegister(ctl);
    };

    /**
     * @brief Disables transmission on all channels
     * 
     */
    inline void disableAll() {
        setControlRegister(0);
    };

    /**
     * @brief Sets the device control register directly by value
     * @param reg The desired contents of the control register
     */
    void setControlRegister(const uint8_t reg);

    /**
     * @brief Calculates the 7-bit I2C address based on the state of the 
     *        PCA9546A device pins
     * 
     * @param a0 A0 pin state, false for LOW, true for HIGH
     * @param a1 A1 pin state, false for LOW, true for HIGH
     * @param a2 A2 pin state, false for LOW, true for HIGH
     * @return The 7-bit I2C address
     */
    static constexpr uint8_t CalculateAddressFromPins(bool a0, bool a1, bool a2) {
        return I2C_ADDR_DEFAULT | ((a2 ? 1 : 0) << 2) | ((a1 ? 1 : 0) << 1) | ((a0 ? 1 : 0) << 0);
    }
protected:
    /**
     * @brief The locally stored copy of the device control register
     */
    uint8_t m_control_reg;

    /**
     * @brief Fetches the control register value from the device and 
     *        updates m_control_reg with that value
     */
    void refreshDeviceStatus();
};



#endif // PCA9547_H