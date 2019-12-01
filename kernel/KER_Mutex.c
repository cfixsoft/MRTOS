/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Mutex.c
*Version    : V1.00
*Description: MRTOS mutex management module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/29
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*!Comment: using mutex module compiler control                               */
#if KER_coIPC_MUTEX_EN > 0u
/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
PRIVATE void vidInheritPriority(KER_tstrTCB *pstrTCB,
                                INT16U u16Priority);

/*******************************************************************************
* @Brief  : inherit priority.
* @Inputs : pstrTCB: task tcb pointer.
*           u16Priority: new assign priority.
* @Outputs: none
* @Returns: KER_enuERR_OK: change priority successful.
*           KER_enuERR_TCB_PTR_INV: tcb pointer invalid.
*           KER_enuERR_PRIORITY_INV: new priority is out of range.
*           KER_enuERR_TASK_STATE_INV: task state invalid.
*           KER_enuERR_IPC_OBJ_TYPE_INV: ipc object type invalid.
* @Note   : none
*******************************************************************************/
PRIVATE void vidInheritPriority(KER_tstrTCB *pstrTCB,
                                INT16U u16Priority)
{
    switch (pstrTCB->u8State)
    {
        case KER_u8TSK_STAT_RDY:
            KER_vidRdyListRemove(pstrTCB);
            pstrTCB->u16CurPriority = u16Priority;
            KER_vidRdyListInsertToHead(pstrTCB);
            break;
        case KER_u8TSK_STAT_DLY:
#if KER_coTASK_SUSPEND_EN > 0u
        case KER_u8TSK_STAT_SPD:
        case KER_u8TSK_STAT_SPD_DLY:
#endif
            pstrTCB->u16CurPriority = u16Priority;
            break;
        case KER_u8TSK_STAT_BLK:
        case KER_u8TSK_STAT_BLK_DLY:
#if KER_coTASK_SUSPEND_EN > 0u
        case KER_u8TSK_STAT_SPD_BLK:
        case KER_u8TSK_STAT_SPD_BLK_DLY:
#endif
        {
            KER_tenuIPCMethod enuIPCMethod;
            KER_tstrList     *pstrListHead;
            void  *vidpIPCObj= (pstrTCB->strIPCObjPend).vidpIPCObject;

            /*!Pseduo: replace node position in pending list                  */
            switch ((pstrTCB->strIPCObjPend).enuIPCObjType)
            {

#if KER_coIPC_MUTEX_EN > 0u
                case KER_enuIPC_OBJ_TYPE_MUTEX:
                    pstrListHead=&(((KER_tstrMutex *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrMutex *)vidpIPCObj)->enuIPCMethod;
                    break;
#endif

#if KER_coIPC_SEM_EN > 0u
                case KER_enuIPC_OBJ_TYPE_SEM:
                    pstrListHead=&(((KER_tstrSem *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrSem *)vidpIPCObj)->enuIPCMethod;
                    break;
#endif

#if KER_coIPC_MSGQ_EN > 0u
                case KER_enuIPC_OBJ_TYPE_MSG_Q:
                    pstrListHead=&(((KER_tstrMsgQ *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrMsgQ *)vidpIPCObj)->enuIPCMethod;
                    break;
#endif

#if KER_coIPC_EVENT_EN > 0u
                case KER_enuIPC_OBJ_TYPE_EVENT:
                    pstrListHead=&(((KER_tstrEvent *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrEvent *)vidpIPCObj)->enuIPCMethod;
                    break;
#endif

#if KER_coBUILT_IN_MSGQ_EN > 0u
                case KER_enuIPC_OBJ_TYPE_BUILT_IN_MSG_Q:
#endif

#if KER_coBUILT_IN_EVENT_EN > 0u
                case KER_enuIPC_OBJ_TYPE_BUILT_IN_EVENT:
#endif

#if KER_coBUILT_IN_SEM_EN > 0u
                case KER_enuIPC_OBJ_TYPE_BUILT_IN_SEM:
#endif

#if (KER_coBUILT_IN_MSGQ_EN > 0u) \
 || (KER_coBUILT_IN_EVENT_EN > 0u) \
 || (KER_coBUILT_IN_SEM_EN > 0u)

                    /*!Pseudo: set new priority                               */
                    pstrTCB->u16CurPriority = u16Priority;
                    /*!Pseudo: return directly                                */
                    return;
#endif
                case KER_enuIPC_OBJ_TYPE_UNKNOWN:
                default:
                    KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
                    return;
            }

            /*!Pseudo: remove pstrTCB node from pending list                  */
            LIST_REMOVE_NODE(pstrTCB->strList);
            /*!Pseudo: set new priority                                       */
            pstrTCB->u16CurPriority = u16Priority;
            /*!Pseudo: re-insert into pending list                            */
            KER_vidPendListInsert(pstrListHead, pstrTCB, enuIPCMethod);
            break;
        }
        default:
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            break;
    }

}

/*******************************************************************************
* @Brief  : initialize mutex object.
* @Inputs : pstrMutex: Mutex object pointer.
*           enuIPCMethod: mutex pend method.
*           psName: Mutex object name pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: Initail mutex object is ok.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function
*           KER_enuERR_IPC_OBJ_INV: ipc object is invalid.
*           KER_enuERR_IPC_METHOD_INV: ipc pending method is invalid.
* @Note   : should not call by interrupt service routine!!!
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMutexInit( KER_tstrMutex *pstrMutex,
                                     KER_tenuIPCMethod enuIPCMethod,
                                     INT8S *ps8Name)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if mutex object is valid                                 */
    if (KER_NULL == pstrMutex)
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

    /*!Pseudo: reset hold counter                                             */
    pstrMutex->u8HoldCnt               = (INT8U)0;

    /*!Pseudo: set ipc pending method                                         */
    pstrMutex->enuIPCMethod            = enuIPCMethod;

    /*!Pseudo: reset owner orignal prioriy                                    */
    pstrMutex->u16OwnerOrignalPriority = KER_u16TASK_PRIO_MAX;

    /*!Pseudo: initial pending state no owner                                 */
    pstrMutex->pstrOwnerTCB            = KER_NULL;

    /*!Pseudo: set mutex object name                                          */
    pstrMutex->ps8Name                 = ps8Name;

    /*!Pseudo: empty pending list                                             */
    LIST_INIT(pstrMutex->strPendList);

    /*!Pseudo: empty holding node                                             */
    LIST_NODE_INIT(pstrMutex->strHoldList);

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: return none error                                              */
    return KER_enuERR_OK;
}

/*******************************************************************************
* @Brief  : pend a mutex object.
* @Inputs : pstrMutex: mutex object valid pointer.
*           u32TimeOut: pending timeout value. if equal zero, pending until
*                       mutex is released. otherwise, pending until timeout.
* @Outputs: none
* @Returns: KER_enuERR_OK: pend mutex successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: IPC object pointer invalid.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_IPC_NESTING_HOLD: mutex nesting hold.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMutexPend(KER_tstrMutex *pstrMutex,INT32U u32TimeOut)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if mutex object is valid                                 */
    if (KER_NULL == pstrMutex)
    {
        /*!Pseudo: ipc object is invalid                                      */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMutex->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if other task hold this mutex ojbect                     */
    if (KER_NULL == pstrMutex->pstrOwnerTCB)
    {
        /*!Pseudo: no other task hold this mutex object, take mutex           */
        /*!Pseudo: fill mutex is holded information                           */
        pstrMutex->u8HoldCnt                = (INT8U)1;
        pstrMutex->pstrOwnerTCB             = KER_pstrCurTCB;
        pstrMutex->u16OwnerOrignalPriority  = KER_pstrCurTCB->u16CurPriority;

        /*!Pseudo: insert mutex object into tcb mutex hold list               */
        LIST_INSERT_HEAD(KER_pstrCurTCB->strMutexHoldList,
                         pstrMutex->strHoldList);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return without error                                       */
        return KER_enuERR_OK;
    }
    else if (KER_pstrCurTCB == pstrMutex->pstrOwnerTCB)
    {
        /*!Pseudo: current task nesting pend mutex, increase hold coutner     */
        pstrMutex->u8HoldCnt++;

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object nesting hold error                  */
        return KER_enuERR_IPC_NESTING_HOLD;
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

        /*!Pseudo: pend mutex object                                          */
        KER_vidObjectPend( pstrMutex,
                           KER_enuIPC_OBJ_TYPE_MUTEX,
                           pstrMutex->enuIPCMethod,
                           u32TimeOut);

        /*!Pseudo: if mutex owner priority, inherit current task priority     */
        if (  KER_pstrCurTCB->u16CurPriority
            < pstrMutex->pstrOwnerTCB->u16CurPriority)
        {
            /*!Pseudo: mutex owner inherit current task priority              */
            vidInheritPriority(pstrMutex->pstrOwnerTCB,
                               KER_pstrCurTCB->u16CurPriority);
        }
        else
        {
            /*!Pseudo: do nothing!!!                                          */
        }
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: schedule to other ready task                               */
        KER_vidSched();

        return KER_pstrCurTCB->enuError;
    }
}

#if KER_coIPC_MUTEX_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend a mutex object.
* @Inputs : pstrMutex: mutex object valid pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: pend mutex successful.
*           KER_enuERR_NOK: try to pend mutex failed.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: IPC object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
*           KER_enuERR_IPC_NESTING_HOLD: mutex nesting hold.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMutexTryPend(KER_tstrMutex *pstrMutex)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if mutex object is valid                                 */
    if (KER_NULL == pstrMutex)
    {
        /*!Pseudo: ipc object is invalid                                      */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMutex->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if other task hold this mutex ojbect                     */
    if (KER_NULL == pstrMutex->pstrOwnerTCB)
    {
        /*!Pseudo: no other task hold this mutex object, take mutex           */
        /*!Pseudo: fill mutex is holded information                           */
        pstrMutex->u8HoldCnt                = (INT8U)1;
        pstrMutex->pstrOwnerTCB             = KER_pstrCurTCB;
        pstrMutex->u16OwnerOrignalPriority  = KER_pstrCurTCB->u16CurPriority;

        /*!Pseudo: insert mutex object into tcb mutex hold list               */
        LIST_INSERT_HEAD(KER_pstrCurTCB->strMutexHoldList,
                         pstrMutex->strHoldList);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return without error                                       */
        return KER_enuERR_OK;
    }
    else if (KER_pstrCurTCB == pstrMutex->pstrOwnerTCB)
    {
        /*!Pseudo: current task nesting pend mutex, increase hold coutner     */
        pstrMutex->u8HoldCnt++;

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object nesting hold error                  */
        return KER_enuERR_IPC_NESTING_HOLD;
    }
    else
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return , try pend mutex failed                             */
        return KER_enuERR_NOK;
    }

}
#endif

/*******************************************************************************
* @Brief  : post mutex
* @Inputs : pstrMutex: mutex object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: post with none error.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: mutex object invalid.
*           KER_enuERR_IPC_NOT_OWNER: poster not the mutex owner.
*           KER_enuERR_IPC_NESTING_HOLD: nesting hold, should nesting post.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMutexPost( KER_tstrMutex *pstrMutex )
{
    KER_CPU_SR_ALLOC();
    KER_tstrList *pstrPendNode;
    KER_tstrTCB  *pstrTCB;

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if mutex object is valid                                 */
    if (KER_NULL == pstrMutex)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMutex->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if current task is the owner of this mutex object        */
    if (KER_pstrCurTCB != pstrMutex->pstrOwnerTCB)
    {
        /*!Pseudo: before return, should leave critical section               */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with current task not mutex owner error             */
        return KER_enuERR_IPC_NOT_OWNER;
    }

    /*!Pseudo: decrease nesting hold mutex counter                            */
    pstrMutex->u8HoldCnt--;

    /*!Pseudo: check if is still nesting hold mutex, then return              */
    if (pstrMutex->u8HoldCnt > (INT8U)0)
    {
        KER_LEAVE_CRITICAL();
        /*!Pseudo: still nesting hold mutex, return                           */
        return KER_enuERR_IPC_NESTING_HOLD;
    }

    /*!Pseudo: check if need restore task priority                            */
    if (pstrMutex->u16OwnerOrignalPriority != KER_pstrCurTCB->u16CurPriority)
    {
        /*!Pseudo: restore priority and insert to the new ready list          */
        KER_vidRdyListRemove(KER_pstrCurTCB);
        KER_pstrCurTCB->u16CurPriority = pstrMutex->u16OwnerOrignalPriority;
        KER_vidRdyListInsertToTail(KER_pstrCurTCB);
    }

    /*!Pseudo: remove this mutex from tcb mutex hold list                     */
    LIST_REMOVE_NODE(pstrMutex->strHoldList);
    /*!Pseudo: release mutex, indicate no task hold this mutex                */
    pstrMutex->pstrOwnerTCB = KER_NULL;

    /*!Pseudo: if no task pending, return directly                            */
    if (LIST_IS_EMPTY(pstrMutex->strPendList))
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();
        /*!Pseudo: no task is pending, return directly                        */
        return KER_enuERR_OK;
    }


    /*!Comment: if run to here, means has tasks in pending list.              */
    /*!Comment: should release head task from the pending list                */

    /*!Pseudo: get head node from pending list                                */
    pstrPendNode = LIST_GET_HEAD(pstrMutex->strPendList);
    pstrTCB      = LIST_NODE_ENTRY_PTR(pstrPendNode, KER_tstrTCB, strList);

    /*!Pseudo: abort pending mutex object                                     */
    KER_vidObjectPendAbort(pstrTCB, KER_enuERR_OK);

    /*!Pseudo: backup task orignal priority into mutex                        */
    pstrMutex->u16OwnerOrignalPriority  = pstrTCB->u16CurPriority;
    /*!Pseudo: set mutex is hold by the new task                              */
    pstrMutex->pstrOwnerTCB             = pstrTCB;
    /*!Pseudo: reset the hold counter to one                                  */
    pstrMutex->u8HoldCnt                = (INT8U)1;

    /*!Pseudo: insert mutex object into tcb mutex hold list                   */
    LIST_INSERT_HEAD(pstrTCB->strMutexHoldList, pstrMutex->strHoldList);

    /*!Pseudo: if task is ready to running, do schedule                       */
    if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State)
    {
        /*!Pseudo: post mutex finished, leave critical section                */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: schedule, for the new ready task has opportunity to run    */
        KER_vidSched();
    }
    else
    {
        /*!Pseudo: post mutex finished, leave critical section                */
        KER_LEAVE_CRITICAL();
    }

    /*!Pseudo: return none error                                              */
    return KER_enuERR_OK;
}

#if KER_coIPC_MUTEX_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pend the specify mutex.
* @Inputs : pstrMutex: mutex object pointer.
*           pstrTCB: tcb pointer.
*           enuOption: schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: Mutex object invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMutexPendAbort( KER_tstrMutex *pstrMutex,
                                          KER_tstrTCB   *pstrTCB,
                                          KER_tenuSchedOpt enuOption )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if mutex object is valid                                 */
    if (KER_NULL == pstrMutex)
    {
        /*!Pseudo: return with ipc object invalid error                       */
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

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMutex->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if pending this mutex                                    */
    if (((pstrTCB->strIPCObjPend).enuIPCObjType != KER_enuIPC_OBJ_TYPE_MUTEX)
        || ((pstrTCB->strIPCObjPend).vidpIPCObject != pstrMutex))
    {
        KER_LEAVE_CRITICAL();
        /*!Pseudo: return error not pending this mutex object                 */
        return KER_enuERR_IPC_NOT_PEND_THIS;
    }

    /*!Pseudo: abort pending mutex object                                     */
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

#if KER_coIPC_MUTEX_ABORT_ALL_EN > 0u
/*******************************************************************************
* @Brief  : abort all tasks pend the specify mutex.
* @Inputs : pstrMutex: mutex object pointer.
*           enuOption: schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort all pending tasks successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: Mutex object invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMutexPendAbortAll( KER_tstrMutex *pstrMutex,
                                             KER_tenuSchedOpt enuOption)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if mutex object is valid                                 */
    if (KER_NULL == pstrMutex)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMutex->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: remove all pending task,set ipc object pending abort error flag*/
    KER_vidPendListRemoveAll(&(pstrMutex->strPendList),
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

#if KER_coIPC_MUTEX_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete mutex object
* @Inputs : pstrMutex: mutex object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: mutex object delete successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: mutex object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object pointer deleted.
*           KER_enuERR_TASK_STATE_INV: pending task state invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMutexDelete( KER_tstrMutex *pstrMutex)
{
    KER_CPU_SR_ALLOC();
    KER_tstrList *pstrNode;
    KER_tstrTCB  *pstrTCB;
    KER_tstrMutex *pstrPrevMutex;

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if mutex object is valid                                 */
    if (KER_NULL == pstrMutex)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMutex->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    pstrTCB = pstrMutex->pstrOwnerTCB;

    /*!Pseduo: treat mutex owner                                              */
    if (pstrTCB)
    {
        /*!Pseudo: remove owner                                               */
        switch (pstrTCB->u8State)
        {
            case KER_u8TSK_STAT_RDY:
                /*!Pseudo: get previous mutex node                            */
                pstrNode = (pstrMutex->strHoldList).pstrPrev;
                /*!Pseudo: check if present previous mutex holding            */
                if (pstrNode == &(pstrTCB->strMutexHoldList))
                {
                    /*!Pseduo: previous mutex not present.
                     *         restore task orignal priority                  */
                    if (pstrMutex->u16OwnerOrignalPriority
                        != pstrTCB->u16CurPriority)
                    {
                        /*!Pseudo: restore task priority,
                         *         insert to new ready list                   */
                        KER_vidRdyListRemove(pstrTCB);

                        pstrTCB->u16CurPriority
                                          = pstrMutex->u16OwnerOrignalPriority;

                        KER_vidRdyListInsertToHead(pstrTCB);
                    }
                }
                else
                {
                    /*!Pseudo: get previous mutex object pointer              */
                    pstrPrevMutex = LIST_NODE_ENTRY(pstrNode,
                                                    KER_tstrMutex,
                                                    strHoldList);
                    /*!Pseudo: change previous mutex owner orignal priority   */
                    pstrPrevMutex->u16OwnerOrignalPriority =
                                            pstrMutex->u16OwnerOrignalPriority;
                }

                LIST_REMOVE_NODE(pstrMutex->strHoldList);

                break;
            case KER_u8TSK_STAT_DLY:
#if KER_coTASK_SUSPEND_EN > 0u
            case KER_u8TSK_STAT_SPD:
            case KER_u8TSK_STAT_SPD_DLY:
#endif
                /*!Pseudo: get previous mutex node                            */
                pstrNode = (pstrMutex->strHoldList).pstrPrev;
                /*!Pseudo: check if present previous mutex holding            */
                if (pstrNode == &(pstrTCB->strMutexHoldList))
                {
                    /*!Pseduo: previous mutex not present.
                     *         restore task orignal priority                  */
                    if (pstrMutex->u16OwnerOrignalPriority
                        != pstrTCB->u16CurPriority)
                    {
                        /*!Pseudo: restore task priority                      */
                        pstrTCB->u16CurPriority
                                          = pstrMutex->u16OwnerOrignalPriority;
                    }
                }
                else
                {
                    /*!Pseudo: get previous mutex object pointer              */
                    pstrPrevMutex = LIST_NODE_ENTRY(pstrNode,
                                                    KER_tstrMutex,
                                                    strHoldList);
                    /*!Pseudo: change previous mutex owner orignal priority   */
                    pstrPrevMutex->u16OwnerOrignalPriority =
                                            pstrMutex->u16OwnerOrignalPriority;
                }

                LIST_REMOVE_NODE(pstrMutex->strHoldList);

                break;
            case KER_u8TSK_STAT_BLK:
            case KER_u8TSK_STAT_BLK_DLY:
#if KER_coTASK_SUSPEND_EN > 0u
            case KER_u8TSK_STAT_SPD_BLK:
            case KER_u8TSK_STAT_SPD_BLK_DLY:
#endif
                /*!Pseudo: get previous mutex node                            */
                pstrNode = (pstrMutex->strHoldList).pstrPrev;
                /*!Pseudo: check if present previous mutex holding            */
                if (pstrNode == &(pstrTCB->strMutexHoldList))
                {
                    /*!Pseduo: previous mutex not present.
                     *         restore task orignal priority                  */
                    if (pstrMutex->u16OwnerOrignalPriority
                        != pstrTCB->u16CurPriority)
                    {
                        /*!Pseudo: restore task priority                      */
                        vidInheritPriority(pstrTCB,
                                           pstrMutex->u16OwnerOrignalPriority);
                    }
                }
                else
                {
                    /*!Pseudo: get previous mutex object pointer              */
                    pstrPrevMutex = LIST_NODE_ENTRY(pstrNode,
                                                    KER_tstrMutex,
                                                    strHoldList);
                    /*!Pseudo: change previous mutex owner orignal priority   */
                    pstrPrevMutex->u16OwnerOrignalPriority =
                                            pstrMutex->u16OwnerOrignalPriority;
                }

                LIST_REMOVE_NODE(pstrMutex->strHoldList);
                break;
            default:
                /*!Pseudo: should never run to here, task state invalid       */
                KER_LEAVE_CRITICAL();
                KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
                return KER_enuERR_TASK_STATE_INV;
        }

        /*!Pseudo: remove all pending task, set ipc object deleted error flag */
        KER_vidPendListRemoveAll(&(pstrMutex->strPendList),
                                 KER_enuERR_IPC_OBJ_DELETED);

        /*!Pseudo: set mutex object invalid                                   */
        pstrMutex->enuIPCMethod = KER_enuIPC_METHOD_UNKNOWN;

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: do schedule                                                */
        KER_vidSched();

        return KER_enuERR_OK;

    }
    else
    {
        /*!Pseudo: no onwer, no pending task, set mutex object invalid        */
        pstrMutex->enuIPCMethod = KER_enuIPC_METHOD_UNKNOWN;

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return none error                                          */
        return KER_enuERR_OK;
    }
}
#endif

#endif
