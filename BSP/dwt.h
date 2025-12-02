#ifndef __DWT_H__
#define __DWT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void DWT_Tick_TimerInit(void);
float DWT_Time_Run(void);



#ifdef __cplusplus
}
#endif
#endif  //__DWT_H__