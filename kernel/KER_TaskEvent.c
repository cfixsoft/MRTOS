/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_TaskEvent.c
*Version    : V1.00
*Description: task built in event module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/24
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coBUILT_IN_EVENT_EN > 0u

/*******************************************************************************
* @Brief  : pending task self built in events.
* @Inputs : evtMask: mask expected received events.
*           enuRecvMethod: receive method front or rear.
*           u32TimeOut: pending timeout value.
* @Outputs: pevtEvent: event receiver pointer.
* @Returns: KER_enuERR_OK: pend event successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_EVENT_RECV_PTR_INV: event receive pointer invalid
*           KER_enuERR_SCHED_LOCK_PENDING: scheduler locked, not allow pending.
*           KER_enuERR_IPC_OBJ_DELETED: event object deleted.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: unknown receive method option.
*           KER_enuERR_EVT_MASK_INV: event mask invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventPend( EVENT_TYPE             *pevtEvent,
                                         EVENT_TYPE              evtMask,
                                         KER_tenuEvtRecvOpt enuRecvMethod,
                                         INT32U                  u32TimeOut)
{
    CPU_REG BOOL bStatus = FALSE;
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function.                              */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if event receiver is valid                               */
    if (KER_NULL == pevtEvent)
    {
        /*!Pseudo: event receiver is invalid                                  */
        return KER_enuERR_EVT_RECV_PTR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

#if KER_coBUILT_IN_EVENT_MASK_EN > 0u

    /*!Pseudo: check if mask is invalid                                       */
    if ((KER_pstrCurTCB->evtBuiltInEventMask & evtMask) != evtMask)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_EVT_MASK_INV;
    }

#endif

    /*!Pseudo: check events received met condition                            */
    switch (enuRecvMethod)
    {
        case KER_enuEVT_RECV_OPT_OR:
        case KER_enuEVT_RECV_OPT_OR_CLR:
            /*!Pseudo: check events received AND logic condition is met       */
            if (KER_pstrCurTCB->evtBuiltInEvent & evtMask)
            {
                bStatus= TRUE;
            }
            break;
        case KER_enuEVT_RECV_OPT_AND:
        case KER_enuEVT_RECV_OPT_AND_CLR:
            /*!Pseudo: check events received OR logic condition is met        */
            if ((KER_pstrCurTCB->evtBuiltInEvent & evtMask) == evtMask)
            {
                bStatus= TRUE;
            }
            break;
        /*!Pseudo: unknow event receive method and default treatment          */
        case KER_enuEVT_RECV_OPT_UNKNOWN:
        default:
            KER_LEAVE_CRITICAL();
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            /*!Pseudo: return error code directly                             */
            return KER_enuERR_EVT_RECV_OPT_UNKNOWN;

    }

    /*!Pseudo: check if have valid events to reception                        */
    if (bStatus)
    {
        /*!Pseudo: have valid events to reception                             */
        *pevtEvent = (KER_pstrCurTCB->evtBuiltInEvent & evtMask);

        /*!Pseudo: check if need clear the received event                     */
        if ((KER_enuEVT_RECV_OPT_AND_CLR == enuRecvMethod)
          ||(KER_enuEVT_RECV_OPT_OR_CLR == enuRecvMethod))
        {
            /*!Pseudo: clear the received event                               */
            KER_pstrCurTCB->evtBuiltInEvent &= ~evtMask;
        }

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();
        /*!Pseudo: return without error                                       */
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

        /*!Pseudo: record the built in event pending information              */
        KER_pstrCurTCB->evtEvent = evtMask;
        KER_pstrCurTCB->enuEventRecvMethod = enuRecvMethod;

        /*!Pseudo: pend built in event object                                 */
        KER_vidBuiltInObjectPend(KER_enuIPC_OBJ_TYPE_BUILT_IN_EVENT,
                                 u32TimeOut);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: schedule to other ready task                               */
        KER_vidSched();

        /*!Pseudo: enter critical section                                     */
        KER_ENTER_CRITICAL();

        if (KER_enuERR_OK == KER_pstrCurTCB->enuError)
        {
            /*!Pseudo: receive event, if no event, received KER_evtEVENT_NONE */
            *pevtEvent = KER_pstrCurTCB->evtEvent;
        }
        else
        {
            /*!Pseudo: event not received, or AND logic condition not met     */
            *pevtEvent = KER_evtEVENT_NONE;
        }

        /*!Pseudo: reset task event                                           */
        KER_pstrCurTCB->evtEvent = KER_evtEVENT_NONE;
        KER_pstrCurTCB->enuEventRecvMethod = KER_enuEVT_RECV_OPT_UNKNOWN;

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return event receive status                                */
        return KER_pstrCurTCB->enuError;
    }
}

#if KER_coBUILT_IN_EVENT_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend task self built in events.
* @Inputs : evtMask: mask expected received events.
*           enuRecvMethod: receive method front or rear.
* @Outputs: pevtEvent: event receiver pointer.
* @Returns: KER_enuERR_OK: pend event successful.
*           KER_enuERR_NOK: no event.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_EVENT_RECV_PTR_INV: event receive pointer invalid
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: unknown receive method option.
*           KER_enuERR_EVT_MASK_INV: event mask invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventTryPend(
                                        EVENT_TYPE             *pevtEvent,
                                        EVENT_TYPE              evtMask,
                                        KER_tenuEvtRecvOpt enuRecvMethod)
{
    CPU_REG BOOL bStatus = FALSE;
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function.                              */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if event receiver is valid                               */
    if (KER_NULL == pevtEvent)
    {
        /*!Pseudo: event receiver is invalid                                  */
        return KER_enuERR_EVT_RECV_PTR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

#if KER_coBUILT_IN_EVENT_MASK_EN > 0u

    /*!Pseudo: check if mask is invalid                                       */
    if ((KER_pstrCurTCB->evtBuiltInEventMask & evtMask) != evtMask)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_EVT_MASK_INV;
    }

#endif

    /*!Pseudo: check events received met condition                            */
    switch (enuRecvMethod)
    {
        case KER_enuEVT_RECV_OPT_OR:
        case KER_enuEVT_RECV_OPT_OR_CLR:
            /*!Pseudo: check events received AND logic condition is met       */
            if (KER_pstrCurTCB->evtBuiltInEvent & evtMask)
            {
                bStatus= TRUE;
            }
            break;
        case KER_enuEVT_RECV_OPT_AND:
        case KER_enuEVT_RECV_OPT_AND_CLR:
            /*!Pseudo: check events received OR logic condition is met        */
            if ((KER_pstrCurTCB->evtBuiltInEvent & evtMask) == evtMask)
            {
                bStatus= TRUE;
            }
            break;
        /*!Pseudo: unknow event receive method and default treatment          */
        case KER_enuEVT_RECV_OPT_UNKNOWN:
        default:
            KER_LEAVE_CRITICAL();
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            /*!Pseudo: return error code directly                             */
            return KER_enuERR_EVT_RECV_OPT_UNKNOWN;

    }

    /*!Pseudo: check if have valid events to reception                        */
    if (bStatus)
    {
        /*!Pseudo: have valid events to reception                             */
        *pevtEvent = (KER_pstrCurTCB->evtBuiltInEvent & evtMask);

        /*!Pseudo: check if need clear the received event                     */
        if ((KER_enuEVT_RECV_OPT_AND_CLR == enuRecvMethod)
          ||(KER_enuEVT_RECV_OPT_OR_CLR == enuRecvMethod))
        {
            /*!Pseudo: clear the received event                               */
            KER_pstrCurTCB->evtBuiltInEvent &= ~evtMask;
        }

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return without error                                       */
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
* @Brief  : post events to task built in event object.
* @Inputs : pstrTCB: task tcb pointer.
*           evtEvent: post events.
* @Outputs: none
* @Returns: KER_enuERR_OK: post event successful.
*           KER_enuERR_TCB_PTR_INV:
*           KER_enuERR_IPC_EVENT_SET_INV: post invalid event(event masked).
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: excetpion, unknown recv option.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventPost(KER_tstrTCB *pstrTCB,
                                        EVENT_TYPE   evtEvent)
{
    CPU_REG BOOL bStatus;
    CPU_REG BOOL bSched = FALSE;

    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if tcb object is valid                                   */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

    /*!Pseudo: check if event is valid to send                                */
    if ( KER_evtEVENT_NONE == evtEvent)
    {
        return KER_enuERR_EVT_SET_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

#if KER_coBUILT_IN_EVENT_MASK_EN > 0u

    /*!Pseudo: check if event is valid to reception                           */
    if ((pstrTCB->evtBuiltInEventMask & evtEvent) != evtEvent)
    {
        KER_LEAVE_CRITICAL();
        return KER_enuERR_EVT_SET_INV;
    }

#endif

    /*!pseudo: store event                                                    */
    pstrTCB->evtBuiltInEvent |= evtEvent;

    /*!Pseudo: check if task pending built in event object                    */
    if (KER_enuIPC_OBJ_TYPE_BUILT_IN_EVENT ==
       (pstrTCB->strIPCObjPend).enuIPCObjType)
    {
        /*!Pseudo: check events received met condition                        */
        switch (pstrTCB->enuEventRecvMethod)
        {
            case KER_enuEVT_RECV_OPT_OR:
            case KER_enuEVT_RECV_OPT_OR_CLR:
                /*!Pseudo: check events received OR logic condition is met   */
                if ((pstrTCB->evtBuiltInEvent) & (pstrTCB->evtEvent))
                {
                    bStatus= TRUE;
                }
                break;
            case KER_enuEVT_RECV_OPT_AND:
            case KER_enuEVT_RECV_OPT_AND_CLR:
                /*!Pseudo: check events received AND logic condition is met   */
                if (((pstrTCB->evtBuiltInEvent) & (pstrTCB->evtEvent))
                    == pstrTCB->evtEvent)
                {
                    bStatus= TRUE;
                }
                break;
            /*!Pseudo: unknow event receive method and default treatment      */
            case KER_enuEVT_RECV_OPT_UNKNOWN:
            default:
                KER_LEAVE_CRITICAL();
                KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
                /*!Pseudo: return error code directly                         */
                return KER_enuERR_EVT_RECV_OPT_UNKNOWN;
        }

        /*!Pseudo: check if have valid events to reception                    */
        if (bStatus)
        {
            /*!Pseudo: abort pending event object                             */
            KER_vidBuiltInObjectPendAbort(pstrTCB, KER_enuERR_OK);

            /*!Pseudo: receive events                                         */
            pstrTCB->evtEvent = pstrTCB->evtBuiltInEvent & pstrTCB->evtEvent;

            /*!Pseudo: check if need clear the received event                 */
            if ((KER_enuEVT_RECV_OPT_AND_CLR
                                               == pstrTCB->enuEventRecvMethod)
                ||(KER_enuEVT_RECV_OPT_OR_CLR
                                               == pstrTCB->enuEventRecvMethod))
            {
                /*!Pseudo: clear the received event                           */
                pstrTCB->evtBuiltInEvent &= ~(pstrTCB->evtEvent);
            }

            /*!Pseudo: reset receive method.                                  */
            pstrTCB->enuEventRecvMethod = KER_enuEVT_RECV_OPT_UNKNOWN;

            /*!Pseudo: if task is ready to running,set need to schedule flag  */
            if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State)
            {
                bSched = TRUE;
            }
        }
    }

    KER_LEAVE_CRITICAL();

    /*!Check if need schedule                                                 */
    if (bSched)
    {
        KER_vidSched();
    }

    return KER_enuERR_OK;
}

#if KER_coBUILT_IN_EVENT_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pending built in event object.
* @Inputs : pstrTCB: task tcb pointer.
*           enuOption: schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort task pending built in event object successful.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending built in event.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventPendAbort(KER_tstrTCB   *pstrTCB,
                                             KER_tenuSchedOpt enuOption)
{
    return KER_enuTaskBuiltInObjPendAbort(pstrTCB,
                                          KER_enuIPC_OBJ_TYPE_BUILT_IN_EVENT,
                                          enuOption);
}
#endif

#if KER_coBUILT_IN_EVENT_MASK_SET_EN > 0u
/*******************************************************************************
* @Brief  : set task built in event reception mask.
* @Inputs : pstrTCB: task tcb pointer.
*           evtMask: related bit is set, corresponding bit event is receptible.
* @Outputs: none
* @Returns: KER_enuERR_OK: set event mask successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventMaskSet(KER_tstrTCB   *pstrTCB,
                                           EVENT_TYPE     evtMask)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();
    /*!Pseudo: set event mask                                                 */
    pstrTCB->evtBuiltInEventMask  |= evtMask;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif

#if KER_coBUILT_IN_EVENT_MASK_CLR_EN > 0u
/*******************************************************************************
* @Brief  : clear task built in event reception mask.
* @Inputs : pstrTCB: task tcb pointer.
*           evtMask: related bit is set, corresponding bit event not receptible.
* @Outputs: none
* @Returns: KER_enuERR_OK: clear event mask successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventMaskClear(KER_tstrTCB   *pstrTCB,
                                             EVENT_TYPE     evtMask)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();
    /*!Pseudo: clear event mask                                               */
    pstrTCB->evtBuiltInEventMask  &= ~evtMask;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif

#if KER_coBUILT_IN_EVENT_MASK_GET_EN > 0u
/*******************************************************************************
* @Brief  : get task built in event mask.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: pevtMask: current event mask.
* @Returns: KER_enuERR_OK: get event mask successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventMaskGet(KER_tstrTCB   *pstrTCB,
                                           EVENT_TYPE    *pevtMask)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: get event mask                                                 */
    *pevtMask = pstrTCB->evtBuiltInEventMask;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif

#if KER_coBUILT_IN_EVENT_RESET_EN > 0u
/*******************************************************************************
* @Brief  : reset task built in event
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: reset event successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskEventReset(KER_tstrTCB *pstrTCB)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: return with tcb object invalid error                       */
        return KER_enuERR_TCB_PTR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();
    /*!Pseudo: clear task built in event                                      */
    pstrTCB->evtBuiltInEvent  = KER_evtEVENT_NONE;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif

#endif