/* -----------------------------------------------------------------
   MspTandV Library
    01/10/2018 - A.T. Original
*/
/*
   This file contains the processor-specific definitions for the
   various calculations needed for temperature and Vcc measurements.
*/

#if defined(__MSP430G2553__)
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
#define ADC_CAL_T30            0x10e2   // 30C calibration 1.5V reference
#define ADC_CAL_T85            0x10e4   // 85C calibration 1.5V reference
#define ADC_CAL_REF1_FACTOR    0x10e6
#define ADC_CAL_REF2_FACTOR    0x10e0
#define ADC_CAL_GAIN_FACTOR    0x10dc
#define ADC_CAL_OFFSET_FACTOR  0x10de

#elif defined(__MSP430F5529__)
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
#define VSENSOR_UNCAL          68000
#define TC_UNCAL               225
#define TC_DELTA               55
#define ADC_STEPS              4095
#define ADC_CAL_T30            0x1a1a   // 30C calibration 1.5V reference
#define ADC_CAL_T85            0x1a1c   // 85C calibration 1.5V reference
#define ADC_CAL_REF1_FACTOR    0x1a2a
#define ADC_CAL_REF2_FACTOR    0x1a28
#define ADC_CAL_GAIN_FACTOR    0x1a16
#define ADC_CAL_OFFSET_FACTOR  0x1a18

#else
#error "MCU type not defined"
#endif
