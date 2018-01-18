MspTandV Library
====================

This library is designed to simplify the reading of the internal temperature
and Vcc values on MSP430 processors.

Getting accurate Vcc readings is particularly useful when powering a project
with batteries, so that you can get an indication of the current battery level
and know when it is time to change batteries.

This library makes use of factory-programmed calibration settings unique to
each MSP430 chip, and runs all its calculations using integer math.

Currently supported MSP430 processors: G2553, F5529, FR4133.

Future support planned for: FR2433, FR6989, FR5969.  

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

Implementation Details
----------------------

The library uses formulas contained in various MPS430 datasheets and Family
User Guides from Texas Instruments. See the file "MspTandV_variants.h" for
specific details on parameters and technical documents used for each
processor type.

In general, the library makes use of the following key formulas. The
actual implementation is modified to scale some of the parameters
to ensure that integer-only operations are needed.

#### Temperature: ####

    TempC = (ADCraw - CAL_ADC_15T30) *
            ((85 - 30) / (CAL_ADC_15T85 - CAL_ADC15T30)) + 30

* The above equation assumes a linear voltage response to temperature changes
and uses two factory-programmed calibration readings to determine the slope
and intercept of the line.

#### Voltage: ####

First, calibrate the ADC reading:

    ADC_Calibrated = (ADCraw * CAL_ADCREF_FACTOR / 2^15) *
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

References
---------------------

+ Texas Instruments E2E Forum thread regarding [ADC calibration] (https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/204428)
+ MSP430G2553 [Family User's Guide] (http://www.ti.com/lit/ug/slau144j/slau144j.pdf).
+ MSP430G2553 [Device Datasheet] (http://www.ti.com/lit/ds/symlink/msp430g2553.pdf).
+ MSP430F5529 [Family User's Guide] (http://www.ti.com/lit/pdf/slau208).
+ MSP430F5529 [Device Datasheet] (http://www.ti.com/lit/ds/symlink/msp430f5529.pdf).
+ MSP430FR4133 [Family User's Guide] (http://www.ti.com/lit/ug/slau445g/slau445g.pdf).
+ MSP430FR4133 [Device Datasheet] (http://www.ti.com/lit/ds/symlink/msp430fr4133.pdf).
