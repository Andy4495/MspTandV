/* -----------------------------------------------------------------
   MspTandV Library
    01/10/2018 - A.T. Original

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


void MspTemp::read() {
    long c2ftemp;
    long uncalAdcScaleFactor;
    long Tc;
    int  ADCraw;

    /// Tc and uncalAdcScaleFactor should be in the constructor and stored with the object.
    Tc = 550000L / ((long)*(int*)ADC_CAL_T85 - (long)*(int*)ADC_CAL_T30);
    /// DEBUG
    Serial.print("Tc: ");
    Serial.println(Tc);

    // Used in the uncalibrated temp calculation to avoid overlowing long
    // 100,000 scaling factor * voltage ref in deci-Volts / # of ADC steps
    uncalAdcScaleFactor = 100000L * (long)TEMP_REF_DV / (long)ADC_STEPS;
    /// DEBUG
    Serial.print("uncalAdcScaleFactor: ");
    Serial.println(uncalAdcScaleFactor);
    // MSP430 internal temp sensor
    analogReference(TEMP_VREF);
    ADCraw = analogRead(TEMPSENSOR_CHAN);
    ADCraw = analogRead(TEMPSENSOR_CHAN);
    /// DEBUG
    Serial.print("Temp ADC Raw: ");
    Serial.println(ADCraw);
    CalibratedTempC = (Tc * (ADCraw - ((long)*(int*)ADC_CAL_T30)) + 300000L) / 1000L;
    c2ftemp = CalibratedTempC * 9L;
    CalibratedTempF = (c2ftemp / 5L) + 320L;
    // VSENSOR_UNCAL and TC_UNCAL are scaled by 100,000
    // Numerator has an extra factor of 10 to return 10ths of degrees
    UncalibratedTempC = (ADCraw * uncalAdcScaleFactor - (long)VSENSOR_UNCAL * 10L) / (long)TC_UNCAL;
    /// DEBUG
    Serial.print("UncalibratedTempC: ");
    Serial.println(UncalibratedTempC);
    c2ftemp = UncalibratedTempC * 9L;
    /// DEBUG
    Serial.print("c2ftemp: ");
    Serial.println(c2ftemp);    UncalibratedTempF = (c2ftemp / 5L) + 320L;
    /// DEBUG
    Serial.print("UncalibratedTempC: ");
    Serial.println(UncalibratedTempC);
//     ((((float)ADCraw * 1.5 / 1023.0) - 0.986) / 0.00355) * 1.8 + 32.0;
// Floating point, in F:     sensordata.MSP_T_uncalibrated = ((((float)ADCraw * 1.5 / 1023.0) - 0.986) / 0.00355) * 1.8 + 32.0;

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
    int  ADCraw, ADCraw1V5;

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
    /// DEBUG
    Serial.print("Vcc ADC Raw: ");
    Serial.println(ADCraw);
    // Need calculation to be Long int due to mV scaling
    msp430mV_unc = ADCraw * 2000L * (long)VCC_REF1_DV;
    /// DEBUG
    Serial.print("msp430mv_unc: ");
    Serial.println(msp430mV_unc);
    msp430mV_unc = msp430mV_unc / (long)ADC_STEPS;
    /// DEBUG
    Serial.print("msp430mv_unc (2nd calc): ");
    Serial.println(msp430mV_unc);
    if (msp430mV_unc < VCC_XOVER) {
      /// DEBUG
      Serial.println("Uncalibrated Voltage under crossover point.");
      analogReference(VCC_REF2);
      ADCraw1V5 = analogRead(VCC_CHAN);
      msp430mV_unc = ADCraw1V5 * 2000L * (long)VCC_REF2_DV;
      msp430mV_unc = msp430mV_unc / (long)ADC_STEPS;
    }
    UncalibratedVcc = msp430mV_unc;
    /// DEBUG
    Serial.print("UncalibratedVcc: ");
    Serial.println(UncalibratedVcc);


    // Shifting by 13 instead of 15 to retain precision through final calculation
    ADCcalibrated = ((long)ADCraw * (*(unsigned int*)ADC_CAL_REF1_FACTOR)) >> 13;
    /// DEBUG
    Serial.print("ADCcalibrated: ");
    Serial.println(ADCcalibrated);
    ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
    /// DEBUG
    Serial.print("ADCcalibrated: ");
    Serial.println(ADCcalibrated);
    // Need to shift the offset by 4 to match scaling on previous calculations
    ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);
    /// DEBUG
    Serial.print("ADCcalibrated: ");
    Serial.println(ADCcalibrated);
    // mV = 1000 mV/V * 2.5 Vref * 2 / 1023
    // --> The extra "2" term above is because we are measuring Vcc/2 on channel 11,
    //     so we need to scale it back to Vcc for final value
    // To keep integer math, combine 2*2.5*1000 -> 5000
    msp430mV = (ADCcalibrated * 2000L * (long)VCC_REF1_DV / (long)ADC_STEPS) + 0x0008L; // Add 8 to round up if bit 3 is 1
    /// DEBUG
    Serial.print("msp430mV: ");
    Serial.println(msp430mV);
    msp430mV = (msp430mV >> 4); // Shift 4 to adjust for scaling above
    /// DEBUG
    Serial.print("msp430mV: ");
    Serial.println(msp430mV);
    if (msp430mV < VCC_XOVER) {
      /// DEBUG
      Serial.println("Uncalibrated Voltage under crossover point.");
      ADCcalibrated = ((long)ADCraw1V5 * (*(unsigned int*)ADC_CAL_REF2_FACTOR)) >> 13;
      ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
      ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);
      msp430mV = (ADCcalibrated * 2000L * (long)VCC_REF2_DV/ (long)ADC_STEPS) + 0x0008L;  // 3000 instead of 5000 because of 1.5V reference
      msp430mV = (msp430mV >> 4);
    }
    CalibratedVcc = msp430mV;
    /// DEBUG
    Serial.print("CalibratedVcc: ");
    Serial.println(CalibratedVcc);
}

int MspVcc::getVccCalibrated(){
    return CalibratedVcc;
}

int MspVcc::getVccUncalibrated(){
    return UncalibratedVcc;
}
