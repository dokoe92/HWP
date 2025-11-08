/*
 * plib.h
 *
 * This is the interface of the peripheral library.
 * Its main purpose is to simulate non-existing peripherals via a connection to another computer. This
 * version of plib uses a serial connection to a pc. On the pc, a fitting software must visualize
 * and calculate those peripherals. 
 *
 * We expect several peripheral devices such as LEDs and switches. In addition, we also expect
 * some more complex devices for special exercises, like a seesaw or an alarm clock. Devices are
 * grouped into "screens". A screen represents a "use case" for a specific exercise which needs
 * a certain bundle of items.
 *
 * How to use the plib:
 *
 * The app must call pl_init() first. This sets up the board, the communication channel to the PC
 * and selects the default screen "0".
 *
 * In the super loop, pl_do() needs to be called to process the messages sent from the virtual peripheral.
 *
 * Specific screens can be selected with plib_screen() which can be called multiple times.
 * At every call, the PC side (re-)initializes the application and the desired
 * screen.
 */

#ifndef PLIB_H_
#define PLIB_H_

#include <stdint.h>

/*
 * Initialize the board (real hardware) and set up communication to the
 * virtual peripheral, if available.
 * Needs to be called at application start.
 *
 */
void pl_init(void);

/**
 * Processes messages sent from the virtual peripheral.
 * Needs to be called in the super loop, otherwise communication with the
 * virtual peripheral won't work.
 */
void pl_do();

/*
 * Switches the user led on the real hardware on (0) or off (1)
 */
void pl_board_led_set(uint8_t onoff);

/*
 * Returns if the user button (on the real hardware) is pressed (0) or not pressed (1)
 *
 */
int pl_board_button_get(void);


/*
 * Switches on a LED on the virtual peripheral.
 * Each of the bits in LEDs corresponds to a single LED, where 0 means off and 1 means switched on.
 */
int pl_led_set(uint8_t leds);

/*
 * Queries the states of the switches on the virtual peripheral.
 * Each bit in switches corresponds to one switch, where 0 means switched off and 1 means switched on.
 */
int pl_switch_get(uint8_t *switches);

/*
 * Get the state of the virtual buttons.
 * There are 4 buttons on the virtual peripheral, and the first 4 bits in buttons
 * correspondent to these buttons. A set bit is a pressed button, 0 means not pressed.
 *
 */
int pl_button_get(uint8_t *buttons);

// get value of an adc channel
int pl_adc_get(uint8_t channel, uint16_t* value);

/*
 * Writes a log message to the visualization of the virtual peripheral.
 *
 */
int pl_log(char* message);

/*
 * Writes a debug message to the visualization of the virtual peripheral.
 * Note that this message will only be displayed if started with the command line option -d
 */
int pl_log_debug(char* message);

/*
 * Open a visualization view (screen) on the virtual peripheral.
 * The following values are valid for screen:
 * - 0: default view
 * - 1: alarm clock
 * - 2: seesaw
 */
int pl_screen_set(uint8_t screen);

/*
 * Send data to display on alarm clock screen.
 */
int pl_alarmclock_display(uint32_t display);

/*
 * Send data to display on the seesaw screen.
 *
 */
int pl_seesaw_display(float reference, float position, float angle, int boing_state);

/*
 * Returns the current time by setting the h/m/s parameters.
 * Prerequisite: pl_tick() is used. See function description.
 */
void pl_get_hms(int *h, int *m, int *s);

/*
 * Needs to be called every tick. When doing so, keeps the clock (see pl_get_hms() function) up-to-date.
 * You need to make sure that this function is called exactly every tick, otherwise the time will drift.
 *
 */
void pl_tick();

#endif
