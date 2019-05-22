// The watchdog module is a hardware abstraction module of the BeagleBone's
// Watchdog timer, which offers a fail-safe for the user in case the application
// becomes unresponsive.

#ifndef _WDTIMER_H_
#define _WDTIMER_H_

void Watchdog_init(void);
void Watchdog_hit(void);

#endif
