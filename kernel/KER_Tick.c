/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Tick.c
*Version    : V1.00
*Description: tick manager module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/27
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
PRIVATE void vidTickListHandler( void );

#if KER_coTICK_TASK_EN > 0u
PRIVATE void vidTickTaskEntry(void *vidpParam);
#endif

PRIVATE void vidTickHandler( void );

/*******************************************************************************
* @Brief  : get current tick counter
* @Inputs : none
* @Outputs: none
* @Returns: current tick counter
* @Note   : none
*******************************************************************************/
PUBLIC INT32U KER_u32TickGet( void )
{
    INT32U u32Tick;
    KER_CPU_SR_ALLOC();

    /*!Pseduo: make sure in 8/16Bits cpu access consistency                   */
    KER_ENTER_CRITICAL();
    u32Tick = KER_u32TickCnt;
    KER_LEAVE_CRITICAL();

    return u32Tick;
}

/*******************************************************************************
* @Brief  : remove tcb from tick wheel list.
* @Inputs : pstrTCB: tcb pointer
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidTickListRemove( KER_tstrTCB *pstrTCB )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrTCB)
    {
        return;
    }

#endif

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if in tick wheel list                                    */
    if (pstrTCB->u16TickWheelIdx < KER_u16TICK_WHEEL_SIZE)
    {
        /*!Pseudo: remove from tick wheel list                                */
        LIST_REMOVE_NODE(pstrTCB->strTickList);
    }
    else
    {
        KER_LEAVE_CRITICAL();
        /*!Pseudo: not in tick wheel list, raise a exception                  */
        KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
        return;
    }

    /*!Pseudo: clear delay state bit                                          */
    pstrTCB->u8State &= (INT8U)(~KER_u8TSK_STAT_BIT_DLY);

    /*!Pseudo: link tick wheel index position                                 */
    pstrTCB->u16TickWheelIdx = KER_u16TICK_WHEEL_SIZE;

    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : insert tcb to tick wheel list.
* @Inputs : pstrTCB: tcb pointer
*           u32Ticks: delay ticks
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidTickListInsert(KER_tstrTCB *pstrTCB, INT32U u32Ticks )
{

    INT32U u32MatchTicks;
    INT32U u32RemainTicks;
    INT16U  u16Idx;
    KER_tstrTCB *pstrNextTCB;
    KER_tstrList *pstrNextNode;
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: if is KER_NULL tcb pointer, return                                 */
    if (KER_NULL == pstrTCB)
    {
        return;
    }

    /*!Pseudo: if u32Ticks is equal zero, not need delay                      */
    if ((INT32U)0 == u32Ticks)
    {
        return;
    }

#endif

    KER_ENTER_CRITICAL();

    /*!Pseudo: calculate absolute match ticks                                 */
    u32MatchTicks = KER_u32TickCnt + u32Ticks;
    /*!Pseudo: set timeout match tick                                         */
    pstrTCB->u32TimeoutMatch = u32MatchTicks;

    /*!Pseudo: calculate insert which tick wheel list                         */
    u16Idx = (INT16U)(u32MatchTicks % KER_u16TICK_WHEEL_SIZE);

    /*!Comment: find and insert to list proper  position,
     *          for tick handler fast treat the list                          */
    for (pstrNextNode = KER_astrTickWheelList[u16Idx].pstrNext;
         pstrNextNode != &KER_astrTickWheelList[u16Idx];
         pstrNextNode = pstrNextNode->pstrNext)
    {
        pstrNextTCB = LIST_NODE_ENTRY_PTR(pstrNextNode,KER_tstrTCB,strTickList);

        u32RemainTicks = pstrNextTCB->u32TimeoutMatch - KER_u32TickCnt;
        if (u32Ticks <= u32RemainTicks)
        {
            /*!Pseudo: find insert position                                   */
            break;
        }
    }

    /*!Pseudo: insert before current node,
     * A special case is not found in the loop end, or list is empty,
     * in this case, insert as a list tail node                               */
    LIST_INSERT_BEFORE_PTR(pstrNextNode, &(pstrTCB->strTickList));

    /*!Pseudo: set delay state bit                                            */
    pstrTCB->u8State |= KER_u8TSK_STAT_BIT_DLY;
    /*!Pseudo: link tick wheel index position                                 */
    pstrTCB->u16TickWheelIdx = u16Idx;

    KER_LEAVE_CRITICAL();
}


/*******************************************************************************
* @Brief  : tick list handler, handle delay tasks.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTickListHandler( void )
{
    INT16U u16Idx;
    KER_tstrTCB *pstrTCB;
    KER_tstrList *pstrNode;
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_u32TickCnt++;
    u16Idx = (INT16U)(KER_u32TickCnt % KER_u16TICK_WHEEL_SIZE);
    pstrNode = KER_astrTickWheelList[u16Idx].pstrNext;
    while (pstrNode != &KER_astrTickWheelList[u16Idx])
    {
        pstrTCB = LIST_NODE_ENTRY_PTR(pstrNode,KER_tstrTCB,strTickList);
        pstrNode = pstrNode->pstrNext;
        if (pstrTCB->u32TimeoutMatch == KER_u32TickCnt)
        {
            switch (pstrTCB->u8State)
            {
                case KER_u8TSK_STAT_DLY:
                    KER_vidTickListRemove(pstrTCB);
                    pstrTCB->u8State = KER_u8TSK_STAT_RDY;
                    KER_vidRdyListInsertToHead(pstrTCB);
                    break;
                case KER_u8TSK_STAT_BLK_DLY:
                    KER_vidTickListRemove(pstrTCB);
                    if (KER_NULL != (pstrTCB->strIPCObjPend).vidpIPCObject)
                    {
                        LIST_REMOVE_NODE(pstrTCB->strList);
                    }
                    pstrTCB->u8State = KER_u8TSK_STAT_RDY;
                    /*!Pseudo: reset task pending object information          */
                    (pstrTCB->strIPCObjPend).enuIPCObjType =
                                                   KER_enuIPC_OBJ_TYPE_UNKNOWN;
                    (pstrTCB->strIPCObjPend).vidpIPCObject = KER_NULL;
                    KER_vidRdyListInsertToHead(pstrTCB);
                    pstrTCB->enuError = KER_enuERR_IPC_PEND_TIMEOUT;
                    break;
#if KER_coTASK_SUSPEND_EN > 0u
                case KER_u8TSK_STAT_SPD_BLK_DLY:
                    KER_vidTickListRemove(pstrTCB);
                    if (KER_NULL != (pstrTCB->strIPCObjPend).vidpIPCObject)
                    {
                        LIST_REMOVE_NODE(pstrTCB->strList);
                    }
                    pstrTCB->u8State = KER_u8TSK_STAT_SPD;
                    /*!Pseudo: reset task pending object information          */
                    (pstrTCB->strIPCObjPend).enuIPCObjType =
                                                   KER_enuIPC_OBJ_TYPE_UNKNOWN;
                    (pstrTCB->strIPCObjPend).vidpIPCObject = KER_NULL;
                    pstrTCB->enuError = KER_enuERR_IPC_PEND_TIMEOUT;
                    break;
#endif

#if KER_coTASK_SUSPEND_EN > 0u
                case KER_u8TSK_STAT_SPD_DLY:
                    KER_vidTickListRemove(pstrTCB);
                    pstrTCB->u8State = KER_u8TSK_STAT_SPD;
                    break;
#endif

                case KER_u8TSK_STAT_RDY:
                case KER_u8TSK_STAT_BLK:

#if KER_coTASK_SUSPEND_EN > 0u
                case KER_u8TSK_STAT_SPD:
                case KER_u8TSK_STAT_SPD_BLK:
#endif

                default:
                    KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
                    break;
            }
        }
        else
        {
            break;
        }
    }

#if KER_coHOOK_TICK_EN > 0u
    if (KER_NULL != KER_pfnHookTick)
    {
        /*!Pseudo: call user hook function                                    */
        KER_pfnHookTick();
    }
#endif

    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : task delay
* @Inputs : u32Ticks: delay ticks
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidDelay( INT32U u32Ticks )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if ((INT32U)0 == u32Ticks)
    {
        return;
    }

#endif

    KER_ENTER_CRITICAL();
    KER_vidTickListInsert( KER_pstrCurTCB, u32Ticks);
    KER_vidRdyListRemove(KER_pstrCurTCB);
    KER_LEAVE_CRITICAL();
    KER_vidSched();
}

#if KER_coDELAY_HMSM_EN > 0u
/*******************************************************************************
* @Brief  : HH:MM:SS:MS format delay.
* @Inputs : u16Hours: hours
*           u8Min: minute
*           u8Sec: second
*           u16Ms: millisecond.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidDelayHMSM( INT16U u16Hours,
                              INT8U u8Min,
                              INT8U u8Sec,
                              INT16U u16Ms )
{
    INT32U u32Ticks;

#if KER_coPARAM_CHECK_EN > 0u
    /*!Pseudo: check if parameter is valid                                    */
    if ( (u16Hours > (INT16U)1192)
      || (u8Min > (INT8U)59)
      || (u8Sec > (INT8U)59)
      || (u16Ms > (INT16U)999))
    {
        return ;
    }
#endif

    u32Ticks = ((INT32U)u16Hours) * ((INT32U)3600000);
    u32Ticks = u32Ticks + ((INT32U)u8Min) * ((INT32U)60000);
    u32Ticks = u32Ticks + ((INT32U)u8Sec) * ((INT32U)1000);
    u32Ticks = u32Ticks + u16Ms;
    u32Ticks = u32Ticks / ((INT32U)1000/(KER_u32TICKS_PER_SECOND));

    KER_vidDelay(u32Ticks);
}
#endif

/*******************************************************************************
* @Brief  : tick handler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTickHandler( void )
{

#if KER_coSLICE_SCHED_EN > 0u
    KER_vidSliceSched();
#endif

    vidTickListHandler();

}

/*******************************************************************************
* @Brief  : trigger tick expired.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : for hardware timer ISR calling to trigger tick expired.
*******************************************************************************/
PUBLIC void KER_vidTickTrigger( void )
{
#if KER_coTICK_TASK_EN > 0u
    /*!Pseudo: configured tick task, post tick task semaphore                 */
    if (TRUE == KER_bKernelRunning)
    {
        KER_enuTaskSemPost(&KER_strTickTaskTcb);
    }
#else
    /*!Pseudo: not configured timer task, ISR call timer handler directly     */
    vidTickHandler();
#endif
}

#if KER_coTICK_TASK_EN > 0u

/*******************************************************************************
* @Brief  : tick task entry.
* @Inputs : vidpParam: task private data pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTickTaskEntry(void *vidpParam)
{
    while (1)
    {
        if (KER_enuTaskSemPend(0u) == KER_enuERR_OK)
        {
            vidTickHandler();
        }
    }
}

/*******************************************************************************
* @Brief  : initial tick task
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidTickTaskInit( void )
{
    /*!Pseudo: create kernel tick task                                        */
    KER_enuTaskInit( &KER_strTickTaskTcb,
                      "MRTOS->TickTask",
                      vidTickTaskEntry,
                      KER_NULL,
                      KER_NULL,
                      KER_astkTickTaskStack,
                      KER_stkszTICK_TASK_STKSZ,
                      (CPU_STK_SIZE)0,
                      KER_u16TICK_TASK_PRIO,
                      (INT16U)10,
                      KER_NULL,
                     (INT32U)0,
                      KER_evtEVENT_NONE,
                      KER_evtMASK_ALL,
                      KER_NULL
                     );
}

#endif
