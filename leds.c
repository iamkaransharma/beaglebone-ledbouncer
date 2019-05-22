#include <stdint.h>
#include <stdbool.h>
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"

#include "leds.h"

#define LED_GPIO_BASE (SOC_GPIO_1_REGS)
#define LED0_PIN (21)
#define LED1_PIN (22)
#define LED2_PIN (23)
#define LED3_PIN (24)

#define NUM_LED_MASKS 4
#define MASK_OF(BIT_NUMBER) (1 << BIT_NUMBER)
#define LED_MASK (MASK_OF(LED0_PIN) | MASK_OF(LED1_PIN) | MASK_OF(LED2_PIN) | MASK_OF(LED3_PIN))

#define LEDS_DEFAULT_SPEED 5

#define BASE_TIME_UNIT_MS 10

static volatile _Bool s_isTimeToUpdateState = true;
static uint8_t s_speed = LEDS_DEFAULT_SPEED;
static ledMode_t s_mode = MODE_BOUNCE;

static uint32_t elapsedTime = 0;
static uint32_t targetTime = 0;

static const uint32_t barLedMasks[NUM_LED_MASKS] = {
	(MASK_OF(LED0_PIN)),
	(MASK_OF(LED0_PIN) | MASK_OF(LED1_PIN)),
	(MASK_OF(LED0_PIN) | MASK_OF(LED1_PIN) | MASK_OF(LED2_PIN)),
	(MASK_OF(LED0_PIN) | MASK_OF(LED1_PIN) | MASK_OF(LED2_PIN) | MASK_OF(LED3_PIN)),
};
static const uint32_t bounceLedMasks[NUM_LED_MASKS] = {
	(MASK_OF(LED0_PIN)),
	(MASK_OF(LED1_PIN)),
	(MASK_OF(LED2_PIN)),
	(MASK_OF(LED3_PIN)),
};
static enum {LEFT, RIGHT} patternDirection;
static uint8_t currentLedMask = 0;

static void updateLedPattern(void);
static uint32_t getFlashTimePeriod(void);

void Leds_init(void)
{
	GPIO1ModuleClkConfig();
	GPIOModuleEnable(LED_GPIO_BASE);
	GPIOModuleReset(LED_GPIO_BASE);
	GPIODirModeSet(LED_GPIO_BASE,
	               LED0_PIN,
	               GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
	               LED1_PIN,
	               GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
	               LED2_PIN,
	               GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
	               LED3_PIN,
	               GPIO_DIR_OUTPUT);
}

void Leds_setSpeed(uint8_t newSpeed)
{
	s_speed = newSpeed;
}

uint8_t Leds_getSpeed(void)
{
	return s_speed;
}

void Leds_setMode(ledMode_t mode)
{
	s_mode = mode;
}

ledMode_t Leds_getMode(void)
{
	return s_mode;
}

void Leds_doBackgroundWork(void)
{
	if (s_isTimeToUpdateState) {
		s_isTimeToUpdateState = false;

		targetTime = getFlashTimePeriod();
		elapsedTime += BASE_TIME_UNIT_MS;
		if (elapsedTime >= targetTime) {
			// Trigger next state change & reset elapsed time
			updateLedPattern();
			elapsedTime = BASE_TIME_UNIT_MS;
		} else {
			// do nothing
		}
	}
}

void Leds_notifyOnTimeIsr(void)
{
	s_isTimeToUpdateState = true;
}

void Leds_toggleMode(void)
{
	if (s_mode == MODE_BOUNCE) {
		s_mode = MODE_BAR;
	} else if (s_mode == MODE_BAR) {
		s_mode = MODE_BOUNCE;
	} else {
		// not possible
	}
}

void Leds_increaseSpeed(uint8_t stepSize)
{
	if ((s_speed + (int)stepSize) < LEDS_MAX_SPEED) {
		s_speed += stepSize;
	} else {
		s_speed = LEDS_MAX_SPEED;
	}
}

void Leds_decreaseSpeed(uint8_t stepSize)
{
	if ((s_speed - (int)stepSize) > LEDS_MIN_SPEED) {
		s_speed -= stepSize;
	} else {
		s_speed = LEDS_MIN_SPEED;
	}
}

static void updateLedPattern(void)
{
	if (currentLedMask == 0) {
		currentLedMask++;
		patternDirection = RIGHT;
	} else if (currentLedMask == NUM_LED_MASKS - 1) {
		currentLedMask--;
		patternDirection = LEFT;
	} else {
		if (patternDirection == RIGHT) {
			currentLedMask++;
		} else {
			currentLedMask--;
		}
	}

	uint32_t nextLedPattern = 0;
	if (s_mode == MODE_BOUNCE) {
		nextLedPattern = bounceLedMasks[currentLedMask];
	} else {
		nextLedPattern = barLedMasks[currentLedMask];
	}

	// Refresh LEDs
	HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = LED_MASK;
	HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = nextLedPattern;
}

static uint32_t getFlashTimePeriod(void)
{
	// flash-time = base-time * (2^(max-speed - current-speed))
	return BASE_TIME_UNIT_MS * (1 << (LEDS_MAX_SPEED - s_speed));
}