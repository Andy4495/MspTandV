/* -----------------------------------------------------------------
   MspTandV Library Implementation
   https://gitlab.com/Andy4495
   MIT License

   01/10/2018 - A.T. - Original

*/
/*
   Library to retrieve chip temperature and Vcc measurement.
   The library uses the chip-specific calibration settings
   to return a calibrated temperature and Vcc measurement.

   The library uses integer-only math and therefore saves
   about 4K of program memory if implemented using floating
   point.

   The values and formulas used in the calculations are
   derived from datasheets and other information from Texas
   Instruments. See the separate README file included
   with this library for details.

   Temperature is returned as 10th degrees Fahrenheit or Celsius
      + 783 represents 78.3 degrees

   Voltage is returned as milliVolts
      + 3033 represents 3.033 volts
*/
/*
   Using the library:
   1. Create a temperature or voltage object:
        MspTemp myMspTemp;
        MspVcc  myMspVcc;
   2. Take a reading:
        myMspTemp.read();
        myMspVcc.read();
   3. Get the results:
        TempF = myMspTemp.getTempCalibrated(FAHRENHEIT); // Degrees Fahrenheit * 10
        TempC = MyMspTemp.getTempCalibrated(CELSIUS);  // Degrees Celsius * 10
        Vcc_mV = MyMspVcc.getVccCalibrated();
*/

#include "MspTandV.h"
#include "Arduino.h"

MspTemp::MspTemp() {
  /// Tc and uncalAdcScaleFactor should be in the constructor and stored with the object.
  Tc = 550000L / ((long)*(int*)ADC_CAL_T85 - (long)*(int*)ADC_CAL_T30);

  // Used in the uncalibrated temp calculation to avoid overlowing long
  // 100,000 scaling factor * voltage ref in deci-Volts / # of ADC steps
  uncalAdcScaleFactor = 100000L * (long)TEMP_REF_DV / (long)ADC_STEPS;

  CalibratedTempC = 0;      // Degrees * 10
  CalibratedTempF = 0;      // Degrees * 10
  UncalibratedTempC = 0;    // Degrees * 10
  UncalibratedTempF = 0;    // Degrees * 10

}

void MspTemp::read() {
    long c2ftemp;
    int  ADCraw;

    // MSP430 internal temp sensor
    analogReference(TEMP_VREF);
    ADCraw = analogRead(TEMPSENSOR_CHAN);
    ADCraw = analogRead(TEMPSENSOR_CHAN);
    CalibratedTempC = (Tc * (ADCraw - ((long)*(int*)ADC_CAL_T30)) + 300000L) / 1000L;
    c2ftemp = CalibratedTempC * 9L;
    CalibratedTempF = (c2ftemp / 5L) + 320L;
    // VSENSOR_UNCAL and TC_UNCAL are scaled by 100,000
    // Numerator has an extra factor of 10 to return 10ths of degrees
    UncalibratedTempC = (ADCraw * uncalAdcScaleFactor - (long)VSENSOR_UNCAL * 10L) / (long)TC_UNCAL;
    c2ftemp = UncalibratedTempC * 9L;
    UncalibratedTempF = (c2ftemp / 5L) + 320L;
}

int MspTemp::getTempCalibratedC() {
      return CalibratedTempC;
}

int MspTemp::getTempUncalibratedC() {
      return UncalibratedTempC;
}

int MspTemp::getTempCalibratedF() {
      return CalibratedTempF;
}

int MspTemp::getTempUncalibratedF() {
      return UncalibratedTempF;
}

void MspVcc::read(){
    long msp430mV, msp430mV_unc, ADCcalibrated;
    unsigned int  ADCraw, ADCraw1V5;

    // *********
    // The ordering of these calculations should be reversed:
    //   -- Use the calibrated value to decide if 2nd measurement
    //      with lower ref voltage is needed. Then calculate calibrated
    //      and uncalibrated at same time.
    // *********
    // This code currently only works with VCC_TYPE of VCCDIV2
    // *********
    // First try the higher reference voltage
    analogReference(VCC_REF1);
    ADCraw = analogRead(VCC_CHAN);
    // Need calculation to be Long int due to mV scaling
    // Multiply by 1000 to convert to mV
    // Multiple by 2 to convert Vcc/2 to VCC
    // Divide by 10 since VCC_REF_DV is scaled by 10
    // --> 1000 * 2 / 10 = 200
    msp430mV_unc = ADCraw * 200L * (long)VCC_REF1_DV;
    msp430mV_unc = msp430mV_unc / (long)ADC_STEPS;
    if (msp430mV_unc < VCC_XOVER) {
      analogReference(VCC_REF2);
      ADCraw1V5 = analogRead(VCC_CHAN);
      msp430mV_unc = ADCraw1V5 * 200L * (long)VCC_REF2_DV;
      msp430mV_unc = msp430mV_unc / (long)ADC_STEPS;
    }
    UncalibratedVcc = msp430mV_unc;

    // Shifting by 13 instead of 15 to retain precision through final calculation
    ADCcalibrated = ((unsigned long)ADCraw * (*(unsigned int*)ADC_CAL_REF1_FACTOR)) >> 13;
    ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
    // Need to shift the offset by 4 to match scaling on previous calculations
    ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);

    // mV = 1000 mV/V * Vref * 2 / 1023
    // --> The extra "2" term above is because we are measuring Vcc/2,
    //     so we need to scale it back to Vcc for final value
    // VCC_REF1_DV is scaled by 10, so mV multipler is 200 instead of 2000
    msp430mV = ((unsigned long)ADCcalibrated * 200UL * (unsigned long)VCC_REF1_DV / (unsigned long)ADC_STEPS) + 0x0008UL; // Add 8 to round up if bit 3 is 1
    msp430mV = (msp430mV >> 4); // Shift 4 to adjust for scaling above
    if (msp430mV < VCC_XOVER) {
      analogReference(VCC_REF2);
      ADCraw1V5 = analogRead(VCC_CHAN);
      ADCcalibrated = ((unsigned long)ADCraw1V5 * (*(unsigned int*)ADC_CAL_REF2_FACTOR)) >> 13;
      ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
      ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);
      msp430mV = ((unsigned long)ADCcalibrated * 200UL * (unsigned long)VCC_REF2_DV / (unsigned long)ADC_STEPS) + 0x0008UL;  // 3000 instead of 5000 because of 1.5V reference
      msp430mV = (msp430mV >> 4);
    }
    CalibratedVcc = msp430mV;
}

int MspVcc::getVccCalibrated(){
    return CalibratedVcc;
}

int MspVcc::getVccUncalibrated(){
    return UncalibratedVcc;
}
