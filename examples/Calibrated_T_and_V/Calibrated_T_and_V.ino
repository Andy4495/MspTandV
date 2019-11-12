/* -----------------------------------------------------------------
   MspTandV Library Example Sketch
   https://github.com/Andy4495/MspTandV
   MIT License

   01/16/2018 - A.T. - Original
   01/18/2018 - A.T. - Add support for other processor variants
*/
/* -----------------------------------------------------------------

   Prints out calibrated and uncalibrated Temp and Voltage
   measurement to Serial every 5 seconds.

   Temperature is displayed in 1/10th degrees
     --> 736 represents 73.6 degrees
   Vcc is displayed in mV
     --> 3295 represents 3.295 volts

*/

#include "MspTandV.h"

const int sleepTime = 5000;

MspTemp myTemp;
MspVcc  myVcc;

int loopCount = 1;

void setup() {

  Serial.begin(9600);

#if defined(__MSP430G2553__)
  Serial.println("msp430g2553");
#define BOARD_LED GREEN_LED
#endif

#if defined(__MSP430F5529__)
  Serial.println("msp430f5529");
#define BOARD_LED GREEN_LED
#endif

#if defined(__MSP430FR4133__)
  Serial.println("msp430fr4133");
#define BOARD_LED LED2
#endif

#if defined(__MSP430FR6989__)
  Serial.println("msp430fr6989");
#define BOARD_LED GREEN_LED
#endif

#if defined(__MSP430FR2433__)
  Serial.println("msp430fr2433");
#define BOARD_LED LED2
#endif

#if defined(__MSP430FR5969__)
  Serial.println("msp430fr5969");
#define BOARD_LED LED2
#endif

  pinMode(BOARD_LED, OUTPUT);

  Serial.println("On-chip Calibration Memory Location and Data");
  Serial.println("--------------------------------------------");
  Serial.print("CAL_ADC_T30:         @");
  Serial.print((int)ADC_CAL_T30, HEX);
  Serial.print("          ");
  Serial.println(*(int*)ADC_CAL_T30);
  Serial.print("CAL_ADC_T85:         @");
  Serial.print((int)ADC_CAL_T85, HEX);
  Serial.print("          ");
  Serial.println(*(int*)ADC_CAL_T85);
  Serial.print("CAL_ADC_REF1_FACTOR: @");
  Serial.print((int)ADC_CAL_REF1_FACTOR, HEX);
  Serial.print("          ");
  Serial.println(*(unsigned int*)ADC_CAL_REF1_FACTOR);
  Serial.print("CAL_ADC_REF2_FACTOR: @");
  Serial.print(ADC_CAL_REF2_FACTOR, HEX);
  Serial.print("          ");
  Serial.println(*(unsigned int*)ADC_CAL_REF2_FACTOR);
  Serial.print("CAL_ADC_GAIN_FACTOR: @");
  Serial.print((int)ADC_CAL_GAIN_FACTOR, HEX);
  Serial.print("          ");
  Serial.println(*(unsigned int*)ADC_CAL_GAIN_FACTOR);
  Serial.print("CAL_ADC_OFFSET:      @");
  Serial.print((int)ADC_CAL_OFFSET_FACTOR, HEX);
  Serial.print("          ");
  Serial.println(*(int*)ADC_CAL_OFFSET_FACTOR);
  Serial.println("----------------");
  Serial.println(" ");

}

void loop() {

  // Alternate the LED each loop to know that program is running
  digitalWrite(BOARD_LED, loopCount++ % 2);

  myTemp.read();
  myVcc.read();

  Serial.println("           Calibrated    Uncalibrated");
  Serial.println("           ----------    ------------");
  Serial.print  ("Temp (C)       ");
  Serial.print(myTemp.getTempCalibratedC());
  Serial.print("           ");
  Serial.println(myTemp.getTempUncalibratedC());
  Serial.print  ("Temp (F)       ");
  Serial.print(myTemp.getTempCalibratedF());
  Serial.print("           ");
  Serial.println(myTemp.getTempUncalibratedF());
  Serial.print  ("Vcc  (mV)     ");
  Serial.print(myVcc.getVccCalibrated());
  Serial.print("          ");
  Serial.println(myVcc.getVccUncalibrated());
  Serial.println(" ");

  delay(sleepTime);
}
