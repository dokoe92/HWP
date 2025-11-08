/*
 * Buffered serial communication for the STM32H533 using UART
 *
 */

#include "plibi_serial.h"

#include "plib_config.h"
#if defined PL_TX_BUSY_LED
	#include "plibi_board.h"
#endif

#include "stm32h5xx.h"

#define UART_CR1_TXEIE (1<<7)
#define UART_CR1_RXNEIE (1<<5)
#define UART_ISR_ORE (1<<3)
#define UART_ISR_RXNE (1<<5)
#define UART_ISR_TXE (1<<7)

#ifndef PL_RX_BUFFER_LEN
#warning "PL_RX_BUFFER_LEN is not defined, use default value"
#define PL_RX_BUFFER_LEN 256
#endif
#ifndef PL_TX_BUFFER_LEN
#warning "PL_TX_BUFFER_LEN is not defined, use default value"
#define PL_TX_BUFFER_LEN 256
#endif

static USART_TypeDef *uart = USART2;

static uint8_t rx_buffer[PL_RX_BUFFER_LEN];
static uint8_t tx_buffer[PL_TX_BUFFER_LEN];

static pli_queue rx_queue, tx_queue;

static int tx_primed = 0;

#if defined PL_TX_BUSY_LED
static uint8_t led_primed=0;
#endif

/*
 * interrupt handler for our serial controller
 */
void USART2_IRQHandler(void) {
	uint8_t b;
	if (uart->ISR & UART_ISR_ORE) {
		// receiver overflow
		uart->ICR = UART_ISR_ORE;
	}
	while (uart->ISR & UART_ISR_RXNE) {
		// receiver buffer not empty
		b = uart->RDR;
		pli_enqueue(&rx_queue, b);
	}
	if (uart->ISR & UART_ISR_TXE) {
		// transmitter buffer  empty
		if (pli_dequeue(&tx_queue, &b)) {
			uart->TDR = b;
		} else {
			// disable transmitter buffer empty interrupt
			uart->CR1 &= ~UART_CR1_TXEIE;
			tx_primed = 0;
#if defined PL_TX_BUSY_LED
			pl_board_led_set(led_primed, 0);
#endif
		}
	}
}

int pli_serial_init(uint32_t baud) {
	static int first_run = 1;
	volatile uint32_t tmp;

	if (!first_run)
		return -1; // error: already initialized
	if ((baud < 100) || (baud > 115200))
		return -2;	// error: invalid baud rate

	first_run = 0;

	// setup queues
	pli_queue_init(&rx_queue, rx_buffer,
			sizeof(rx_buffer) / sizeof(rx_buffer[0]));
	pli_queue_init(&tx_queue, tx_buffer,
			sizeof(tx_buffer) / sizeof(tx_buffer[0]));

	/*
	 * setup serial controller
	 */
	// we use USART2, RxD..PA3, TxD..PA2

	// 1. enable clocks
	// set usart2 clock source
	MODIFY_REG(RCC->CCIPR1, RCC_CCIPR1_USART2SEL, (uint32_t )(0));

	// enable clock for usart2
	SET_BIT(RCC->APB1LENR, RCC_APB1LENR_USART2EN);
	/* Delay after an RCC peripheral clock enabling */
	tmp = READ_BIT(RCC->APB1LENR, RCC_APB1LENR_USART2EN);

	// enable clock for GPIOA
	SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
	/* Delay after an RCC peripheral clock enabling */
	tmp = READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);

	// 2. set PA2 and PA3 to AF7
	MODIFY_REG(GPIOA->MODER, 0xf0, 0xa0);
	MODIFY_REG(GPIOA->AFR[0], 0xff00, 0x7700);

	// 3. USART2
	// disable it first
	MODIFY_REG(USART2->CR1, 1, 0);

	MODIFY_REG(USART2->CR1, 0, 0x000c); // enable rx and tx
	USART2->BRR = 1563; // clock is 15 000 000 Hz / 9600 (baud) = 1562,5

	/* In asynchronous mode, the following bits must be kept cleared:
	 - LINEN and CLKEN bits in the USART_CR2 register,
	 - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
	CLEAR_BIT(USART2->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
	CLEAR_BIT(USART2->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

	// enable usart
	MODIFY_REG(USART2->CR1, 0, 1);

	// enable rx interrupt
	uart->CR1 |= UART_CR1_RXNEIE;

	// init interrupt for USART2
	tmp = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(tmp, 0, 0));
	NVIC_EnableIRQ(USART2_IRQn);

	tx_primed = 0;

	return 1;
}

void pli_serial_write(uint8_t data) {
	while (pli_enqueue(&tx_queue, data) == 0) {
		// wait for free places in transmitter queue
	}

	if (!tx_primed) {
		// activate transmitter interrupt
		tx_primed = 1;

		uart->CR1 |= UART_CR1_TXEIE;

#ifdef PL_TX_BUSY_LED
		pl_board_led_set(led_primed, 1);
#endif

	}
}

int pli_serial_read(uint8_t *data) {
	if (pli_dequeue(&rx_queue, data) > 0)
		return 1;
	return 0;
}

#ifdef PL_QUEUE_STATISTICS
void pli_serial_statistics_read(int *read, int *write) {
	*read = rx_queue.min;
	pli_queue_statistics_reset(&rx_queue);
	*write = tx_queue.min;
	pli_queue_statistics_reset(&tx_queue);
}
#endif

uint32_t pli_serial_get_status() {
	uint32_t status;
	status = uart->ISR;
	return status;
}


