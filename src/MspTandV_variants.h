/* -----------------------------------------------------------------
   MspTandV Library - Processor Specific Definitions
   https://gitlab.com/Andy4495
   MIT License

   01/16/2018 - A.T. - Original
   01/17/2018 - A.T. - Add FR4133 support
   01/18/2018 - A.T. - Add FR6989 support
   01/22/2018 - A.T. - Add FR2433 and FR5969 support
*/
/*
   This file contains the processor-specific definitions for the
   various calculations needed for temperature and Vcc measurements.
*/
/*
Notes on choice of values:
- VCC_XOVER value is chosen to ensure valid Vcc measurement with either
  reference.
- If there is no label defined in Energia for an ADC input channel parameter,
  then need to add 128 to the channel number from the datasheet.
- The voltage reference used for temperature measurement is chosen to use
  the lowest reference voltage to allow operation over widest Vcc
  range. This simplifies coding so that a crossover point isn't needed
  for temperature measurements.
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
*/
#define TEMPSENSOR_CHAN        TEMPSENSOR
#define TEMP_VREF              INTERNAL1V5
#define TEMP_REF_DV            15       // deciVolts
#define VCC_TYPE               VCCDIV2  // Measure Vcc/2 wrt internal voltage reference
#define VCC_CHAN               139       // 128 + 11
#define VCC_REF1               INTERNAL2V5
#define VCC_REF2               INTERNAL1V5
#define VCC_REF1_DV            25       // deciVolts
#define VCC_REF2_DV            15       // deciVolts
#define VCC_XOVER              2950     // milliVolts
#define VSENSOR_UNCAL          98600    // mV, Scaled * 100,000
#define TC_UNCAL               355      // mV/C, Scaled * 100,000
#define TC_DELTA               55       // Degrees C between calibration points
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
*/
#define TEMPSENSOR_CHAN        TEMPSENSOR
#define TEMP_VREF              INTERNAL1V5
#define TEMP_REF_DV            15        // deciVolts
#define VCC_TYPE               VCCDIV2   // Measure Vcc/2 wrt internal voltage reference
#define VCC_CHAN               A11
#define VCC_REF1               INTERNAL2V0
#define VCC_REF2               INTERNAL1V5
#define VCC_REF1_DV            20        // deciVolts
#define VCC_REF2_DV            15        // deciVolts
#define VCC_XOVER              2850      // milliVolts
#define VSENSOR_UNCAL          68000     // mV, Scaled * 100,000
#define TC_UNCAL               225       // mV/C, Scaled * 100,000
#define TC_DELTA               55        // Degrees C between calibration points
#define ADC_STEPS              4095
#define ADC_CAL_T30            0x1a1a       // 1.5V reference
#define ADC_CAL_T85            0x1a1c       // 1.5V reference
#define ADC_CAL_REF1_FACTOR    0x1a2a
#define ADC_CAL_REF2_FACTOR    0x1a28
#define ADC_CAL_GAIN_FACTOR    0x1a16
#define ADC_CAL_OFFSET_FACTOR  0x1a18

#elif defined(__MSP430FR4133__)
/* MSP430FR4133 - Reference Information
   Device datasheet doc number:           slas865
      ADC Input Channels                  6.9.10
      ADC electrical characteristics:     5.12.7
      Calibration data memory location:   6.10
      Uncalibrated temp measurement       Table 5-19
   Family guide doc number:               slau445
      ADC description                     Ch. 20
      Temp sensor transfer function       20.2.7.8
      Factory Calibration Values          1.13.3
   ADC type:                              ADC, 10-bit
   Voltage references available:          INTERNAL1V5
   Min Vcc for default system freq:       1.8 V
   Min Vcc for INTERNAL1V5:               2.0 V (minimum ADC supply voltage)
*/
#define TEMPSENSOR_CHAN        TEMPSENSOR
#define TEMP_VREF              INTERNAL1V5
#define TEMP_REF_DV            15             // deciVolts
#define VCC_TYPE               VCC            // Measure 1.5V reference wrt Vcc
#define VCC_CHAN               141            // 128+13 (1V5 Reference input channel)
#define VCC_REF1               DEFAULT        // Use Vcc as the reference to measure 1V5
#define VCC_REF2               0              // FR4133 does not have a 2nd reference
#define VCC_REF1_DV            15             // deciVolts
#define VCC_REF2_DV            0              // FR4133 does not have a 2nd reference
#define VCC_XOVER              0              // Single reference, so no crossover point
#define VSENSOR_UNCAL          101            // mV, Scaled * 100,000
// Note: The FR4133 datasheets lists TCsensor as 3.35 mV/C; however, this gives
//       completely inaccurate results. The TC_UNCAL value below assumes that
//       correct value is actually 33.5 mV/C.
#define TC_UNCAL               3350           // mV/C, Scaled * 100,000
#define TC_DELTA               55             // Degrees C between calibration points
#define ADC_STEPS              1023
#define ADC_CAL_T30            0x1a1a         // 1.5V reference
#define ADC_CAL_T85            0x1a1c         // 1.5V reference
#define ADC_CAL_REF1_FACTOR    0x1a20
#define ADC_CAL_REF2_FACTOR    0              // FR4133 does not have a 2nd reference
#define ADC_CAL_GAIN_FACTOR    0x1a16
#define ADC_CAL_OFFSET_FACTOR  0x1a18

#elif defined(__MSP430FR6989__)
/* MSP430FR6989 - Reference Information
   Device datasheet doc number:           slas789
      ADC electrical characteristics:     Table 5-34
      Calibration data memory location:   6.12
      Uncalibrated temp measurement       Table 5-32
   Family guide doc number:               slau367
      ADC description                     Ch. 34
      Calibrated temp measurement         1.14.3.3
   ADC type:                              ADC12_B
   Voltage references available:          INTERNAL1V5
                                          INTERNAL2V0
                                          INTERNAL2V5
   Min Vcc for default system freq:       1.8 V
   Min Vcc for INTERNAL2V5:               2.7 V (not used with this library)
   Min Vcc for INTERNAL2V0:               2.2 V
   Min Vcc for INTERNAL1V2:               1.8 V
*/
#define TEMPSENSOR_CHAN        TEMPSENSOR   // A30
#define TEMP_VREF              INTERNAL1V2
#define TEMP_REF_DV            12           // deciVolts
#define VCC_TYPE               VCCDIV2      // Measure Vcc/2 wrt internal voltage reference
#define VCC_CHAN               159          // 128+31: A31 "Battery Monitor" AVcc/2
#define VCC_REF1               INTERNAL2V0
#define VCC_REF2               INTERNAL1V2
#define VCC_REF1_DV            20        // deciVolts
#define VCC_REF2_DV            12        // deciVolts
#define VCC_XOVER              2300      // milliVolts
#define VSENSOR_UNCAL          70000     // mV, Scaled * 100,000
#define TC_UNCAL               250       // mV/C, Scaled * 100,000
#define TC_DELTA               55        // Degrees C between calibration points
#define ADC_STEPS              4095
#define ADC_CAL_T30            0x1a1a    // 1.2V reference
#define ADC_CAL_T85            0x1a1c    // 1.2V reference
#define ADC_CAL_REF1_FACTOR    0x1a2a    // 2.0 V reference
#define ADC_CAL_REF2_FACTOR    0x1a28    // 1.2 V reference
#define ADC_CAL_GAIN_FACTOR    0x1a16
#define ADC_CAL_OFFSET_FACTOR  0x1a18

#elif defined(__MSP430FR2433__)
/* MSP430FR6989 - Reference Information
   Device datasheet doc number:           slase59
      ADC electrical characteristics:     5.11.8
      Calibration data memory location:   6.12
      Uncalibrated temp measurement       5.11.8
   Family guide doc number:               slau445
      ADC description                     Ch. 20
      Temp sensor transfer function       20.2.7.8
      Factory Calibration Values          1.13.3
   ADC type:                              ADC (10-bit)
   Voltage references available:          INTERNAL1V5
   Min Vcc for default system freq:       1.8 V
   Min Vcc for INTERNAL1V5:               2.0 V    // Minimum voltage for ADC operation
*/
#define TEMPSENSOR_CHAN           TEMPSENSOR
#define TEMP_VREF                 INTERNAL1V5
#define TEMP_REF_DV               15          // deciVolts
#define VCC_TYPE                  VCC         // Measure 1.5V reference wrt Vcc
#define VCC_CHAN                  141         // 128+13: 1.5V reference channel
#define VCC_REF1                  DEFAULT
#define VCC_REF2                  0           // No REF2 with FR2433
#define VCC_REF1_DV               15          // deciVolts
#define VCC_REF2_DV               0           // deciVolts
#define VCC_XOVER                 0           // Single reference, so no crossover
#define VSENSOR_UNCAL             91300       // mV, Scaled * 100,000
#define TC_UNCAL                  335         // mV/C, Scaled * 100,000
#define TC_DELTA                  55          // Degrees C between calibration points
#define ADC_STEPS                 1023
#define ADC_CAL_T30               0x1a1a      // 1.5 V reference
#define ADC_CAL_T85               0x1a1c      // 1.5 V reference
#define ADC_CAL_REF1_FACTOR       0x1a20      // 1.5 V reference
#define ADC_CAL_REF2_FACTOR       0x1a20      // Not available for FR2433
#define ADC_CAL_GAIN_FACTOR       0x1a16
#define ADC_CAL_OFFSET_FACTOR     0x1a18

#elif defined(__MSP430FR5969__)
/* MSP430FR6989 - Reference Information
   Device datasheet doc number:           slas704
      ADC electrical characteristics:     5.12.8
      Calibration data memory location:   6.12
      Uncalibrated temp measurement       Table 5-30
   Family guide doc number:               slau367
      ADC description                     Ch. 34
      Calibrated temp measurement         1.14.3.3
   ADC type:                              ADC12_B
   Voltage references available:          INTERNAL2V5    (not used with this library)
                                          INTERNAL2V0
                                          INTERNAL1V2
   Min Vcc for default system freq:       1.8 V
   Min Vcc for INTERNAL2V5:               2.7 V          (not used with this library)
   Min Vcc for INTERNAL2V0:               2.2 V
   Min Vcc for INTERNAL1V2:               1.8 V
*/
#define TEMPSENSOR_CHAN        TEMPSENSOR   // A30
#define TEMP_VREF              INTERNAL1V2
#define TEMP_REF_DV            12           // deciVolts
#define VCC_TYPE               VCCDIV2      // Measure Vcc/2 wrt internal voltage reference
#define VCC_CHAN               159          // 128+31: A31 "Battery Monitor" AVcc/2
#define VCC_REF1               INTERNAL2V0
#define VCC_REF2               INTERNAL1V2
#define VCC_REF1_DV            20           // deciVolts
#define VCC_REF2_DV            12           // deciVolts
#define VCC_XOVER              2300         // milliVolts
#define VSENSOR_UNCAL          70000        // mV, Scaled * 100,000
#define TC_UNCAL               250          // mV/C, Scaled * 100,000
#define TC_DELTA               55           // Degrees C between calibration points
#define ADC_STEPS              4095
#define ADC_CAL_T30            0x1a1a       // 1.2V reference
#define ADC_CAL_T85            0x1a1c       // 1.2V reference
#define ADC_CAL_REF1_FACTOR    0x1a2a       // 2.0 V reference
#define ADC_CAL_REF2_FACTOR    0x1a28       // 1.2 V reference
#define ADC_CAL_GAIN_FACTOR    0x1a16
#define ADC_CAL_OFFSET_FACTOR  0x1a18

#else
#error "MCU type not defined"
#endif
