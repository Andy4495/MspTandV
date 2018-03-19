MspTandV Library
====================

This Energia library provides simple, easy-to-use functions to return the
calibrated internal temperature and Vcc level on supported MSP430 processor types.

This library makes use of the factory-programmed calibration settings unique to
each chip, and runs all of its calculations using integer math.

Getting accurate Vcc readings is particularly useful when powering a project
with batteries, so that you can get an indication of the current battery level
and know when it is time to change batteries.

Supported MSP430 processor types: F5529, FR4133, FR6989, FR2433, FR5969, G2553 (see Note below
for considerations with G2553 low voltage operation).

Usage
-----

Include the library header file:

    #include "MspTandV.h"

Create temperature and/or voltage objects to retrieve the data:

    MspTemp myMspTemp;
    MspVcc  myMspVcc;

Take a reading:

    myMspTemp.read(CAL_ONLY);
        - Optional parameter "CAL_ONLY" saves a little processing time
          by only calculating calibrated temperature and skipping the
          uncalibrated temperature calculation.
    myMspVcc.read(CAL_ONLY);
        - Optional parameter "CAL_ONLY" saves a little processing time
          by only calculating calibrated Vcc and skipping the
          uncalibrated Vcc calculation.

Get the results:

        TempF = myMspTemp.getTempCalibratedF();    // Degrees Fahrenheit * 10
        TempC = MyMspTemp.getTempCalibratedC();    // Degrees Celsius * 10
        Vcc_mV = MyMspVcc.getVccCalibrated();      // Voltage in mV

All return values are of type `int.`

Implementation Details
----------------------

The library uses formulas contained in various MPS430 datasheets and Family
User Guides from Texas Instruments. See the file "MspTandV_variants.h" for
specific details on parameters and technical documents used for each
processor type.

In general, the library makes use of the following key formulas. The
actual implementation is modified to scale some of the parameters
to ensure that the calculations are performed using integer math.

#### Temperature: ####

    TempC = (ADCraw - CAL_ADC_15T30) *
            ((85 - 30) / (CAL_ADC_15T85 - CAL_ADC15T30)) + 30

* The above equation assumes a linear voltage response to temperature changes
and uses two factory-programmed calibration readings to determine the slope
and intercept of the line.

#### Voltage: ####

First, calibrate the ADC reading:

    ADC_Calibrated = (ADCraw * CAL_ADC_REF_FACTOR / 2^15) *
                     (CAL_ADC_GAIN_FACTOR / 2 ^ 15) + CAL_ADC_OFFSET

For ADC types that have a "Vcc/2" input channel:

    Vcc/2 = ADC_Calibrated * Vref / ADC_STEPS

Some ADC types do not have a Vcc/2 input channel, but instead have
an input channel for the internal voltage reference. In this case, get
the ADC reading for the known voltage reference level, and correlate
that to Vcc:

    Vref = ADC_Calibrated * Vcc / ADC_STEPS

Solving for Vcc where Vref is a known value (typically 1.5 V):

    Vcc = Vref * ADC_STEPS / ADC_Calibrated

In my experience using a small sample size of MSP430 chips, I have found
that using a calibrated measurement for temperature is
absolutely necessary, as the uncalibrated and calibrated temperature readings
across chips can vary significantly (10s of degrees Fahrenheit).

Calibrating the Vcc reading had an impact of a few 10s of mV at the most.

Note on FR4133 Usage
--------------------

In my sample of four FR4133 processors (Rev B), none of them had the "1.5 V
Reference Factor" calibration value programmed in the TLV structure. If the
library detects an unprogrammed Reference Factor value, it uses a calibration
factor of "1", which has the effect of ignoring the reference voltage portion
of the Vcc calibration.

The value of TCsensor given in the FR4133 datasheet appears to be off
by a factor of 10. The value used by the library is adjusted to account
for this. This only affects the uncalibrated temperature reading.

Note on G2553 Low Voltage Operation
-----------------------------------

Per the MSP430G2553 [Device Datasheet](http://www.ti.com/lit/ug/slau144j/slau144j.pdf)
Figure 1, the G2553 should only be run at
16 Mhz with a supply voltage of 3.3 V. In order to run it at lower voltages
(e.g. in a battery-operated setup), you will need to configure a lower system
frequency. The device and all its peripherals can be run at a supply voltage
as low as 2.2 V when running at 8 MHz. By default, Energia sets the G2553
system frequency at 16 Mhz. To run it a 8 MHz, the `boards.txt` file needs
to be edited to add an 8 MHz entry. See
[this discussion](https://forum.43oh.com/topic/4094-msp430g2553-1mhz-or-16mhz-how-to-set-it/)
for tips on editing `boards.txt` to change the system frequency.

References
---------------------

+ Texas Instruments E2E Forum thread regarding [ADC calibration](https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/204428)
+ MSP430G2553 [Family User's Guide](http://www.ti.com/lit/ug/slau144j/slau144j.pdf).
+ MSP430G2553 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430g2553.pdf).
+ MSP430F5529 [Family User's Guide](http://www.ti.com/lit/pdf/slau208).
+ MSP430F5529 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430f5529.pdf).
+ MSP430FR4133 and MSP430FR2433 [Family User's Guide](http://www.ti.com/lit/ug/slau445g/slau445g.pdf).
+ MSP430FR4133 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430fr4133.pdf).
+ MSP430FR6989 and MSP430FR5969 [Family User's Guide](http://www.ti.com/lit/ug/slau367o/slau367o.pdf).
+ MSP430FR6989 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430fr6989.pdf).
+ MSP430FR2433 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430fr2433.pdf).
+ MSP430FR5969 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430fr5969.pdf).
