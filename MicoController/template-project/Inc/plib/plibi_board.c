/*
 *
 * Board-specific initialization and functions,
 * access the on-board HW (user led, user button).
 *
 *  Note that the configuration of oscillators and clocks
 *  is taken from stm's HAL.
 */

#include "plibi_board.h"
#include "plibi_mini_hal.h"

static uint32_t uwTickPrio = (1UL << __NVIC_PRIO_BITS); /* Invalid PRIO */
static int uwTickFreq = 1U; /* 1KHz */

static void SystemClock_Config(void);

/*
 * initializes the user button (GPIO PC13)
 */
static void userbutton_init() {
	volatile uint32_t tmp;

	// Button on PC13
	SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);
	/* Delay after an RCC peripheral clock enabling */
	tmp = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);

	MODIFY_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_Msk,
			GPIO_PULLDOWN << GPIO_PUPDR_PUPD13_Pos);
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE13_Msk,
			GPIO_MODE_INPUT << GPIO_MODER_MODE13_Pos);
	UNUSED(tmp);
}

/*
 * initializes the programmable user led (GPIO PA5)
 */
static void sysled_init() {
	volatile uint32_t tmp;

	SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
	/* Delay after an RCC peripheral clock enabling */
	tmp = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
	UNUSED(tmp);

	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE5_Msk,
			GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODE5_Pos);
	GPIOA->BRR = 1 << 5;
}

/*
 * returns if the user button is pressed (0) or not pressed (1)
 *
 */
int pl_board_button_get(void) {
	// read from PC13
	return (GPIOC->IDR & (1 << 13)) != 0;
}

/*
 * switches the user led on (0) or off (1)
 */
void pl_board_led_set(uint8_t val) {
	if (val) {
		GPIOA->BSRR = 1 << 5;
	} else {
		GPIOA->BRR = 1 << 5;
	}
}

/*
 * initialization of the board (clock, systick timer)
 * and peripherials
 *
 */
void pli_board_init(void) {
	SystemClock_Config();

	// configure instruction cache
	MODIFY_REG(ICACHE->CR, ICACHE_CR_WAYSEL, 0);

	sysled_init();

	userbutton_init();
}

void pl_error(int component, int code) {
	while (1)
		;
}

/**
 * @brief  Return the SYSCLK frequency.
 *
 * @note   The system frequency computed by this function may not be the real
 *         frequency in the chip. It is calculated based on the predefined
 *         constants of the selected clock source:
 * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
 * @note     If SYSCLK source is CSI, function returns values based on CSI_VALUE(**)
 * @note     If SYSCLK source is HSE, function returns values based on HSE_VALUE(***)
 * @note     If SYSCLK source is PLL, function returns values based on HSI_VALUE(*), CSI_VALUE(**)
 *           or HSE_VALUE(***) multiplied/divided by the PLL factors.
 * @note     (*) HSI_VALUE is a constant defined in stm32h5xx_hal_conf.h file (default value
 *               64 MHz) but the real value may vary depending on the variations
 *               in voltage and temperature.
 * @note     (**) CSI_VALUE is a constant defined in stm32h5xx_hal_conf.h file (default value
 *               4 MHz) but the real value may vary depending on the variations
 *               in voltage and temperature.
 * @note     (***) HSE_VALUE is a constant defined in stm32h5xx_hal_conf.h file (default value
 *                24 MHz), user has to ensure that HSE_VALUE is same as the real
 *                frequency of the crystal used. Otherwise, this function may
 *                have wrong result.
 *
 * @note   The result of this function could be not correct when using fractional
 *         value for HSE crystal.
 *
 * @note   This function can be used by the user application to compute the
 *         baudrate for the communication peripherals or configure other parameters.
 *
 * @note   Each time SYSCLK changes, this function must be called to update the
 *         right SYSCLK value. Otherwise, any configuration based on this function will be incorrect.
 *
 *
 * @retval SYSCLK frequency
 */
static uint32_t HAL_RCC_GetSysClockFreq(void) {
	uint32_t pllsource;
	uint32_t pllp;
	uint32_t pllm;
	uint32_t pllfracen;
	uint32_t sysclockfreq;
	uint32_t hsivalue;
	float_t fracn1;
	float_t pllvco;

	if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_CSI) {
		/* CSI used as system clock  source */
		sysclockfreq = CSI_VALUE;
	} else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI) {
		/* HSI used as system clock source */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIVF) != 0U) {
			sysclockfreq = (uint32_t) (HSI_VALUE
					>> (__HAL_RCC_GET_HSI_DIVIDER() >> RCC_CR_HSIDIV_Pos));
		} else {
			sysclockfreq = (uint32_t) HSI_VALUE;
		}
	} else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSE) {
		/* HSE used as system clock source */
		sysclockfreq = HSE_VALUE;
	}

	else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) {
		/* PLL used as system clock  source */

		pllsource = (RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1SRC);
		pllm = ((RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1M) >> RCC_PLL1CFGR_PLL1M_Pos);
		pllfracen = ((RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1FRACEN)
				>> RCC_PLL1CFGR_PLL1FRACEN_Pos);
		fracn1 = (float_t) (uint32_t) (pllfracen * ((RCC->PLL1FRACR &
		RCC_PLL1FRACR_PLL1FRACN) >> RCC_PLL1FRACR_PLL1FRACN_Pos));

		if (pllm != 0U) {
			switch (pllsource) {
			case RCC_PLL1_SOURCE_HSI: /* HSI used as PLL1 clock source */

				if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIVF) != 0U) {
					hsivalue =
							(HSI_VALUE
									>> (__HAL_RCC_GET_HSI_DIVIDER()
											>> RCC_CR_HSIDIV_Pos));
					pllvco =
							((float_t) hsivalue / (float_t) pllm)
									* ((float_t) (uint32_t) (RCC->PLL1DIVR
											& RCC_PLL1DIVR_PLL1N)
											+ (fracn1 / (float_t) 0x2000)
											+ (float_t) 1);
				} else {
					pllvco =
							((float_t) HSI_VALUE / (float_t) pllm)
									* ((float_t) (uint32_t) (RCC->PLL1DIVR
											& RCC_PLL1DIVR_PLL1N)
											+ (fracn1 / (float_t) 0x2000)
											+ (float_t) 1);
				}

				break;

			case RCC_PLL1_SOURCE_HSE: /* HSE used as PLL1 clock source */
				pllvco = ((float_t) HSE_VALUE / (float_t) pllm)
						* ((float_t) (uint32_t) (RCC->PLL1DIVR
								& RCC_PLL1DIVR_PLL1N)
								+ (fracn1 / (float_t) 0x2000) + (float_t) 1);

				break;

			case RCC_PLL1_SOURCE_CSI: /* CSI used as PLL1 clock source */
			default:
				pllvco = ((float_t) CSI_VALUE / (float_t) pllm)
						* ((float_t) (uint32_t) (RCC->PLL1DIVR
								& RCC_PLL1DIVR_PLL1N)
								+ (fracn1 / (float_t) 0x2000) + (float_t) 1);
				break;
			}

			pllp = (((RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1P)
					>> RCC_PLL1DIVR_PLL1P_Pos) + 1U);
			sysclockfreq = (uint32_t) (float_t) (pllvco / (float_t) pllp);
		} else {
			sysclockfreq = 0;
		}
	}

	else {
		/* HSI is the default system clock source */
		sysclockfreq = (uint32_t) HSI_VALUE;
	}

	return sysclockfreq;
}

/**
 * @brief  Get the SysTick clock source configuration.
 * @retval  SysTick clock source that can be one of the following values:
 *             @arg SYSTICK_CLKSOURCE_LSI: LSI clock selected as SysTick clock source.
 *             @arg SYSTICK_CLKSOURCE_LSE: LSE clock selected as SysTick clock source.
 *             @arg SYSTICK_CLKSOURCE_HCLK: AHB clock selected as SysTick clock source.
 *             @arg SYSTICK_CLKSOURCE_HCLK_DIV8: AHB clock divided by 8 selected as SysTick clock source.
 */
static uint32_t HAL_SYSTICK_GetCLKSourceConfig_mine(void) {
	uint32_t systick_source;
	uint32_t systick_rcc_source;

	/* Read SysTick->CTRL register for internal or external clock source */
	if (READ_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk) != 0U) {
		/* Internal clock source */
		systick_source = SYSTICK_CLKSOURCE_HCLK;
	} else {
		/* External clock source, check the selected one in RCC */
		systick_rcc_source = READ_BIT(RCC->CCIPR4, RCC_CCIPR4_SYSTICKSEL);

		switch (systick_rcc_source) {
		case (0x00000000U):
			systick_source = SYSTICK_CLKSOURCE_HCLK_DIV8;
			break;

		case (RCC_CCIPR4_SYSTICKSEL_0):
			systick_source = SYSTICK_CLKSOURCE_LSI;
			break;

		case (RCC_CCIPR4_SYSTICKSEL_1):
			systick_source = SYSTICK_CLKSOURCE_LSE;
			break;

		default:
			systick_source = SYSTICK_CLKSOURCE_HCLK_DIV8;
			break;
		}
	}
	return systick_source;
}

/**
 * @brief  Initialize the System Timer with interrupt enabled and start the System Tick Timer (SysTick):
 *         Counter is in free running mode to generate periodic interrupts.
 * @param  TicksNumb: Specifies the ticks Number of ticks between two interrupts.
 * @retval status:  - 0  Function succeeded.
 *                  - 1  Function failed.
 */
static uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb) {
	if ((TicksNumb - 1UL) > SysTick_LOAD_RELOAD_Msk) {
		/* Reload value impossible */
		return (1UL);
	}

	/* Set reload register */
	WRITE_REG(SysTick->LOAD, (uint32_t )(TicksNumb - 1UL));

	/* Load the SysTick Counter Value */
	WRITE_REG(SysTick->VAL, 0UL);

	/* Enable SysTick IRQ and SysTick Timer */
	SET_BIT(SysTick->CTRL,
			(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk));

	/* Function successful */
	return (0UL);
}

/**
 * @brief  Set the priority of an interrupt.
 * @param  IRQn: External interrupt number.
 *         This parameter can be an enumerator of IRQn_Type enumeration
 *         (For the complete STM32 Devices IRQ Channels list, please refer to the appropriate
 *          CMSIS device file (stm32h5xxxx.h))
 * @param  PreemptPriority: The pre-emption priority for the IRQn channel.
 *         This parameter can be a value between 0 and 15
 *         A lower priority value indicates a higher priority
 * @param  SubPriority: the subpriority level for the IRQ channel.
 *         This parameter can be a value between 0 and 15
 *         A lower priority value indicates a higher priority.
 * @retval None
 */
static void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority,
		uint32_t SubPriority) {
	uint32_t prioritygroup;

	prioritygroup = NVIC_GetPriorityGrouping();

	NVIC_SetPriority(IRQn,
			NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

/**
 * @brief This function configures the source of the time base.
 *        The time source is configured to have 1ms time base with a dedicated
 *        Tick interrupt priority.
 * @note This function is called  automatically at the beginning of program after
 *       reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
 * @note In the default implementation, SysTick timer is the source of time base.
 *       It is used to generate interrupts at regular time intervals.
 *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
 *       The SysTick interrupt must have higher priority (numerically lower)
 *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
 *       The function is declared as __weak  to be overwritten  in case of other
 *       implementation  in user file.
 * @param TickPriority: Tick interrupt priority.
 * @retval HAL status
 */

__weak int HAL_InitTick(uint32_t TickPriority) {
	//HAL_InitTick();
	uint32_t ticknumber = 0U;
	uint32_t systicksel;

	/* Check uwTickFreq for MisraC 2012 (even if uwTickFreq is a enum type that don't take the value zero)*/
	if ((uint32_t) uwTickFreq == 0UL) {
		return 1; //HAL_ERROR;
	}

	/* Check Clock source to calculate the tickNumber */
	if (READ_BIT(SysTick->CTRL,
			SysTick_CTRL_CLKSOURCE_Msk) == SysTick_CTRL_CLKSOURCE_Msk) {
		/* HCLK selected as SysTick clock source */
		ticknumber = SystemCoreClock / (1000UL / (uint32_t) uwTickFreq);
	} else {
		systicksel = HAL_SYSTICK_GetCLKSourceConfig_mine();
		switch (systicksel) {
		/* HCLK_DIV8 selected as SysTick clock source */
		case SYSTICK_CLKSOURCE_HCLK_DIV8:
			/* Calculate tick value */
			ticknumber = (SystemCoreClock / (8000UL / (uint32_t) uwTickFreq));
			break;
			/* LSI selected as SysTick clock source */
		case SYSTICK_CLKSOURCE_LSI:
			/* Calculate tick value */
			ticknumber = (LSI_VALUE / (1000UL / (uint32_t) uwTickFreq));
			break;
			/* LSE selected as SysTick clock source */
		case SYSTICK_CLKSOURCE_LSE:
			/* Calculate tick value */
			ticknumber = (LSE_VALUE / (1000UL / (uint32_t) uwTickFreq));
			break;
		default:
			/* Nothing to do */
			break;
		}
	}

	/* Configure the SysTick to have interrupt in 1ms time basis*/
	if (HAL_SYSTICK_Config(ticknumber) > 0U) {
		return 1; //HAL_ERROR;
	}

	/* Configure the SysTick IRQ priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
	uwTickPrio = TickPriority;

	/* Return function status */
	return 0; //HAL_OK;
}

/**
 * @brief  Initialize the CPU, AHB and APB busses clocks according to the specified
 *         parameters in the pClkInitStruct.
 * @param  pClkInitStruct  pointer to an RCC_OscInitTypeDef structure that
 *         contains the configuration information for the RCC peripheral.
 * @param  FLatency  FLASH Latency
 *          This parameter can be one of the following values:
 *            @arg FLASH_LATENCY_0   FLASH 0 Latency cycle
 *            @arg FLASH_LATENCY_1   FLASH 1 Latency cycle
 *            @arg FLASH_LATENCY_2   FLASH 2 Latency cycles
 *            @arg FLASH_LATENCY_3   FLASH 3 Latency cycles
 *            @arg FLASH_LATENCY_4   FLASH 4 Latency cycles
 *            @arg FLASH_LATENCY_5   FLASH 5 Latency cycles
 *
 * @note   The SystemCoreClock CMSIS variable is used to store System Clock Frequency
 *         and updated by HAL_RCC_GetHCLKFreq() function called within this function
 *
 * @note   The HSI is used by default as system clock source after
 *         startup from Reset, wake-up from STANDBY mode. After restart from Reset,
 *         the HSI frequency is set to its default value 64 MHz.
 *
 * @note   The HSI or CSI can be selected as system clock source after wake-up
 *         from STOP modes or in case of failure of the HSE when used directly or indirectly
 *         as system clock (if the Clock Security System CSS is enabled).
 *
 * @note   A switch from one clock source to another occurs only if the target
 *         clock source is ready (clock stable after startup delay or PLL locked).
 *         If a clock source which is not yet ready is selected, the switch will
 *         occur when the clock source is ready.
 *
 * @note   You can use HAL_RCC_GetClockConfig() function to know which clock is
 *         currently used as system clock source.
 *
 * @retval HAL Status.
 */
static int HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef *pClkInitStruct,
		uint32_t FLatency) {
	int halstatus;

	/* To correctly read data from FLASH memory, the number of wait states (LATENCY)
	 must be correctly programmed according to the frequency of the CPU clock
	 (HCLK) and the supply voltage of the device. */

	/* Increasing the number of wait states because of higher CPU frequency */
	if (FLatency > __HAL_FLASH_GET_LATENCY()) {
		/* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
		__HAL_FLASH_SET_LATENCY(FLatency);

		/* Check that the new number of wait states is taken into account to access the Flash
		 memory by reading the FLASH_ACR register */
		if (__HAL_FLASH_GET_LATENCY() != FLatency) {
			return 1;
		}
	}

	/*-------------------------- HCLK Configuration --------------------------*/
	if (((pClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK) {
		if ((pClkInitStruct->AHBCLKDivider) > (RCC->CFGR2 & RCC_CFGR2_HPRE)) {
			MODIFY_REG(RCC->CFGR2, RCC_CFGR2_HPRE,
					pClkInitStruct->AHBCLKDivider);
		}
	}

	/*------------------------- SYSCLK Configuration ---------------------------*/
	if (((pClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK)
			== RCC_CLOCKTYPE_SYSCLK) {

		/* PLL is selected as System Clock Source */
		if (pClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK) {
			/* Check the PLL ready flag */
			if (READ_BIT(RCC->CR, RCC_CR_PLL1RDY) == 0U) {
				return 1;
			}
		}

		MODIFY_REG(RCC->CFGR1, RCC_CFGR1_SW, pClkInitStruct->SYSCLKSource);

		if (pClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK) {
			while (__HAL_RCC_GET_SYSCLK_SOURCE()
					!= RCC_SYSCLKSOURCE_STATUS_PLLCLK)
				;
		}
	}

	/* Update the SystemCoreClock global variable */
	SystemCoreClock =
			HAL_RCC_GetSysClockFreq()
					>> AHBPrescTable[(RCC->CFGR2 & RCC_CFGR2_HPRE)
							>> RCC_CFGR2_HPRE_Pos];

	/* Configure the source of time base considering new system clocks settings*/
	halstatus = HAL_InitTick(uwTickPrio);

	return halstatus;
}

/**
 * @brief  Initialize the RCC Oscillators according to the specified parameters in the
 *         RCC_OscInitTypeDef.
 * @param  pOscInitStruct pointer to an RCC_OscInitTypeDef structure that
 *         contains the configuration information for the RCC Oscillators.
 * @note   The PLL is not disabled when used as system clock.
 * @note   Transitions LSE Bypass to LSE On and LSE On to LSE Bypass are not
 *         supported by this macro. User should request a transition to LSE Off
 *         first and then LSE On or LSE Bypass.
 * @note   Transition HSE Bypass to HSE On and HSE On to HSE Bypass are not
 *         supported by this macro. User should request a transition to HSE Off
 *         first and then HSE On or HSE Bypass.
 * @retval HAL status
 */
static int HAL_RCC_OscConfig(const RCC_OscInitTypeDef *pOscInitStruct) {
	uint32_t temp_sysclksrc;

	// get sysclock source
	temp_sysclksrc = (uint32_t) (RCC->CFGR1 & RCC_CFGR1_SWS);

	// CSI configuration, see RM chapter 11
	/* Enable the Internal High Speed oscillator (CSI). */
	SET_BIT(RCC->CR, RCC_CR_CSION);

	/*-------------------------------- PLL1 Configuration -----------------------*/
	if ((pOscInitStruct->PLL.PLLState) != RCC_PLL_NONE) {
		/* Check if the PLL1 is used as system clock or not */
		if (temp_sysclksrc != RCC_SYSCLKSOURCE_STATUS_PLLCLK) {
			if ((pOscInitStruct->PLL.PLLState) == RCC_PLL_ON) {
				/* Configure the PLL1 clock source, multiplication and division factors. */
				__HAL_RCC_PLL1_CONFIG(pOscInitStruct->PLL.PLLSource,
						pOscInitStruct->PLL.PLLM, pOscInitStruct->PLL.PLLN,
						pOscInitStruct->PLL.PLLP, pOscInitStruct->PLL.PLLQ,
						pOscInitStruct->PLL.PLLR);

				/* Disable PLL1FRACN . */
				__HAL_RCC_PLL1_FRACN_DISABLE();

				/* Configure PLL  PLL1FRACN */
				__HAL_RCC_PLL1_FRACN_CONFIG(pOscInitStruct->PLL.PLLFRACN);

				/* Enable PLL1FRACN . */
				__HAL_RCC_PLL1_FRACN_ENABLE();

				/* Select PLL1 input reference frequency range: VCI */
				__HAL_RCC_PLL1_VCIRANGE(pOscInitStruct->PLL.PLLRGE);

				/* Select PLL1 output frequency range : VCO */
				__HAL_RCC_PLL1_VCORANGE(pOscInitStruct->PLL.PLLVCOSEL);

				/* Enable PLL1 System Clock output. */
				__HAL_RCC_PLL1_CLKOUT_ENABLE(RCC_PLL1_DIVP);

				/* Enable the PLL1. */
				__HAL_RCC_PLL1_ENABLE();

				/* Wait till PLL1 is ready */
				while (READ_BIT(RCC->CR, RCC_CR_PLL1RDY) == 0U)
					;
			}
		}
	}
	return 0;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
static void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	// set voltage scaling to "scale 0", see RM, chapter 10 "Power control"
	PWR->VOSCR |= PWR_VOSCR_VOS;
	// wait for completion of new voltage scaling
	while (!READ_BIT(PWR->VOSSR, PWR_VOSSR_VOSRDY)) {
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI;
	RCC_OscInitStruct.CSIState = RCC_CSI_ON;
	RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 120;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != 0) {
		pl_error(1, 1);
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != 0) {
		pl_error(1, 2);
	}

	/** Configure the programming delay
	 */
	__HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

__weak void SysTick_Handler(void){
	/*
	 * This is the dummy systick handler.
	 * Use your own handler if you want ticks,
	 * but do not change here!
	 */
	volatile int i=0;
	i=i;
}
