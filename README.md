MspTandV Library
====================

This library is designed to simplify the reading of the internal temperature
and Vcc values on MSP430 processors.

Getting accurate Vcc readings is particularly useful when powering a project
with batteries, so that you can get an indication of current battery level
and know when it is time to change batteries.

This library makes use of factory-programmed calibration settings unique to
each MSP430 chip, and runs all its calculations using integer math.

Usage
-----

Include the library header file:

    #include "MspTandV.h"

Create temperature and/or voltage objects to retrieve the data:

    MspTemp myMspTemp;
    MspVcc  myMspVcc;

Take a reading:

    myMspTemp.read();
        - Optional parameter "CAL_ONLY" saves a little processing time
          by only calculating calibrated temperature.
    myMspVcc.read();
        - Optional parameter "CAL_ONLY" saves a little processing time
          by only calculating calibrated Vcc.

Get the results:

        TempF = myMspTemp.getTempCalibrated(FAHRENHEIT); // Degrees Fahrenheit * 10
        TempC = MyMspTemp.getTempCalibrated(CELSIUS);    // Degrees Celsius * 10
        Vcc_mV = MyMspVcc.getVccCalibrated();            // Voltage in mV

Implementation Details
----------------------

The library uses formulas contained in various MPS430 datasheets and Family
User Guides from Texas Instruments. See the file "MspTandV_variants.h" for
specific details on parameters and technical documents used for each
processor type.

In general, the library makes use of two key formulas:

Temperature:

    TempC = (ADCraw - CAL_ADC_15T30) *
            ((85 - 30) / (CAL_ADC_15T85 - CAL_ADC15T30)) + 30

* The above equation assumes a linear voltage response to temperature changes
and uses two factory-programmed calibration readings to determine the slope
and intercept of the line.

Voltage:

    ADC_Calibrated = (ADCraw * CAL_ADCREF_FACTOR / 2^15) *
                     (CAL_ADC_GAIN_FACTOR / 2 ^ 15) + CAL_ADC_OFFSET

In my experience using a small sample size of MSP430 chips, I have found
that using a calibrated measurement for temperature is
absolutely necessary, as the uncalibrated and calibrated temperature readings
across chips can vary significantly -- 10s of degrees Fahrenheit.

Calibrating the Vcc reading had an impact of a few 10s of mV.

References
---------------------

+ MSP430G2553 [Family User's Guide] (http://www.ti.com/lit/ug/slau144j/slau144j.pdf).
+ MSP430G2553 [Device Datasheet] (http://www.ti.com/lit/ds/symlink/msp430g2553.pdf).
+ Texas Instruments E2E Forum thread regarding [ADC calibration] (https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/204428)
