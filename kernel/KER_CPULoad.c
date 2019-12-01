/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_CPULoad.c
*Version    : V1.00
*Description: cpu load calculation module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2016/3/6
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coCPULOAD_MODULE_EN > 0u
/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
PRIVATE void vidCPULoadCalc( void );

/*******************************************************************************
* @Brief  : calculate CPU Load
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PRIVATE void vidCPULoadCalc( void )
{
    INT32U u32TotalCnt;

    u32TotalCnt = KER_u32CPUUsingCnt + KER_u32CPUISRCnt + KER_u32CPUIdleCnt;

    KER_u32CPULoadISR = (INT32U)(KER_u32CPUISRCnt * (INT32U)1000 / u32TotalCnt);

    KER_u32CPULoad = (INT32U)
         ((KER_u32CPUUsingCnt + KER_u32CPUISRCnt) * (INT32U)1000 / u32TotalCnt);
}

/*******************************************************************************
* @Brief  : get CPULoad value.
* @Inputs : none.
* @Outputs: none.
* @Returns: CPULoad: Unit:0.1%
* @Note   : none.
*******************************************************************************/
PUBLIC INT32U KER_u32CPULoadGet( void )
{
    return KER_u32CPULoad;
}

/*******************************************************************************
* @Brief  : get ISR CPULoad value.
* @Inputs : none.
* @Outputs: none.
* @Returns: CPULoad: Unit:0.1%
* @Note   : none.
*******************************************************************************/
PUBLIC INT32U KER_u32CPULoadISRGet( void )
{
    return KER_u32CPULoadISR;
}

/*******************************************************************************
* @Brief  : update CPU using related counter.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : Only could be called by Task context switch procedure.
*******************************************************************************/
PUBLIC void KER_vidCPULoadPerformance( void )
{
    INT32U u32Cnt;

    u32Cnt = KER_u32CPULoadTimerCounterGet();

    if (KER_pstrCurTCB != &KER_strIdleTaskTcb)
    {
#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

        if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
        {
            KER_u32CPUUsingCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
        }
        else
        {
            KER_u32CPUUsingCnt += KER_u32CPULoadTimerLastCnt
                                 + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
        }

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

        if (KER_u32CPULoadTimerLastCnt <= u32Cnt)
        {
            KER_u32CPUUsingCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
        }
        else
        {
            KER_u32CPUUsingCnt += u32Cnt
               + (KER_u32CPULOAD_TIM_EXPIRED_VAL - KER_u32CPULoadTimerLastCnt);
        }

#else
        #error "Please configurate CPULoad timer counter direction!!!"
#endif
    }
    else
    {
#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE
        if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
        }
        else
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt
                                 + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
        }
#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE
        KER_u32CPUIdleCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
#else
        #error "Please configurate CPULoad timer counter direction!!!"
#endif

#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

        if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
        }
        else
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt
                                 + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
        }

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

        if (KER_u32CPULoadTimerLastCnt <= u32Cnt)
        {
            KER_u32CPUIdleCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
        }
        else
        {
            KER_u32CPUIdleCnt += u32Cnt
               + (KER_u32CPULOAD_TIM_EXPIRED_VAL - KER_u32CPULoadTimerLastCnt);
        }

#else
        #error "Please configurate CPULoad timer counter direction!!!"
#endif

    }

    /*!Pseudo: record current timer counter                                   */
    KER_u32CPULoadTimerLastCnt = u32Cnt;
}

/*******************************************************************************
* @Brief  : update CPU load related counter at ISR entry.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : should only called by category 1 interrupt service routine.
*******************************************************************************/
PUBLIC void KER_vidCPULoadISRStart( void )
{
    INT32U u32Cnt;
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();

    u32Cnt = KER_u32CPULoadTimerCounterGet();

    KER_u8CPULoadIntrNestedCnt++;

    if (KER_u8CPULoadIntrNestedCnt > (INT8U)1)
    {
#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

        if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
        {
            KER_u32CPUISRCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
        }
        else
        {
            KER_u32CPUISRCnt += KER_u32CPULoadTimerLastCnt
                            + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
        }

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

        if (KER_u32CPULoadTimerLastCnt <= u32Cnt)
        {
            KER_u32CPUISRCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
        }
        else
        {
            KER_u32CPUISRCnt += u32Cnt
               + (KER_u32CPULOAD_TIM_EXPIRED_VAL - KER_u32CPULoadTimerLastCnt);
        }

#else
    #error "Please configurate CPULoad timer counter direction!!!"
#endif

        /*!Pseudo: record current timer counter                               */
        KER_u32CPULoadTimerLastCnt = u32Cnt;

        KER_LEAVE_CRITICAL();
        return;
    }
    else if (KER_pstrCurTCB != &KER_strIdleTaskTcb)
    {
#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

        if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
        {
            KER_u32CPUUsingCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
        }
        else
        {
            KER_u32CPUUsingCnt += KER_u32CPULoadTimerLastCnt
                                 + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
        }

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

        if (KER_u32CPULoadTimerLastCnt <= u32Cnt)
        {
            KER_u32CPUUsingCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
        }
        else
        {
            KER_u32CPUUsingCnt += u32Cnt
               + (KER_u32CPULOAD_TIM_EXPIRED_VAL - KER_u32CPULoadTimerLastCnt);
        }

#else
        #error "Please configurate CPULoad timer counter direction!!!"
#endif
    }
    else
    {
#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE
        if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
        }
        else
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt
                                 + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
        }
#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE
        KER_u32CPUIdleCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
#else
        #error "Please configurate CPULoad timer counter direction!!!"
#endif

#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

        if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
        }
        else
        {
            KER_u32CPUIdleCnt += KER_u32CPULoadTimerLastCnt
                                 + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
        }

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

        if (KER_u32CPULoadTimerLastCnt <= u32Cnt)
        {
            KER_u32CPUIdleCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
        }
        else
        {
            KER_u32CPUIdleCnt += u32Cnt
               + (KER_u32CPULOAD_TIM_EXPIRED_VAL - KER_u32CPULoadTimerLastCnt);
        }

#else
        #error "Please configurate CPULoad timer counter direction!!!"
#endif

    }

    /*!Pseudo: record current timer counter                                   */
    KER_u32CPULoadTimerLastCnt = u32Cnt;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : update CPU load related counter at ISR end.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : should only called by category 1 interrupt service routine.
*******************************************************************************/
PUBLIC void KER_vidCPULoadISREnd( void )
{
    INT32U u32Cnt;
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();

    u32Cnt = KER_u32CPULoadTimerCounterGet();

    KER_u8CPULoadIntrNestedCnt--;

#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

    if (KER_u32CPULoadTimerLastCnt >= u32Cnt)
    {
        KER_u32CPUISRCnt += KER_u32CPULoadTimerLastCnt - u32Cnt;
    }
    else
    {
        KER_u32CPUISRCnt += KER_u32CPULoadTimerLastCnt
                            + (KER_u32CPULOAD_TIM_RELOAD_VAL - u32Cnt);
    }

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

    if (KER_u32CPULoadTimerLastCnt <= u32Cnt)
    {
        KER_u32CPUISRCnt += u32Cnt - KER_u32CPULoadTimerLastCnt;
    }
    else
    {
        KER_u32CPUISRCnt += u32Cnt
               + (KER_u32CPULOAD_TIM_EXPIRED_VAL - KER_u32CPULoadTimerLastCnt);
    }

#else
    #error "Please configurate CPULoad timer counter direction!!!"
#endif

    /*!Pseudo: record current timer counter                                   */
    KER_u32CPULoadTimerLastCnt = u32Cnt;

    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : CPU Load calculation timer ISR.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidCPULoadTimer( void )
{
    KER_u32CPULoadCalcPeriod--;

    if ((INT32U)0 == KER_u32CPULoadCalcPeriod)
    {
        KER_CPU_SR_ALLOC();

        KER_ENTER_CRITICAL();
        vidCPULoadCalc();
        KER_u32CPUUsingCnt = (INT32U)1;
        KER_u32CPUISRCnt   = (INT32U)0;
        KER_u32CPUIdleCnt  = (INT32U)0;
        KER_LEAVE_CRITICAL();

        KER_u32CPULoadCalcPeriod = KER_u32CPULOAD_CALC_PERIOD;
    }
}

#endif