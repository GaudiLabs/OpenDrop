# SlowSoftI2CMaster


Another bit-banging I2C library, very similar to *SoftI2CMaster*, allowing you to use any Arduino pins for SDA and SCL.  The difference to the SoftI2CMaster library is that it is entirely written in C++, so you can use it also on ARM boards, such as Zero and Due.


## Features

This library has the following features:
* architecture independent, compatible with all MCUs supported by the Arduino IDE
* can make use of any pin 
* supports multiple I2C buses
* internal MCU pullup resistors can be used 
* timeout on ACK polling for busy devices (new!)
* supports only master mode
* no bus arbitration (i.e., only one master allowed on bus)
* no clock stretching supported
* **slow** (70 kHz on a Zero, 45 kHz on an UNO)
* optional <code>Wire</code> library compatible interface
* LGPL license

## Usage

The library can be used in a similar way as the [SoftI2CMaster](https://github.com/felias-fogg/SoftI2CMaster) library. See the description of the methods in the documentation of this library.
However, here all parameters and options are not defined as compile-time constants, but they are passed as parameters when creating a new instance.

## Example

    // Simple sketch to read out one register of an I2C device
    #include <SlowSoftI2CMaster.h>

    // create new instance with A4 as SDA, A5 as SCL and enable internal pullups
    SlowSoftI2CMaster si = SlowSoftI2CMaster(A4, A5, true);

    #define I2C_7BITADDR 0x68 // DS1307
    #define MEMLOC 0x0A
    #define ADDRLEN 1

  
    void setup(void) {
        Serial.begin(38400);
        if (!si.i2c_init()) // initialize I2C module
            Serial.println("I2C init failed");
    }

    void loop(void){
        if (!si.i2c_start((I2C_7BITADDR<<1)|I2C_WRITE)) { // init transfer
            Serial.println("I2C device busy");
            return;
        }
        si.i2c_write(MEMLOC); // send memory to device
        si.i2c_rep_start((I2C_7BITADDR<<1)|I2C_READ); // restart for reading
        byte val = si.i2c_read(true); // read one byte and send NAK afterwards
        si.i2c_stop(); // stop communication
        Serial.println(val);
        delay(1000);
    }


You can use the library in a way such that the internal pullup resistors of the AVR are used (as shown in the example). However, note that this implies that when switching between HIGH and LOW the bus will temporarily in a high-impedance state, which is outside the I2C specification. Furthermore, the internal pull-ups are around 50k and so might not deliver enough current. Be careful when using this option and consider it as a potential source of errors.

## Alternative interface
As in the case of the SoftI2CMaster library, there exists a [wrapper library](https://github.com/felias-fogg/SlowSoftWire) that emulates the functionality of the Wire class.

This software is published under the [LGPL](http://www.gnu.org/licenses/lgpl-3.0.html)
