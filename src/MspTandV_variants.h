/* -----------------------------------------------------------------
   MspTandV Library - Processor Specific Definitions
   https://gitlab.com/Andy4495
   MIT License

   01/16/2018 - A.T. - Original
*/
/*
   This file contains the processor-specific definitions for the
   various calculations needed for temperature and Vcc measurements.
*/

#if defined(__MSP430G2553__)
/* MSP430G2553 - Reference Information
   Device datasheet doc number:           slas735
      ADC electrical characteristics:     pp. 38, 40
      Calibration data memory location:   p. 15
   Family guide doc number:               slau144
      ADC description                     Ch. 22
      Uncalibrated temp measurement:      Ch. 22.2.8
      Calibrated temp measurement         Ch 24.2.2.1
   ADC type:                              ADC10
   Voltage references available:          INTERNAL2V5
                                          INTERNAL1V5
   Min Vcc for default system freq:       2.2 V
   Min Vcc for INTERNAL2V5:               2.9 V
   Min Vcc for INTERNAL1V5:               2.2 V

  Notes:
  - VCC_XOVER value chosen to ensure valid Vcc measurement with either
    reference.
  - If there is no label defined in Energia for an ADC input channel parameter,
    then need to add 128 to the channel number from the datasheet.
  - Voltage reference used for temperature measurement is chosen to use
    the lowest reference voltage to allow operation over widest Vcc
    range. This simplifies coding so that a crossover point isn't needed
    for temperature measurements.

*/
#define VCC_CHAN               139       // 128 + 11
#define TEMPSENSOR_CHAN        TEMPSENSOR
#define TEMP_VREF              INTERNAL1V5
#define TEMP_REF_DV            15       // deciVolts
#define VCC_TYPE               VCCDIV2
#define VCC_REF1               INTERNAL2V5
#define VCC_REF2               INTERNAL1V5
#define VCC_REF1_DV            25       // deciVolts
#define VCC_REF2_DV            15       // deciVolts
#define VCC_XOVER              2950     // milliVolts
#define VSENSOR_UNCAL          98600    // Scaled * 100,000
#define TC_UNCAL               355      // Scaled * 100,000
#define TC_DELTA               55
#define ADC_STEPS              1023
#define ADC_CAL_T30            0x10e2   // 0x10da + CAL_ADC_15T30  // 1.5V reference
#define ADC_CAL_T85            0x10e4   // 0x10da + CAL_ADC_15T85  // 1.5V reference
#define ADC_CAL_REF1_FACTOR    0x10e6   // 0x10da + CAL_ADC_25VREF_FACTOR
#define ADC_CAL_REF2_FACTOR    0x10e0   // 0x10da + CAL_ADC_15VREF_FACTOR
#define ADC_CAL_GAIN_FACTOR    0x10dc   // 0x10da + CAL_ADC_GAIN_FACTOR
#define ADC_CAL_OFFSET_FACTOR  0x10de   // 0x10da + CAL_ADC_OFFSET

#elif defined(__MSP430F5529__)
/* MSP430F5529 - Reference Information
   Device datasheet doc number:           slas590
      ADC electrical characteristics:     5.41
      Calibration data memory location:   6.11
      Uncalibrated temp measurement       5.39, note 2
   Family guide doc number:               slau208
      ADC description                     Ch. 28
      Calibrated temp measurement         1.13.5.3
   ADC type:                              ADC12_a
   Voltage references available:          INTERNAL2V5
                                          INTERNAL2V0 (not used in this library)
                                          INTERNAL1V5
   Min Vcc for default system freq:       2.4 V
   Min Vcc for INTERNAL2V5:               2.8 V
   Min Vcc for INTERNAL2V0:               2.3 V (not used in this library)
   Min Vcc for INTERNAL1V5:               2.2 V

  Notes:
  - VCC_XOVER value chosen to ensure valid Vcc measurement with either
    reference.
  - If there is no label defined in Energia for an ADC input channel parameter,
    then need to add 128 to the channel number from the datasheet.
  - Voltage reference used for temperature measurement is chosen to use
    the lowest reference voltage to allow operation over widest Vcc
    range. This simplifies coding so that a crossover point isn't needed
    for temperature measurements.

*/
#define VCC_CHAN               A11
#define TEMPSENSOR_CHAN        TEMPSENSOR
#define TEMP_VREF              INTERNAL1V5
#define TEMP_REF_DV            15        // deciVolts
#define VCC_TYPE               VCCDIV2
#define VCC_REF1               INTERNAL2V0
#define VCC_REF2               INTERNAL1V5
#define VCC_REF1_DV            20        // deciVolts
#define VCC_REF2_DV            15        // deciVolts
#define VCC_XOVER              2850      // milliVolts
#define VSENSOR_UNCAL          68000     // Scaled * 100,000
#define TC_UNCAL               225       // Scaled * 100,000
#define TC_DELTA               55
#define ADC_STEPS              4095
#define ADC_CAL_T30            0x1a1a       // 1.5V reference
#define ADC_CAL_T85            0x1a1c       // 1.5V reference
#define ADC_CAL_REF1_FACTOR    0x1a2a
#define ADC_CAL_REF2_FACTOR    0x1a28
#define ADC_CAL_GAIN_FACTOR    0x1a16
#define ADC_CAL_OFFSET_FACTOR  0x1a18

// Other processor types to add:
// FR2433, FR6989, FR4133, FR5969
#else
#error "MCU type not defined"
#endif
