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
*/

#ifndef MSPTANDV
#define MSPTANDV

#include "MspTandV_variants.h"

class MspTemp {
public:
  void read();
  int getTempCalibratedC();
  int getTempUncalibratedC();
  int getTempCalibratedF();
  int getTempUncalibratedF();
private:
  int CalibratedTempC;      // Degrees * 10
  int CalibratedTempF;      // Degrees * 10
  int UncalibratedTempC;    // Degrees * 10
  int UncalibratedTempF;    // Degrees * 10
};

class MspVcc {
public:
  void read();
  int getVccCalibrated();
  int getVccUncalibrated();

private:
  int CalibratedVcc;       // milliVolts
  int UncalibratedVcc;     // milliVolts
};

enum VCC_TYPE {VCCDIV2, VCC};

#endif
