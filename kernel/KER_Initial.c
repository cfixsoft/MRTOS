/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Initial.c
*Version    : V1.00
*Description: MRTOS kernel intialise module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/25
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif


/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
PRIVATE void vidKernelDataInitialise( void );
PRIVATE void vidTCBLinkListInitialise( void );
PRIVATE void vidTickWheelListInitialise( void );
#if KER_coTIMER_EN > 0u
PRIVATE void vidTimerWheelListInitialise( void );
#endif
PRIVATE void vidRdyListInitialise( void );

/*!Comment: kernel self initailize kernel data, not depend on startup code    */
#if KER_coKERNEL_DATA_SELF_INIT_EN > 0u
/*******************************************************************************
* @Brief  : initial kernel data
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidKernelDataInitialise( void )
{
    INT16U u16Idx;

    for (u16Idx = (INT16U)0; u16Idx < KER_u16TASK_RDY_BITMAP_SIZE; u16Idx++)
    {
        KER_awTaskRdyBitMap[u16Idx] = (CPU_WORD)0;
    }

    KER_u32IdleCnt            = (INT32U)0;
    KER_u32TaskCnt            = (INT32U)0;
    KER_u32TickCnt            = (INT32U)0;
    KER_u8IntrNestCnt         = (INT8U)0;
    KER_u8SchedLockNestCnt    = (INT8U)0;
    KER_bKernelRunning        = FALSE;
    KER_pstrCurTCB            = KER_NULL;
    KER_pstrNextTCB    = KER_NULL;

#if KER_coTIMER_EN > 0u
    KER_u32TimerCnt           = (INT32U)0;
#endif

#if KER_coCPULOAD_MODULE_EN > 0u
    /*!Pseudo: CPU Load (Unit:00.1%, Range: 0.0%~100.0%)                      */
    KER_u32CPULoad              = (INT8U)0;
    KER_u32CPULoadISR           = (INT8U)0;
    /*!Pseudo: CPU performance time counter                                   */
    KER_u32CPUUsingCnt         = (INT32U)1;
    KER_u32CPUISRCnt           = (INT32U)0;
    /*!Pseudo: CPU idle time counter                                          */
    KER_u32CPUIdleCnt          = (INT32U)0;
    /*!Pseudo: reset ISR nested level for CPU Load calculation                */
    KER_u8CPULoadIntrNestedCnt = (INT8U)0;

    /*!Pseudo: CPU time last counter record                                   */
    #if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE
        KER_u32CPULoadTimerLastCnt = KER_u32CPULOAD_TIM_RELOAD_VAL;
    #elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE
        KER_u32CPULoadTimerLastCnt = (INT32U)0;
    #else
        #error "Please configurate CPULoad timer counter direction!!!"
    #endif

    /*!Pseudo: calculate CPU Load period                                      */
    KER_u32CPULoadCalcPeriod = KER_u32CPULOAD_CALC_PERIOD;

#endif

}

#endif

/*******************************************************************************
* @Brief  : initial task TCB link list
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTCBLinkListInitialise( void )
{
    /*!Pseudo: initial list head to pointer itself                            */
    LIST_INIT(KER_strTCBLinkList);
}

/*******************************************************************************
* @Brief  : initial tick wheel list
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTickWheelListInitialise( void )
{
    INT16U u16Index;

    /*!Pseudo: loop to initial tick wheel list head to pointer itself         */
    for (u16Index = (INT16U)0; u16Index < KER_u16TICK_WHEEL_SIZE; u16Index++)
    {
        LIST_INIT(KER_astrTickWheelList[u16Index]);
    }
}

#if KER_coTIMER_EN > 0u
/*******************************************************************************
* @Brief  : initial timer tick wheel list
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTimerWheelListInitialise( void )
{
    INT16U u16Index;

    /*!Pseudo: loop to initial timer tick wheel list head to pointer itself   */
    for (u16Index = (INT16U)0; u16Index < KER_u16TIMER_WHEEL_SIZE; u16Index++)
    {
        LIST_INIT(KER_astrTimerWheelList[u16Index]);
    }
}
#endif

/*******************************************************************************
* @Brief  : initial task ready list
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidRdyListInitialise( void )
{
    INT16U u16Index;

    /*!Pseudo: loop to initial task ready list head to pointer itself         */
    for (u16Index = (INT16U)0; u16Index < KER_u16TASK_PRIO_MAX; u16Index++)
    {
        LIST_INIT(KER_astrTaskRdyList[u16Index]);
    }
}

/*******************************************************************************
* @Brief  : kernel initialise
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidInitialise( void )
{


#if KER_coKERNEL_DATA_SELF_INIT_EN > 0u
    /*!Pseudo: kernel data initialise                                         */
    vidKernelDataInitialise();

    #if KER_coHOOK_EN > 0u
    /*!Pseudo: initial hook function pointer                                  */
    KER_vidHookFunctionsAllClr();
    #endif

#endif

    /*!Pseudo: initial task TCB link list                                     */
    vidTCBLinkListInitialise();
    /*!Pseudo: initial task ready list                                        */
    vidRdyListInitialise();
    /*!Pseudo: initial tick wheel list                                        */
    vidTickWheelListInitialise();

#if KER_coTIMER_EN > 0u
    /*!Pseudo: initial timer tick wheel list                                  */
    vidTimerWheelListInitialise();
#endif

    /*!Pseudo: create idle task                                               */
    KER_vidIdleTaskInit();

#if KER_coTICK_TASK_EN > 0u

    KER_vidTickTaskInit();

#endif

#if KER_coTIMER_EN > 0u

    #if KER_coTIMER_TASK_EN > 0u

    KER_vidTimerTaskInit();

    #endif

#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))

    KER_vidStackCheckTaskInit();

#endif

}

/*******************************************************************************
* @Brief  : MRTOS start
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : called after kernel initialized and user static task created.
*******************************************************************************/
PUBLIC void KER_vidStart( void )
{
    if (FALSE == KER_bKernelRunning)
    {
        CPU_REG INT16U u16HighRdyPrio;

        u16HighRdyPrio = KER_u16GetRdyHighestPriority();
        KER_pstrCurTCB = LIST_NODE_ENTRY_PTR(
                               KER_astrTaskRdyList[u16HighRdyPrio].pstrNext,
                               KER_tstrTCB,
                               strList);

        KER_pstrNextTCB = KER_pstrCurTCB;
        KER_bKernelRunning = TRUE;
        KER_vidStartHighRdyTask();
    }
}