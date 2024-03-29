/* -----------------------------------------------------------------
   MspTandV Library Header
   https://github.com/Andy4495/MspTandV
   MIT License

   01/16/2018 - A.T. - Original
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
*/

#ifndef MSPTANDV
#define MSPTANDV

#include "MspTandV_variants.h"
#include "Arduino.h"

enum MEAS_TYPE {CAL_AND_UNCAL, CAL_ONLY};

class MspTemp {
public:
  MspTemp();
  void read(int meas_type = CAL_AND_UNCAL);
  int getTempCalibratedC();
  int getTempUncalibratedC();
  int getTempCalibratedF();
  int getTempUncalibratedF();
private:
  int CalibratedTempC;      // Degrees * 10
  int CalibratedTempF;      // Degrees * 10
  int UncalibratedTempC;    // Degrees * 10
  int UncalibratedTempF;    // Degrees * 10
  long Tc;                  // Temperature calibration factor
  long uncalAdcScaleFactor; // Scaling factor used in uncalibrated calc
};

class MspVcc {
public:
  MspVcc();
  void read(int meas_type = CAL_AND_UNCAL);
  int getVccCalibrated();
  int getVccUncalibrated();

private:
  int CalibratedVcc;       // milliVolts
  int UncalibratedVcc;     // milliVolts
  unsigned int Vref1Calibration;
};

class MspAdc {
public:
  MspAdc(uint8_t channel, uint8_t voltage_ref_number);
  void read();
  uint16_t getAdcCalibrated();
  uint16_t getAdcRaw();

private:
  uint16_t CalibratedAdc;
  uint16_t ADCraw;
  uint16_t VrefCalibration;
  uint8_t  _channel;
  uint8_t  _voltage_ref;
};

enum VCC_TYPE {VCCDIV2, VCC};

#endif
