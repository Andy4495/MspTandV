/* -----------------------------------------------------------------
   MspTandV Library Implementation
   https://gitlab.com/Andy4495
   MIT License

   01/16/2018 - A.T. - Original
   01/17/2018 - A.T. - Add support for VCC_TYPE of VCC
   07/06/2018 - A.T. - Switch order of internal reference voltages used in Vcc
                       calibration (i.e., use lower-voltage ref first). This is
                       to properly support G2 processors in low voltage
                       configurations.
*/
/*
   Library to retrieve chip temperature and Vcc measurement.
   The library uses the chip-specific calibration settings
   to return a calibrated temperature and Vcc measurement.

   The library uses integer-only math and therefore saves
   about 4K of program memory compared to using floating
   point math.

   The values and formulas used in the calculations are
   derived from datasheets and other information from Texas
   Instruments. See the separate README file included
   with this library for details.

   Temperature is returned as 10th degrees Fahrenheit or Celsius
      For example, 783 represents 78.3 degrees

   Voltage is returned as milliVolts
      For example, 3033 represents 3.033 volts
*/
/*
   Using the library:
   1. Create a temperature or voltage object:
        MspTemp myMspTemp;
        MspVcc  myMspVcc;
   2. Take a reading:
        myMspTemp.read();
        - Optional parameter "CAL_ONLY" saves a little processing time
          by only calculating calibrated temperature.
        myMspVcc.read();
        - Optional parameter "CAL_ONLY" saves a little processing time
          by only calculating calibrated Vcc.
   3. Get the results:
        TempF = myMspTemp.getTempCalibratedF();    // Degrees Fahrenheit * 10
        TempC = MyMspTemp.getTempCalibratedC();    // Degrees Celsius * 10
        Vcc_mV = MyMspVcc.getVccCalibrated();      // Voltage in mV
*/

#include "MspTandV.h"
#include "Arduino.h"

MspTemp::MspTemp() {
  // Tc and uncalAdcScaleFactor calculated in constructor and stored with object.
  Tc = 550000L / ((long)*(int*)ADC_CAL_T85 - (long)*(int*)ADC_CAL_T30);

  // Used in the uncalibrated temp calculation to avoid overflowing long
  // 100,000 scaling factor * voltage ref in deci-Volts / # of ADC steps
  uncalAdcScaleFactor = 100000L * (long)TEMP_REF_DV / (long)ADC_STEPS;

  CalibratedTempC = 0;      // Degrees * 10
  CalibratedTempF = 0;      // Degrees * 10
  UncalibratedTempC = 0;    // Degrees * 10
  UncalibratedTempF = 0;    // Degrees * 10

}

void MspTemp::read(int meas_type) {
    long c2ftemp;
    int  ADCraw;

    // MSP430 internal temp sensor
    analogReference(TEMP_VREF);
    ADCraw = analogRead(TEMPSENSOR_CHAN);
    ADCraw = analogRead(TEMPSENSOR_CHAN);
    CalibratedTempC = (Tc * (ADCraw - ((long)*(int*)ADC_CAL_T30)) + 300000L) / 1000L;
    c2ftemp = CalibratedTempC * 9L;
    CalibratedTempF = (c2ftemp / 5L) + 320L;
    if (meas_type == CAL_AND_UNCAL) {
      // VSENSOR_UNCAL and TC_UNCAL are scaled by 100,000
      // Numerator has an extra factor of 10 to return 10ths of degrees
      UncalibratedTempC = (ADCraw * uncalAdcScaleFactor - (long)VSENSOR_UNCAL * 10L) / (long)TC_UNCAL;
      c2ftemp = UncalibratedTempC * 9L;
      UncalibratedTempF = (c2ftemp / 5L) + 320L;
    }
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

MspVcc::MspVcc() {
  CalibratedVcc = 0;
  UncalibratedVcc = 0;

  // My sample of FR4133 processors are all missing the 1.5V Reference
  // Factor calibration value in the TLV structure -- it is set to
  // a default value of 65535.
  // If the value appears to be unprogrammed (0xFFFF), then set it to
  // a default value of "1" which is represented by 0x8000 since it is
  // scaled by 32768
  if (*(unsigned int*)ADC_CAL_REF1_FACTOR == 0xFFFF)
    Vref1Calibration = 0x8000;
  else
    Vref1Calibration = *(unsigned int*)ADC_CAL_REF1_FACTOR;
}

void MspVcc::read(int meas_type){
    long msp430mV, msp430mV_unc, ADCcalibrated;
    unsigned int  ADCraw, ADCrawXoverRef;

    //
    // **** VCC_TYPE == VCCDIV2 *****
    // For VCC_TYPE of VCCDIV2, we are measuring Vcc/2 wrt the
    // internal voltage references. Then multiply the result by 2
    // in order to get back to Vcc.
    // First try the lower reference voltage, since the G2 microcontrollers
    // need a higher Vcc for proper operation of the higher reference voltage.
    if (VCC_TYPE == VCCDIV2){
      analogReference(VCC_REF2);
      ADCraw = analogRead(VCC_CHAN);
      if (meas_type == CAL_AND_UNCAL) {
        // Need calculation to be Long int due to mV scaling
        // Multiply by 1000 to convert to mV
        // Multiply by 2 to convert Vcc/2 to VCC
        // Divide by 10 since VCC_REF_DV is scaled by 10
        // --> 1000 * 2 / 10 = 200
        msp430mV_unc = ADCraw * 200L * (long)VCC_REF2_DV;
        msp430mV_unc = msp430mV_unc / (long)ADC_STEPS;
        if (msp430mV_unc > VCC_XOVER) {
          analogReference(VCC_REF1);
          ADCrawXoverRef = analogRead(VCC_CHAN);
          msp430mV_unc = ADCrawXoverRef * 200L * (long)VCC_REF1_DV;
          msp430mV_unc = msp430mV_unc / (long)ADC_STEPS;
        }
        UncalibratedVcc = msp430mV_unc;
      }
      // Shifting by 13 instead of 15 to retain precision through final calculation
      ADCcalibrated = ((unsigned long)ADCraw * (*(unsigned int*)ADC_CAL_REF2_FACTOR)) >> 13;
      ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
      // Need to shift the offset by 4 to match scaling on previous calculations
      ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);

      // mV = 1000 mV/V * Vref * 2 / 1023
      // --> The extra "2" term above is because we are measuring Vcc/2,
      //     so we need to scale it back to Vcc for final value
      // VCC_REF1_DV is scaled by 10, so mV multipler is 200 instead of 2000
      msp430mV = ((unsigned long)ADCcalibrated * 200UL * (unsigned long)VCC_REF2_DV / (unsigned long)ADC_STEPS) + 0x0008UL; // Add 8 to round up if bit 3 is 1
      msp430mV = (msp430mV >> 4); // Shift 4 to adjust for scaling above
      if (msp430mV > VCC_XOVER) {
        analogReference(VCC_REF1);
        ADCrawXoverRef = analogRead(VCC_CHAN);
        ADCcalibrated = ((unsigned long)ADCrawXoverRef * (*(unsigned int*)ADC_CAL_REF1_FACTOR)) >> 13;
        ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
        ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);
        msp430mV = ((unsigned long)ADCcalibrated * 200UL * (unsigned long)VCC_REF1_DV / (unsigned long)ADC_STEPS) + 0x0008UL;
        msp430mV = (msp430mV >> 4);
      }
      CalibratedVcc = msp430mV;
    }
    else {   // **** VCC_TYPE == VCC *****
      // For VCC_TYPE of VCC, we are measuring the internal voltage
      // reference wrt Vcc.
      // Vref = ADCraw * Vcc / ADC_STEPS
      //  --> Vcc = Vref * ADC_STEPS / ADCraw
      analogReference(VCC_REF1);
      ADCraw = analogRead(VCC_CHAN);
      if (meas_type == CAL_AND_UNCAL) {
        // Measure ADC reference voltage wrt Vcc
        // VREF = ADCraw * Vcc / 1023
        // --> Vcc = VREF * 1023 / ADCraw
        // Need calculation to be Long int due to mV scaling
        // Multiply by 1000 to convert to mV
        // Divide by 10 since VCC_REF_DV is scaled by 10
        // --> 1000 * / 10 = 100
        msp430mV_unc = (long)VCC_REF1_DV * 100L * (long)ADC_STEPS;
        msp430mV_unc = msp430mV_unc / (long)ADCraw;
        if (msp430mV_unc < VCC_XOVER) {
          analogReference(VCC_REF2);
          ADCrawXoverRef = analogRead(VCC_CHAN);
          msp430mV_unc = (long)VCC_REF2_DV * 100L * (long)ADC_STEPS;
          msp430mV_unc = msp430mV_unc / (long)ADCrawXoverRef;
        }
        UncalibratedVcc = msp430mV_unc;
      }
      // Shifting by 13 instead of 15 to retain precision through final calculation
      ADCcalibrated = ((unsigned long)ADCraw * (Vref1Calibration)) >> 13;
      ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
      // Need to shift the offset by 4 to match scaling on previous calculations
      ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);
      // Similar calculation as above, except using calibrated ADC reading
      ADCcalibrated = ADCcalibrated + 0x0008UL; // Add 8 to round up if bit 3 is 1
      ADCcalibrated = ADCcalibrated >> 4; // Un-scale the ADC value before dividing
      msp430mV = (((unsigned long)VCC_REF1_DV * 100UL * (unsigned long) ADC_STEPS) / (unsigned long) ADCcalibrated);
      if (msp430mV < VCC_XOVER) {
        analogReference(VCC_REF2);
        ADCrawXoverRef = analogRead(VCC_CHAN);
        ADCcalibrated = ((unsigned long)ADCrawXoverRef * (*(unsigned int*)ADC_CAL_REF2_FACTOR)) >> 13;
        ADCcalibrated = (ADCcalibrated * (*(unsigned int*)ADC_CAL_GAIN_FACTOR)) >> 13;
        ADCcalibrated = ADCcalibrated + ((*(int*)ADC_CAL_OFFSET_FACTOR) << 4);
        ADCcalibrated = ADCcalibrated + 0x0008UL; // Add 8 to round up if bit 3 is 1
        ADCcalibrated = ADCcalibrated >> 4; // Un-scale the ADC value before dividing
        msp430mV = (((unsigned long)VCC_REF2_DV * 100UL * (unsigned long) ADC_STEPS) / (unsigned long) ADCcalibrated);
      }
      CalibratedVcc = msp430mV;
    }
}

int MspVcc::getVccCalibrated(){
    return CalibratedVcc;
}

int MspVcc::getVccUncalibrated(){
    return UncalibratedVcc;
}
