// The leds module initializes the 4 LEDs on the BeagleBone and handles the
// display of 2 patterns (bounce and bar), taking into account the pattern's speed

#ifndef _LEDS_H_
#define _LEDS_H_

#define LEDS_MIN_SPEED 0
#define LEDS_MAX_SPEED 9

typedef enum {MODE_BOUNCE, MODE_BAR} ledMode_t;

void Leds_init(void);
void Leds_doBackgroundWork(void);
void Leds_notifyOnTimeIsr(void);

void Leds_setSpeed(uint8_t newSpeed);
uint8_t Leds_getSpeed(void);

void Leds_setMode(ledMode_t mode);
ledMode_t Leds_getMode(void);

void Leds_toggleMode(void);
void Leds_increaseSpeed(uint8_t stepSize);
void Leds_decreaseSpeed(uint8_t stepSize);

#endif
