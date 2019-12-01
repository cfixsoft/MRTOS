/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_TaskSem.c
*Version    : V1.00
*Description: task built in semaphore module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/24
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coBUILT_IN_SEM_EN > 0u

/*******************************************************************************
* @Brief  : pend task built in semaphore.
* @Inputs : u32TimeOut: pending timeout value.
* @Outputs: none
* @Returns: KER_enuERR_OK: pending semaphore successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskSemPend(INT32U u32TimeOut)
{
    KER_CPU_SR_ALLOC();

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if built in semaphore value is valid                     */
    if (KER_pstrCurTCB->u32BuiltInSemValue > (INT32U)0)
    {
        /*!Pseudo: take semaphore, decrease the semaphore value               */
        KER_pstrCurTCB->u32BuiltInSemValue--;

        KER_LEAVE_CRITICAL();
        return KER_enuERR_OK;
    }
    else
    {
        /*!Pseudo: if scheduler locked, task should not block                 */
        if (KER_u8SchedLockNestCnt > (INT8U)0)
        {
            /*!Pseudo: leave critical section                                 */
            KER_LEAVE_CRITICAL();

            /*!Pseudo: return with scheduler locked error                     */
            return KER_enuERR_SCHED_LOCK_PENDING;
        }
        else
        {
            /*!Pseudo: do nothing!!!                                      */
        }

        /*!Pseudo: pending built in semaphore                                 */
        KER_vidBuiltInObjectPend(KER_enuIPC_OBJ_TYPE_BUILT_IN_SEM, u32TimeOut);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: schedule to other ready task                               */
        KER_vidSched();

        /*!Pseudo: waiting re-scheduled, return pending semaphore status      */
        return KER_pstrCurTCB->enuError;
    }
}

#if KER_coBUILT_IN_SEM_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend task built in semaphore.
* @Inputs : none
* @Outputs: none
* @Returns: KER_enuERR_OK: try to pend semaphore successful.
*           KER_enuERR_NOK: try to pend semaphore failed.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskSemTryPend( void )
{
    KER_CPU_SR_ALLOC();

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if built in semaphore value is valid                     */
    if (KER_pstrCurTCB->u32BuiltInSemValue > (INT32U)0)
    {
        /*!Pseudo: take semaphore, decrease the semaphore value               */
        KER_pstrCurTCB->u32BuiltInSemValue--;

        KER_LEAVE_CRITICAL();

        return KER_enuERR_OK;
    }
    else
    {
        KER_LEAVE_CRITICAL();

        return KER_enuERR_NOK;
    }
}
#endif

/*******************************************************************************
* @Brief  : post task built in semaphore.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: post semaphore to a task successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskSemPost(KER_tstrTCB *pstrTCB)
{
    KER_CPU_SR_ALLOC();

    /*!Pseudo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if task is pending built in semaphore                    */
    if (KER_enuIPC_OBJ_TYPE_BUILT_IN_SEM
        == (pstrTCB->strIPCObjPend).enuIPCObjType)
    {
        /*!Pseudo: abort pending built in semaphore                           */
        KER_vidBuiltInObjectPendAbort(pstrTCB, KER_enuERR_OK);

        /*!Pseudo: if task is ready to running                                */
        if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State)
        {
            /*!Pseudo: post mutex finished, leave critical section            */
            KER_LEAVE_CRITICAL();

            /*!Pseudo: schedule, for the new ready task has opportunity to run*/
            KER_vidSched();
        }
        else
        {
            /*!Pseudo: post sempahore finished, leave critical section        */
            KER_LEAVE_CRITICAL();
        }
    }
    else
    {
        /*!Pseudo: task not pending its built in semaphore, increase value    */
        pstrTCB->u32BuiltInSemValue++;

        KER_LEAVE_CRITICAL();
    }

    return KER_enuERR_OK;
}

#if KER_coBUILT_IN_SEM_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pending semaphore.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort task pending built in object successful.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending built in object.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskSemPendAbort( KER_tstrTCB *pstrTCB,
                                            KER_tenuSchedOpt enuOption)
{
    return KER_enuTaskBuiltInObjPendAbort(pstrTCB,
                                          KER_enuIPC_OBJ_TYPE_BUILT_IN_SEM,
                                          enuOption);
}
#endif

#if KER_coBUILT_IN_SEM_RESET_EN > 0u
/*******************************************************************************
* @Brief  : reset a task's built in semaphore.
* @Inputs : pstrTCB: task tcb pointer(KER_NULL reset task self)
* @Outputs: none
* @Returns: KER_enuERR_OK: reset successful.
*           KER_enuERR_ISR_FAULT_USING: ISR should specify tcb pointer.
* @Note   : Note: if ISR reset one task's built in semaphore, it should specify
*                 the task's tcb pointer. otherwise return NOK.
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskSemReset( KER_tstrTCB *pstrTCB )
{
    KER_CPU_SR_ALLOC();

    if ( KER_NULL == pstrTCB)
    {
        /*!Pseudo: ISR not allow pass KER_NULL parameter when call this function  */
        if (KER_u8IntrNestCnt > (INT8U)0)
        {
            return KER_enuERR_ISR_FAULT_USING;
        }

        KER_ENTER_CRITICAL();
        KER_pstrCurTCB->u32BuiltInSemValue = (INT32U)0;
        KER_LEAVE_CRITICAL();
    }
    else
    {
        KER_ENTER_CRITICAL();
        pstrTCB->u32BuiltInSemValue = (INT32U)0;
        KER_LEAVE_CRITICAL();
    }

    return KER_enuERR_OK;
}
#endif

#endif