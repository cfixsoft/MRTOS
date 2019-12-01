/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: arch_c.c
*Version    : V1.00
*Description: cpu architecture porting related C code.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/26
*******************************************************************************/
#include <KER_Internal.h>
extern INT32U SYSTICK_u32CurrentCounterGet(void);

/*******************************************************************************
* @Brief  : get CPU Load calculation timer current counter.
* @Inputs : none.
* @Outputs: none.
* @Returns: timer current counter.
* @Note   : none.
*******************************************************************************/
PUBLIC INT32U KER_u32CPULoadTimerCounterGet( void )
{
    return SYSTICK_u32CurrentCounterGet() >> KER_u8CPULOAD_CNT_RSH_DIV;
}

/*******************************************************************************
* @Brief  : Initial task stack
* @Inputs : pstkStack: before initial stack top pointer.
*           pfEntry: task entry pointer.
*           vidpParam: task entry parameter pointer.
* @Outputs: none
* @Returns: after initial stack top pointer.
* @Note   : only called by create task function.
*******************************************************************************/
PUBLIC CPU_STK *KER_pstkInitTaskStack ( CPU_STK    *pstkStack,
                                        void      (*pfnEntry)(void *vidpParam),
                                        void       *vidpParam )
{
    /* !Comment: xPSR                                                         */
    *(--pstkStack) = (CPU_STK)0x01000000;
    /* !Comment: PC-->Task Entry                                              */
    *(--pstkStack) = (CPU_STK)pfnEntry;
    /* !Comment: LR-->Task Exit                                               */
    *(--pstkStack) = (CPU_STK)KER_vidTaskDefaultExit;
    /* !Comment: R12                                                          */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R3                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R2                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R1                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R0                                                           */
    *(--pstkStack) = (CPU_STK)vidpParam;

    /* !Comment: R11                                                          */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R10                                                          */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R9                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R8                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R7                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R6                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R5                                                           */
    *(--pstkStack) = (CPU_STK)0;
    /* !Comment: R4                                                           */
    *(--pstkStack) = (CPU_STK)0;

    return pstkStack;
}