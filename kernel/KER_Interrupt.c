/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Interrupt.c
*Version    : V1.00
*Description: kernel interrupt manager module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/27
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*******************************************************************************
* @Brief  : called at the begin by interrupt routine service functions.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : should only called by category 2 interrupt service routine.
*******************************************************************************/
PUBLIC void KER_vidInterruptEnter( void )
{
    KER_CPU_SR_ALLOC();

    /*!Pseudo: check if kernel is running                                     */
    if (FALSE == KER_bKernelRunning)
    {
        return;
    }

#if KER_coCPULOAD_MODULE_EN > 0u
    KER_vidCPULoadISRStart();
#endif

    KER_ENTER_CRITICAL();

    if (KER_u8IntrNestCnt >= KER_u8INTR_NESTING_LEVEL_LIMIT)
    {
        KER_LEAVE_CRITICAL();
        KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
        return;
    }

    KER_u8IntrNestCnt++;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : called at the end by interrupt routine service functions.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : should only called by category 2 interrupt service routine.
*******************************************************************************/
PUBLIC void KER_vidInterruptExit( void )
{
    CPU_REG INT16U u16HighRdyPrio;
    KER_CPU_SR_ALLOC();

    /*!Pseudo: check if kernel is running                                     */
    if (FALSE == KER_bKernelRunning)
    {
        return;
    }

#if KER_coCPULOAD_MODULE_EN > 0u
    KER_vidCPULoadISREnd();
#endif

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if prevent nest counter wrapping                         */
    if ((INT8U)0 == KER_u8IntrNestCnt)
    {
        KER_LEAVE_CRITICAL();
        return;
    }

    /*!Pseudo: decrease interrupt nesting counter                             */
    KER_u8IntrNestCnt--;

    /*!Pseudo: check if interrupt still nested                                */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        KER_LEAVE_CRITICAL();
        return;
    }

    /*!Pseudo: check if scheduler locked                                      */
    if (KER_u8SchedLockNestCnt > (INT8U)0)
    {
        KER_LEAVE_CRITICAL();
        return;
    }

    /*!Pseudo: trigger interrupt task context switch                          */
    /*!Pseudo: get highest priority task                                      */
    u16HighRdyPrio = KER_u16GetRdyHighestPriority();
    KER_pstrNextTCB = LIST_NODE_ENTRY_PTR(
                       KER_astrTaskRdyList[u16HighRdyPrio].pstrNext,
                       KER_tstrTCB,
                       strList);

    /*!Pseudo: check if not need switch to new task                           */
    if (KER_pstrNextTCB != KER_pstrCurTCB)
    {
        /*!Pseudo: switch to next task                                        */
        KER_INTR_SW();
    }

    KER_LEAVE_CRITICAL();
}
