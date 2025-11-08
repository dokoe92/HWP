/*
 * Application logic for plib, which includes
 * - helper functions
 * - logic for communication with the virtual peripherals
 *
*/

#include <string.h>
#include <stdint.h>
#include "plib.h"
#include "plibi_serial.h"
#include "plib_config.h"

typedef uint32_t systick_t;
extern volatile systick_t tick;

/*
 * configuration definitions
 */
#define MAX_READ_FROM_VISU 32

typedef struct time_stamp_t {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} time_stamp_t;

enum error_codes {
	E_NONE = 0,
	E_UNSPEC,
	E_UNKNOWN_SCREEN,
	E_UNKNOWN_ITEM,
	E_LENGTH,
	E_DECODE,
	E_VALUE
};

/*
 * local data
 */
// represents its state: 0..uninitialized, 1..init, 2..set a screen
static uint8_t state = 0;
static uint8_t state_switch = 0;
static uint8_t state_button = 0;
static uint8_t state_led = 0;
uint16_t state_adc[PL_NUMBER_ADCS];

static char screen = 0;
static time_stamp_t time_current;

static double random() {
	// this gives random values in the range 0..1
	double b;
	const unsigned long a = 65539;
	const unsigned long m = 2147483647;
	static unsigned long xi = 7;

	xi = (a * xi) % m;
	b = (double) xi / (double) m;
	return b;
}

static char hex_digit(int digit) {
	return digit <= 9 ? '0' + digit : 'a' + digit - 10;
}

static void encode8(uint8_t s, char *d) {
	// encode 8 bit to hex
	*d++ = hex_digit(s >> 4);
	*d++ = hex_digit(s & 0x0f);
}

static void encode16(uint16_t s, char *d) {
	// encode 16 bit to hex
	for (int i = 0; i <= 1; i++) {
		uint8_t b = s >> ((1 - i) * 8);
		*d++ = hex_digit(b >> 4);
		*d++ = hex_digit(b & 0x0f);
	}
}

static void encode32(uint32_t s, char *d) {
	// encode 32 bit to hex
	for (int i = 0; i <= 3; i++) {
		uint8_t b = s >> ((3 - i) * 8);
		*d++ = hex_digit(b >> 4);
		*d++ = hex_digit(b & 0x0f);
	}
}

static void send_string(char *prefix, char *data, int newline) {
	// write prefix and data to PC, optionally add a '\n'
	if (prefix)
		while (*prefix)
			pli_serial_write(*prefix++);
	while (*data)
		pli_serial_write(*data++);
	if (newline)
		pli_serial_write('\n');
}

/*
 * Convert nr_digits in text from hex to int
 * Returns 0 on fail, positive on success
 *
 */
static int from_hex(char *text, uint32_t *out, int nr_digits) {
	uint32_t value = 0;
	int valid = 1;

	while (nr_digits && valid) {
		if (((*text >= '0') && (*text <= '9'))
				|| ((*text >= 'a') && (*text <= 'f'))) {
			value = (value << 4)
					+ (*text <= '9' ? *text - '0' : *text - 'a' + 10);
			nr_digits--;
			text++;
		} else
			valid = 0;
	}
	if (valid) {
		*out = value;
		return 1;
	} else
		return 0;
}

/*
 * Converts nr_digits in text from dec to int
 * Return 0 on fail, positive on success
 *
 */
static int from_dec(char *text, uint32_t *out, int nr_digits) {
	uint32_t value = 0;
	int valid = 1;

	while (nr_digits && valid) {
		if ((*text >= '0') && (*text <= '9')) {

			value = value * 10 + *text - '0';
			nr_digits--;
			text++;
		} else
			valid = 0;
	}
	if (valid) {
		*out = value;
		return 1;
	} else
		return 0;
}

static void send_screen() {
	char packet[] = "dS ";
	packet[2] = screen + '0';
	send_string(0, packet, 1);
}

static void incoming_error(char *msg, int len) {
	pl_log("Error received, going into infinite loop");

	while (1)
		;
}

static enum error_codes incoming_request(char *msg, int len) {
	// msg points to content of the request, which is after the '?'
	char item_id;
	char response[4];

	if (len < 1)
		return E_LENGTH;	// too short, silently ignore this packet

	item_id = msg[0];
	msg++; // msg now points to the data after the item_id
	len--;

	switch (item_id) {
	case 'S':	// screen
		if (len) {
			return E_LENGTH;
			break;
		}
		response[0] = 'S';
		response[1] = screen + '0';
		response[2] = 0;
		break;
	case '0':	// led, just for fun
		response[0] = '0';
		encode8(state_led, response + 1);
		response[3] = 0;
		break;
	default:
		// we have an unknown item_id
		return E_UNKNOWN_ITEM;
	}
	send_string("d", response, 1);

	return E_NONE;
}

static enum error_codes incoming_setter_screen0(char *msg, int len) {
	char item_id = msg[0];
	int handled;
	uint32_t value;

	msg++;
	len--;
	switch (item_id) {
	case '1':	// switch
	case '2':	// button
		// get a byte, store it
		if (len != 2)
			return E_LENGTH;
		else {
			handled = from_hex(msg, &value, 2);
			if (handled) {
				if (item_id == '1') {
					state_switch = value & 0xff;
				} else {
					state_button = value & 0xff;
				}
			} else
				return E_DECODE;	// decoding
		}
		break;

	default:
		if (('a' <= item_id) && (item_id < 'a' + PL_NUMBER_ADCS)) {
			// get an analog value, store it
			if (len != 4)
				return E_LENGTH;	// length
			else {
				uint8_t channel = item_id - 'a';
				handled = from_hex(msg, &value, 4);
				if (handled) {
					state_adc[channel] = value & 0xffff;
				} else
					return E_DECODE;
			}
		} else
			return E_UNKNOWN_ITEM;
	}
	return E_NONE;
}

static enum error_codes incoming_setter_screen_unnamed(char item_id, char *msg,
		int len) {
	uint32_t value;

	switch (item_id) {
	case 'T':
		// get and process time string which is yyyymmddhhMMss
		time_stamp_t ts;	// store incoming time stamp

		if (len != 14) {
			return E_LENGTH;
			break;
		}

		if (!from_dec(msg, &value, 4)) {
			return E_DECODE;
			break;
		}
		if ((value < 2000) || (value > 3000)) {
			return E_VALUE;
			break;
		}
		ts.year = value;
		msg += 4;

		if (!from_dec(msg, &value, 2)) {
			return E_DECODE;
			break;
		}
		if ((value < 1) || (value > 12)) {
			return E_VALUE;
			break;
		}
		ts.month = value;
		msg += 2;

		if (!from_dec(msg, &value, 2)) {
			return E_DECODE;
			break;
		}
		if ((value < 1) || (value > 31)) {
			return E_VALUE;
			break;
		}
		ts.day = value;
		msg += 2;

		if (!from_dec(msg, &value, 2)) {
			return E_DECODE;
			break;
		}
		if ((value < 1) || (value > 23)) {
			return E_VALUE;
			break;
		}
		ts.hour = value;
		msg += 2;

		if (!from_dec(msg, &value, 2)) {
			return E_DECODE;
			break;
		}
		if ((value < 1) || (value > 60)) {
			return E_VALUE;
			break;
		}
		ts.min = value;
		msg += 2;

		if (!from_dec(msg, &value, 2)) {
			return E_DECODE;
			break;
		}
		if ((value < 1) || (value > 60)) {
			return E_VALUE;
			break;
		}
		ts.sec = value;
		msg += 2;
		time_current = ts;
		break;
	case 'V':
		// version information, for future use
		// nothing to do at the moment
		break;
	default:
		// invalid item
		return E_UNKNOWN_ITEM;
	}
	return E_NONE;
}

static enum error_codes incoming_setter(char *msg, int len) {
	// we got a set-packet from PC
	char screen_item_id;
	uint8_t error = E_NONE;

	if (len < 1) {
		error = E_LENGTH;	// too short, silently ignore this packet
	} else {
		screen_item_id = msg[0];
		msg++;
		len--;
		// msg now points to the data after the screen_item_id
		if (('0' <= screen_item_id) && (screen_item_id <= '9')) {
			if (screen_item_id == '0') {
				// screen 0: only button and switch to receive here
				error = incoming_setter_screen0(msg, len);
			} else {
				error = E_UNKNOWN_SCREEN;
			}
		} else {
			// screen_item_id is an id from the unnamed screen
			error = incoming_setter_screen_unnamed(screen_item_id, msg, len);
		}
	}
	return error;
}

static void incoming_from_visu(char *msg) {
	enum error_codes error = E_NONE;
	char c = msg[0];
	char *whole_msg = msg;
	int_fast16_t len = strlen(msg);

	if (c == 'd') {
		error = incoming_setter(msg + 1, len - 1);// string to parse, len of string
	} else if (c == '?') {	// request
		error = incoming_request(msg + 1, len - 1);
	} else if (c == 'e') {
		incoming_error(msg + 1, len - 1);	// string, len
	} else {
		// currently no other PDUs allowed
		// ignore other PDUs
		pl_log("Unknown PDU");
	}
	if (error != E_NONE) {
		char answer[] = "exx";
		encode8(error, answer + 1);
		send_string(answer, whole_msg, 1);
	}
}

static int days_of_month(int year, int month) {
	if (month == 2) {
		if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
			return 29;
		} else {
			return 28;
		}
	}

	if ((month == 4) || (month == 6) || (month == 9) || (month == 11)) {
		return 30;
	}
	return 31;
}

static void handle_1sec() {
	if (++time_current.sec == 60) {
		time_current.sec = 0;
		if (++time_current.min == 60) {
			time_current.min = 0;
			if (++time_current.hour == 24) {
				time_current.hour = 0;
				if (++time_current.day
						> days_of_month(time_current.month,
								time_current.year)) {
					time_current.day = 1;
					if (++time_current.month >= 12) {
						time_current.month = 1;
						time_current.year++;
					}
				}
			}
		}
	}
}

/*
 * global functions
 */
void pl_init() {
	pli_board_init();
	pli_serial_init(PL_BAUD);
	pl_screen_set(0);	// set default screen and announce this to PC
	send_string(0, "dS0\n?T\nd0000\n?01\n?02\n?0a\n?0b\n", 0); // request initial state + date and time
	state = 1;
}

int pl_screen_set(uint8_t screen_p) {
	if (screen_p == screen)
		return 0;
	// check
	if (!((0 <= screen_p) && (screen_p <= 9))) {
		return -1;
	}

	screen = screen_p;
	send_screen();
	return 1;
}

int pl_alarmclock_display(uint32_t display) {
	static uint32_t old_display = 0;
	char packet[9];

	if (display == old_display)
		return 0;
	old_display = display;
	encode32(display, packet);
	packet[8] = 0;
	send_string("d1", packet, 1);
	return 1;
}

int pl_seesaw_display(float reference, float position, float angle,
		int boing_state) {
	int16_t data;
	char packet[14];
	char *destination = packet;

	data = reference * 50000;
	encode16((uint16_t) data, destination);
	destination += 4;

	data = position * 50000;
	encode16((uint16_t) data, destination);
	destination += 4;

	data = angle * 2000;
	encode16((uint16_t) data, destination);
	destination += 4;

	*destination++ = boing_state ? 't' : 'f';

	*destination = 0;

	send_string("d2", packet, 1);
	return 1;
}

static void send_led() {
	char packet[] = "xx";
	encode8(state_led, packet);
	send_string("d00", packet, 1);
}

int pl_led_set(uint8_t leds) {
	if (state_led == leds)
		return 1;
	state_led = leds;
	send_led();
	return 1;
}

void pl_do() {
	static char message[MAX_READ_FROM_VISU + 1];
	static uint_fast8_t message_pos = 0;
	uint8_t data;

	if (pli_serial_read(&data)) {
		// got something to work on
		if ((data == '\n') || (data == '\r')) {
			message[message_pos] = 0;
			message_pos = 0;
			incoming_from_visu(message);
		} else {
			message[message_pos++] = data;
			if (message_pos == MAX_READ_FROM_VISU) {
				message[message_pos] = 0;
				// silently ignore this almost endless PDU
			}
		}
	}
}

void pl_tick() {
	static systick_t divider_1sec = 0;
	if (++divider_1sec >= PL_TICKS_PER_SECOND) {
		divider_1sec -= PL_TICKS_PER_SECOND;
		handle_1sec();
	}
}

// get state of the switches
int pl_switch_get(uint8_t *switches) {
	*switches = state_switch;
	return 1;
}

// get state of the buttons
int pl_button_get(uint8_t *buttons) {
	*buttons = state_button;
	return 1;
}

// get value of an virtual adc
int pl_adc_get(uint8_t channel, uint16_t *value) {
	int v;
	v = random() * 7.0 - 3.5;
	if (channel < PL_NUMBER_ADCS) {
		v += state_adc[channel];
		if (v < 0)
			v = 0;
		if (v > 1023)
			v = 1023;
		*value = v;
		return 1;
	}
	return 0;
}

// writes a log message to the visualization
int pl_log(char *message) {
	if (!state)
		return 0;
	send_string("dL", message, 1);
	return 1;
}

int pl_log_debug(char *message) {
	if (!state)
		return 0;
	send_string("dD", message, 1);
	return 1;
}

void pl_get_hms(int *h, int *m, int *s) {
	*h = time_current.hour;
	*m = time_current.min;
	*s = time_current.sec;
}

