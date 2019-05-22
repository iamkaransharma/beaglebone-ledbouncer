// The serial_prompt module manages the command prompt & allows various callbacks
// to be registered; one for each command or error action that may occur

#ifndef _SERIALPROMPT_H_
#define _SERIALPROMPT_H_

#include <stdint.h>

typedef struct {
	void (*helpAction)(void);
	void (*numberAction)(uint8_t);
	void (*aAction)(void);
	void (*bAction)(void);
	void (*xAction)(void);
	void (*errorAction)(void);
} promptActions_t;

void SerialPrompt_init(promptActions_t actions);
void SerialPrompt_doBackgroundWork(void);

#endif
