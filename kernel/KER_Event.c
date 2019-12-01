/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Event.c
*Version    : V1.00
*Description: MRTOS event management module.
*             Evnet mask logic description:
*             Example: Mask   Bit: 10010100    (EVENT_TYPE is 8bits length)
*                      Post Event: 10000000    is valid, receptible.
*                      Post Event: 01000000    is invalid, not receptible.
*                      Post Event: 00100000    is invalid, not receptible.
*                      Post Event: 00010000    is valid, receptible.
*                      Post Event: 00001000    is invalid, not receptible.
*                      Post Event: 00000100    is valid, receptible
*                      post Event: 00000010    is invalid, not receptible.
*                      post Event: 00000001    is invalid, not receptible.
*             Also all the bit event can be combine to post together.
*             but should respect up mask logic, otherwise, the masked bit event
*             not receptible.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/3
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*!Comment: using event module compiler control                               */
#if KER_coIPC_EVENT_EN > 0u

/*******************************************************************************
* @Brief  : initial a event object.
* @Inputs : pstrEvent: event object pointer.
*           evtEventMask: event mask.
*           evtInitEvent: initial event.
*           enuIPCMethod: ipc pending list schedule strategy.
*           ps8Name: event object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial event object successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_METHOD_INV: ipc method invalid.
*           KER_enuERR_IPC_EVT_SET_INV: initial invalid event(event masked).
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventInit( KER_tstrEvent    *pstrEvent,
                                     EVENT_TYPE        evtEventMask,
                                     EVENT_TYPE        evtInitEvent,
                                     KER_tenuIPCMethod enuIPCMethod,
                                     INT8S            *ps8Name)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if event object is valid                                 */
    if (KER_NULL == pstrEvent)
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

#if KER_coIPC_EVENT_MASK_EN > 0u

    /*!Pseudo: set initial event                                              */
    pstrEvent->evtEvent = evtEventMask & evtInitEvent;
    /*!Pseudo: event mask to filter the invalid event                         */
    pstrEvent->evtMask  = evtEventMask;

#else

    /*!Pseudo: avoid not used argument warning                                */
    KER_NOT_USED_ARGUMENT(evtEventMask);
    /*!Pseudo: set initial event                                              */
    pstrEvent->evtEvent = evtInitEvent;

#endif

    /*!Pseudo: initial pending list                                           */
    LIST_INIT(pstrEvent->strPendList);
    /*!Pseudo: pending list schedule strategy                                 */
    pstrEvent->enuIPCMethod = enuIPCMethod;
    /*!Pseudo: event object name                                              */
    pstrEvent->ps8Name = ps8Name;

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    if ((evtEventMask & evtInitEvent) != evtInitEvent)
    {
        /*!Pseudo: initial event masked invalid, return error                 */
        return KER_enuERR_EVT_SET_INV;
    }
    else
    {
        /*!Pseudo: return none error                                          */
        return KER_enuERR_OK;
    }
}

#if KER_coIPC_EVENT_MASK_SET_EN > 0u
/*******************************************************************************
* @Brief  : set event mask
* @Inputs : pstrEvent: event object pointer.
*           evtMask: related bit is set, corresponding bit event is receptible.
* @Outputs: none
* @Returns: KER_enuERR_OK: set mask successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventMaskSet( KER_tstrEvent *pstrEvent,
                                        EVENT_TYPE evtMask )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrEvent)
    {
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if event object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: set event mask                                                 */
    pstrEvent->evtMask  |= evtMask;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif

#if KER_coIPC_EVENT_MAKS_CLEAR_EN > 0u
/*******************************************************************************
* @Brief  : clear mask event.
* @Inputs : pstrEvent: event object pointer.
*           evtMask: related bit is set, corrsponding bit event not receptible.
* @Outputs: none
* @Returns: KER_enuERR_OK: clear mask successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventMaskClear( KER_tstrEvent *pstrEvent,
                                          EVENT_TYPE evtMask)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrEvent)
    {
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if event object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: clear event mask                                               */
    pstrEvent->evtMask  &= ~evtMask;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif

#if KER_coIPC_EVENT_MASK_GET_EN > 0u
/*******************************************************************************
* @Brief  : get event mask
* @Inputs : pstrEvent: event object pointer.
* @Outputs: pevtMask: current event mask.
* @Returns: KER_enuERR_OK: get mask successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventMaskGet( KER_tstrEvent *pstrEvent,
                                        EVENT_TYPE    *pevtMask )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    if (KER_NULL == pstrEvent)
    {
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if event object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: get event mask                                                 */
    *pevtMask = pstrEvent->evtMask;
    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    return KER_enuERR_OK;
}
#endif


/*******************************************************************************
* @Brief  : pend event.
* @Inputs : pstrEvent: event object pointer.
*           evtMask: specify what task want to receive events.
*           enuRecvMethod: receive method logic(AND, OR, CLEAR logic).
*           u32TimeOut: pending timeout value. if pass 0, pending until event
*                       reception, if pass not 0, pending until timeout.
* @Outputs: pevtEvent: event receiver pointer.
* @Returns: KER_enuERR_OK: pend event successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_EVENT_RECV_PTR_INV: event receive pointer invalid
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_IPC_OBJ_DELETED: event object deleted.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: unknown receive method option.
*           KER_enuERR_EVT_MASK_INV: event mask invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventPend(KER_tstrEvent          *pstrEvent,
                                    EVENT_TYPE              evtMask,
                                    KER_tenuEvtRecvOpt enuRecvMethod,
                                    INT32U                  u32TimeOut,
                                    EVENT_TYPE             *pevtEvent)
{
    CPU_REG BOOL bStatus = FALSE;
    KER_CPU_SR_ALLOC();


#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_ISR_FAULT_USING;
    }

    /*!Pseudo: check if event object is valid                                 */
    if (KER_NULL == pstrEvent)
    {
        /*!Pseudo: ipc object is invalid                                      */
        return KER_enuERR_IPC_OBJ_INV;
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

    /*!Pseudo: check if event object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

#if KER_coIPC_EVENT_MASK_EN > 0u

    /*!Pseudo: check if mask is invalid                                       */
    if ((pstrEvent->evtMask & evtMask) != evtMask)
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
            /*!Pseudo: check events received OR logic condition is met        */
            if (pstrEvent->evtEvent & evtMask)
            {
                bStatus= TRUE;
            }
            break;
        case KER_enuEVT_RECV_OPT_AND:
        case KER_enuEVT_RECV_OPT_AND_CLR:
            /*!Pseudo: check events received AND logic condition is met       */
            if ((pstrEvent->evtEvent & evtMask) == evtMask)
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
        *pevtEvent = (pstrEvent->evtEvent & evtMask);

        /*!Pseudo: check if need clear the received event                     */
        if ((KER_enuEVT_RECV_OPT_AND_CLR == enuRecvMethod)
          ||(KER_enuEVT_RECV_OPT_OR_CLR == enuRecvMethod))
        {
            /*!Pseudo: clear the received event                               */
            pstrEvent->evtEvent &= ~evtMask;
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
            /*!Pseudo: do nothing!!!                                          */
        }

        /*!Pseudo: record the event pending information                       */
        KER_pstrCurTCB->evtEvent = evtMask;
        KER_pstrCurTCB->enuEventRecvMethod = enuRecvMethod;

        /*!Pseudo: pend event object                                          */
        KER_vidObjectPend( pstrEvent,
                           KER_enuIPC_OBJ_TYPE_EVENT,
                           pstrEvent->enuIPCMethod,
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

#if KER_coIPC_EVENT_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend event.
* @Inputs : pstrEvent: event object pointer.
*           evtMask: specify what task want to receive events.
*           enuRecvMethod: receive method logic(AND, OR, CLEAR logic).
* @Outputs: pevtEvent: event receiver pointer.
* @Returns: KER_enuERR_OK: pend event successful.
*           KER_enuERR_NOK: no event.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_EVENT_RECV_PTR_INV: event receive pointer invalid
*           KER_enuERR_IPC_OBJ_DELETED: event object deleted.
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: unknown receive method option.
*           KER_enuERR_EVT_MASK_INV: event mask invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventTryPend(KER_tstrEvent          *pstrEvent,
                                       EVENT_TYPE              evtMask,
                                       KER_tenuEvtRecvOpt enuRecvMethod,
                                       EVENT_TYPE             *pevtEvent)
{

    CPU_REG BOOL bStatus = FALSE;
    KER_CPU_SR_ALLOC();


#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if event object is valid                                 */
    if (KER_NULL == pstrEvent)
    {
        /*!Pseudo: ipc object is invalid                                      */
        return KER_enuERR_IPC_OBJ_INV;
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

    /*!Pseudo: check if event object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

#if KER_coIPC_EVENT_MASK_EN > 0u

    /*!Pseudo: check if mask is invalid                                       */
    if ((pstrEvent->evtMask & evtMask) != evtMask)
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
            /*!Pseudo: check events received OR logic condition is met        */
            if (pstrEvent->evtEvent & evtMask)
            {
                bStatus= TRUE;
            }
            break;
        case KER_enuEVT_RECV_OPT_AND:
        case KER_enuEVT_RECV_OPT_AND_CLR:
            /*!Pseudo: check events received AND logic condition is met       */
            if ((pstrEvent->evtEvent & evtMask) == evtMask)
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
        *pevtEvent = (pstrEvent->evtEvent & evtMask);

        /*!Pseudo: check if need clear the received event                     */
        if ((KER_enuEVT_RECV_OPT_AND_CLR == enuRecvMethod)
          ||(KER_enuEVT_RECV_OPT_OR_CLR == enuRecvMethod))
        {
            /*!Pseudo: clear the received event                               */
            pstrEvent->evtEvent &= ~evtMask;
        }

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();
        /*!Pseudo: return without error                                       */
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
* @Brief  : post events.
* @Inputs : pstrEvent: event object pointer.
*           evtEvent: set event.
* @Outputs: none
* @Returns: KER_enuERR_OK: post event successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_EVT_SET_INV: post invalid event(event masked).
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: unknown event receive method option
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventPost(KER_tstrEvent *pstrEvent,
                                    EVENT_TYPE     evtEvent)
{
    CPU_REG KER_tstrList *pstrNode;
    CPU_REG KER_tstrTCB  *pstrTCB;
    CPU_REG BOOL bStatus;
    CPU_REG BOOL bSched = FALSE;
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if event object is valid                                 */
    if (KER_NULL == pstrEvent)
    {
        /*!Pseudo: ipc object is invalid                                      */
        return KER_enuERR_IPC_OBJ_INV;
    }

    /*!Pseudo: check if event is valid to send                                */
    if ( KER_evtEVENT_NONE == evtEvent)
    {
        return KER_enuERR_EVT_SET_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if event object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

#if KER_coIPC_EVENT_MASK_EN > 0u

    /*!Pseudo: check if event is valid to reception                           */
    if ((pstrEvent->evtMask & evtEvent) != evtEvent)
    {
        KER_LEAVE_CRITICAL();
        return KER_enuERR_EVT_SET_INV;
    }

#endif

    /*!pseudo: store event                                                    */
    pstrEvent->evtEvent |= evtEvent;

    /*!Pseudo: get pending list head                                          */
    pstrNode = LIST_GET_HEAD(pstrEvent->strPendList);

    while (pstrNode != &(pstrEvent->strPendList))
    {
        pstrTCB  = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrTCB, strList);
        pstrNode = pstrNode->pstrNext;
        bStatus = FALSE;

        /*!Pseudo: check events received met condition                        */
        switch (pstrTCB->enuEventRecvMethod)
        {
            case KER_enuEVT_RECV_OPT_OR:
            case KER_enuEVT_RECV_OPT_OR_CLR:
                /*!Pseudo: check events received OR logic condition is met    */
                if ((pstrEvent->evtEvent) & (pstrTCB->evtEvent))
                {
                    bStatus= TRUE;
                }
                break;
            case KER_enuEVT_RECV_OPT_AND:
            case KER_enuEVT_RECV_OPT_AND_CLR:
                /*!Pseudo: check events received AND logic condition is met   */
                if (((pstrEvent->evtEvent) & (pstrTCB->evtEvent))
                    ==pstrTCB->evtEvent)
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
            KER_vidObjectPendAbort(pstrTCB, KER_enuERR_OK);

            /*!Pseudo: receive events                                         */
            pstrTCB->evtEvent = (pstrEvent->evtEvent & pstrTCB->evtEvent);


            /*!Pseudo: check if need clear the received event                 */
            if ((KER_enuEVT_RECV_OPT_AND_CLR
                                               == pstrTCB->enuEventRecvMethod)
                ||(KER_enuEVT_RECV_OPT_OR_CLR
                                               == pstrTCB->enuEventRecvMethod))
            {
                /*!Pseudo: clear the received event                           */
                pstrEvent->evtEvent &= ~(pstrTCB->evtEvent);
            }

            /*!Pseudo: reset receive method.                                  */
            pstrTCB->enuEventRecvMethod = KER_enuEVT_RECV_OPT_UNKNOWN;

            /*!Pseudo: if task is ready to running, set need to schedule flag */
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

#if KER_coIPC_EVENT_PEND_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort a task pending the specify event object.
* @Inputs : pstrEvent: event object pointer.
*           pstrTCB: task tcb pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending tasks successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending this event object.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventPendAbort(KER_tstrEvent *pstrEvent,
                                         KER_tstrTCB   *pstrTCB,
                                         KER_tenuSchedOpt enuOption)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if event object is valid                                 */
    if (KER_NULL == pstrEvent)
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

    /*!Pseudo: check if event object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: check if pending this event                                    */
    if (((pstrTCB->strIPCObjPend).enuIPCObjType != KER_enuIPC_OBJ_TYPE_EVENT)
        || ((pstrTCB->strIPCObjPend).vidpIPCObject != pstrEvent))
    {
        KER_LEAVE_CRITICAL();
        /*!Pseudo: return error not pending this object                       */
        return KER_enuERR_IPC_NOT_PEND_THIS;
    }

    /*!Pseudo: abort pending event object                                     */
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

#if KER_coIPC_EVENT_PEND_ABORT_ALL_EN > 0u
/*******************************************************************************
* @Brief  : abort all tasks pending the specify event object.
* @Inputs : pstrEvent: event object pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort all pending tasks successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventPendAbortAll(KER_tstrEvent *pstrEvent,
                                            KER_tenuSchedOpt enuOption)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if event object is valid                                 */
    if (KER_NULL == pstrEvent)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if event object is deleted                               */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: remove all pending task,set ipc object pending abort error flag*/
    KER_vidPendListRemoveAll(&(pstrEvent->strPendList),
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


#if KER_coIPC_EVENT_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete a event object.
* @Inputs : pstrEvent: event object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: deleted successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuEventDelete( KER_tstrEvent *pstrEvent )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if event object is valid                                 */
    if (KER_NULL == pstrEvent)
    {
        /*!Pseudo: return with ipc object invalid error                       */
        return KER_enuERR_IPC_OBJ_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if mutex object is using                                 */
    if (KER_enuIPC_METHOD_UNKNOWN == pstrEvent->enuIPCMethod)
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return with ipc object already deleted error               */
        return KER_enuERR_IPC_OBJ_DELETED;
    }

    /*!Pseudo: remove all pending task,set ipc object pending abort error flag*/
    KER_vidPendListRemoveAll(&(pstrEvent->strPendList),
                               KER_enuERR_IPC_OBJ_DELETED);

    /*!Pseudo: no onwer, no pending task, set mutex object invalid            */
    pstrEvent->enuIPCMethod = KER_enuIPC_METHOD_UNKNOWN;

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: for the new ready task has opportunity to run                  */
    KER_vidSched();

    return KER_enuERR_OK;

}
#endif

#endif
