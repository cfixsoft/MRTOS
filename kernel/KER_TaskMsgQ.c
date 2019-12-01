/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_TaskMsgQ.c
*Version    : V1.00
*Description: task built in message queue module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/24
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coBUILT_IN_MSGQ_EN > 0u

/*******************************************************************************
* @Brief  : initial task built in message queue.
* @Inputs : pstrMsgPoolObj: message queue pool object.
*           vidpMsgPool: message queue buffer pool.
*           u32MsgPoolSize: message buffer pool total size.
*           u16MsgSize: set one message maximum size.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial ok.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_POOL_INV: message pool invalid.
*           KER_enuERR_MQ_POOL_SZ_INV: message pool size invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
* @Note   : if task use built in message queue, should be call this function to
*           initial built in message queue object before create the task.
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskMsgQInit(KER_tstrMsgPool *pstrMsgPoolObj,
                                       void            *vidpMsgPool,
                                       INT32U           u32MsgPoolSize,
                                       INT16U           u16MsgSize)
{

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        /*!Pseudo: return interrupt calling error                             */
        return KER_enuERR_ISR_FAULT_USING;
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

    /*!Pseudo: initial message pool                                           */
    KER_vidMsgPoolInit(pstrMsgPoolObj, vidpMsgPool, u32MsgPoolSize, u16MsgSize);

    return KER_enuERR_OK;
}

/*******************************************************************************
* @Brief  : pending task built in message queue.
* @Inputs : enuRecvMethod: receive message queue front or rear.
*           u32TimeOut: pending timeout value.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: received message size output pointer.
* @Returns: KER_enuERR_OK: pending built in message queue successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_NO_BUILT_IN_MQ: task not have built in MSG_Q.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskMsgQPend( void *vidpMsg,
                                        INT16U *pu16Size,
                                        KER_tenuMsgQRecvOpt enuRecvMethod,
                                        INT32U u32TimeOut)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        /*!Pseudo: return interrupt calling error                             */
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: task should not pending not configured built in message queue  */
    if (KER_NULL == KER_pstrCurTCB->pstrBuiltInMsgPool)
    {
        return KER_enuERR_MQ_NO_BUILT_IN_MQ;
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

    if (KER_u16MsgPoolMsgNbrQuery(KER_pstrCurTCB->pstrBuiltInMsgPool)>(INT16U)0)
    {
        (void)KER_enuMsgPoolRead( KER_pstrCurTCB->pstrBuiltInMsgPool,
                                  vidpMsg,
                                  pu16Size,
                                  enuRecvMethod);
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

        /*!Pseudo: set the receiver buffer pointer                            */
        KER_pstrCurTCB->vidpMsg = vidpMsg;
        KER_pstrCurTCB->u16MsgSize = (INT16U)0;

        /*!Pseudo: set pending built in message quque                         */
        KER_vidBuiltInObjectPend(KER_enuIPC_OBJ_TYPE_BUILT_IN_MSG_Q,
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

#if KER_coBUILT_IN_MSGQ_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend task built in message queue.
* @Inputs : enuRecvMethod: receive message queue front or rear.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: received message size output pointer.
* @Returns: KER_enuERR_OK: pending built in message queue successful.
*           KER_enuERR_NOK: built in message queue empty, not received message.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_NO_BUILT_IN_MQ: task not have built in MSG_Q.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskMsgQTryPend(void *vidpMsg,
                                          INT16U *pu16Size,
                                          KER_tenuMsgQRecvOpt enuRecvMethod)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: task should not pending not configured built in message queue  */
    if (KER_NULL == KER_pstrCurTCB->pstrBuiltInMsgPool)
    {
        return KER_enuERR_MQ_NO_BUILT_IN_MQ;
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
    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if message pool has message                              */
    if (KER_u16MsgPoolMsgNbrQuery(KER_pstrCurTCB->pstrBuiltInMsgPool)>(INT16U)0)
    {
        /*!Pseudo: read message                                               */
        (void)KER_enuMsgPoolRead( KER_pstrCurTCB->pstrBuiltInMsgPool,
                                  vidpMsg,
                                  pu16Size,
                                  enuRecvMethod);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with message received                               */
        return KER_enuERR_OK;
    }
    else
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return without message received                            */
        return KER_enuERR_NOK;
    }
}
#endif

/*******************************************************************************
* @Brief  : post message to task.
* @Inputs : pstrTCB: task tcb pointer invalid.
*           vidpMsg: message buffer.
*           u16Size: message size.
*           enuSendMethod: post to message queue front or rear.
* @Outputs: none
* @Returns: KER_enuERR_OK: post message to task successful.
*           KER_enuERR_TCB_PTR_INV: task tcb invalid.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
*           KER_enuERR_MQ_SEND_OPT_UNKNOWN: send method invalid.
*           KER_enuERR_MQ_FULL: message queue buffer full.
*           KER_enuERR_MQ_NO_BUILT_IN_MQ: task not have built in MSG_Q.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskMsgQPost( KER_tstrTCB *pstrTCB,
                                        void *vidpMsg,
                                        INT16U u16Size,
                                        KER_tenuMsgQSendOpt enuSendMethod)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if pstrTCB pointer is valid                              */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

    /*!Pseudo: not allow post to a not configured built in message queue task */
    if (KER_NULL == KER_pstrCurTCB->pstrBuiltInMsgPool)
    {
        return KER_enuERR_MQ_NO_BUILT_IN_MQ;
    }

    /*!Pseudo: check if send buffer is valid                                  */
    if (KER_NULL == vidpMsg)
    {
        /*!Pseudo: send buffer is invalid                                     */
        return KER_enuERR_MQ_MSG_BUF_PTR_INV;
    }

    /*!Pseudo: check if send size is in range                                 */
    if (((INT16U)0 == u16Size)
       || (u16Size > (KER_pstrCurTCB->pstrBuiltInMsgPool->u16MsgSize)))
    {
        /*!Pseudo: invalid size                                               */
        return KER_enuERR_MQ_MSG_SZ_INV;
    }

    /*!Pseudo: check if send method is valid                                  */
    if (!MSG_Q_SEND_METHOD_IS_VALID(enuSendMethod))
    {
        /*!Pseudo: return message send method invalid error                   */
        return KER_enuERR_MQ_SEND_OPT_UNKNOWN;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if task pending built in message queue object            */
    if (KER_enuIPC_OBJ_TYPE_BUILT_IN_MSG_Q ==
       (pstrTCB->strIPCObjPend).enuIPCObjType)
    {
        /*!Pseudo: abort pending                                              */
        KER_vidBuiltInObjectPendAbort(pstrTCB, KER_enuERR_OK);

        /*!Pseudo: copy message to task buffer                                */
        KER_vidMsgPoolMsgCopy(pstrTCB->vidpMsg,vidpMsg,u16Size);

        /*!Pseudo: set received message size                                  */
        pstrTCB->u16MsgSize = u16Size;

        /*!Pseudo: if task is ready to running, do a schedule                 */
        if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State)
        {
            /*!Pseudo: post message finished, leave critical section          */
            KER_LEAVE_CRITICAL();
            /*!Pseudo: schedule                                               */
            KER_vidSched();
        }
        else
        {
            /*!Pseudo: post message finished, leave critical section          */
            KER_LEAVE_CRITICAL();
        }

        return KER_enuERR_OK;

    }

    /*!Pseudo: check message queue is not full                                */
    if (KER_bMsgPoolIsFull(KER_pstrCurTCB->pstrBuiltInMsgPool) == FALSE)
    {
        /*!Pseudo: store message to message pool                              */
        (void)KER_enuMsgPoolWrite( pstrTCB->pstrBuiltInMsgPool,
                                   vidpMsg,
                                   u16Size,
                                   enuSendMethod);
        KER_LEAVE_CRITICAL();

        return KER_enuERR_OK;
    }
    else
    {
        /*!Pseudo: message queue is full                                      */
        KER_LEAVE_CRITICAL();
        return KER_enuERR_MQ_FULL;
    }
}

#if KER_coBUILT_IN_MSGQ_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort pending built in message queue.
* @Inputs : none
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending successful.
*           KER_enuERR_TCB_PTR_INV: tcb pointer invalid.
*           KER_enuERR_IPC_NOT_PEND_THIS: not pending built in message queue.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskMsgQPendAbort ( KER_tstrTCB        *pstrTCB,
                                              KER_tenuSchedOpt enuOption)
{
    return KER_enuTaskBuiltInObjPendAbort(pstrTCB,
                                          KER_enuIPC_OBJ_TYPE_BUILT_IN_MSG_Q,
                                          enuOption);
}
#endif

#endif
