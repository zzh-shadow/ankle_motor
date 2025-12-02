#include "dwt.h"

#define  DWT_CR             (*(volatile uint32_t *)0xE0001000)
#define  DWT_CYCCNT         (*(volatile uint32_t *)0xE0001004)
#define  DWT_LAR	        (*(volatile uint32_t *)0xE0001FB0)
#define  DWT_LAR_UNLOCK	    (uint32_t)0xC5ACCE55


#define  DEM_CR             (*(volatile uint32_t *)0xE000EDFC)
#define  DEM_CR_TRCENA      (1 << 24)
#define  DWT_CR_CYCCNTENA   (1 <<  0)

void DWT_Tick_TimerInit(void)
{
    if(!((DEM_CR & DEM_CR_TRCENA) && DWT_CR & DWT_CR_CYCCNTENA))
    {
        DEM_CR         |=  DEM_CR_TRCENA; 
		DWT_LAR        |=  DWT_LAR_UNLOCK;		
        DWT_CR         |=  DWT_CR_CYCCNTENA;  
        DWT_CYCCNT      = 0u;               
    }    
}

void DWT_Tick_TimerClear(void)
{  
    DWT_CYCCNT = 0u;
}

uint32_t DWT_Tick_TimerGet(void)
{
    return DWT_CYCCNT;
}

#pragma pack(4)
int64_t SysTime=0,SysTime_N=0;
int64_t Tick_Time_last;
int32_t Tick_Time_Star=1;
float DWT_Time_Run(void)
{
    int64_t Tick_Time=DWT_Tick_TimerGet();
    if(Tick_Time_Star)
    {
		Tick_Time_Star = 0;
		Tick_Time_last = Tick_Time;
    }
    if((Tick_Time_last-Tick_Time)>0)
    {
        SysTime_N=SysTime_N+4294967296;
    }
    SysTime=SysTime_N+Tick_Time;
    Tick_Time_last=Tick_Time;
    return 	(double)SysTime / 80000000.0f;
}
#pragma pack()

