#include <stdint.h>
#include <stdbool.h>
#include "hw_types.h"
#include "consoleUtils.h"

#define RESET_REG_BASE_ADDR (0x44E00F00)
#define RESET_REG_OFFSET (0x8)
#define PRM_RSTST (HWREG(RESET_REG_BASE_ADDR + RESET_REG_OFFSET))

#define BIT_GLOBAL_COLD_RESET 0
#define BIT_GLOBAL_WARM_RESET 1
#define BIT_WATCHDOG_RESET 4
#define BIT_EXTERNAL_WARM_RESET 5
#define BIT_ICEPICK_RESET 9

#define MASK_OF(BIT_NUMBER) (1 << BIT_NUMBER)

static uint32_t readResetSource(uint32_t bitNumber)
{
	return PRM_RSTST & MASK_OF(bitNumber);
}

static void clearResetSource(uint32_t bitNumber)
{
	PRM_RSTST |= MASK_OF(bitNumber);
}

static _Bool isFlagSet(uint32_t bitNumber)
{
	return (readResetSource(bitNumber) != 0);
}

static void clearAllResetSources()
{
	clearResetSource(BIT_GLOBAL_COLD_RESET);
	clearResetSource(BIT_GLOBAL_WARM_RESET);
	clearResetSource(BIT_WATCHDOG_RESET);
	clearResetSource(BIT_EXTERNAL_WARM_RESET);
	clearResetSource(BIT_ICEPICK_RESET);
}

void ResetRegister_printSummary(void)
{
	ConsoleUtilsPrintf("Reset source (0x%x) = ", PRM_RSTST);
	if (isFlagSet(BIT_GLOBAL_COLD_RESET)) {
		ConsoleUtilsPrintf("Cold Reset, ");
	}
	if (isFlagSet(BIT_GLOBAL_WARM_RESET)) {
		ConsoleUtilsPrintf("Warm Reset, ");
	}
	if (isFlagSet(BIT_WATCHDOG_RESET)) {
		ConsoleUtilsPrintf("Watchdog Reset, ");
	}
	if (isFlagSet(BIT_EXTERNAL_WARM_RESET)) {
		ConsoleUtilsPrintf("External Reset, ");
	}
	if (isFlagSet(BIT_ICEPICK_RESET)) {
		ConsoleUtilsPrintf("IcePick Reset");
	}
	ConsoleUtilsPrintf("\n");

	clearAllResetSources();
}
