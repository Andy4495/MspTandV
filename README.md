# MspTandV Library

[//]: # ( The formulas in this writeup are rendered with GitHub's support of LaTeX formatted math expressions. See references [10] and [11]. Since these expressions can be difficult to read as plain text, there is a Markdown comment with a plaintext readable version before the LaTeX formatted expression.  )

[![Arduino Compile Sketches](https://github.com/Andy4495/MspTandV/actions/workflows/arduino-compile-sketches.yml/badge.svg)](https://github.com/Andy4495/MspTandV/actions/workflows/arduino-compile-sketches.yml)
[![Check Markdown Links](https://github.com/Andy4495/MspTandV/actions/workflows/CheckMarkdownLinks.yml/badge.svg)](https://github.com/Andy4495/MspTandV/actions/workflows/CheckMarkdownLinks.yml)

This library provides simple, easy-to-use functions to return the calibrated internal temperature and Vcc level on supported MSP430 processor types.

This library makes use of the factory-programmed calibration settings unique to each chip, and runs all of its calculations using integer math.

Getting accurate Vcc readings is particularly useful when powering a project with batteries, so that you can get an indication of the current battery level and know when it is time to change batteries.

Supported MSP430 processor types: F5529, FR4133, FR6989, FR2433, FR5969, G2553, G2452.

**See [Note](#note-on-g2553g2452-low-voltage-operation) below for considerations with G2553/G2452 low voltage operation.**

## Usage

Include the library header file:

```cpp
#include "MspTandV.h"
```

Create temperature and/or voltage objects to retrieve the data:

```cpp
MspTemp myMspTemp;
MspVcc  myMspVcc;
```

Take a reading:

```cpp
/* Optional parameter "CAL_ONLY" saves a little processing time by only calculating calibrated temperature and skipping the uncalibrated temperature calculation. */

myMspTemp.read(CAL_ONLY);

myMspVcc.read(CAL_ONLY);
```

Get the results:

```cpp
TempF = myMspTemp.getTempCalibratedF();    // Degrees Fahrenheit * 10
TempC = myMspTemp.getTempCalibratedC();    // Degrees Celsius * 10
Vcc_mV = myMspVcc.getVccCalibrated();      // Voltage in mV
```

All return values are of type `int`.

## Implementation Details

The library uses formulas contained in various MPS430 datasheets and Family User Guides from Texas Instruments. See the file `MspTandV_variants.h` for specific details on parameters and technical documents used for each processor type.

In general, the library makes use of the following key formulas. The internal implementation uses slightly modified formulas to scale some of the parameters to ensure that the calculations can be performed using integer math.

### Temperature

The following equation assumes a linear voltage response to temperature changes and uses two factory-programmed calibration readings to determine the ADC to temperature relationship:

[//]: # ( TempC = [ADCraw - CAL_ADC_15T30] * [[85 - 30] / [CAL_ADC_15T85 - CAL_ADC15T30]] + 30 )

$$ TempC = (ADC_{raw} - CAL\textunderscore ADC\textunderscore 15T30) \times \left({85 - 30 \over CAL\textunderscore ADC\textunderscore 15T85 - CAL\textunderscore ADC\textunderscore 15T30}\right) + 30 $$

In my experience, I have found that using a calibrated measurement for temperature is absolutely necessary, as the uncalibrated and calibrated temperature readings can vary significantly (10s of degrees Fahrenheit).

### Voltage

First, set the ADC input channel. If the ADC has "Vcc/2" input channel, use that. Otherwise, use the internal voltage reference for the input channel. Take the ADC reading, which is referred to as $ ADC_{raw} $ below. 

Next, calibrate the ADC reading:

[//]: # ( ADC_Calibrated = [ADCraw * CAL_ADC_REF_FACTOR / 2^15] * [CAL_ADC_GAIN_FACTOR / 2 ^ 15] + CAL_ADC_OFFSET )

$$ ADC_{Calibrated} = \left(ADC_{raw} \times CAL\textunderscore ADC\textunderscore REF\textunderscore FACTOR \over 2^{15} \right) \times \left(CAL\textunderscore ADC\textunderscore GAIN\textunderscore FACTOR \over 2^{15} \right) + CAL\textunderscore ADC\textunderscore OFFSET $$

$ V_{cc} $ for ADC types that have a "Vcc/2" input channel is calculated with: 

[//]: # ( Vcc/2 = ADC_Calibrated * Vref / ADC_STEPS )

$$ V_{cc} = {ADC_{Calibrated} \over ADC\textunderscore STEPS} \times V_{ref} \times 2 $$

For ADC types that do not have a Vcc/2 input channel, $ V_{cc} $ is calculated using the known voltage from the internal reference, $ V_{ref} $:

[//]: # ( Vref = ADC_Calibrated * Vcc / ADC_STEPS )

$$ V_{ref} = {ADC_{Calibrated} \over ADC\textunderscore STEPS} \times V_{cc} $$

And solving for $ V_{cc} $ since $ V_{ref} $ is a known value:

[//]: # ( Vcc = Vref * ADC_STEPS / ADC_Calibrated )

$$ V_{cc} = {ADC\textunderscore STEPS \over ADC_{Calibrated}} \times V_{ref} $$

Based on my experience using a relatively small sample size of MSP430 chips, I have found that calibrating the Vcc reading had an impact of a few 10s of mV. However, I would still recommend using calibrated Vcc measurements over uncalibrated values.

## Note on FR4133 Usage

In my sample of four FR4133 processors (Rev B), none of them had the "1.5 V Reference Factor" calibration value programmed in the TLV structure. If the library detects an unprogrammed Reference Factor value on an FR4133 device, it uses a calibration factor of "1", which has the effect of ignoring the reference voltage portion of the Vcc calibration.

The value of TCsensor given in the [FR4133 datasheet][3] appears to be off by a factor of 10. The value used by the library is adjusted to account for this. This only affects the uncalibrated temperature reading.

## Note on G2553/G2452 Low Voltage Operation

The G2553 and G2452 device types can only run at a 16 Mhz system clock frequency when powered at 3.3 V. See Figure 1 in the [MSP430G2553][1] and [MSP430G2452][2] Device Datasheets. When operating with supply voltages lower than 3.3 V (e.g. in a battery-operated setup), you will need to configure a lower system clock frequency. The devices can be run at a supply voltage as low as 2.2 V when running at 8 MHz. By default, Energia sets the G2553 and G2452 system clock frequency at 16 Mhz. To run the device at 8 MHz, the `boards.txt` file needs to be edited to add an 8 MHz entry. See [this discussion](https://forum.43oh.com/topic/4094-msp430g2553-1mhz-or-16mhz-how-to-set-it/) for tips on editing `boards.txt` to change the system frequency.

Also, the internal 2.5V reference on the G2 devices needs a Vcc of at least 2.9V for proper operation. To allow proper Vcc readings in a low-voltage (e.g. battery-operated) environment, the library takes a voltage reading from the lower voltage reference first. It only takes a reading from the higher voltage reference if Vcc is high enough for proper operation of the higher voltage reference. This applies to all devices that use the "Vcc/2" ADC input  channel.

## References

- Texas Instruments E2E Forum thread regarding [ADC calibration](https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/204428)
- MSP430G2553 and MSP430G2452 [Family User's Guide](http://www.ti.com/lit/ug/slau144j/slau144j.pdf).
- MSP430G2553 [Device Datasheet][1].
- MSP430G2452 [Device Datasheet][2].
- MSP430F5529 [Family User's Guide](http://www.ti.com/lit/pdf/slau208).
- MSP430F5529 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430f5529.pdf).
- MSP430FR4133 and MSP430FR2433 [Family User's Guide](http://www.ti.com/lit/ug/slau445i/slau445i.pdf).
- MSP430FR4133 [Device Datasheet][3].
- MSP430FR2433 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430fr2433.pdf).
- MSP430FR6989 and MSP430FR5969 [Family User's Guide](https://www.ti.com/lit/ug/slau367p/slau367p.pdf).
- MSP430FR6989 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430fr6989.pdf).
- MSP430FR5969 [Device Datasheet](http://www.ti.com/lit/ds/symlink/msp430fr5969.pdf).

## License

The software and other files in this repository are released under what is commonly called the [MIT License][100]. See the file [`LICENSE.txt`][101] in this repository.

[1]: http://www.ti.com/lit/ds/symlink/msp430g2553.pdf
[2]: http://www.ti.com/lit/ds/symlink/msp430g2112.pdf
[3]: http://www.ti.com/lit/ds/symlink/msp430fr4133.pdf
[10]: https://github.blog/2022-05-19-math-support-in-markdown/
[11]: https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/writing-mathematical-expressions
[100]: https://choosealicense.com/licenses/mit/
[101]: ./LICENSE.txt
[200]: https://github.com/Andy4495/MspTandV
