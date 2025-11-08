/*
 * plibi_mini_hal.h
 *
 * NOTE: This is a subset of (and in parts taken from) stm32h5xx_hal_rcc.h
 * WHY: We want to get rid of the complete HAL from STM, but the RCC is quite
 * a complex thing and many know how is within these definitions.
 */

#ifndef PLIB_PLIBI_MINI_HAL_H_
#define PLIB_PLIBI_MINI_HAL_H_

#include "stm32h5xx.h"

/** @defgroup GPIO_pull GPIO pull
  * @brief GPIO Pull-Up or Pull-Down Activation
  * @{
  */
#define  GPIO_NOPULL        (0x00000000U)   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        (0x00000001U)   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      (0x00000002U)   /*!< Pull-down activation                */

/** @defgroup GPIO_mode GPIO mode
  * @brief GPIO Configuration Mode
  *        Elements values convention: 0xX0yz00YZ
  *           - X  : GPIO mode or EXTI Mode
  *           - y  : External IT or Event trigger detection
  *           - z  : IO configuration on External IT or Event
  *           - Y  : Output type (Push Pull or Open Drain)
  *           - Z  : IO Direction mode (Input, Output, (Alternate or Analog))
  * @{
  */
/*!< Input Floating Mode                                                 */
#define  GPIO_MODE_INPUT                        (0x00000000U)
/*!< Output Push Pull Mode                                               */
#define  GPIO_MODE_OUTPUT_PP                    (0x00000001U)
/*!< Output Open Drain Mode                                             */
#define  GPIO_MODE_OUTPUT_OD                    (0x00000011U)
/*!< Alternate Function Push Pull Mode                                   */
#define  GPIO_MODE_AF_PP                        (0x00000002U)
/*!< Alternate Function Open Drain Mode                                  */
#define  GPIO_MODE_AF_OD                        (0x00000012U)
/*!< Analog Mode                                                         */
#define  GPIO_MODE_ANALOG                       (0x00000003U)
/*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_RISING                    (0x10110000U)
/*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_FALLING                   (0x10210000U)
/*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
#define  GPIO_MODE_IT_RISING_FALLING            (0x10310000U)
/*!< External Event Mode with Rising edge trigger detection             */
#define  GPIO_MODE_EVT_RISING                   (0x10120000U)
/*!< External Event Mode with Falling edge trigger detection            */
#define  GPIO_MODE_EVT_FALLING                  (0x10220000U)
/*!< External Event Mode with Rising/Falling edge trigger detection      */
#define  GPIO_MODE_EVT_RISING_FALLING           (0x10320000U)

/**
  * @brief  RCC PLL1 configuration structure definition
  */
typedef struct
{
  uint32_t PLLState;   /*!< PLLState: The new state of the PLL1.
                            This parameter can be a value of @ref RCC_PLL1_Config                      */

  uint32_t PLLSource;  /*!< PLLSource: PLL entry clock source.
                            This parameter must be a value of @ref RCC_PLL1_Clock_Source               */

  uint32_t PLLM;       /*!< PLLM: Division factor for PLL1 VCO input clock.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 63    */

  uint32_t PLLN;       /*!< PLLN: Multiplication factor for PLL1 VCO output clock.
                            This parameter must be a number between Min_Data = 4 and Max_Data = 512   */

  uint32_t PLLP;       /*!< PLLP: Division factor for system clock.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 128
                            odd division factors are not allowed                                      */

  uint32_t PLLQ;       /*!< PLLQ: Division factor for peripheral clocks.
                            This parameter must be a number between Min_Data = 1 and Max_Data = 128   */

  uint32_t PLLR;       /*!< PLLR: Division factor for peripheral clocks.
                            This parameter must be a number between Min_Data = 1 and Max_Data = 128   */

  uint32_t PLLRGE;     /*!< PLLRGE: PLL1 clock Input range
                            This parameter must be a value of @ref RCC_PLL1_VCI_Range                 */

  uint32_t PLLVCOSEL;  /*!< PLLVCOSEL: PLL1 clock Output range
                            This parameter must be a value of @ref RCC_PLL1_VCO_Range                 */

  uint32_t PLLFRACN;   /*!< PLLFRACN: Specifies Fractional Part Of The Multiplication Factor for
                            PLL1 VCO It should be a value between 0 and 8191                          */

} RCC_PLLInitTypeDef;


/**
  * @brief  RCC Internal/External Oscillator (HSE, HSI, CSI, LSE and LSI) configuration structure definition
  */
typedef struct
{
  uint32_t OscillatorType;       /*!< The oscillators to be configured.
                                      This parameter can be a value of @ref RCC_Oscillator_Type                   */

  uint32_t HSEState;             /*!< The new state of the HSE.
                                      This parameter can be a value of @ref RCC_HSE_Config                        */

  uint32_t LSEState;             /*!< The new state of the LSE.
                                      This parameter can be a value of @ref RCC_LSE_Config                        */

  uint32_t HSIState;             /*!< The new state of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Config                        */

  uint32_t HSIDiv;               /*!< The division factor of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Div                           */

  uint32_t HSICalibrationValue;  /*!< The calibration trimming value (default is RCC_HSICALIBRATION_DEFAULT).
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x7F
                                      on the other devices */

  uint32_t LSIState;             /*!< The new state of the LSI.
                                      This parameter can be a value of @ref RCC_LSI_Config                        */

  uint32_t CSIState;             /*!< The new state of the CSI.
                                      This parameter can be a value of @ref RCC_CSI_Config */

  uint32_t CSICalibrationValue;  /*!< The calibration trimming value (default is RCC_CSICALIBRATION_DEFAULT).
                                      This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x3F */

  uint32_t HSI48State;            /*!< The new state of the HSI48.
                                        This parameter can be a value of @ref RCC_HSI48_Config                    */

  RCC_PLLInitTypeDef PLL;         /*!<  PLL1 structure parameters                                                 */

} RCC_OscInitTypeDef;

/**
  * @brief  RCC System, AHB and APB busses clock configuration structure definition
  */
typedef struct
{
  uint32_t ClockType;             /*!< The clock to be configured.
                                       This parameter can be a value of @ref RCC_System_Clock_Type      */

  uint32_t SYSCLKSource;          /*!< The clock source used as system clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_System_Clock_Source    */

  uint32_t AHBCLKDivider;         /*!< The AHB clock (HCLK) divider. This clock is derived from the system clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_AHB_Clock_Source       */

  uint32_t APB1CLKDivider;        /*!< The APB1 clock (PCLK1) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_APB2_APB3_Clock_Source */

  uint32_t APB2CLKDivider;        /*!< The APB2 clock (PCLK2) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_APB2_APB3_Clock_Source */

  uint32_t APB3CLKDivider;        /*!< The APB3 clock (PCLK3) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_APB1_APB2_APB3_Clock_Source      */
} RCC_ClkInitTypeDef;


/** @brief  Macro to get the HSE division factor for RTC clock.
  *
  * @retval The HSE division factor for RTC clock. The returned value can be one
  *         of the following:
  *               @arg @ref RCC_RTC_HSE_NOCLOCK : No HSE Clock selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV2  : HSE Divided by 2 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV3  : HSE Divided by 3 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV4  : HSE Divided by 4 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV5  : HSE Divided by 5 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV6  : HSE Divided by 6 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV7  : HSE Divided by 7 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV8  : HSE Divided by 8 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV9  : HSE Divided by 9 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV10 : HSE Divided by 10 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV11 : HSE Divided by 11 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV12 : HSE Divided by 12 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV13 : HSE Divided by 13 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV14 : HSE Divided by 14 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV15 : HSE Divided by 15 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV16 : HSE Divided by 16 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV17 : HSE Divided by 17 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV18 : HSE Divided by 18 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV19 : HSE Divided by 19 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV20 : HSE Divided by 20 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV21 : HSE Divided by 21 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV22 : HSE Divided by 22 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV23 : HSE Divided by 23 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV24 : HSE Divided by 24 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV25 : HSE Divided by 25 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV26 : HSE Divided by 26 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV27 : HSE Divided by 27 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV28 : HSE Divided by 28 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV29 : HSE Divided by 29 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV30 : HSE Divided by 30 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV31 : HSE Divided by 31 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV32 : HSE Divided by 32 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV33 : HSE Divided by 33 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV34 : HSE Divided by 34 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV35 : HSE Divided by 35 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV36 : HSE Divided by 36 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV37 : HSE Divided by 37 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV38 : HSE Divided by 38 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV39 : HSE Divided by 39 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV40 : HSE Divided by 40 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV41 : HSE Divided by 41 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV42 : HSE Divided by 42 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV43 : HSE Divided by 43 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV44 : HSE Divided by 44 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV45 : HSE Divided by 45 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV46 : HSE Divided by 46 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV47 : HSE Divided by 47 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV48 : HSE Divided by 48 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV49 : HSE Divided by 49 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV50 : HSE Divided by 50 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV51 : HSE Divided by 51 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV52 : HSE Divided by 52 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV53 : HSE Divided by 53 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV54 : HSE Divided by 54 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV55 : HSE Divided by 55 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV56 : HSE Divided by 56 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV57 : HSE Divided by 57 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV58 : HSE Divided by 58 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV59 : HSE Divided by 59 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV60 : HSE Divided by 60 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV61 : HSE Divided by 61 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV62 : HSE Divided by 62 selected as RTC clock
  *               @arg @ref RCC_RTC_HSE_DIV63 : HSE Divided by 63 selected as RTC clock
  */
#define __HAL_RCC_GET_RTC_HSE_PRESCALER() ((uint32_t)(READ_BIT(RCC->CFGR1, RCC_CFGR1_RTCPRE)))

/** @brief  Macros to enable or disable the main PLL.
  * @note   After enabling the main PLL, the application software should wait on
  *         PLLRDY flag to be set indicating that PLL clock is stable and can
  *         be used as system clock source.
  * @note   The main PLL can not be disabled if it is used as system clock source
  * @note   The main PLL is disabled by hardware when entering STOP and STANDBY modes.
  */
#define __HAL_RCC_PLL1_ENABLE()         SET_BIT(RCC->CR, RCC_CR_PLL1ON)
#define __HAL_RCC_PLL1_DISABLE()        CLEAR_BIT(RCC->CR, RCC_CR_PLL1ON)

/**
  * @brief  Enables or disables each clock output (PLL1P_CLK, PLL1Q_CLK, PLL1R_CLK)
  * @note   Enabling/disabling  Those Clocks can be any time  without the need to stop the PLL1,
  *        (except the ck_pll_p of the System PLL that cannot be stopped if used as System
  *         Clock. This is mainly used to save Power.
  * @param  __PLL1_CLOCKOUT__: specifies the PLL clock to be outputted
  *          This parameter can be one of the following values:
  *            @arg RCC_PLL1_DIVP: This Clock is used to generate the high speed system clock (up to 250MHz)
  *            @arg RCC_PLL1_DIVQ: This Clock is used to generate the clock for USB (48 MHz), RNG (<=48 MHz),
  *                                OCTOSPI, SPI, SAI and Ethernet
  *            @arg RCC_PLL1_DIVR: This Clock is used to generate an accurate clock
  * @retval None
  *
  */
#define __HAL_RCC_PLL1_CLKOUT_ENABLE(__PLL1_CLOCKOUT__)   SET_BIT(RCC->PLL1CFGR, (__PLL1_CLOCKOUT__))

#define __HAL_RCC_PLL1_CLKOUT_DISABLE(__PLL1_CLOCKOUT__)  CLEAR_BIT(RCC->PLL1CFGR, (__PLL1_CLOCKOUT__))

/**
  * @brief  Macro to get the PLL clock output enable status.
  * @param  __PLL1_CLOCKOUT__ specifies the PLL1 clock to be output.
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL1_DIVP: This Clock is used to generate the high speed system clock (up to 250MHz)
  *            @arg RCC_PLL1_DIVQ: This Clock is used to generate the clock for USB (48 MHz), RNG (<=48 MHz),
  *                                OCTOSPI, SPI, SAI and Ethernet
  *            @arg RCC_PLL1_DIVR: This Clock is used to generate an accurate clock
  * @retval SET / RESET
  */
#define __HAL_RCC_GET_PLL1_CLKOUT_CONFIG(__PLL1_CLOCKOUT__)  READ_BIT(RCC->PLL1CFGR, (__PLL1_CLOCKOUT__))

/**
  * @brief  Enables or disables Fractional Part Of The Multiplication Factor of PLL1 VCO
  * @note   Enabling/disabling  Fractional Part can be any time  without the need to stop the PLL1
  * @retval None
  */
#define __HAL_RCC_PLL1_FRACN_ENABLE()   SET_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1FRACEN)

#define __HAL_RCC_PLL1_FRACN_DISABLE()  CLEAR_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1FRACEN)

/**
  * @brief  Macro to configures the main PLL (PLL1) clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL1 is disabled.
  *
  * @param  __PLL1SOURCE__: specifies the PLL entry clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_PLL1_SOURCE_CSI: CSI oscillator clock selected as PLL1 clock entry
  *            @arg RCC_PLL1_SOURCE_HSI: HSI oscillator clock selected as PLL1 clock entry
  *            @arg RCC_PLL1_SOURCE_HSE: HSE oscillator clock selected as PLL1 clock entry
  * @note   This clock source (__PLL1SOURCE__) is the clock source for PLL1 (main PLL) and is different
            from PLL2 & PLL3 clock sources.
  *
  * @param  __PLL1M__: specifies the division factor for PLL VCO input clock
  *          This parameter must be a number between 1 and 63.
  * @note   You have to set the PLL1M parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 16 MHz.
  *
  * @param  __PLL1N__: specifies the multiplication factor for PLL VCO output clock
  *          This parameter must be a number between 4 and 512.
  * @note   You have to set the PLL1N parameter correctly to ensure that the VCO
  *         output frequency is between 150 and 420 MHz (when in medium VCO range) or
  *         between 192 and 836 MHZ (when in wide VCO range)
  *
  * @param  __PLL1P__: specifies the division factor for system  clock.
  *          This parameter must be a number between 2 and 128 (where odd numbers not allowed)
  *
  * @param  __PLL1Q__: specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128
  *
  * @param  __PLL1R__: specifies the division factor for peripheral kernel clocks
  *          This parameter must be a number between 1 and 128
  *
  * @retval None
  */
#define __HAL_RCC_PLL1_CONFIG(__PLL1SOURCE__, __PLL1M__, __PLL1N__, __PLL1P__, __PLL1Q__, __PLL1R__) \
  do{ MODIFY_REG(RCC->PLL1CFGR, (RCC_PLL1CFGR_PLL1SRC | RCC_PLL1CFGR_PLL1M), \
                   ((__PLL1SOURCE__) << RCC_PLL1CFGR_PLL1SRC_Pos) | ((__PLL1M__) << RCC_PLL1CFGR_PLL1M_Pos));\
    WRITE_REG(RCC->PLL1DIVR , ( (((__PLL1N__) - 1U ) & RCC_PLL1DIVR_PLL1N) | \
                                ((((__PLL1P__) - 1U ) << RCC_PLL1DIVR_PLL1P_Pos) & RCC_PLL1DIVR_PLL1P) | \
                                ((((__PLL1Q__) - 1U) << RCC_PLL1DIVR_PLL1Q_Pos) & RCC_PLL1DIVR_PLL1Q) | \
                                ((((__PLL1R__) - 1U) << RCC_PLL1DIVR_PLL1R_Pos) & RCC_PLL1DIVR_PLL1R))); \
  } while(0)

/** @brief  Macro to configure the PLL1 clock source.
  * @note   This function must be used only when PLL1 is disabled.
  * @param  __PLL1SOURCE__: specifies the PLL1 entry clock source.
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL1_SOURCE_CSI: CSI oscillator clock selected as PLL1 clock entry
  *            @arg RCC_PLL1_SOURCE_HSI: HSI oscillator clock selected as PLL1 clock entry
  *            @arg RCC_PLL1_SOURCE_HSE: HSE oscillator clock selected as PLL1 clock entry
  *
  */
#define __HAL_RCC_PLL1_PLLSOURCE_CONFIG(__PLL1SOURCE__) \
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1SRC, (__PLL1SOURCE__))

/** @brief  Macro to configure the PLL1 input clock division factor M.
  *
  * @note   This function must be used only when the PLL1 is disabled.
  * @note   PLL1 clock source is common with the main PLL (configured through
  *         __HAL_RCC_PLL1_CONFIG() macro)
  *
  * @param  __PLL1M__ specifies the division factor for PLL1 clock.
  *         This parameter must be a number between Min_Data = 1 and Max_Data = 63.
  *         In order to save power when PLL1 is not used, the value of PLL1M must be set to 0.
  *
  * @retval None
  */
#define __HAL_RCC_PLL1_DIVM_CONFIG(__PLL1M__) \
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1M, (__PLL1M__) << RCC_PLL1CFGR_PLL1M_Pos)

/**
  * @brief  Macro to configures the main PLL clock Fractional Part Of The Multiplication Factor
  *
  * @note   These bits can be written at any time, allowing dynamic fine-tuning of the PLL1 VCO
  *
  * @param  __PLL1FRACN__: specifies Fractional Part Of The Multiplication Factor for PLL1 VCO
  *                            It should be a value between 0 and 8191
  * @note   Warning: The software has to set correctly these bits to insure that the VCO
  *                  output frequency is between its valid frequency range, which is:
  *                   192 to 836 MHz if PLL1VCOSEL = 0
  *                   150 to 420 MHz if PLL1VCOSEL = 1.
  *
  *
  * @retval None
  */
#define  __HAL_RCC_PLL1_FRACN_CONFIG(__PLL1FRACN__) WRITE_REG(RCC->PLL1FRACR, \
                                                              (uint32_t)(__PLL1FRACN__) << RCC_PLL1FRACR_PLL1FRACN_Pos)

/** @brief  Macro to select the PLL1 reference frequency range.
  * @param  __PLL1VCIRange__: specifies the PLL1 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL1_VCIRANGE_0: Range frequency is between 1 and 2 MHz
  *            @arg RCC_PLL1_VCIRANGE_1: Range frequency is between 2 and 4 MHz
  *            @arg RCC_PLL1_VCIRANGE_2: Range frequency is between 4 and 8 MHz
  *            @arg RCC_PLL1_VCIRANGE_3: Range frequency is between 8 and 16 MHz
  * @retval None
  */
#define __HAL_RCC_PLL1_VCIRANGE(__PLL1VCIRange__) \
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1RGE, (__PLL1VCIRange__))

/** @brief  Macro to select the PLL1 reference frequency range.
  * @param  __RCC_PLL1VCORange__: specifies the PLL1 input frequency range
  *         This parameter can be one of the following values:
  *            @arg RCC_PLL1_VCORANGE_WIDE: Range frequency is between 192 and 836 MHz
  *            @arg RCC_PLL1_VCORANGE_MEDIUM: Range frequency is between 150 and 420 MHz
  *
  *
  * @retval None
  */
#define __HAL_RCC_PLL1_VCORANGE(__RCC_PLL1VCORange__) \
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1VCOSEL, (__RCC_PLL1VCORange__))

/** @brief  Macro to get the oscillator used as PLL1 clock source.
  * @retval The oscillator used as PLL1 clock source. The returned value can be one
  *         of the following:
  *              - RCC_PLL1_SOURCE_NONE: No oscillator is used as PLL clock source.
  *              - RCC_PLL1_SOURCE_CSI: CSI oscillator is used as PLL clock source.
  *              - RCC_PLL1_SOURCE_HSI: HSI oscillator is used as PLL clock source.
  *              - RCC_PLL1_SOURCE_HSE: HSE oscillator is used as PLL clock source.
  */
#define __HAL_RCC_GET_PLL1_OSCSOURCE() ((uint32_t)(RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1SRC))

/**
  * @brief  Macro to configure the system clock source.
  * @param  __SYSCLKSOURCE__: specifies the system clock source.
  *          This parameter can be one of the following values:
  *              - RCC_SYSCLKSOURCE_CSI: CSI oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_HSI: HSI oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_HSE: HSE oscillator is used as system clock source.
  *              - RCC_SYSCLKSOURCE_PLL1CLK: PLL1P output is used as system clock source.
  * @retval None
  */
#define __HAL_RCC_SYSCLK_CONFIG(__SYSCLKSOURCE__) \
  MODIFY_REG(RCC->CFGR1, RCC_CFGR1_SW, (__SYSCLKSOURCE__))

/** @brief  Macro to get the clock source used as system clock.
  * @retval The clock source used as system clock. The returned value can be one
  *         of the following:
  *              - RCC_SYSCLKSOURCE_STATUS_CSI: CSI used as system clock.
  *              - RCC_SYSCLKSOURCE_STATUS_HSI: HSI used as system clock.
  *              - RCC_SYSCLKSOURCE_STATUS_HSE: HSE used as system clock.
  *              - RCC_SYSCLKSOURCE_STATUS_PLL1CLK: PLL1P used as system clock.
  */
#define __HAL_RCC_GET_SYSCLK_SOURCE() ((uint32_t)(RCC->CFGR1 & RCC_CFGR1_SWS))

#define __HAL_FLASH_GET_LATENCY()               READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY)
#define __HAL_FLASH_SET_LATENCY(__LATENCY__)    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, (__LATENCY__))

#define RCC_CLOCKTYPE_SYSCLK           (0x00000001U)  /*!< SYSCLK to configure */
#define RCC_CLOCKTYPE_HCLK             (0x00000002U)  /*!< HCLK to configure */
#define RCC_CLOCKTYPE_PCLK1            (0x00000004U)  /*!< PCLK1 to configure */
#define RCC_CLOCKTYPE_PCLK2            (0x00000008U)  /*!< PCLK2 to configure */
#define RCC_CLOCKTYPE_PCLK3            (0x00000010U)  /*!< PCLK3 to configure */

#define RCC_SYSCLKSOURCE_PLLCLK          (RCC_CFGR1_SW_0 | RCC_CFGR1_SW_1)  /*!< PLL1 selection as system clock */

#define RCC_SYSCLKSOURCE_STATUS_HSI      (0x00000000U)                       /*!< HSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_CSI      RCC_CFGR1_SWS_0                     /*!< CSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_HSE      RCC_CFGR1_SWS_1                     /*!< HSE used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_PLLCLK   (RCC_CFGR1_SWS_0 | RCC_CFGR1_SWS_1) /*!< PLL1 used as system clock */

#define RCC_FLAG_MASK             (0x1FU)
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U) ? RCC->CR :                    \
                                         ((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR :                 \
                                          ((((__FLAG__) >> 5U) == 3U) ? RCC->RSR : RCC->CIFR))) &  \
                                        (1U << ((__FLAG__) & RCC_FLAG_MASK))) != 0U) ? 1U : 0U)

#define RCC_CR_REG_INDEX              (1U)
#define RCC_FLAG_HSIDIVF               ((uint32_t)((RCC_CR_REG_INDEX << 5U) | RCC_CR_HSIDIVF_Pos))  /*!< HSI divider flag */
#define __HAL_RCC_GET_HSI_DIVIDER() ((uint32_t)(READ_BIT(RCC->CR, RCC_CR_HSIDIV)))


/**
  * @brief Internal Core Speed oscillator (CSI) default value.
  *        This value is the default CSI range value after Reset.
  */
#if !defined  (CSI_VALUE)
  #define CSI_VALUE              4000000UL /*!< Value of the Internal oscillator in Hz*/
#endif /* CSI_VALUE */

/**
  * @brief Internal High Speed oscillator (HSI) value.
  *        This value is used by the RCC HAL module to compute the system frequency
  *        (when HSI is used as system clock source, directly or through the PLL).
  */
#if !defined  (HSI_VALUE)
  #define HSI_VALUE              64000000UL /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

/**
  * @brief Internal High Speed oscillator (HSI48) value for USB FS, SDMMC and RNG.
  *        This internal oscillator is mainly dedicated to provide a high precision clock to
  *        the USB peripheral by means of a special Clock Recovery System (CRS) circuitry.
  *        When the CRS is not used, the HSI48 RC oscillator runs on it default frequency
  *        which is subject to manufacturing process variations.
  */
#if !defined  (HSI48_VALUE)
  #define HSI48_VALUE             48000000UL /*!< Value of the Internal High Speed oscillator for USB FS/SDMMC/RNG in Hz.
                                                  The real value my vary depending on manufacturing process variations.*/
#endif /* HSI48_VALUE */

/**
  * @brief Internal Low Speed oscillator (LSI) value.
  */
#if !defined  (LSI_VALUE)
  #define LSI_VALUE  32000UL    /*!< LSI Typical Value in Hz*/
#endif /* LSI_VALUE */                     /*!< Value of the Internal Low Speed oscillator in Hz
                                                The real value may vary depending on the variations
                                                */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    25000000U /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#define RCC_PLL1_SOURCE_HSI              RCC_PLL1CFGR_PLL1SRC_0
#define RCC_PLL1_SOURCE_CSI              RCC_PLL1CFGR_PLL1SRC_1
#define RCC_PLL1_SOURCE_HSE              (RCC_PLL1CFGR_PLL1SRC_0 | RCC_PLL1CFGR_PLL1SRC_1)


#define RCC_PLL_NONE                 (0x00000000U)
#define RCC_PLL_OFF                  (0x00000001U)
#define RCC_PLL_ON                   (0x00000002U)

#define RCC_PLL1_DIVP                RCC_PLL1CFGR_PLL1PEN
#define RCC_PLL1_DIVQ                RCC_PLL1CFGR_PLL1QEN
#define RCC_PLL1_DIVR                RCC_PLL1CFGR_PLL1REN

#define RCC_OSCILLATORTYPE_NONE        (0x00000000U)   /*!< Oscillator configuration unchanged */
#define RCC_OSCILLATORTYPE_HSE         (0x00000001U)   /*!< HSE to configure */
#define RCC_OSCILLATORTYPE_HSI         (0x00000002U)   /*!< HSI to configure */
#define RCC_OSCILLATORTYPE_LSE         (0x00000004U)   /*!< LSE to configure */
#define RCC_OSCILLATORTYPE_LSI         (0x00000008U)   /*!< LSI to configure */
#define RCC_OSCILLATORTYPE_CSI         (0x00000010U)   /*!< CSI to configure */
#define RCC_OSCILLATORTYPE_HSI48       (0x00000020U)   /*!< HSI48 to configure */

#define RCC_CSI_ON                     RCC_CR_CSION        /*!< CSI clock activation */
#define RCC_CSICALIBRATION_DEFAULT     (0x20U)   /*!< Default CSI calibration trimming value */
#define RCC_PLL1_VCIRANGE_2              RCC_PLL1CFGR_PLL1RGE_1                               /*!< Clock range frequency between 4 and 8 MHz  */
#define RCC_PLL1_VCORANGE_WIDE           (0x00000000U)              /*!< Clock range frequency between 192 and 836 MHz  */

#define RCC_SYSCLK_DIV16               (RCC_CFGR2_HPRE_0 | RCC_CFGR2_HPRE_1 | RCC_CFGR2_HPRE_3)                     /*!< SYSCLK divided by 16 */
#define RCC_HCLK_DIV1                  (0x00000000U)                                               /*!< HCLK not divided */

#define FLASH_PROGRAMMING_DELAY_2   FLASH_ACR_WRHIGHFREQ_1 /*!< programming delay set for Flash running between 168 MHz
                                                                and 250 MHz */
#define FLASH_LATENCY_5          FLASH_ACR_LATENCY_5WS   /*!< FLASH Five wait cycles     */
#define __HAL_FLASH_SET_PROGRAM_DELAY(__DELAY__)  MODIFY_REG(FLASH->ACR, FLASH_ACR_WRHIGHFREQ, (__DELAY__))

#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif /* __packed */
#endif /* __GNUC__ */

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= ARMCC_MIN_VERSION)
#ifndef __weak
#define __weak  __WEAK
#endif /* __weak */
#ifndef __packed
#define __packed  __PACKED
#endif /* __packed */
#endif

#define SYSTICK_CLKSOURCE_HCLK_DIV8     0x0U /*!< AHB clock divided by 8 selected as SysTick clock source */
#define SYSTICK_CLKSOURCE_LSI           0x1U /*!< LSI clock selected as SysTick clock source              */
#define SYSTICK_CLKSOURCE_LSE           0x2U /*!< LSE clock selected as SysTick clock source              */
#define SYSTICK_CLKSOURCE_HCLK          0x4U /*!< AHB clock selected as SysTick clock source              */

/**
  * @brief External Low Speed oscillator (LSE) value.
  *        This value is used by the UART, RTC HAL module to compute the system frequency
  */
#if !defined  (LSE_VALUE)
  #define LSE_VALUE  32768UL    /*!< Value of the External oscillator in Hz*/
#endif /* LSE_VALUE */

#endif /* PLIB_PLIBI_MINI_HAL_H_ */
