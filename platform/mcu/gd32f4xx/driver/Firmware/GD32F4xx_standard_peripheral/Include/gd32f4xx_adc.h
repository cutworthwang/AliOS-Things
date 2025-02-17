/*!
    \file  gd32f4xx_adc.h
    \brief definitions for the ADC
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.1, firmware for GD32F4xx
*/

#ifndef GD32F4XX_ADC_H
#define GD32F4XX_ADC_H

#include "gd32f4xx.h"

/* ADC definitions */
#define ADC0                            ADC_BASE
#define ADC1                            (ADC_BASE + 0x100U)
#define ADC2                            (ADC_BASE + 0x200U)

/* registers definitions */
#define ADC_STAT(adcx)                  REG32((adcx) + 0x00U)             /*!< ADC status register */
#define ADC_CTL0(adcx)                  REG32((adcx) + 0x04U)             /*!< ADC control register 0 */
#define ADC_CTL1(adcx)                  REG32((adcx) + 0x08U)             /*!< ADC control register 1 */
#define ADC_SAMPT0(adcx)                REG32((adcx) + 0x0CU)             /*!< ADC sampling time register 0 */
#define ADC_SAMPT1(adcx)                REG32((adcx) + 0x10U)             /*!< ADC sampling time register 1 */
#define ADC_IOFF0(adcx)                 REG32((adcx) + 0x14U)             /*!< ADC inserted channel data offset register 0 */
#define ADC_IOFF1(adcx)                 REG32((adcx) + 0x18U)             /*!< ADC inserted channel data offset register 1 */
#define ADC_IOFF2(adcx)                 REG32((adcx) + 0x1CU)             /*!< ADC inserted channel data offset register 2 */
#define ADC_IOFF3(adcx)                 REG32((adcx) + 0x20U)             /*!< ADC inserted channel data offset register 3 */
#define ADC_WDHT(adcx)                  REG32((adcx) + 0x24U)             /*!< ADC watchdog high threshold register */
#define ADC_WDLT(adcx)                  REG32((adcx) + 0x28U)             /*!< ADC watchdog low threshold register */
#define ADC_RSQ0(adcx)                  REG32((adcx) + 0x2CU)             /*!< ADC regular sequence register 0 */
#define ADC_RSQ1(adcx)                  REG32((adcx) + 0x30U)             /*!< ADC regular sequence register 1 */
#define ADC_RSQ2(adcx)                  REG32((adcx) + 0x34U)             /*!< ADC regular sequence register 2 */
#define ADC_ISQ(adcx)                   REG32((adcx) + 0x38U)             /*!< ADC inserted sequence register */
#define ADC_IDATA0(adcx)                REG32((adcx) + 0x3CU)             /*!< ADC inserted data register 0 */
#define ADC_IDATA1(adcx)                REG32((adcx) + 0x40U)             /*!< ADC inserted data register 1 */
#define ADC_IDATA2(adcx)                REG32((adcx) + 0x44U)             /*!< ADC inserted data register 2 */
#define ADC_IDATA3(adcx)                REG32((adcx) + 0x48U)             /*!< ADC inserted data register 3 */
#define ADC_RDATA(adcx)                 REG32((adcx) + 0x4CU)             /*!< ADC regular data register */
#define ADC_OVSAMPCTL(adcx)             REG32((adcx) + 0x80U)             /*!< ADC oversampling control register */
#define ADC_SSTAT                       REG32((ADC_BASE) + 0x300U)        /*!< ADC summary status register */
#define ADC_SYNCCTL                     REG32((ADC_BASE) + 0x304U)        /*!< ADC synchronization control register */
#define ADC_SYNCDATA                    REG32((ADC_BASE) + 0x308U)        /*!< ADC synchronization regular data register */

/* bits definitions */
/* ADC_STAT */
#define ADC_STAT_WDE                    BIT(0)                           /*!< analog watchdog event flag */
#define ADC_STAT_EOC                    BIT(1)                           /*!< end of conversion */
#define ADC_STAT_EOIC                   BIT(2)                           /*!< inserted channel end of conversion */
#define ADC_STAT_STIC                   BIT(3)                           /*!< inserted channel start flag */
#define ADC_STAT_STRC                   BIT(4)                           /*!< regular channel start flag */
#define ADC_STAT_ROVF                   BIT(5)                           /*!< regular data register overflow */

/* ADC_CTL0 */
#define ADC_CTL0_WDCHSEL                BITS(0,4)                        /*!< analog watchdog channel select bits */
#define ADC_CTL0_EOCIE                  BIT(5)                           /*!< interrupt enable for EOC */
#define ADC_CTL0_WDEIE                  BIT(6)                           /*!< analog watchdog interrupt enable */
#define ADC_CTL0_EOICIE                 BIT(7)                           /*!< interrupt enable for inserted channels */
#define ADC_CTL0_SM                     BIT(8)                           /*!< scan mode */
#define ADC_CTL0_WDSC                   BIT(9)                           /*!< when in scan mode, analog watchdog is effective on a single channel */
#define ADC_CTL0_ICA                    BIT(10)                          /*!< automatic inserted group conversion */
#define ADC_CTL0_DISRC                  BIT(11)                          /*!< discontinuous mode on regular channels */
#define ADC_CTL0_DISIC                  BIT(12)                          /*!< discontinuous mode on inserted channels */
#define ADC_CTL0_DISNUM                 BITS(13,15)                      /*!< discontinuous mode channel count */
#define ADC_CTL0_IWDEN                  BIT(22)                          /*!< analog watchdog enable on inserted channels */
#define ADC_CTL0_RWDEN                  BIT(23)                          /*!< analog watchdog enable on regular channels */
#define ADC_CTL0_DRES                   BITS(24,25)                      /*!< ADC data resolution */
#define ADC_CTL0_ROVFIE                 BIT(26)                          /*!< interrupt enable for ROVF */ 

/* ADC_CTL1 */
#define ADC_CTL1_ADCON                  BIT(0)                           /*!< ADC converter on */
#define ADC_CTL1_CTN                    BIT(1)                           /*!< continuous conversion */
#define ADC_CTL1_CLB                    BIT(2)                           /*!< ADC calibration */
#define ADC_CTL1_RSTCLB                 BIT(3)                           /*!< reset calibration */
#define ADC_CTL1_DMA                    BIT(8)                           /*!< direct memory access mode */
#define ADC_CTL1_DDM                    BIT(9)                           /*!< DMA disable mode */
#define ADC_CTL1_EOCM                   BIT(10)                          /*!< end of conversion mode */
#define ADC_CTL1_DAL                    BIT(11)                          /*!< data alignment */
#define ADC_CTL1_ETSIC                  BITS(16,19)                      /*!< external event select for inserted group */
#define ADC_CTL1_ETMIC                  BITS(20,21)                      /*!< external trigger conversion mode for inserted channels */
#define ADC_CTL1_SWICST                 BIT(22)                          /*!< start conversion of inserted channels */
#define ADC_CTL1_ETSRC                  BITS(24,27)                      /*!< external event select for regular group */
#define ADC_CTL1_ETMRC                  BITS(28,29)                      /*!< external trigger conversion mode for regular channels */
#define ADC_CTL1_SWRCST                 BIT(30)                          /*!< start conversion of regular channels */

/* ADC_SAMPTx x=0..1 */
#define ADC_SAMPTX_SPTN                 BITS(0,2)                        /*!< channel x sample time selection */

/* ADC_IOFFx x=0..3 */
#define ADC_IOFFX_IOFF                  BITS(0,11)                       /*!< data offset for inserted channel x */

/* ADC_WDHT */
#define ADC_WDHT_WDHT                   BITS(0,11)                       /*!< analog watchdog high threshold */

/* ADC_WDLT */
#define ADC_WDLT_WDLT                   BITS(0,11)                       /*!< analog watchdog low threshold */

/* ADC_RSQx */
#define ADC_RSQX_RSQN                   BITS(0,4)                        /*!< x conversion in regular sequence */
#define ADC_RSQ0_RL                     BITS(20,23)                      /*!< regular channel sequence length */

/* ADC_ISQ */
#define ADC_ISQ_ISQN                    BITS(0,4)                        /*!< x conversion in regular sequence */
#define ADC_ISQ_IL                      BITS(20,21)                      /*!< inserted sequence length */

/* ADC_IDATAx x=0..3*/
#define ADC_IDATAX_IDATAN               BITS(0,15)                       /*!< inserted data x */

/* ADC_RDATA */
#define ADC_RDATA_RDATA                 BITS(0,15)                       /*!< regular data */

/* ADC_OVSAMPCTL */
#define ADC_OVSAMPCTL_OVSEN             BIT(0)                           /*!< oversampling enable */
#define ADC_OVSAMPCTL_OVSR              BITS(2,4)                        /*!< oversampling ratio */
#define ADC_OVSAMPCTL_OVSS              BITS(5,8)                        /*!< oversampling shift */
#define ADC_OVSAMPCTL_TOVS              BIT(9)                           /*!< triggered oversampling */

/* ADC_SSTAT */
#define ADC_SSTAT_WDE0                  BIT(0)                           /*!< the mirror image of the WDE bit of ADC0 */
#define ADC_SSTAT_EOC0                  BIT(1)                           /*!< the mirror image of the EOC bit of ADC0 */
#define ADC_SSTAT_EOIC0                 BIT(2)                           /*!< the mirror image of the EOIC bit of ADC0 */
#define ADC_SSTAT_STIC0                 BIT(3)                           /*!< the mirror image of the STIC bit of ADC0 */
#define ADC_SSTAT_STRC0                 BIT(4)                           /*!< the mirror image of the STRC bit of ADC0 */
#define ADC_SSTAT_ROVF0                 BIT(5)                           /*!< the mirror image of the ROVF bit of ADC0 */
#define ADC_SSTAT_WDE1                  BIT(8)                           /*!< the mirror image of the WDE bit of ADC1 */
#define ADC_SSTAT_EOC1                  BIT(9)                           /*!< the mirror image of the EOC bit of ADC1 */
#define ADC_SSTAT_EOIC1                 BIT(10)                          /*!< the mirror image of the EOIC bit of ADC1 */
#define ADC_SSTAT_STIC1                 BIT(11)                          /*!< the mirror image of the STIC bit of ADC1 */
#define ADC_SSTAT_STRC1                 BIT(12)                          /*!< the mirror image of the STRC bit of ADC1 */
#define ADC_SSTAT_ROVF1                 BIT(13)                          /*!< the mirror image of the ROVF bit of ADC1 */
#define ADC_SSTAT_WDE2                  BIT(16)                          /*!< the mirror image of the WDE bit of ADC2 */
#define ADC_SSTAT_EOC2                  BIT(17)                          /*!< the mirror image of the EOC bit of ADC2 */
#define ADC_SSTAT_EOIC2                 BIT(18)                          /*!< the mirror image of the EOIC bit of ADC2 */
#define ADC_SSTAT_STIC2                 BIT(19)                          /*!< the mirror image of the STIC bit of ADC2 */
#define ADC_SSTAT_STRC2                 BIT(20)                          /*!< the mirror image of the STRC bit of ADC2 */
#define ADC_SSTAT_ROVF2                 BIT(21)                          /*!< the mirror image of the ROVF bit of ADC2 */

/* ADC_SYNCCTL */
#define ADC_SYNCCTL_SYNCM               BITS(0,4)                        /*!< ADC synchronization mode */
#define ADC_SYNCCTL_SYNCDLY             BITS(8,11)                       /*!< ADC synchronization delay */
#define ADC_SYNCCTL_SYNCDDM             BIT(13)                          /*!< ADC synchronization DMA disable mode */
#define ADC_SYNCCTL_SYNCDMA             BITS(14,15)                      /*!< ADC synchronization DMA mode selection */
#define ADC_SYNCCTL_ADCCK               BITS(16,18)                      /*!< ADC clock */
#define ADC_SYNCCTL_VBATEN              BIT(22)                          /*!< channel 18 (1/4 voltate of external battery) enable of ADC0 */
#define ADC_SYNCCTL_TSVREN              BIT(23)                          /*!< channel 16 (temperature sensor) and 17 (internal reference voltage) enable of ADC0 */

/* ADC_SYNCDATA */
#define ADC_SYNCDATA_SYNCDATA0          BITS(0,15)                       /*!< regular data1 in ADC synchronization mode */
#define ADC_SYNCDATA_SYNCDATA1          BITS(16,31)                      /*!< regular data2 in ADC synchronization mode */

/* constants definitions */
/* ADC channel group definitions */
#define ADC_REGULAR_CHANNEL             ((uint8_t)0x00U)                  /*!< adc regular channel group */
#define ADC_INSERTED_CHANNEL            ((uint8_t)0x01U)                  /*!< adc inserted channel group */
#define ADC_REGULAR_INSERTED_CHANNEL    ((uint8_t)0x02U)                  /*!< both regular and inserted channel group */
#define ADC_CHANNEL_DISCON_DISABLE      ((uint8_t)0x04U)                  /*!< disable discontinuous mode of regular & inserted channel */

/* external trigger mode for regular and inserted  channel */
#define EXTERNAL_TRIGGER_DISABLE        ((uint32_t)0x00000000U)           /*!< external trigger disable */
#define EXTERNAL_TRIGGER_RISING         ((uint32_t)0x00000001U)           /*!< rising edge of external trigger */
#define EXTERNAL_TRIGGER_FALLING        ((uint32_t)0x00000002U)           /*!< falling edge of external trigger */
#define EXTERNAL_TRIGGER_RISING_FALLING ((uint32_t)0x00000003U)           /*!< rising and falling edge of external trigger */

/* ADC inserted channel definitions */
#define ADC_INSERTED_CHANNEL_0          ((uint8_t)0x00U)                  /*!< adc inserted channel 0 */
#define ADC_INSERTED_CHANNEL_1          ((uint8_t)0x01U)                  /*!< adc inserted channel 1 */
#define ADC_INSERTED_CHANNEL_2          ((uint8_t)0x02U)                  /*!< adc inserted channel 2 */
#define ADC_INSERTED_CHANNEL_3          ((uint8_t)0x03U)                  /*!< adc inserted channel 3 */

/* ADC special function definitions */
#define ADC_SCAN_MODE                   ((uint8_t)0x00U)                  /*!< scan mode */
#define ADC_INSERTED_CHANNEL_AUTO       ((uint8_t)0x01U)                  /*!< inserted channel group convert automatically */
#define ADC_VBAT_CHANNEL_SWITCH         ((uint8_t)0x02U)                  /*!< VBAT channel */
#define ADC_TEMP_VREF_CHANNEL_SWITCH    ((uint8_t)0x03U)                  /*!< Vref and Vtemp channel */
#define ADC_CONTINUOUS_MODE             ((uint8_t)0x04U)                  /*!< continuous mode */

/* ADC channel definitions */
#define ADC_CHANNEL_0                   ((uint8_t)0x00U)                  /*!< ADC channel 0 */
#define ADC_CHANNEL_1                   ((uint8_t)0x01U)                  /*!< ADC channel 1 */
#define ADC_CHANNEL_2                   ((uint8_t)0x02U)                  /*!< ADC channel 2 */
#define ADC_CHANNEL_3                   ((uint8_t)0x03U)                  /*!< ADC channel 3 */
#define ADC_CHANNEL_4                   ((uint8_t)0x04U)                  /*!< ADC channel 4 */
#define ADC_CHANNEL_5                   ((uint8_t)0x05U)                  /*!< ADC channel 5 */
#define ADC_CHANNEL_6                   ((uint8_t)0x06U)                  /*!< ADC channel 6 */
#define ADC_CHANNEL_7                   ((uint8_t)0x07U)                  /*!< ADC channel 7 */
#define ADC_CHANNEL_8                   ((uint8_t)0x08U)                  /*!< ADC channel 8 */
#define ADC_CHANNEL_9                   ((uint8_t)0x09U)                  /*!< ADC channel 9 */
#define ADC_CHANNEL_10                  ((uint8_t)0x0AU)                  /*!< ADC channel 10 */
#define ADC_CHANNEL_11                  ((uint8_t)0x0BU)                  /*!< ADC channel 11 */
#define ADC_CHANNEL_12                  ((uint8_t)0x0CU)                  /*!< ADC channel 12 */
#define ADC_CHANNEL_13                  ((uint8_t)0x0DU)                  /*!< ADC channel 13 */
#define ADC_CHANNEL_14                  ((uint8_t)0x0EU)                  /*!< ADC channel 14 */
#define ADC_CHANNEL_15                  ((uint8_t)0x0FU)                  /*!< ADC channel 15 */
#define ADC_CHANNEL_16                  ((uint8_t)0x10U)                  /*!< ADC channel 16 */
#define ADC_CHANNEL_17                  ((uint8_t)0x11U)                  /*!< ADC channel 17 */
#define ADC_CHANNEL_18                  ((uint8_t)0x12U)                  /*!< ADC channel 18 */

/* ADC channel sample time */
#define ADC_SAMPLETIME_3                ((uint8_t)0x00U)                  /*!< 3 sampling cycles */
#define ADC_SAMPLETIME_15               ((uint8_t)0x01U)                  /*!< 15 sampling cycles */
#define ADC_SAMPLETIME_28               ((uint8_t)0x02U)                  /*!< 28 sampling cycles */
#define ADC_SAMPLETIME_56               ((uint8_t)0x03U)                  /*!< 56 sampling cycles */
#define ADC_SAMPLETIME_84               ((uint8_t)0x04U)                  /*!< 84 sampling cycles */
#define ADC_SAMPLETIME_112              ((uint8_t)0x05U)                  /*!< 112 sampling cycles */
#define ADC_SAMPLETIME_144              ((uint8_t)0x06U)                  /*!< 144 sampling cycles */
#define ADC_SAMPLETIME_480              ((uint8_t)0x07U)                  /*!< 480 sampling cycles */

/* ADC data alignment */
#define ADC_DATAALIGN_RIGHT             ((uint8_t)0x00U)                  /*!< LSB alignment */
#define ADC_DATAALIGN_LEFT              ((uint8_t)0x01U)                  /*!< MSB alignment */

/* ADC status flag */
#define ADC_FLAG_WDE                    ADC_STAT_WDE                     /*!< analog watchdog event flag */
#define ADC_FLAG_EOC                    ADC_STAT_EOC                     /*!< end of conversion */
#define ADC_FLAG_EOIC                   ADC_STAT_EOIC                    /*!< inserted channel end of conversion */
#define ADC_FLAG_STIC                   ADC_STAT_STIC                    /*!< inserted channel start flag */
#define ADC_FLAG_STRC                   ADC_STAT_STRC                    /*!< regular channel start flag */
#define ADC_FLAG_ROVF                   ADC_STAT_ROVF                    /*!< regular data register overflow */

/* ADC interrupt flag */
#define ADC_INT_WDE                     ADC_STAT_WDE                     /*!< analog watchdog event interrupt */
#define ADC_INT_EOC                     ADC_STAT_EOC                     /*!< end of group conversion interrupt */
#define ADC_INT_EOIC                    ADC_STAT_EOIC                    /*!< end of inserted group conversion interrupt */
#define ADC_INT_ROVF                    ADC_STAT_ROVF                    /*!< regular data register overflow */

/* ADC resolution definitions */
#define CTL0_DRES(regval)               (BITS(24,25) & ((uint32_t)(regval) << 24))
#define ADC_RESOLUTION_12B              CTL0_DRES(0)                     /*!< 12-bit ADC resolution */
#define ADC_RESOLUTION_10B              CTL0_DRES(1)                     /*!< 10-bit ADC resolution */
#define ADC_RESOLUTION_8B               CTL0_DRES(2)                     /*!< 8-bit ADC resolution */
#define ADC_RESOLUTION_6B               CTL0_DRES(3)                     /*!< 6-bit ADC resolution */

/* ADC external trigger select for regular channel */
#define CTL1_ETSRC(regval)              (BITS(24,27) & ((uint32_t)(regval) << 24))
#define ADC_EXTTRIG_REGULAR_T0_CH0      CTL1_ETSRC(0)                    /*!< timer 0 CC0 event select */
#define ADC_EXTTRIG_REGULAR_T0_CH1      CTL1_ETSRC(1)                    /*!< timer 0 CC1 event select */
#define ADC_EXTTRIG_REGULAR_T0_CH2      CTL1_ETSRC(2)                    /*!< timer 0 CC2 event select */
#define ADC_EXTTRIG_REGULAR_T1_CH1      CTL1_ETSRC(3)                    /*!< timer 1 CC1 event select */
#define ADC_EXTTRIG_REGULAR_T1_CH2      CTL1_ETSRC(4)                    /*!< timer 1 CC2 event select */
#define ADC_EXTTRIG_REGULAR_T1_CH3      CTL1_ETSRC(5)                    /*!< timer 1 CC3 event select */
#define ADC_EXTTRIG_REGULAR_T1_TRGO     CTL1_ETSRC(6)                    /*!< timer 1 TRGO event select */
#define ADC_EXTTRIG_REGULAR_T2_CH0      CTL1_ETSRC(7)                    /*!< timer 2 CC0 event select */
#define ADC_EXTTRIG_REGULAR_T2_TRGO     CTL1_ETSRC(8)                    /*!< timer 2 TRGO event select */
#define ADC_EXTTRIG_REGULAR_T3_CH3      CTL1_ETSRC(9)                    /*!< timer 3 CC3 event select */
#define ADC_EXTTRIG_REGULAR_T4_CH0      CTL1_ETSRC(10)                   /*!< timer 4 CC0 event select */
#define ADC_EXTTRIG_REGULAR_T4_CH1      CTL1_ETSRC(11)                   /*!< timer 4 CC1 event select */
#define ADC_EXTTRIG_REGULAR_T4_CH2      CTL1_ETSRC(12)                   /*!< timer 4 CC2 event select */
#define ADC_EXTTRIG_REGULAR_T7_CH0      CTL1_ETSRC(13)                   /*!< timer 7 CC0 event select */
#define ADC_EXTTRIG_REGULAR_T7_TRGO     CTL1_ETSRC(14)                   /*!< timer 7 TRGO event select */
#define ADC_EXTTRIG_REGULAR_EXTI_11     CTL1_ETSRC(15)                   /*!< extiline 11 select  */

/* ADC external trigger select for inserted channel */
#define CTL1_ETSIC(regval)              (BITS(16,19) & ((uint32_t)(regval) << 16))
#define ADC_EXTTRIG_INSERTED_T0_CH3     CTL1_ETSIC(0)                    /*!< timer0 capture compare 3 */
#define ADC_EXTTRIG_INSERTED_T0_TRGO    CTL1_ETSIC(1)                    /*!< timer0 TRGO event */
#define ADC_EXTTRIG_INSERTED_T1_CH0     CTL1_ETSIC(2)                    /*!< timer1 capture compare 0 */
#define ADC_EXTTRIG_INSERTED_T1_TRGO    CTL1_ETSIC(3)                    /*!< timer1 TRGO event */
#define ADC_EXTTRIG_INSERTED_T2_CH1     CTL1_ETSIC(4)                    /*!< timer2 capture compare 1 */
#define ADC_EXTTRIG_INSERTED_T2_CH3     CTL1_ETSIC(5)                    /*!< timer2 capture compare 3 */
#define ADC_EXTTRIG_INSERTED_T3_CH0     CTL1_ETSIC(6)                    /*!< timer3 capture compare 0 */
#define ADC_EXTTRIG_INSERTED_T3_CH1     CTL1_ETSIC(7)                    /*!< timer3 capture compare 1 */
#define ADC_EXTTRIG_INSERTED_T3_CH2     CTL1_ETSIC(8)                    /*!< timer3 capture compare 2 */
#define ADC_EXTTRIG_INSERTED_T3_TRGO    CTL1_ETSIC(9)                    /*!< timer3 capture compare TRGO */
#define ADC_EXTTRIG_INSERTED_T4_CH3     CTL1_ETSIC(10)                   /*!< timer4 capture compare 3 */
#define ADC_EXTTRIG_INSERTED_T4_TRGO    CTL1_ETSIC(11)                   /*!< timer4 capture compare TRGO */
#define ADC_EXTTRIG_INSERTED_T7_CH1     CTL1_ETSIC(12)                   /*!< timer7 capture compare 1 */
#define ADC_EXTTRIG_INSERTED_T7_CH2     CTL1_ETSIC(13)                   /*!< timer7 capture compare 2 */
#define ADC_EXTTRIG_INSERTED_T7_CH3     CTL1_ETSIC(14)                   /*!< timer7 capture compare 3 */
#define ADC_EXTTRIG_INSERTED_EXTI_15    CTL1_ETSIC(15)                   /*!< external interrupt line 15 */

/* ADC oversampling mode */
#define ADC_OVERSAMPLING_ALL_CONVERT    0U                                /*!< all oversampled conversions for a channel are done consecutively after a trigger */
#define ADC_OVERSAMPLING_ONE_CONVERT    1U                                /*!< each oversampled conversion for a channel needs a trigger */

/* ADC oversampling shift */
#define OVCTL_OVSS(regval)              (BITS(5,8) & ((uint32_t)(regval) << 5))
#define ADC_OVERSAMPLING_SHIFT_NONE     OVCTL_OVSS(0)                    /*!< no oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_1B       OVCTL_OVSS(1)                    /*!< 1-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_2B       OVCTL_OVSS(2)                    /*!< 2-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_3B       OVCTL_OVSS(3)                    /*!< 3-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_4B       OVCTL_OVSS(4)                    /*!< 4-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_5B       OVCTL_OVSS(5)                    /*!< 5-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_6B       OVCTL_OVSS(6)                    /*!< 6-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_7B       OVCTL_OVSS(7)                    /*!< 7-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_8B       OVCTL_OVSS(8)                    /*!< 8-bit oversampling shift */

/* ADC oversampling ratio */
#define OVCTL_OVSR(regval)              (BITS(2,4) & ((uint32_t)(regval) << 2))
#define ADC_OVERSAMPLING_RATIO_MUL2       OVCTL_OVSR(0)                  /*!< oversampling ratio multiple 2 */
#define ADC_OVERSAMPLING_RATIO_MUL4       OVCTL_OVSR(1)                  /*!< oversampling ratio multiple 4 */
#define ADC_OVERSAMPLING_RATIO_MUL8       OVCTL_OVSR(2)                  /*!< oversampling ratio multiple 8 */
#define ADC_OVERSAMPLING_RATIO_MUL16      OVCTL_OVSR(3)                  /*!< oversampling ratio multiple 16 */
#define ADC_OVERSAMPLING_RATIO_MUL32      OVCTL_OVSR(4)                  /*!< oversampling ratio multiple 32 */
#define ADC_OVERSAMPLING_RATIO_MUL64      OVCTL_OVSR(5)                  /*!< oversampling ratio multiple 64 */
#define ADC_OVERSAMPLING_RATIO_MUL128     OVCTL_OVSR(6)                  /*!< oversampling ratio multiple 128 */
#define ADC_OVERSAMPLING_RATIO_MUL256     OVCTL_OVSR(7)                  /*!< oversampling ratio multiple 256 */

/* configure the ADC clock for all the ADCs */
#define SYNCCTL_ADCCK(regval)           (BITS(16,18) & ((uint32_t)(regval) << 16))
#define ADC_ADCCK_PCLK2_DIV2            SYNCCTL_ADCCK(0)                 /*!< PCLK2 div2 */
#define ADC_ADCCK_PCLK2_DIV4            SYNCCTL_ADCCK(1)                 /*!< PCLK2 div4 */
#define ADC_ADCCK_PCLK2_DIV6            SYNCCTL_ADCCK(2)                 /*!< PCLK2 div6 */
#define ADC_ADCCK_PCLK2_DIV8            SYNCCTL_ADCCK(3)                 /*!< PCLK2 div8 */
#define ADC_ADCCK_HCLK_DIV5             SYNCCTL_ADCCK(4)                 /*!< HCLK div5 */
#define ADC_ADCCK_HCLK_DIV6             SYNCCTL_ADCCK(5)                 /*!< HCLK div6 */
#define ADC_ADCCK_HCLK_DIV10            SYNCCTL_ADCCK(6)                 /*!< HCLK div10 */
#define ADC_ADCCK_HCLK_DIV20            SYNCCTL_ADCCK(7)                 /*!< HCLK div20 */

/* ADC synchronization mode */
#define ADC_SYNC_MODE_INDEPENDENT                           ((uint32_t)0x00000000U)    /*!< ADC synchronization mode disabled.All the ADCs work independently */
#define ADC_DAUL_REGULAL_PARALLEL_INSERTED_PARALLEL         ((uint32_t)0x00000001U)    /*!< ADC0 and ADC1 work in combined regular parallel & inserted parallel mode. ADC2 works independently */
#define ADC_DAUL_REGULAL_PARALLEL_INSERTED_ROTATION         ((uint32_t)0x00000002U)    /*!< ADC0 and ADC1 work in combined regular parallel & trigger rotation mode. ADC2 works independently */
#define ADC_DAUL_INSERTED_PARALLEL                          ((uint32_t)0x00000005U)    /*!< ADC0 and ADC1 work in inserted parallel mode. ADC2 works independently */
#define ADC_DAUL_REGULAL_PARALLEL                           ((uint32_t)0x00000006U)    /*!< ADC0 and ADC1 work in regular parallel mode. ADC2 works independently */
#define ADC_DAUL_REGULAL_FOLLOW_UP                          ((uint32_t)0x00000007U)    /*!< ADC0 and ADC1 work in follow-up mode. ADC2 works independently */
#define ADC_DAUL_INSERTED_TRRIGGER_ROTATION                 ((uint32_t)0x00000009U)    /*!< ADC0 and ADC1 work in trigger rotation mode. ADC2 works independently */
#define ADC_ALL_REGULAL_PARALLEL_INSERTED_PARALLEL          ((uint32_t)0x00000011U)    /*!< all ADCs work in combined regular parallel & inserted parallel mode */
#define ADC_ALL_REGULAL_PARALLEL_INSERTED_ROTATION          ((uint32_t)0x00000012U)    /*!< all ADCs work in combined regular parallel & trigger rotation mode */
#define ADC_ALL_INSERTED_PARALLEL                           ((uint32_t)0x00000015U)    /*!< all ADCs work in inserted parallel mode */
#define ADC_ALL_REGULAL_PARALLEL                            ((uint32_t)0x00000016U)    /*!< all ADCs work in regular parallel mode */
#define ADC_ALL_REGULAL_FOLLOW_UP                           ((uint32_t)0x00000017U)    /*!< all ADCs work in follow-up mode */
#define ADC_ALL_INSERTED_TRRIGGER_ROTATION                  ((uint32_t)0x00000019U)    /*!< all ADCs work in trigger rotation mode */

/* ADC synchronization delay */
#define ADC_SYNC_DELAY_5CYCLE                               ((uint32_t)0x00000000U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 5 ADC clock cycles. */
#define ADC_SYNC_DELAY_6CYCLE                               ((uint32_t)0x00000100U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 6 ADC clock cycles. */
#define ADC_SYNC_DELAY_7CYCLE                               ((uint32_t)0x00000200U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 7 ADC clock cycles. */
#define ADC_SYNC_DELAY_8CYCLE                               ((uint32_t)0x00000300U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 8 ADC clock cycles. */
#define ADC_SYNC_DELAY_9CYCLE                               ((uint32_t)0x00000400U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 9 ADC clock cycles. */
#define ADC_SYNC_DELAY_10CYCLE                              ((uint32_t)0x00000500U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 10 ADC clock cycles. */
#define ADC_SYNC_DELAY_11CYCLE                              ((uint32_t)0x00000600U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 11 ADC clock cycles. */
#define ADC_SYNC_DELAY_12CYCLE                              ((uint32_t)0x00000700U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 12 ADC clock cycles. */
#define ADC_SYNC_DELAY_13CYCLE                              ((uint32_t)0x00000800U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 13 ADC clock cycles. */
#define ADC_SYNC_DELAY_14CYCLE                              ((uint32_t)0x00000900U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 14 ADC clock cycles. */
#define ADC_SYNC_DELAY_15CYCLE                              ((uint32_t)0x00000A00U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 15 ADC clock cycles. */
#define ADC_SYNC_DELAY_16CYCLE                              ((uint32_t)0x00000B00U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 16 ADC clock cycles. */
#define ADC_SYNC_DELAY_17CYCLE                              ((uint32_t)0x00000C00U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 17 ADC clock cycles. */
#define ADC_SYNC_DELAY_18CYCLE                              ((uint32_t)0x00000D00U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 18 ADC clock cycles. */
#define ADC_SYNC_DELAY_19CYCLE                              ((uint32_t)0x00000E00U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 19 ADC clock cycles. */
#define ADC_SYNC_DELAY_20CYCLE                              ((uint32_t)0x00000F00U)    /*!< the delay between 2 sampling phases in ADC synchronization modes to 20 ADC clock cycles. */

/* ADC synchronization DMA mode selection */
#define ADC_SYNC_DMA_DISABLE                                ((uint32_t)0x00000000U)    /*!< ADC synchronization DMA disabled */
#define ADC_SYNC_DMA_MODE0                                  ((uint32_t)0x00004000U)    /*!< ADC synchronization DMA mode 0 */
#define ADC_SYNC_DMA_MODE1                                  ((uint32_t)0x00008000U)    /*!< ADC synchronization DMA mode 1 */

/* end of conversion mode */
#define ADC_EOC_SET_SEQUENCE                                ((uint8_t)0x00U)           /*!< only at the end of a sequence of regular conversions, the EOC bit is set */
#define ADC_EOC_SET_CONVERSION                              ((uint8_t)0x01U)           /*!< at the end of each regular conversion, the EOC bit is set */

/* function declarations */
/* ADC reset */
void adc_deinit(void);
/* enable ADC interface */
void adc_enable(uint32_t adc_periph);
/* disable ADC interface */
void adc_disable(uint32_t adc_periph);
/* ADC data alignment config */
void adc_data_alignment_config(uint32_t adc_periph , uint8_t data_alignment);
/* ADC resolution config */
void adc_resolution_config(uint32_t adc_periph , uint32_t resolution);
/* ADC calibration and reset calibration */
void adc_calibration_enable(uint32_t adc_periph);
/* ADC discontinuous mode config */
void adc_discontinuous_mode_config(uint32_t adc_periph , uint8_t adc_channel_group , uint8_t length);
/* config end of conversion mode */
void adc_end_of_conversion_config(uint32_t adc_periph , uint8_t end_selection);
/* ADC special function enable or disable */
void adc_special_function_config(uint32_t adc_periph , uint8_t function , ControlStatus newvalue);
/* configure the ADC clock for all the ADCs */
void adc_clock_config(uint32_t prescaler);

/* ADC channel */
/* configure the ADC clock for all the ADCs */
void adc_channel_16_to_18(uint8_t function,ControlStatus newvalue);
/* config the length of regular channel group or inserted channel group */
void adc_channel_length_config(uint32_t adc_periph , uint8_t adc_channel_group , uint32_t length);

/* ADC trigger */
/* ADC external trigger enable */
void adc_external_trigger_config(uint32_t adc_periph , uint8_t adc_channel_group , uint32_t trigger_mode);
/* ADC external trigger source config */
void adc_external_trigger_source_config(uint32_t adc_periph , uint8_t adc_channel_group , uint32_t external_trigger_source);
/* ADC software trigger enable */
void adc_software_trigger_enable(uint32_t adc_periph , uint8_t adc_channel_group);

/* ADC flag and interrupt */
/* get the ADC flag bits */
FlagStatus adc_flag_get(uint32_t adc_periph , uint32_t adc_flag);
/* clear the ADC flag bits */
void adc_flag_clear(uint32_t adc_periph , uint32_t adc_flag);
/* get the ADC interrupt bits */
FlagStatus adc_interrupt_flag_get(uint32_t adc_periph , uint32_t adc_interrupt);
/* clear the ADC flag */
void adc_interrupt_flag_clear(uint32_t adc_periph , uint32_t adc_interrupt);
/* ADC interrupt enable */
void adc_interrupt_enable(uint32_t adc_periph , uint32_t adc_interrupt);
/* ADC interrupt disable */
void adc_interrupt_disable(uint32_t adc_periph , uint32_t adc_interrupt);

/* ADC analog watchdog  */
/* ADC analog watchdog single channel disable */
void adc_watchdog_single_channel_disable(uint32_t adc_periph );
/* ADC analog watchdog single channel enable */
void adc_watchdog_single_channel_enable(uint32_t adc_periph , uint8_t adc_channel);
/* adc analog watchdog group channel config */
void adc_watchdog_enable(uint32_t adc_periph , uint8_t adc_channel_group);
/* ADC analog watchdog disable */
void adc_watchdog_disable(uint32_t adc_periph , uint8_t adc_channel_group);
/* ADC analog watchdog threshold config */
void adc_watchdog_threshold_config(uint32_t adc_periph , uint16_t low_threshold , uint16_t high_threshold);

/* regular channel */
/* ADC regular channel config */
void adc_regular_channel_config(uint32_t adc_periph , uint8_t rank , uint8_t adc_channel , uint32_t sample_time);
/* ADC regular group data register read */
uint16_t adc_regular_data_read(uint32_t adc_periph);

/* inserted channel */
/* ADC inserted channel config */
void adc_inserted_channel_config(uint32_t adc_periph , uint8_t rank , uint8_t adc_channel , uint8_t sample_time);
/* ADC inserted channel offset config */
void adc_inserted_channel_offset_config(uint32_t adc_periph , uint8_t inserted_channel , uint16_t offset);
/* ADC inserted group data register read */
uint16_t adc_inserted_data_read(uint32_t adc_periph , uint8_t inserted_channel);

/* ADC DMA */
/* DMA request enable */
void adc_dma_mode_enable(uint32_t adc_periph);
/* DMA request disable */
void adc_dma_mode_disable(uint32_t adc_periph);
/* when DMA=1, the DMA engine issues a request at end of each regular conversion */
void adc_dma_request_after_last_enable(uint32_t adc_periph);
/* the DMA engine is disabled after the end of transfer signal from DMA controller is detected */
void adc_dma_request_after_last_disable(uint32_t adc_periph);

/* ADC oversample */
/* ADC oversample mode config */
void adc_oversample_mode_config(uint32_t adc_periph , uint8_t mode , uint16_t shift , uint8_t ratio);
/* ADC oversample mode enable */
void adc_oversample_mode_enable(uint32_t adc_periph );
/* ADC oversample mode disable */
void adc_oversample_mode_disable(uint32_t adc_periph );

/* ADC synchronization */
/* configure the ADC sync mode */
void adc_sync_mode_config(uint32_t sync_mode);
/* configure the delay between 2 sampling phases in ADC sync modes */
void adc_sync_delay_config(uint32_t sample_delay);
/* configure ADC sync DMA mode selection */
void adc_sync_dma_config(uint32_t dma_mode );
/* configure ADC sync DMA engine is disabled after the end of transfer signal from DMA controller is detected */
void adc_sync_dma_request_after_last_enable(void);
/* configure ADC sync DMA engine issues requests according to the SYNCDMA bits */
void adc_sync_dma_request_after_last_disable(void);
/* ADC sync regular data register read */
uint32_t adc_sync_regular_data_read(void);

#endif /* GD32F4XX_ADC_H */
