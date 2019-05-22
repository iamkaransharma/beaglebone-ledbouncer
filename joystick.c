#include <stdint.h>
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"
#include "watchdog.h"
#include "uart_irda_cir.h"
#include "hw_cm_per.h"
#include "consoleUtils.h"

#include "leds.h"

#define LEFT_BUTTON_GPIO_BASE     (SOC_GPIO_2_REGS)
#define LEFT_BUTTON_PIN           (1)

#define UP_BUTTON_GPIO_BASE       (SOC_GPIO_0_REGS)
#define UP_BUTTON_PIN             (26)

#define DOWN_BUTTON_GPIO_BASE     (SOC_GPIO_1_REGS)
#define DOWN_BUTTON_PIN           (14)

#define LED_SPEED_STEP_SIZE 1

static _Bool lastLeftButtonState = false;
static _Bool lastUpButtonState = false;
static _Bool lastDownButtonState = false;

static _Bool readLeftButton(void);
static _Bool readUpButton(void);
static _Bool readDownButton(void);
static void GPIO2ModuleClkConfig(void);

void Joystick_init(void)
{
	// NOTE: Since GPIO1 is already prepared by the leds module,
	//       we should not configure it again. This assumes the
	//       leds module is initialized before the joystick module.
	GPIODirModeSet(DOWN_BUTTON_GPIO_BASE,
	               DOWN_BUTTON_PIN,
	               GPIO_DIR_INPUT);

	GPIO0ModuleClkConfig();
	GPIOModuleEnable(UP_BUTTON_GPIO_BASE);
	GPIOModuleReset(UP_BUTTON_GPIO_BASE);
	GPIODirModeSet(UP_BUTTON_GPIO_BASE,
	               UP_BUTTON_PIN,
	               GPIO_DIR_INPUT);

	GPIO2ModuleClkConfig();
	GPIOModuleEnable(LEFT_BUTTON_GPIO_BASE);
	GPIOModuleReset(LEFT_BUTTON_GPIO_BASE);
	GPIODirModeSet(LEFT_BUTTON_GPIO_BASE,
	               LEFT_BUTTON_PIN,
	               GPIO_DIR_INPUT);
}

void Joystick_doBackgroundWork(void)
{
	_Bool isLeftButtonPressed = readLeftButton();
	_Bool isUpButtonPressed = readUpButton();
	_Bool isDownButtonPressed = readDownButton();

	if (lastLeftButtonState != isLeftButtonPressed) {
		if (lastLeftButtonState && !isLeftButtonPressed) {
			Leds_toggleMode();
		}
		lastLeftButtonState = isLeftButtonPressed;
	} else if (lastUpButtonState != isUpButtonPressed) {
		if (lastUpButtonState && !isUpButtonPressed) {
			Leds_increaseSpeed(LED_SPEED_STEP_SIZE);
		}
		lastUpButtonState = isUpButtonPressed;
	} else if (lastDownButtonState != isDownButtonPressed) {
		if (lastDownButtonState && !isDownButtonPressed) {
			Leds_decreaseSpeed(LED_SPEED_STEP_SIZE);
		}
		lastDownButtonState = isDownButtonPressed;
	}
}

static _Bool readLeftButton(void)
{
	return GPIOPinRead(LEFT_BUTTON_GPIO_BASE, LEFT_BUTTON_PIN) == 0;
}

static _Bool readUpButton(void)
{
	return GPIOPinRead(UP_BUTTON_GPIO_BASE, UP_BUTTON_PIN) == 0;
}

static _Bool readDownButton(void)
{
	return GPIOPinRead(DOWN_BUTTON_GPIO_BASE, DOWN_BUTTON_PIN) == 0;
}

static void GPIO2ModuleClkConfig(void)
{
	HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
	    CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE;

	while (CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE !=
	        (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
	         CM_PER_GPIO2_CLKCTRL_MODULEMODE));

	HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
	    CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK;

	while (CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK !=
	        (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
	         CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK));

	while ((CM_PER_GPIO2_CLKCTRL_IDLEST_FUNC <<
	        CM_PER_GPIO2_CLKCTRL_IDLEST_SHIFT) !=
	        (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
	         CM_PER_GPIO2_CLKCTRL_IDLEST));

	while (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK !=
	        (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
	         CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK));
}