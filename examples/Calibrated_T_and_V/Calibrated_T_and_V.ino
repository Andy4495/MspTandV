/* -----------------------------------------------------------------
    Example for MspTandV library.

    01/12/2018 - A.T. - Initial version
*/
/* -----------------------------------------------------------------
   Prints out calibrated and uncalibrated Temp and Voltage
   measurement to Serial every 5 seconds.

   The example prints out temperature in Fahrenheit; this can
   be changed to Celcius by changing the accessor function used.
*/

#include "MspTandV.h"

const int sleepTime = 5000;

MspTemp myTemp;
MspVcc  myVcc;

int loopCount = 0;

void setup() {

  Serial.begin(9600);
  pinMode(GREEN_LED, OUTPUT);


  Serial.println("Calibration Data");
  Serial.println("----------------");
  Serial.print("CAL_ADC_T30: ");
  Serial.println((int)*CAL_ADC_T30;
  Serial.print("CAL_ADC_T85: ");
  Serial.println((int)*CAL_ADC_T85);
  Serial.print("CAL_ADC_REF1_FACTOR: ");
  Serial.println(*CAL_ADC_REF1_FACTOR);
  Serial.print("CAL_ADC_REF2_FACTOR: ");
  Serial.println(*CAL_ADC_REF2_FACTOR);
  Serial.print("CAL_ADC_GAIN_FACTOR: ");
  Serial.println(*CAL_ADC_GAIN_FACTOR);
  Serial.print("CAL_ADC_OFFSET: ");
  Serial.println(*CAL_ADC_OFFSET);
  Serial.println("----------------");
  Serial.println(" ");
}

void loop() {

  digitalWrite(GREEN_LED, loopCount % 2);

  loopCount++;

  myTemp.read();
  myVcc.read();


  Serial.println("       Calibrated    Uncalibrated");
  Serial.println("       ----------    ------------");
  Serial.print  ("Temp   ");
  Serial.print(myTemp.getTempCalibratedF());
  Serial.print("           ");
  Serial.println(myTemp.getTempUncalibratedF());
  Serial.print  ("Batt   ");
  Serial.print(myVcc.getVccCalibrated());
  Serial.print("           ");
  Serial.println(myVcc.getVccUncalibrated());
  Serial.println(" ");

  delay(sleepTime);
}
