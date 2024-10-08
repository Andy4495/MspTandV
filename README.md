# MspTandV Library

[//]: # ( The formulas in this writeup are rendered with GitHub's support of LaTeX formatted math expressions. See references [10] and [11]. Since these expressions can be difficult to read as plain text, there is a Markdown comment with a plaintext readable version before the LaTeX formatted expression.  )

[![Arduino Compile Sketches](https://github.com/Andy4495/MspTandV/actions/workflows/arduino-compile-sketches.yml/badge.svg)](https://github.com/Andy4495/MspTandV/actions/workflows/arduino-compile-sketches.yml)
[![Check Markdown Links](https://github.com/Andy4495/MspTandV/actions/workflows/check-links.yml/badge.svg)](https://github.com/Andy4495/MspTandV/actions/workflows/check-links.yml)

This library provides simple, easy-to-use functions to return the calibrated internal temperature and `Vcc` level on supported MSP430 processor types.

In addition to temperature and Vcc readings, see  [below](#calibrated-adc-value) for details on the `MspAdc` class to return calibrated ADC readings when using the chip's internal voltage references.

This library makes use of the factory-programmed calibration settings unique to each chip, and runs all of its calculations using integer math.

Getting accurate `Vcc` readings is particularly useful when powering a project with batteries, so that you can get an indication of the current battery level and know when it is time to change batteries.

Supported MSP430 processor types: F5529, FR4133, FR6989, FR2433, FR5969, G2553, G2452.

**See [Note](#clock-frequency-and-low-voltage-operation) below regarding clock frequencies when running the  G2553/G2452 in low voltage configurations.**

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

$$ TempC = ( ADC_{raw} - {CAL\\\_ ADC\\\_\textit{15}T\textit{30}} ) \times \left({85 - 30 \over CAL\\\_ADC\\\_\textit{15}T\textit{85} - CAL\\\_ADC\\\_\textit{15}T\textit{30}}\right) + 30 $$

#### Impact of Using Calibrated Temperature

In my experience, I have found that using a calibrated measurement for temperature is absolutely necessary, as the uncalibrated and calibrated temperature readings can vary significantly (tens of degrees Fahrenheit).

### Voltage

The voltage measurement and calculation differs depending on whether the ADC has a `Vcc/2` input channel.

#### ADC With `Vcc/2` Input Channel

*This applies to the G2, F5529, FR6989, and FR5969 processor types.*

First, set the ADC reference to the chip's lower internal voltage reference (start with the lower reference because `Vcc` might not be high enough to be in spec to support the higher reference).

Next, take the raw ADC reading ( $ADC_{raw}$ ) on the `Vcc/2` input channel. If the measured voltage is less than the crossover voltage, then continue with calculating the calibrated value. Otherwise, set the ADC reference to the higher internal voltage reference and re-take the ADC reading and then continue with calculating the calibrated value.

To calculate a calibrated value, $ADC_{Calibrated}$ , from the raw ADC reading, $ADC_{raw}$ , use the following formula:

[//]: # ( ADC_Calibrated = [ADCraw * CAL_ADC_REF_FACTOR / 2^15] * [CAL_ADC_GAIN_FACTOR / 2 ^ 15] + CAL_ADC_OFFSET )

$$ ADC_{Calibrated} = \left(ADC_{raw} \times CAL\\\_ADC\\\_REF\\\_FACTOR \over 2^{15} \right) \times \left(CAL\\\_ADC\\\_GAIN\\\_FACTOR \over 2^{15} \right) + {CAL\\\_ADC\\\_OFFSET} $$

Finally, calculate $V_{cc}$:

[//]: # ( Vcc/2 = ADC_Calibrated * Vref / ADC_STEPS )

$$ V_{cc} = {ADC_{Calibrated} \over ADC\\\_STEPS} \times V_{ref} \times 2 $$

#### ADC Without `Vcc/2` Input Channel

*This applies to the FR4133 and FR2433 processor types.*

First, set the ADC reference to `Vcc`.

Next, take the raw ADC reading ( $ADC_{raw}$ ) on the internal voltage reference ( $V_{ref}$ ) input channel.

Then, calculate a calibrated value, $ADC_{Calibrated}$ , from the raw ADC reading, $ADC_{raw}$ :

[//]: # ( ADC_Calibrated = [ADCraw * CAL_ADC_REF_FACTOR / 2^15] * [CAL_ADC_GAIN_FACTOR / 2 ^ 15] + CAL_ADC_OFFSET )

$$ ADC_{Calibrated} = \left(ADC_{raw} \times CAL\\\_ADC\\\_REF\\\_FACTOR \over 2^{15} \right) \times \left(CAL\\\_ADC\\\_GAIN\\\_FACTOR \over 2^{15} \right) + {CAL\\\_ADC\\\_OFFSET} $$

Finally, $V_{cc}$ is calculated using the known voltage from the internal reference, $V_{ref}$:

[//]: # ( Vref = ADC_Calibrated * Vcc / ADC_STEPS )

$$ V_{ref} = {ADC_{Calibrated} \over ADC\\\_STEPS} \times V_{cc} $$

And solving for $V_{cc}$ since $V_{ref}$ is a known value:

[//]: # ( Vcc = Vref * ADC_STEPS / ADC_Calibrated )

$$ V_{cc} = {ADC\\\_STEPS \over ADC_{Calibrated}} \times V_{ref} $$

#### Impact of Using Calibrated Voltage

Based on my experience using a relatively small sample size of MSP430 chips, I have found that calibrating the Vcc reading had an impact of a few tens of mV.

## Calibrated ADC Value

The library also has a class `MspAdc` which calculates a calibrated ADC value when using one of the chip's internal voltage references.

The constructor, `MspAdc(uint8_t pin, uint8_t ref_num)`, is called with the pin number to be read by the ADC along with the internal voltage reference number as listed in the table below.

| Processor Type   | Internal Reference | Reference Number in Constructor |
| --------------   | :----------------: | :-----------------------------: |
| G2553 or G2452   | 2.5 V              | 1                               |
|                  | 1.5 V              | 2                               |
| F5529            | 2.5 V              | 0                               |
|                  | 2.0 V              | 1                               |
|                  | 1.5 V              | 2                               |
| FR4133 or FR2433 | 1.5 V              | 1                               |
| FR6989 or FR5969 | 2.5 V              | 0                               |
|                  | 2.0 V              | 1                               |
|                  | 1.2 V              | 2                               |

For example, if reading pin 10 using the 1.5 V reference on an MSP430FR2433, use the constructor:

```cpp
MspAdc myAdc(10, 1);
```

The public methods are:

```cpp
void     read();              // ADC read of pin using internal voltage reference ref_num
uint16_t getAdcCalibrated();  // Returns the calibrated ADC value from previous read()
uint16_t getAdcRaw();         // Returns the raw ADC value from previous read()
```

Note that `getAdcCalibrated()` and `getAdcRaw()` do not initiate an `analogRead()`; they just return the data acquired from the last `read()`. You must call `read()` each time you want a new ADC measurement taken. See the [`Calibrated_ADC.ino` sketch][9] for an example on the usage.

## Supply Voltage Versus Clock Frequency

The various supported MSP430 processors have different minimum supply voltage requirements to run at the default system frequency. This becomes important in a battery-operated environment where `Vcc` may drop significantly below 3.3 V.

| Processor Type | Default Freq | Min `Vcc` at Default | Lower `Vcc` Frequency Limit |
| -------------- | :----------: | :------------------: | --------------------------- |
| F5529          | 25 MHz       | 2.4 V                | 8 MHz at `Vcc` &ge; 1.8 V   |
| FR4133         | 16 MHz       | 1.8 V                |                             |
| FR6989         | 16 MHz       | 1.8 V                |                             |
| FR2433         | 16 MHz       | 1.8 V                |                             |
| FR5969         | 16 MHz       | 1.8 V                |                             |
| G2553          | 16 MHz       | 3.3 V                | 8 MHz at `Vcc` &ge; 2.1 V   |
| G2452          | 16 MHz       | 3.3 V                | 8 MHz at `Vcc` &ge; 2.1 V   |

## Note on FR4133 Processors

In my sample of four FR4133 processors (Rev B), none of them had the "1.5 V Reference Factor" calibration value programmed in the TLV structure. If the library detects an unprogrammed Reference Factor value on an FR4133 device, it uses a calibration factor of "1", which has the effect of ignoring the reference voltage portion of the Vcc calibration.

The value of TCsensor given in the [FR4133 datasheet][3] appears to be off by a factor of 10. The value used by the library is adjusted to account for this. This only affects the uncalibrated temperature reading.

## Note on G2553/G2452 Processors

### Clock Frequency and Low Voltage Operation

Per Figure 1 in the [MSP430G2553][1] and [MSP430G2452][2] Device Datasheets, the G2553 and G2452 device types need a supply voltage of 3.3 V to run at a system frequency of 16 MHz. When operating at lower supply voltages (e.g. in a battery-operated setup), a lower system clock frequency must be used. The devices can be run at a supply voltage as low as 2.1 V when running at 8 MHz.

By default, the MPS430 boards package sets the G2553 and G2452 system frequency at 16 Mhz. To run the device at 8 MHz, the `boards.txt` file needs to be edited to add an 8 MHz entry. This repo contains an edited [`boards.txt`](./extras/boards.txt) file which is based on the MSP 1.0.7 board package. An 8 MHz entry for the G2553 processor has been added, along with editing the original G2553 entry to clarify that it is 16 MHz.

The `boards.txt` file used by Energia is located at `~/Library/Energia15/packages/energia/hardware/msp430/1.0.7` on MacOS, and a similar path on Windows and Linux.

The `boards.txt` file used by Arduino or Visual Studio Code is located at `~/Library/Arduino15/packages/energia/hardware/msp430/1.0.7` on MacOS, and a similar path on Windows and Linux. Note that the specific package version (1.0.7 in this case) may be different depending on what you have installed.

### Internal Voltage Reference

The internal 2.5 V reference on the G2 devices needs a `Vcc` of at least 2.9V for proper operation. To allow proper `Vcc` readings in a low-voltage (e.g. battery-operated) environment, the library takes a voltage reading from the lower voltage reference first. It only takes a reading from the higher voltage reference if `Vcc` is high enough for proper operation of the higher voltage reference.

## References

- Texas Instruments E2E Forum thread regarding [ADC calibration](https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/204428)
- [43oh discussion][7] ([archived version][8]) on editing `boards.txt` to change the system frequency.
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
[7]: https://forum.43oh.com/topic/4094-msp430g2553-1mhz-or-16mhz-how-to-set-it/
[8]: ./extras/43oh-MSP430g2553-1mhz.pdf
[9]: ./examples/Calibrated_ADC/Calibrated_ADC.ino
[//]: # ([10]: https://github.blog/2022-05-19-math-support-in-markdown/)
[//]: # ([11]: https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/writing-mathematical-expressions)
[100]: https://choosealicense.com/licenses/mit/
[101]: ./LICENSE.txt
[//]: # ([200]: https://github.com/Andy4495/MspTandV)
