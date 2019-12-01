/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Sched.c
*Version    : V1.00
*Description: task scheduler module
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/27
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*******************************************************************************
* @Brief  : remove a tcb node from a ready list.
* @Inputs : pstrTCB: tcb pointer
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidRdyListRemove(KER_tstrTCB *pstrTCB)
{
    /*!Pseudo: remove TCB from ready list                                     */
    LIST_REMOVE_NODE(pstrTCB->strList);

    /*!Pseudo: check if ready list is empty, then clear ready state bit       */
    if (LIST_IS_EMPTY(KER_astrTaskRdyList[pstrTCB->u16CurPriority]))
    {
        /*!Pseudo: clear ready state bit                                      */
        KER_vidClrRdyBitMap(pstrTCB->u16CurPriority);
    }
}

/*******************************************************************************
* @Brief  : insert tcb to ready list tail.
* @Inputs : pstrTCB: tcb pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidRdyListInsertToTail( KER_tstrTCB *pstrTCB )
{
    /*!Pseudo: insert to ready list tail                                      */
    LIST_INSERT_TAIL(KER_astrTaskRdyList[pstrTCB->u16CurPriority],
                     pstrTCB->strList);

    /*!Pseudo: set ready state bit                                            */
    KER_vidSetRdyBitMap(pstrTCB->u16CurPriority);
}

/*******************************************************************************
* @Brief  : insert tcb to ready list head.
* @Inputs : pstrTCB: tcb pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidRdyListInsertToHead( KER_tstrTCB *pstrTCB )
{
    /*!Pseudo: insert to ready list head                                      */
    LIST_INSERT_HEAD(KER_astrTaskRdyList[pstrTCB->u16CurPriority],
                     pstrTCB->strList);

    /*!Pseudo: set ready state bit                                            */
    KER_vidSetRdyBitMap(pstrTCB->u16CurPriority);
}

/*******************************************************************************
* @Brief  : lock scheduler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidSchedLock( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_u8SchedLockNestCnt++;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : unlock scheduler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidSchedUnlock( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_u8SchedLockNestCnt--;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : task scheduler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidSched( void )
{
    CPU_REG INT16U u16HighRdyPrio;
    KER_CPU_SR_ALLOC();

    /*!Pseudo: if interrupt nested, return                                    */
    if (KER_u8IntrNestCnt)
    {
        return;
    }

    /*!Pseudo: kernel enter critical code section                             */
    KER_ENTER_CRITICAL();

    /*!Pseudo: if scheduler locked, return                                    */
    if (KER_u8SchedLockNestCnt)
    {
        KER_LEAVE_CRITICAL();
        return;
    }

    /*!Pseudo: get highest priority task                                      */
    u16HighRdyPrio = KER_u16GetRdyHighestPriority();
    KER_pstrNextTCB = LIST_NODE_ENTRY_PTR(
                       KER_astrTaskRdyList[u16HighRdyPrio].pstrNext,
                       KER_tstrTCB,
                       strList);

    if (KER_pstrNextTCB != KER_pstrCurTCB)
    {
        /*!Pseudo: switch to next task                                        */
        KER_TASK_SW();
    }

    /*!Pseudo: kernel leave critical code section                             */
    KER_LEAVE_CRITICAL();
}

#if KER_coSLICE_SCHED_EN > 0u
/*******************************************************************************
* @Brief  : slice schedule.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : called by kernel tick interrupt routine.
*******************************************************************************/
PUBLIC void KER_vidSliceSched( void )
{

    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();

    if (KER_pstrCurTCB == &KER_strIdleTaskTcb)
    {
        KER_LEAVE_CRITICAL();
        return;
    }

    if (KER_pstrCurTCB->u16RemainTicks)
    {
        (KER_pstrCurTCB->u16RemainTicks)--;
    }

    if (KER_pstrCurTCB->u16RemainTicks)
    {
        KER_LEAVE_CRITICAL();
        return;
    }

    if (KER_u8SchedLockNestCnt)
    {
        KER_LEAVE_CRITICAL();
        return;
    }

    if ((INT16U)0 == KER_pstrCurTCB->u16ReloadTicks)
    {
        KER_pstrCurTCB->u16RemainTicks = KER_u16RELOAD_TICKS_DEFAULT_VALUE;
    }
    else
    {
        KER_pstrCurTCB->u16RemainTicks = KER_pstrCurTCB->u16ReloadTicks;
    }

    if ((KER_pstrCurTCB->u8State == KER_u8TSK_STAT_RDY)
       &&
       ((KER_pstrCurTCB->strList).pstrNext!=(KER_pstrCurTCB->strList).pstrPrev))
    {
        LIST_REMOVE_NODE(KER_pstrCurTCB->strList);

        LIST_INSERT_TAIL(KER_astrTaskRdyList[KER_pstrCurTCB->u16CurPriority],
                         KER_pstrCurTCB->strList);
    }

    KER_LEAVE_CRITICAL();

    return;
}
#endif

