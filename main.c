#include "consoleUtils.h"
#include <stdint.h>
#include <stdbool.h>

#include "serial.h"
#include "timer.h"
#include "wdtimer.h"
#include "serial_prompt.h"
#include "reset_register.h"
#include "leds.h"
#include "joystick.h"

static _Bool s_shouldHitWatchdog = true;

static void printHelp(void);
static void handleInvalidInput(void);
static void handleActionA(void);
static void handleActionB(void);
static void handleNumber(uint8_t value);
static void stopHittingWatchdog(void);

int main(void)
{
	promptActions_t actions = {
		.helpAction = printHelp,
		.numberAction = handleNumber,
		.aAction = handleActionA,
		.bAction = handleActionB,
		.xAction = stopHittingWatchdog,
		.errorAction = handleInvalidInput,
	};
	SerialPrompt_init(actions);

	Timer_init();
	Timer_setTimerIsrCallback(Leds_notifyOnTimeIsr);

	Leds_init();
	Joystick_init();
	Watchdog_init();

	ConsoleUtilsPrintf("\nLightBouncer:\n"
	                   "      by Karan Sharma\n"
	                   "---------------------\n");
	ResetRegister_printSummary();
	printHelp();

	while (1) {
		SerialPrompt_doBackgroundWork();
		Leds_doBackgroundWork();
		Joystick_doBackgroundWork();
		if (Timer_isIsrFlagSet()) {
			Timer_clearIsrFlag();
			if (s_shouldHitWatchdog) {
				Watchdog_hit();
			}
		}
	}
}

static void handleNumber(uint8_t value)
{
	ConsoleUtilsPrintf("\nSetting LED speed to %d\n", value);
	Leds_setSpeed(value);
}

static void handleActionA(void)
{
	ConsoleUtilsPrintf("\nChanging to bounce mode.\n");
	Leds_setMode(MODE_BOUNCE);
}

static void handleActionB(void)
{
	ConsoleUtilsPrintf("\nChanging to bar mode.\n");
	Leds_setMode(MODE_BAR);
}

static void stopHittingWatchdog(void)
{
	ConsoleUtilsPrintf("\nNo longer hitting the watchdog.\n");
	s_shouldHitWatchdog = false;
}

static void printHelp(void)
{
	ConsoleUtilsPrintf("\n\nKeyboard Commands:\n"
	                   "  ? : Display this help message.\n"
	                   "  0-9 : Set speed 0 (slow) to 9 (fast).\n"
	                   "  a : Select pattern A (bounce).\n"
	                   "  b : Select pattern B (bar).\n"
	                   "  x : Stop hitting the watchdog.\n"
	                   "Joystick Commands:\n"
	                   "  LEFT-BTN : Push-button to toggle mode.\n"
	                   "  UP-BTN   : Push-button to increase speed by 1.\n"
	                   "  DOWN-BTN : Push-button to decrease speed by 1.\n");
}

static void handleInvalidInput(void)
{
	ConsoleUtilsPrintf("\nInvalid command.\n");
	printHelp();
}