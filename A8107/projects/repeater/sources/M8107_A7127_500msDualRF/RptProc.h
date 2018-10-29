#ifndef _RPT_PROC_H_
#define _RPT_PROC_H_

#include <stdbool.h>
#include <stdint.h>

#ifndef REPEATER_500MS_MODE
#define REPEATER_500MS_MODE
#endif //REPEATER_500MS_MODE

#ifndef USE_A7127RF_MODULE
#define USE_A7127RF_MODULE
#endif //USE_A7127RF_MODULE

#ifndef M8107RF_MAIN_MODE
#define M8107RF_MAIN_MODE
#endif //M8107RF_MAIN_MODE

void Rpt_GlobalVar_Init(void);
void Rpt_Process(void);

#endif /* _RPT_PROC_H_ */
