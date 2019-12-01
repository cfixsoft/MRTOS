/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_MsgQ.c
*Version    : V1.00
*Description: MRTOS message queue management module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/5
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*!Comment: using message queue module compiler control                       */
#if KER_coIPC_MSGQ_EN > 0u

/*******************************************************************************
* @Brief  : initial message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
*           enuIPCMethod: ipc pending list method.
*           vidpMsgPool: message pool pointer.
*           u32MsgPoolSize: message pool size.
*           u16MsgSize: each message size.
*           ps8Name: message queue object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial message queue successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_METHOD_INV: ipc method invalid.
*           KER_enuERR_MQ_POOL_INV: message pool invalid.
*           KER_enuERR_MQ_POOL_SZ_INV: message pool size invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgQInit(KER_tstrMsgQ      *pstrMsgQ,
                                   KER_tenuIPCMethod  enuIPCMethod,
                                   void              *vidpMsgPool,
                                   INT32U             u32MsgPoolSize,
                                   INT16U             u16MsgSize,
                                   INT8S             *ps8Name)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if message queue object is valid                         */
    if (KER_NULL == pstrMsgQ)
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

    /*!Pseudo: check if message pool is valid                                 */
    if (KER_NULL == vidpMsgPool)
    {
        /*!Pseudo: return message pool is invalid error                       */
        return KER_enuERR_MQ_POOL_INV;
    }

    /*!Pseudo: check if message pool size is invalid                          */
    if ((INT32U)0 == u32MsgPoolSize)
    {
        /*!Pseudo: message pool size is invalid error                         */
        return KER_enuERR_MQ_POOL_SZ_INV;
    }

    /*!Pseudo: check if message size is invalid                               */
    if ((INT16U)0 == u16MsgSize)
    {
        /*!Pseudo: message size is invalid                                    */
        return KER_enuERR_MQ_MSG_SZ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: set message queue object name                                  */
    pstrMsgQ->ps8Name = ps8Name;

    /*!Pseudo: initial pending list                                           */
    LIST_INIT(pstrMsgQ->strPendList);

    /*!Pseudo: set ipc pending list schedule method                           */
    pstrMsgQ->enuIPCMethod = enuIPCMethod;

    /*!Pseudo: initial message pool                                           */
    KER_vidMsgPoolInit( &(pstrMsgQ->strMsgPool),
                          vidpMsgPool,
                          u32MsgPoolSize,
                          u16MsgSize);

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;

}

/*******************************************************************************
* @Brief  : pend message queue message.
* @Inputs : pstrMsgQ: message queue object.
*           enuRecvMethod: receive from message queue front or rear.
*           u32TimeOut: pending timeout value.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: message received size.
* @Returns: KER_enuERR_OK: pending message successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: receive buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: message queue object deleted.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgQPend( KER_tstrMsgQ *pstrMsgQ,
                                    void *vidpMsg,
                                    INT16U *pu16Size,
                                    KER_tenuMsgQRecvOpt enuRecvMethod,
                                    INT32U u32TimeOut)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if message queue object is valid                         */
    if (KER_NULL == pstrMsgQ)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

    /*!Pseudo: check if vidpMsg pointer is invalid                            */
    if (KER_NULL == vidpMsg)
    {
        /*!Pseudo: return invalid message receive pointer invalid error       */
        return KER_enuERR_MQ_MSG_BUF_PTR_INV;
    }

    /*!Pseudo: check if message receive size pointer invalid error            */
    if (KER_NULL == pu16Size)
    {
        /*!Pseudo: return message receive size pointer invalid error          */
        return KER_enuERR_MQ_MSG_SZ_PTR_INV;
    }

    /*!Pseudo: check if message queue receive method is valid                 */
    if (!MSG_Q_RECV_METHOD_IS_VALID(enuRecvMethod))
    {
        /*!Pseudo: return message receive mehtod invalid error                */
        return KER_enuERR_MQ_RECV_OPT_UNKNOWN;
    }

#endif

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if message queue object is using                         */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMsgQ->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if message queue have messages                           */
    if (KER_u16MsgPoolMsgNbrQuery(&(pstrMsgQ->strMsgPool)) > (INT16U)0)
    {
        /*!Pseudo: read one message from message pool                         */
        (void)KER_enuMsgPoolRead( &(pstrMsgQ->strMsgPool),
                                    vidpMsg,
                                    pu16Size,
                                    enuRecvMethod);

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

        /*!Pseudo: set the receiver buffer pointer                            */
        KER_pstrCurTCB->vidpMsg = vidpMsg;
        KER_pstrCurTCB->u16MsgSize = (INT16U)0;

        /*!Pseudo: pend message queue object                                  */
        KER_vidObjectPend( pstrMsgQ,
                           KER_enuIPC_OBJ_TYPE_MSG_Q,
                           pstrMsgQ->enuIPCMethod,
                           u32TimeOut);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: schedule to other ready task                               */
        KER_vidSched();

        /*!Pseudo: enter critical section                                     */
        KER_ENTER_CRITICAL();

        /*!Pseudo: set received message size(if not received is zero)         */
        *pu16Size = KER_pstrCurTCB->u16MsgSize;

        /*!Pseudo: reset message receiver buffer pointer                      */
        KER_pstrCurTCB->vidpMsg = KER_NULL;

        /*!Pseudo: reset message received size                                */
        KER_pstrCurTCB->u16MsgSize = (INT16U)0;

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return event receive status                                */
        return KER_pstrCurTCB->enuError;
    }
}

#if KER_coIPC_MSGQ_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend message queue message.
* @Inputs : pstrMsgQ: message queue object.
*           enuRecvMethod: receive from message queue front or rear.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: message received size.
* @Returns: KER_enuERR_OK: pending message successful.
*           KER_enuERR_NOK: message queue is empty, not received message.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: receive buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: message queue object deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgQTryPend( KER_tstrMsgQ *pstrMsgQ,
                                       void *vidpMsg,
                                       INT16U *pu16Size,
                                       KER_tenuMsgQRecvOpt enuRecvMethod)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if message queue object is valid                         */
    if (KER_NULL == pstrMsgQ)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

    /*!Pseudo: check if vidpMsg pointer is invalid                            */
    if (KER_NULL == vidpMsg)
    {
        /*!Pseudo: return invalid message receive pointer invalid error       */
        return KER_enuERR_MQ_MSG_BUF_PTR_INV;
    }

    /*!Pseudo: check if message receive size pointer invalid error            */
    if (KER_NULL == pu16Size)
    {
        /*!Pseudo: return message receive size pointer invalid error          */
        return KER_enuERR_MQ_MSG_SZ_PTR_INV;
    }

    /*!Pseudo: check if message queue receive method is valid                 */
    if (!MSG_Q_RECV_METHOD_IS_VALID(enuRecvMethod))
    {
        /*!Pseudo: return message receive mehtod invalid error                */
        return KER_enuERR_MQ_RECV_OPT_UNKNOWN;
    }

#endif

    KER_ENTER_CRITICAL();

    /*!Pseudo: check if message queue object is using                         */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMsgQ->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if message queue have messages                           */
    if (KER_u16MsgPoolMsgNbrQuery(&(pstrMsgQ->strMsgPool)) > (INT16U)0)
    {
        /*!Pseudo: read one message from message pool                         */
        (void)KER_enuMsgPoolRead( &(pstrMsgQ->strMsgPool),
                                    vidpMsg,
                                    pu16Size,
                                    enuRecvMethod);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        return KER_enuERR_OK;
    }
    else
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        return KER_enuERR_NOK;
    }

}
#endif

/*******************************************************************************
* @Brief  : post one message to message queue.
* @Inputs : pstrMsgQ: message queue object pointer.
*           vidpMsg: message send buffer.
*           u16Size: send size.
*           enuSendMethod: send method.
* @Outputs: none
* @Returns: KER_enuERR_OK: send message successful.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: message queue object deleted.
*           KER_enuERR_MQ_FULL: message queue is full.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_SEND_OPT_UNKNOWN: send method invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgQPost( KER_tstrMsgQ *pstrMsgQ,
                                    void *vidpMsg,
                                    INT16U u16Size,
                                    KER_tenuMsgQSendOpt enuSendMethod)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if message queue object is valid                         */
    if (KER_NULL == pstrMsgQ)
    {
        /*!Pseudo: ipc object invalid                                         */
        return KER_enuERR_IPC_OBJ_INV;
    }

    /*!Pseudo: check if send method is valid                                  */
    if (!MSG_Q_SEND_METHOD_IS_VALID(enuSendMethod))
    {
        /*!Pseudo: return message send method invalid error                   */
        return KER_enuERR_MQ_SEND_OPT_UNKNOWN;
    }

    /*!Pseudo: check if send buffer is valid                                  */
    if (KER_NULL == vidpMsg)
    {
        /*!Pseudo: send buffer is invalid                                     */
        return KER_enuERR_MQ_MSG_BUF_PTR_INV;
    }

    /*!Pseudo: check if send size is in range                                 */
    if (((INT16U)0 == u16Size) || (u16Size > (pstrMsgQ->strMsgPool).u16MsgSize))
    {
        /*!Pseudo: invalid size                                               */
        return KER_enuERR_MQ_MSG_SZ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if message queue object is valid to using                */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMsgQ->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check message queue is not full                                */
    if (KER_bMsgPoolIsFull(&(pstrMsgQ->strMsgPool)) == FALSE)
    {
        /*!Pseudo: check if have tasks pending messge queue                   */
        if (LIST_IS_EMPTY(pstrMsgQ->strPendList))
        {
            /*!Pseudo: store message to message pool                          */
            (void)KER_enuMsgPoolWrite( &(pstrMsgQ->strMsgPool),
                                       vidpMsg,
                                       u16Size,
                                       enuSendMethod);
            KER_LEAVE_CRITICAL();
        }
        else
        {
            /*!Comment: wakeup pending task, copy message to task buffer      */
            KER_tstrList *pstrNode;
            KER_tstrTCB  *pstrTCB;

            /*!Pseudo: get pending list head task                             */
            pstrNode = LIST_GET_HEAD(pstrMsgQ->strPendList);
            pstrTCB  = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrTCB, strList);

            /*!Pseudo: abort pending message queue object                     */
            KER_vidObjectPendAbort(pstrTCB, KER_enuERR_OK);

            /*!Pseudo: copy message to task buffer                            */
            KER_vidMsgPoolMsgCopy(pstrTCB->vidpMsg,vidpMsg,u16Size);

            /*!Pseudo: set received message size                              */
            pstrTCB->u16MsgSize = u16Size;

            /*!Pseudo: if task is ready to running, do schedule               */
            if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State)
            {
                /*!Pseudo: post message finished, leave critical section      */
                KER_LEAVE_CRITICAL();

                /*!Pseudo: schedule                                           */
                KER_vidSched();
            }
            else
            {
                /*!Pseudo: post message finished, leave critical section      */
                KER_LEAVE_CRITICAL();
            }
        }

        return KER_enuERR_OK;
    }
    else
    {
        /*!Pseudo: message queue is full                                      */
        KER_LEAVE_CRITICAL();
        return KER_enuERR_MQ_FULL;
    }
}

#if KER_coIPC_MSGQ_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort a task pending the specify message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
*           pstrTCB: task tcb pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending task successful.
*           KER_enuERR_IPC_OBJ_INV: object pointer invalid.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: object already deleted.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending this object.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgQPendAbort( KER_tstrMsgQ       *pstrMsgQ,
                                         KER_tstrTCB        *pstrTCB,
                                         KER_tenuSchedOpt enuOption)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if message queue object is valid                         */
    if (KER_NULL == pstrMsgQ)
    {
        /*!Pseudo: ipc object is invalid                                      */
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

    /*!Pseudo: check if message queue object is deleted                       */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMsgQ->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if task already pending this message queue object        */
    if (((pstrTCB->strIPCObjPend).enuIPCObjType != KER_enuIPC_OBJ_TYPE_MSG_Q)
        || ((pstrTCB->strIPCObjPend).vidpIPCObject != pstrMsgQ))
    {
        KER_LEAVE_CRITICAL();
        /*!Pseudo: return error not pending this object                       */
        return KER_enuERR_IPC_NOT_PEND_THIS;
    }

    /*!Pseudo: abort pending message queue object                             */
    KER_vidObjectPendAbort(pstrTCB, KER_enuERR_IPC_PEND_ABORT);

    /*!Pseudo: if task is ready to running                                    */
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

#if KER_coIPC_MSGQ_ABORT_ALL_EN > 0u
/*******************************************************************************
* @Brief  : abort all tasks pending the specify message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort all pending tasks successful.
*           KER_enuERR_IPC_OBJ_INV: object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgQPendAbortAll(KER_tstrMsgQ        *pstrMsgQ,
                                            KER_tenuSchedOpt enuOption)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if message object is valid                               */
    if (KER_NULL == pstrMsgQ)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if message queue object is deleted                       */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMsgQ->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: remove all pending task,set ipc object pending abort error flag*/
    KER_vidPendListRemoveAll(&(pstrMsgQ->strPendList),
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

#if KER_coIPC_MSGQ_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete a message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: deleted successful.
*           KER_enuERR_IPC_OBJ_INV: object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgQDelete( KER_tstrMsgQ *pstrMsgQ )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if message queue object is valid                         */
    if (KER_NULL == pstrMsgQ)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if message queue object is deleted                       */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrMsgQ->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: remove all pending task,set ipc object pending abort error flag*/
    KER_vidPendListRemoveAll(&(pstrMsgQ->strPendList),
                               KER_enuERR_IPC_OBJ_DELETED);

    /*!Pseudo: no onwer, no pending task, set mutex object invalid            */
    pstrMsgQ->enuIPCMethod = KER_enuIPC_METHOD_UNKNOWN;

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: for the new ready task has opportunity to run                  */
    KER_vidSched();

    return KER_enuERR_OK;

}
#endif

#endif

