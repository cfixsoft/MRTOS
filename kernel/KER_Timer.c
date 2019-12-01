/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Timer.c
*Version    : V1.00
*Description: MRTOS user timer management module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/16
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coTIMER_EN > 0u

/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
#if KER_coTIMER_TASK_EN > 0u
PRIVATE void vidTimerTaskEntry(void *vidpParam);
#endif

PRIVATE void vidTimerHandler( void );

/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
PRIVATE void vidTimerWheelListInsert( KER_tstrTimer *pstrTimer );

/*******************************************************************************
* @Brief  : insert a timer to timer tick wheel list.
* @Inputs : pstrTimer: timer object pointer.
* @Outputs: none
* @Returns: none
* @Note   : pstrTimer->u32Period should be assigned value before call this func.
*******************************************************************************/
PRIVATE void vidTimerWheelListInsert( KER_tstrTimer *pstrTimer )
{
    CPU_REG INT16U u16Idx;
    CPU_REG KER_tstrList  *pstrNextNode;
    CPU_REG KER_tstrTimer *pstrNextTimer;
    CPU_REG INT32U u32RemainTicks;

    /*!Pseudo: calculate insert which timer tick wheel list                   */
    u16Idx = (INT16U)((pstrTimer->u32MatchTicks) % KER_u16TIMER_WHEEL_SIZE);
    /*!Comment: find and insert to list proper  position,
     *          for tick handler fast treat the list                          */
    for (pstrNextNode = KER_astrTimerWheelList[u16Idx].pstrNext;
         pstrNextNode != &KER_astrTimerWheelList[u16Idx];
         pstrNextNode = pstrNextNode->pstrNext)
    {
        pstrNextTimer = LIST_NODE_ENTRY_PTR(pstrNextNode,
                                            KER_tstrTimer,
                                            strList);

        u32RemainTicks = pstrNextTimer->u32MatchTicks - KER_u32TimerCnt;
        if (pstrTimer->u32Period <= u32RemainTicks)
        {
            /*!Pseudo: find insert position                                   */
            break;
        }
    }

    /*!Pseudo: set timer running status                                       */
    pstrTimer->enuStatus = KER_enuTIMER_STATUS_RUNNING;

    /*!Pseudo: insert before current node,
     * A special case is not found in the loop end, or list is empty,
     * in this case, insert as a list tail node                               */
    LIST_INSERT_BEFORE_PTR(pstrNextNode, &(pstrTimer->strList));

}

/*******************************************************************************
* @Brief  : intial timer object
* @Inputs : pstrTimer: timer object pointer.
*           pfnAction: timeout callback function pointer.
*           vidpParam: timeout callback function private parameter pointer.
*           u32Period: timer period.
*           enuInitStatus: timer initial status.
*           enuMode: timer running mode.
*           ps8Name: timer object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial timer is ok.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_ACTION_INV: timeout action pointer invalid.
*           KER_enuERR_TIEMR_ZERO_PERIOD: period set to zero error.
*           KER_enuERR_TIMER_STATUS_INV: invalid status.
*           KER_enuERR_TIEMR_MODE_INV: invalid running mode.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerInit( KER_tstrTimer *pstrTimer,
                                     KER_tpfnTimerAction pfnAction,
                                     void *vidpParam,
                                     INT32U u32Period,
                                     KER_tenuTimerStatus enuInitStatus,
                                     KER_tenuTimerMode enuMode,
                                     INT8S *ps8Name
                                     )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u
    if (KER_NULL == pstrTimer)
    {
        return KER_enuERR_TIMER_PTR_INV;
    }

    if (KER_NULL == pfnAction)
    {
        return KER_enuERR_TIMER_ACTION_INV;
    }

    if ((INT32U)0 == u32Period)
    {
        return KER_enuERR_TIEMR_ZERO_PERIOD;
    }

    if (!TIMER_STATUS_IS_VALID(enuInitStatus))
    {
        return KER_enuERR_TIMER_STATUS_INV;
    }

    if (!TIMER_MODE_IS_VALID(enuMode))
    {
        return KER_enuERR_TIEMR_MODE_INV;
    }
#endif

    pstrTimer->pfnAction = pfnAction;
    pstrTimer->vidpParam = vidpParam;
    pstrTimer->ps8Name   = ps8Name;
    pstrTimer->enuMode   = enuMode;
    pstrTimer->u32Period = u32Period;
    pstrTimer->enuStatus = enuInitStatus;

    KER_ENTER_CRITICAL();
    pstrTimer->u32MatchTicks = KER_u32TimerCnt + u32Period;
    switch (enuInitStatus)
    {
        case KER_enuTIMER_STATUS_RUNNING:
            vidTimerWheelListInsert(pstrTimer);
            KER_LEAVE_CRITICAL();
            break;
        case KER_enuTIMER_STATUS_STOP:
        case KER_enuTIMER_STATUS_COMPLETED:
            LIST_INIT(pstrTimer->strList);
            KER_LEAVE_CRITICAL();
            break;
        case KER_enuTIMER_STATUS_INV:
        default:
            KER_LEAVE_CRITICAL();
            return KER_enuERR_TIEMR_MODE_INV;
    }
    return KER_enuERR_OK;
}

#if KER_coTIMER_HMSM_INIT_EN > 0u
/*******************************************************************************
* @Brief  : intial timer object (HMSM format)
* @Inputs : pstrTimer: timer object pointer.
*           pfnAction: timeout callback function pointer.
*           vidpParam: timeout callback function private parameter pointer.
*           u16Hours: hours.
*           u8Min: minute.
*           u8Sec: second.
*           u16Ms: millisecond.
*           enuInitStatus: timer initial status.
*           enuMode: timer running mode.
*           ps8Name: timer object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial timer is ok.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_ACTION_INV: timeout action pointer invalid.
*           KER_enuERR_TIEMR_ZERO_PERIOD: period set to zero error.
*           KER_enuERR_TIMER_STATUS_INV: invalid status.
*           KER_enuERR_TIEMR_MODE_INV: invalid running mode.
*           KER_enuERR_TIMER_HMSM_INV: HMSM paramter exceed limit value.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerHMSMInit( KER_tstrTimer *pstrTimer,
                                         KER_tpfnTimerAction pfnAction,
                                         void *vidpParam,
                                         INT16U u16Hours,
                                         INT8U u8Min,
                                         INT8U u8Sec,
                                         INT16U u16Ms,
                                         KER_tenuTimerStatus enuInitStatus,
                                         KER_tenuTimerMode enuMode,
                                         INT8S *ps8Name
                                       )
{
    CPU_REG INT32U u32Period;
    KER_tenuERR enuRet;

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if parameter is valid                                    */
    if ( (u16Hours > (INT16U)1192)
      || (u8Min > (INT8U)59)
      || (u8Sec > (INT8U)59)
      || (u16Ms > (INT16U)999))
    {
        return KER_enuERR_TIMER_HMSM_INV;
    }

#endif

    u32Period = ((INT32U)u16Hours) * ((INT32U)3600000);
    u32Period = u32Period + ((INT32U)u8Min) * ((INT32U)60000);
    u32Period = u32Period + ((INT32U)u8Sec) * ((INT32U)1000);
    u32Period = u32Period + u16Ms;
    u32Period = u32Period / ((INT32U)1000/(KER_u32TIMER_TICKS_PER_SECOND));

    enuRet = KER_enuTimerInit( pstrTimer,
                               pfnAction,
                               vidpParam,
                               u32Period,
                               enuInitStatus,
                               enuMode,
                               ps8Name);
    return enuRet;
}
#endif

#if KER_coTIMER_START_EN > 0u
/*******************************************************************************
* @Brief  : start a not running timer
* @Inputs : pstrTimer
* @Outputs: none
* @Returns: KER_enuERR_OK: start timer successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_IS_RUNNING: duplicate request timer to running.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerStart( KER_tstrTimer *pstrTimer )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrTimer)
    {
        return KER_enuERR_TIMER_PTR_INV;
    }

#endif

    KER_ENTER_CRITICAL();

    if (KER_enuTIMER_STATUS_RUNNING != pstrTimer->enuStatus)
    {
        pstrTimer->u32MatchTicks = KER_u32TimerCnt + pstrTimer->u32Period;
        vidTimerWheelListInsert(pstrTimer);
        KER_LEAVE_CRITICAL();
        return KER_enuERR_OK;
    }
    else
    {
        KER_LEAVE_CRITICAL();
        return KER_enuERR_TIMER_IS_RUNNING;
    }
}
#endif

#if KER_coTIMER_STOP_EN > 0u
/*******************************************************************************
* @Brief  : stop timer
* @Inputs : pstrTimer: timer object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: stop timer successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_IS_STOP: duplicate request timer to stop.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerStop( KER_tstrTimer *pstrTimer )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrTimer)
    {
        return KER_enuERR_TIMER_PTR_INV;
    }

#endif

    KER_ENTER_CRITICAL();

    if (KER_enuTIMER_STATUS_RUNNING == pstrTimer->enuStatus)
    {
        LIST_REMOVE_NODE(pstrTimer->strList);
        pstrTimer->enuStatus = KER_enuTIMER_STATUS_STOP;
        KER_LEAVE_CRITICAL();
        return KER_enuERR_OK;
    }
    else
    {
        KER_LEAVE_CRITICAL();
        return KER_enuERR_TIMER_IS_STOP;
    }
}
#endif

#if KER_coTIMER_MODE_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : change timer mode
* @Inputs : pstrTimer: timer object pointer.
*           enuMode: timer new mode.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerModeChange( KER_tstrTimer *pstrTimer,
                                           KER_tenuTimerMode enuMode)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrTimer)
    {
        return KER_enuERR_TIMER_PTR_INV;
    }

    if (!TIMER_MODE_IS_VALID(enuMode))
    {
        return KER_enuERR_TIEMR_MODE_INV;
    }

#endif

    KER_ENTER_CRITICAL();

    if (pstrTimer->enuMode != enuMode)
    {
        switch (enuMode)
        {
            case KER_enuTIMER_MODE_ONE_SHOT:
            case KER_enuTIMER_MODE_PERIODIC:
                pstrTimer->enuMode = enuMode;
                break;
            default:
                break;
        }

        if (KER_enuTIMER_STATUS_RUNNING == pstrTimer->enuStatus)
        {
            LIST_REMOVE_NODE(pstrTimer->strList);
            pstrTimer->u32MatchTicks = KER_u32TimerCnt + pstrTimer->u32Period;
            vidTimerWheelListInsert(pstrTimer);
        }

        KER_LEAVE_CRITICAL();
        return KER_enuERR_OK;
    }
    else
    {
        KER_LEAVE_CRITICAL();
        return KER_enuERR_TIMER_MODE_SAME;
    }

}
#endif

#if KER_coTIMER_PERIOD_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : change timer period
* @Inputs : pstrTimer: timer object pointer.
*           u32Period: timer period
* @Outputs: none
* @Returns: KER_enuERR_OK: change timer period successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIEMR_ZERO_PERIOD: set zero period error.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerPeriodChange( KER_tstrTimer *pstrTimer,
                                             INT32U u32Period)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrTimer)
    {
        return KER_enuERR_TIMER_PTR_INV;
    }

    if ((INT32U)0 == u32Period)
    {
        return KER_enuERR_TIEMR_ZERO_PERIOD;
    }

#endif

    KER_ENTER_CRITICAL();
    pstrTimer->u32Period = u32Period;
    KER_LEAVE_CRITICAL();
    return KER_enuERR_OK;
}
#endif

#if KER_coTIMER_HMSM_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : Timer period (HMSM format) change
* @Inputs : pstrTimer: timer object pointer.
*           u16Hours: hours
*           u8Min: minute
*           u8Sec: second
*           u16Ms: millisecond
* @Outputs: none
* @Returns: KER_enuERR_OK: change timer period successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_HMSM_INV: HMSM paramter exceed limit value.
*           KER_enuERR_TIEMR_ZERO_PERIOD: set zero period error.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerHMSMChange( KER_tstrTimer *pstrTimer,
                                           INT16U u16Hours,
                                           INT8U u8Min,
                                           INT8U u8Sec,
                                           INT16U u16Ms )
{
    KER_CPU_SR_ALLOC();
    CPU_REG INT32U u32Period;

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrTimer)
    {
        return KER_enuERR_TIMER_PTR_INV;
    }

    /*!Pseudo: check if parameter is valid                                    */
    if ( (u16Hours > (INT16U)1192)
      || (u8Min > (INT8U)59)
      || (u8Sec > (INT8U)59)
      || (u16Ms > (INT16U)999))
    {
        return KER_enuERR_TIMER_HMSM_INV;
    }

#endif

    u32Period = ((INT32U)u16Hours) * ((INT32U)3600000);
    u32Period = u32Period + ((INT32U)u8Min) * ((INT32U)60000);
    u32Period = u32Period + ((INT32U)u8Sec) * ((INT32U)1000);
    u32Period = u32Period + u16Ms;
    u32Period = u32Period / ((INT32U)1000/(KER_u32TIMER_TICKS_PER_SECOND));

    if ((INT32U)0 == u32Period)
    {
        return KER_enuERR_TIEMR_ZERO_PERIOD;
    }

    KER_ENTER_CRITICAL();
    pstrTimer->u32Period = u32Period;
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif

#if KER_coTIMER_ACTION_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : change timer timeout action function
* @Inputs : pstrTimer: timer object pointer.
*           pfnAction: timeout action function pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: change successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_ACTION_INV: timer action invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerActionChange( KER_tstrTimer *pstrTimer,
                                             KER_tpfnTimerAction pfnAction)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrTimer)
    {
        return KER_enuERR_TIMER_PTR_INV;
    }

    if (KER_NULL == pfnAction)
    {
        return KER_enuERR_TIMER_ACTION_INV;
    }

#endif

    KER_ENTER_CRITICAL();
    pstrTimer->pfnAction = pfnAction;
    KER_LEAVE_CRITICAL();
    return KER_enuERR_OK;
}
#endif

/*******************************************************************************
* @Brief  : timer handler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTimerHandler( void )
{
    CPU_REG INT16U u16Idx;
    CPU_REG KER_tstrTimer *pstrTimer;
    CPU_REG KER_tstrList *pstrNode;
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();

    KER_u32TimerCnt++;
    u16Idx = (INT16U)(KER_u32TimerCnt % KER_u16TICK_WHEEL_SIZE);
    pstrNode = KER_astrTimerWheelList[u16Idx].pstrNext;

    while (pstrNode != &KER_astrTimerWheelList[u16Idx])
    {
        pstrTimer = LIST_NODE_ENTRY_PTR(pstrNode,KER_tstrTimer,strList);
        pstrNode = pstrNode->pstrNext;
        if (pstrTimer->u32MatchTicks == KER_u32TimerCnt)
        {
            if (KER_NULL != pstrTimer->pfnAction)
            {
                pstrTimer->pfnAction(pstrTimer->vidpParam);
            }

            if (KER_enuTIMER_MODE_ONE_SHOT == pstrTimer->enuMode)
            {
                pstrTimer->enuStatus = KER_enuTIMER_STATUS_COMPLETED;
                LIST_REMOVE_NODE(pstrTimer->strList);
            }

            LIST_REMOVE_NODE(pstrTimer->strList);
            pstrTimer->u32MatchTicks = KER_u32TimerCnt + pstrTimer->u32Period;
            vidTimerWheelListInsert(pstrTimer);
        }
        else
        {
            break;
        }
    }

#if (KER_coHOOK_TIMER_EN > 0u) && ( KER_coTIMER_EN > 0u )
    if (KER_NULL != KER_pfnHookTimer)
    {
        /*!Pseudo: call user hook function                                    */
        KER_pfnHookTimer();
    }
#endif

    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : trigger timer expired.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : for hardware timer ISR calling to trigger software timer expired.
*******************************************************************************/
PUBLIC void KER_vidTimerTrigger( void )
{

/*!TODO: need to consider how to manage timer task
         when not have built in semaphore support                             */

#if (KER_coTIMER_TASK_EN > 0u) && (KER_coBUILT_IN_SEM_EN > 0u)
    /*!Pseudo: configured timer task, post timer task semaphore               */
    if (TRUE == KER_bKernelRunning)
    {
        KER_enuTaskSemPost(&KER_strTimerTaskTcb);
    }
#else
    /*!Pseudo: not configured timer task, ISR call timer handler directly     */
    vidTimerHandler();
#endif
}

#if KER_coTIMER_TASK_EN > 0u
/*******************************************************************************
* @Brief  : timer task entry.
* @Inputs : vidpParam: task private data pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTimerTaskEntry(void *vidpParam)
{
    while (1)
    {
        if (KER_enuTaskSemPend(0u) == KER_enuERR_OK)
        {
            vidTimerHandler();
        }
    }
}

/*******************************************************************************
* @Brief  : initial timer task
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidTimerTaskInit( void )
{
    /*!Pseudo: create kernel timer task                                       */
    KER_enuTaskInit( &KER_strTimerTaskTcb,
                      "MRTOS->TimerTask",
                      vidTimerTaskEntry,
                      KER_NULL,
                      KER_NULL,
                      KER_astkTimerTaskStack,
                      KER_stkszTIMER_TASK_STKSZ,
                      (CPU_STK_SIZE)0,
                      KER_u16TIMER_TASK_PRIO,
                      (INT16U)10,
                      KER_NULL,
                     (INT32U)0,
                      KER_evtEVENT_NONE,
                      KER_evtMASK_ALL,
                      KER_NULL
                     );
}

#endif

#endif
