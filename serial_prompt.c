#include <stdbool.h>
#include <ctype.h>
#include "hw_types.h"
#include "consoleUtils.h"

#include "serial.h"
#include "serial_prompt.h"

static promptActions_t s_actions;
static volatile uint8_t s_rxByte = 0;

static void serialRxIsrCallback(uint8_t rxByte);

void SerialPrompt_init(promptActions_t actions)
{
	Serial_init();
	Serial_setRxIsrCallback(serialRxIsrCallback);
	s_actions = actions;
}

static void serialRxIsrCallback(uint8_t rxByte)
{
	s_rxByte = rxByte;
}

void SerialPrompt_doBackgroundWork(void)
{
	if (s_rxByte != 0) {
		uint8_t normalizedCommand = tolower(s_rxByte);
		if (normalizedCommand == '?') {
			s_actions.helpAction();
		} else if (normalizedCommand == 'a') {
			s_actions.aAction();
		} else if (normalizedCommand == 'b') {
			s_actions.bAction();
		} else if (normalizedCommand == 'x') {
			s_actions.xAction();
		} else if ('0' <= normalizedCommand && normalizedCommand <= '9') {
			s_actions.numberAction(normalizedCommand - '0');
		} else {
			s_actions.errorAction();
		}
		s_rxByte = 0;
	}
}

