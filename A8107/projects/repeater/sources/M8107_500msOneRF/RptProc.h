#ifndef _RPT_PROC_H_
#define _RPT_PROC_H_

#include <stdbool.h>
#include <stdint.h>

#ifndef REPEATER_500MS_MODE
#define REPEATER_500MS_MODE
#endif //REPEATER_500MS_MODE

#ifndef M8107RF_MAIN_MODE
#define M8107RF_MAIN_MODE
#endif //M8017RF_MAIN_MODE

#ifndef M8107RF_ONLY_ONE
#define M8107RF_ONLY_ONE
#endif //M8107RF_ONLY_ONE

void Rpt_Process(void);

#endif /* _RPT_PROC_H_ */
