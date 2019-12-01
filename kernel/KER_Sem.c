/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Sem.c
*Version    : V1.00
*Description: MRTOS semaphore management module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/4
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coIPC_SEM_EN > 0u

/*******************************************************************************
* @Brief  : initial semaphore object.
* @Inputs : pstrSem: semaphore object pointer.
*           u32Value: semaphore initial value.
*           enuIPCmethod: pending list schedule strategy.
*           ps8Name: semaphore object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial successful.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_METHOD_INV: ipc method invalid.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuSemInit( KER_tstrSem      *pstrSem,
                                   INT32U            u32Value,
                                   KER_tenuIPCMethod enuIPCMethod,
                                   INT8S            *ps8Name)
{

    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if semaphore object is valid                             */
    if (KER_NULL == pstrSem)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

    /*!Pseudo: check if ipc pending method is valid                           */
    if (!IPC_METHOD_IS_VALID(enuIPCMethod))
    {
        /*!Pseudo: ipc pending method is invalid                              */
        return KER_enuERR_IPC_METHOD_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();
    /*!Pseudo: set initial semaphore value                                    */
    pstrSem->u32Value = u32Value;
    /*!Pseudo: initial pending list                                           */
    LIST_INIT(pstrSem->strPendList);
    /*!Pseudo: set ipc method                                                 */
    pstrSem->enuIPCMethod = enuIPCMethod;
    /*!Pseudo: set semaphore object name                                      */
    pstrSem->ps8Name = ps8Name;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}

/*******************************************************************************
* @Brief  : pend semaphore.
* @Inputs : pstrSem: semaphore object pointer.
*           u32TimeOut: pending timeout time.
* @Outputs: none
* @Returns: KER_enuERR_OK: pend semaphore successful.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semaphore object deleted.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_SCHED_LOCK_PENDING: scheduler locked, not allow pending.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuSemPend( KER_tstrSem *pstrSem,
                                   INT32U u32TimeOut )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if semaphore object is valid                             */
    if (KER_NULL == pstrSem)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if semaphore object is deleted                           */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrSem->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if semaphore value is valid to pend                      */
    if (pstrSem->u32Value)
    {
        /*!Pseudo: decrease semaphore value                                   */
        (pstrSem->u32Value)--;

        /*!Pseudo: leave critical section                                     */
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
            /*!Pseudo: do nothing!!!                                          */
        }

        /*!Pseudo: pend semaphore object                                      */
        KER_vidObjectPend( pstrSem,
                           KER_enuIPC_OBJ_TYPE_MUTEX,
                           pstrSem->enuIPCMethod,
                           u32TimeOut);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: schedule to other ready task                               */
        KER_vidSched();

        return KER_pstrCurTCB->enuError;

    }
}

#if KER_coIPC_SEM_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend semaphore.
* @Inputs : none
* @Outputs: none
* @Returns: KER_enuERR_OK: try to pend semaphore successful.
*           KER_enuERR_NOK: try to pend semaphore failed.
*           KER_enuERR_IPC_OBJ_INV: semahpre object pointer invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuSemTryPend( KER_tstrSem *pstrSem)
{
    KER_CPU_SR_ALLOC();

    /*!Pseudo: check if semaphore object is valid                             */
    if (KER_NULL == pstrSem)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if semaphore value is valid to pend                      */
    if (pstrSem->u32Value)
    {
        /*!Pseudo: decrease semaphore value                                   */
        (pstrSem->u32Value)--;

        /*!Pseudo: leave critical section                                     */
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
* @Brief  : post semaphore.
* @Inputs : pstrSem: semaphore object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: post semaphore successful.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semaphore object deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuSemPost( KER_tstrSem *pstrSem )
{
    KER_CPU_SR_ALLOC();

    CPU_REG KER_tstrList *pstrNode;
    CPU_REG KER_tstrTCB  *pstrTCB;

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if semaphore object is valid                             */
    if (KER_NULL == pstrSem)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if semaphore object is deleted                           */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrSem->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseduo: check if no task pending                                       */
    if (LIST_IS_EMPTY(pstrSem->strPendList))
    {
        /*!Pseudo: no task pending, increase semaphore value                  */
        (pstrSem->u32Value)++;
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();
    }
    else
    {
        /*!Pseudo: have tasks pending this semaphore                          */
        /*!Pseudo: get head node from pending list                            */
        pstrNode = LIST_GET_HEAD(pstrSem->strPendList);
        pstrTCB  = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrTCB, strList);

        /*!Pseudo: abort pending semaphore object                             */
        KER_vidObjectPendAbort(pstrTCB, KER_enuERR_OK);

        /*!Pseudo: if task is ready to running, do schedule                   */
        if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State)
        {
            /*!Pseudo: post mutex finished, leave critical section            */
            KER_LEAVE_CRITICAL();

            /*!Pseudo: schedule, for the new ready task has opportunity to run*/
            KER_vidSched();
        }
        else
        {
            /*!Pseudo: post mutex finished, leave critical section            */
            KER_LEAVE_CRITICAL();
        }
    }

    /*!Pseudo: return with no error                                           */
    return KER_enuERR_OK;
}

#if KER_coIPC_SEM_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pending semaphore object.
* @Inputs : pstrSem: semaphore object pointer.
*           pstrTCB: task tcb pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending semaphore successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semaphore object deleted.
*           KER_enuERR_IPC_NOT_PEND_THIS: Not pending this semaphore object.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuSemPendAbort( KER_tstrSem *pstrSem,
                                        KER_tstrTCB *pstrTCB,
                                        KER_tenuSchedOpt enuOption)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if semaphore object is valid                             */
    if (KER_NULL == pstrSem)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

    /*!Pseudo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if semaphore object is deleted                           */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrSem->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if pending this event                                    */
    if (((pstrTCB->strIPCObjPend).enuIPCObjType != KER_enuIPC_OBJ_TYPE_SEM)
        || ((pstrTCB->strIPCObjPend).vidpIPCObject != pstrSem))
    {
        KER_LEAVE_CRITICAL();
        /*!Pseudo: return error not pending this mutex object                 */
        return KER_enuERR_IPC_NOT_PEND_THIS;
    }

    /*!Pseudo: abort pending semaphore object                                 */
    KER_vidObjectPendAbort(pstrTCB, KER_enuERR_IPC_PEND_ABORT);

    /*!Pseudo: if task is ready to running, then insert to ready list head    */
    if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State )
    {
        /*!Pseudo: post mutex finished, leave critical section                */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: check if need schedule to new ready task                   */
        if (KER_enuSCHED_OPT_NEED == enuOption)
        {
            /*!Pseudo: schedule, for the new ready task has opportunity to run*/
            KER_vidSched();
        }
    }
    else
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();
    }

    return KER_enuERR_OK;
}
#endif

#if KER_coIPC_SEM_ABORT_ALL_EN > 0u
/*******************************************************************************
* @Brief  : abort all tasks pending the specify semaphore object.
* @Inputs : pstrSem: semaphore object pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort all pending tasks successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuSemPendAbortAll( KER_tstrSem *pstrSem,
                                           KER_tenuSchedOpt enuOption)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if semaphore object is valid                             */
    if (KER_NULL == pstrSem)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if semaphore object is deleted                           */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrSem->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: remove all pending task,set ipc object pending abort error flag*/
    KER_vidPendListRemoveAll(&(pstrSem->strPendList),
                               KER_enuERR_IPC_PEND_ABORT);

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: check if need schedule to new ready task                       */
    if (KER_enuSCHED_OPT_NEED == enuOption)
    {
        /*!Pseudo: for the new ready task has opportunity to run              */
        KER_vidSched();
    }

    return KER_enuERR_OK;
}
#endif

#if KER_coIPC_SEM_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete a semaphore object.
* @Inputs : pstrEvent: semaphore object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: deleted successful.
*           KER_enuERR_IPC_OBJ_INV: semphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semphore object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuSemDelete( KER_tstrSem *pstrSem )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if semaphore object is valid                             */
    if (KER_NULL == pstrSem)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrSem->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: remove all pending task,set ipc object pending abort error flag*/
    KER_vidPendListRemoveAll(&(pstrSem->strPendList),
                               KER_enuERR_IPC_OBJ_DELETED);

    /*!Pseudo: no onwer, no pending task, set mutex object invalid            */
    pstrSem->enuIPCMethod = KER_enuIPC_METHOD_UNKNOWN;

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: for the new ready task has opportunity to run                  */
    KER_vidSched();

    return KER_enuERR_OK;
}
#endif

#endif
