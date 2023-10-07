/* -----------------------------------------------------------------
   MspTandV Library Example Sketch
   https://github.com/Andy4495/MspTandV
   MIT License

   06-Sep-2023 Andy4495 Original
*/
/* -----------------------------------------------------------------

   Prints out calibrated and uncalibrated ADC reading on 
   pin 10. 

   This example is written generically enough that it 
   probably works on all supported processors, but it was 
   only tested on the FR2433 variant.

*/

#include "MspTandV.h"

const unsigned long delayTime = 1000;
unsigned long prevMillis = 0;

// Read pin 10 and use voltage reference 1 (1.5V on FR2433)
MspAdc myAdc(10, 1);

void setup() {

  Serial.begin(9600);

  Serial.println("On-chip Calibration Memory Location and Data");
  Serial.println("--------------------------------------------");
  Serial.print("CAL_ADC_REF1_FACTOR: @");
  Serial.print((int)ADC_CAL_REF1_FACTOR, HEX);
  Serial.print("          ");
  Serial.println(*(unsigned int*)ADC_CAL_REF1_FACTOR);
  Serial.print("CAL_ADC_GAIN_FACTOR: @");
  Serial.print((int)ADC_CAL_GAIN_FACTOR, HEX);
  Serial.print("          ");
  Serial.println(*(unsigned int*)ADC_CAL_GAIN_FACTOR);
  Serial.print("CAL_ADC_OFFSET:      @");
  Serial.print((int)ADC_CAL_OFFSET_FACTOR, HEX);
  Serial.print("          ");
  Serial.println(*(int*)ADC_CAL_OFFSET_FACTOR);
  Serial.println("----------------");
  Serial.println("");
}

void loop() {

  uint32_t millivolts;

  if (millis() - prevMillis > delayTime) {
    prevMillis = millis();
    myAdc.read();

    Serial.print("ADC reading Uncalibrated, Calibrated, mV: ");
    Serial.print(myAdc.getAdcRaw());
    Serial.print(", ");
    Serial.print(myAdc.getAdcCalibrated());
    Serial.print(", ");
    // VCC_REF1_DV is in decivolts, so need to multiply by 100 to get millivolts
    millivolts = myAdc.getAdcCalibrated() * VCC_REF1_DV * 100UL / ADC_STEPS;
    Serial.print(millivolts);
    Serial.println("");
  }
}
