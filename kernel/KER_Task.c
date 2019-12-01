/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Task.c
*Version    : V1.00
*Description: task manager module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/26
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
#if KER_coIPC_MUTEX_EN > 0u
PRIVATE void vidTaskMutexHoldListOrignalPriorityChange( KER_tstrList *pstrList,
                                                        INT16U u16Priority);
#endif

/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
PRIVATE void vidStackAddressAlign( CPU_STK *pstkStackBasePtr,
                                   CPU_STK_SIZE stkszStackSize,
                                   CPU_STK **ppstkAlignStackPtr);

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))
PRIVATE void vidTaskStackPreSet(CPU_STK  *pstkStackBasePtr,
                                CPU_STK_SIZE stkszStackSize );
#endif

/*******************************************************************************
* @Brief  : align statck access address
* @Inputs : pstkStackBasePtr: stack base address
*           stkszStackSize: stack size
* @Outputs: ppstkAlignStackPtr: after align, stack top address pointer
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidStackAddressAlign( CPU_STK *pstkStackBasePtr,
                                   CPU_STK_SIZE stkszStackSize,
                                   CPU_STK **ppstkAlignStackPtr)
{
    CPU_STK *pstkSP;

#if (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_FULL_DOWN)
    pstkSP = (CPU_STK *)((CPU_ADDR)pstkStackBasePtr + stkszStackSize);
    #if (KER_u8CPU_STK_BITS == 64u)
        pstkSP = (CPU_STK *)((CPU_ADDR)pstkSP & (~(CPU_ADDR)0x7));
    #elif (KER_u8CPU_STK_BITS == 32u)
        pstkSP = (CPU_STK *)((CPU_ADDR)pstkSP & (~(CPU_ADDR)0x3));
    #elif (KER_u8CPU_STK_BITS == 16u)
        pstkSP = (CPU_STK *)((CPU_ADDR)pstkSP & (~(CPU_ADDR)0x1));
    #elif (KER_u8CPU_STK_BITS == 8u)
        /* !Comment: For 8Bits Stack data, Not need align anymore             */
    #endif
#elif (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_FULL_UP)
    pstkSP = (CPU_STK *)((CPU_ADDR)pstkStackBasePtr);
    #if (KER_u8CPU_STK_BITS == 64u)
        pstkSP = (CPU_STK *)(((CPU_ADDR)pstkSP + (CPU_ADDR)0x7)
                                             & (~(CPU_ADDR)0x7));
    #elif (KER_u8CPU_STK_BITS == 32u)
        pstkSP = (CPU_STK *)(((CPU_ADDR)pstkSP + (CPU_ADDR)0x3)
                                             & (~(CPU_ADDR)0x3));
    #elif (KER_u8CPU_STK_BITS == 16u)
        pstkSP = (CPU_STK *)(((CPU_ADDR)pstkSP + (CPU_ADDR)0x1)
                                             & (~(CPU_ADDR)0x1));
    #elif (KER_u8CPU_STK_BITS == 8u)
        /* !Comment: For 8Bits Stack data, Not need align anymore             */
    #endif
#elif (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_EMPTY_DOWN)
    pstkSP = (CPU_STK *)((CPU_ADDR)pstkStackBasePtr + stkszStackSize);
    #if (KER_u8CPU_STK_BITS == 64u)
        pstkSP = (CPU_STK *)((((CPU_ADDR)pstkSP) & (~(CPU_ADDR)0x7))
                                                 - (CPU_ADDR)0x8);
    #elif (KER_u8CPU_STK_BITS == 32u)
        pstkSP = (CPU_STK *)((((CPU_ADDR)pstkSP) & (~(CPU_ADDR)0x3))
                                                 - (CPU_ADDR)0x4);
    #elif (KER_u8CPU_STK_BITS == 16u)
        pstkSP = (CPU_STK *)((((CPU_ADDR)pstkSP) & (~(CPU_ADDR)0x1))
                                                 - (CPU_ADDR)0x2);
    #elif (KER_u8CPU_STK_BITS == 8u)
        pstkSP = (CPU_STK *)((CPU_ADDR)pstkSP - (CPU_ADDR)0x1);
    #endif
#elif (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_EMPTY_UP)
    pstkSP = (CPU_STK *)((CPU_ADDR)pstkStackBasePtr);
    #if (KER_u8CPU_STK_BITS == 64u)
        pstkSP = (CPU_STK *)(((CPU_ADDR)pstkSP + (CPU_ADDR)0x15)
                                             & (~(CPU_ADDR)0x7));
    #elif (KER_u8CPU_STK_BITS == 32u)
        pstkSP = (CPU_STK *)(((CPU_ADDR)pstkSP + (CPU_ADDR)0x7)
                                             & (~(CPU_ADDR)0x3));
    #elif (KER_u8CPU_STK_BITS == 16u)
        pstkSP = (CPU_STK *)(((CPU_ADDR)pstkSP + (CPU_ADDR)0x3)
                                             & (~(CPU_ADDR)0x1));
    #elif (KER_u8CPU_STK_BITS == 8u)
        pstkSP = (CPU_STK *)((CPU_ADDR)pstkSP + (CPU_ADDR)0x1);
    #endif
#else
    #error "!Not supported stack growth configuration"
#endif

    *ppstkAlignStackPtr = pstkSP;
}

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))
/*******************************************************************************
* @Brief  : pre-set task stack for stack usage limit check
* @Inputs : stkszStackSize: stack size.
* @Outputs: pstkStackBasePtr: the pointer stack memory will be reset to zero.
* @Returns: none
* @Note   :
*******************************************************************************/
PRIVATE void vidTaskStackPreSet(CPU_STK  *pstkStackBasePtr,
                                CPU_STK_SIZE stkszStackSize )
{
    CPU_REG INT8U u8TokenIdx;
    CPU_REG CPU_STK *pstkStack;

#if ((KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_FULL_DOWN) \
  || (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_EMPTY_DOWN))
    CPU_REG CPU_STK *pstkStackEnd;
#endif


#if ((KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_FULL_DOWN) \
  || (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_EMPTY_DOWN))

    /*!Pseudo: calculate set stack token start lowest address                 */
    pstkStack = (CPU_STK *)KER_ADDR_HI_ALIGN(pstkStackBasePtr);
    pstkStackEnd = (CPU_STK *)KER_ADDR_LO_ALIGN((((INT8U *)pstkStackBasePtr)
                                                        + stkszStackSize));
    /*!Pseduo: initial token index to access lowest cell of CPU_STK type      */
    u8TokenIdx = (INT8U)0;

    /*!Pseudo: pre-set task stack space stack token string                    */
    while (pstkStack < pstkStackEnd)
    {
        *pstkStack = KER_kuniStackToken.astkToken[u8TokenIdx];

        pstkStack++;

        if (u8TokenIdx < (KER_u8STACK_TOKEN_LENGTH / sizeof(CPU_STK) - 1u))
        {
            u8TokenIdx++;
        }
        else
        {
            u8TokenIdx = (INT8U)0;
        }
    }

#elif ((KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_FULL_UP) \
    || (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_EMPTY_UP))

    /*!Pseudo: calculate set stack token start highest address                */
    pstkStack = (CPU_STK *)KER_ADDR_LO_ALIGN((((INT8U *)pstkStackBasePtr)
                                                        + stkszStackSize)
                                                        - sizeof(CPU_STK));

    /*!Pseduo: initial token index to access highest cell of CPU_STK type     */
    u8TokenIdx = (INT8U)(KER_u8STACK_TOKEN_LENGTH / sizeof(CPU_STK) - 1u);

    /*!Pseudo: pre-set task stack space stack token string                    */
    while (pstkStack >= pstkStackBasePtr)
    {
        *pstkStack = KER_kuniStackToken.astkToken[u8TokenIdx];

        pstkStack--;
        u8TokenIdx--;
        u8TokenIdx = u8TokenIdx
                    & (INT8U)(KER_u8STACK_TOKEN_LENGTH / sizeof(CPU_STK) - 1u);
    }

#else
    #error "!Not supported stack growth configuration"
#endif

}
#endif

/*******************************************************************************
* @Brief  : Task default exit execution.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : should be only called by kernel, user can't call it.
*******************************************************************************/
PUBLIC void KER_vidTaskDefaultExit( void )
{
    /*!Pseudo: task delete self                                               */
    KER_enuTaskDelete(KER_NULL);
}

/*******************************************************************************
* @Brief  : initial task.
* @Inputs :
* @Outputs:
* @Returns:
* @Note   :
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskInit( KER_tstrTCB *pstrTCB,
                                    INT8S       *ps8TaskName,
                                    void       (*pfnEntry)(void *vidpParam),
                                    void        *vidpParam,
                                    void       (*pfnExit)(INT8U u8ExitArg),
                                    CPU_STK     *pstkStackBasePtr,
                                    CPU_STK_SIZE stkszStackSize,
                                    CPU_STK_SIZE stkszStackLimitSize,
                                    INT16U       u16Priority,
                                    INT16U       u16Ticks,
                                    KER_tstrMsgPool *pstrMsgPool,
                                    INT32U           u32BuiltInSemInitValue,
                                    EVENT_TYPE       evtInitEvent,
                                    EVENT_TYPE       evtEventMask,
                                    void            *vidpUserData
                                    )
{
    CPU_STK *pstkSP;

    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        /*!Pseudo: tcb pointer invalid                                        */
        return KER_enuERR_TCB_PTR_INV;
    }

    /*!Pseudo: check if task entry is valid                                   */
    if (KER_NULL == pfnEntry)
    {
        /*!Pseudo: task entry is invalid                                      */
        return KER_enuERR_TASK_ENTRY_INV;
    }

    /*!Pseudo: check if task stack base address is valid                      */
    if (KER_NULL == pstkStackBasePtr)
    {
        /*!Pseudo: task stack base address is invalid                         */
        return KER_enuERR_STACK_PTR_INV;
    }

    /*!Pseudo: stack limit should not exceed stack size                       */
    if (stkszStackLimitSize > stkszStackSize)
    {
        return KER_enuERR_STK_SET_LMT_INV;
    }

    /*!Pseudo: check if task set priority is in valid range                   */
    if (u16Priority >= KER_u16TASK_PRIO_MAX)
    {
        /*!Pseudo: priority is out of valid range                             */
        return KER_enuERR_PRIORITY_INV;
    }

#endif

#if KER_coTASK_STACK_USAGE_EN > 0u
    /*!Pseudo: pre-set task stack for stack usage calculation                 */
    vidTaskStackPreSet(pstkStackBasePtr, stkszStackSize);

#elif KER_coTASK_STACK_LIMIT_EN > 0u
    /*!Pseudo: pre-set task stack for stack usage limit check                 */
    if (stkszStackLimitSize != (CPU_STK_SIZE)0)
    {
        vidTaskStackPreSet(pstkStackBasePtr, stkszStackSize);
    }
#endif

    /*!Pseudo: align stack access address,cut non-align stack address memory  */
    vidStackAddressAlign(pstkStackBasePtr, stkszStackSize, &pstkSP);
    /*!Pseudo: initial task stack                                             */
    pstkSP = KER_pstkInitTaskStack(pstkSP, pfnEntry, vidpParam);

    /*!Pseudo: initial stack top pointer                                      */
    pstrTCB->pstkStackPtr = pstkSP;

#if KER_coTASK_STACK_LIMIT_EN > 0u

    pstrTCB->stkszStackLimitSize = stkszStackLimitSize;

#endif

#if KER_coTASK_STACK_USAGE_EN > 0u
    /*!Pseudo: reset task stack using max value                               */
    pstrTCB->stkszStackUsingMax = (CPU_STK_SIZE)0;
#endif

    /*!Pseudo: save stack initial base pointer                                */
    pstrTCB->pstkStackBasePtr = pstkStackBasePtr;
    /*!Pseudo: save stack size                                                */
    pstrTCB->stkszStackSize = stkszStackSize;
    /*!Pseudo: set task current priority                                      */
    pstrTCB->u16CurPriority = u16Priority;
    /*!Pseudo: set task initial priority                                      */
    pstrTCB->u16InitPriority = u16Priority;

#if KER_coSLICE_SCHED_EN > 0u
    /*!Pseudo: set remain ticks                                               */
    pstrTCB->u16RemainTicks = u16Ticks;
    /*!Pseudo: set reload ticks                                               */
    pstrTCB->u16ReloadTicks = u16Ticks;
#endif

#if KER_coSWITCH_CONTEXT_CNT_EN > 0u
    /*!Pseudo: reset task switch context counter                              */
    pstrTCB->u32SwCtxCnt = (INT32U)0;
#endif

    /*!Pseudo: set task initial state                                         */
    pstrTCB->u8State = KER_u8TSK_STAT_RDY;
    /*!Pseudo: clear error flag                                               */
    pstrTCB->enuError = KER_enuERR_OK;
    /*!Pseudo: invalid tick wheel index                                       */
    pstrTCB->u16TickWheelIdx = KER_u16TICK_WHEEL_SIZE;

    /*!Pseudo: set task name                                                  */
    if ( KER_NULL != ps8TaskName)
    {
        /*!Pseudo: user named task                                            */
        pstrTCB->ps8TaskName = ps8TaskName;
    }
    else
    {
        /*!Pseudo: user unamed task                                           */
        pstrTCB->ps8TaskName = "Unnamed task";
    }

    /*!Pseudo: set task entry                                                 */
    pstrTCB->pfEntry = pfnEntry;
    /*!Pseudo: set user private exit execution function                       */
    pstrTCB->pfExit  = pfnExit;

    /*!Pseudo: initial tick list pointer to itself                            */
    LIST_NODE_INIT(pstrTCB->strTickList);

#if KER_coIPC_MUTEX_EN > 0u
    /*!Pseudo: initial mutex holding list pointer to itself                   */
    LIST_INIT(pstrTCB->strMutexHoldList);
#endif

    /*!Pseudo: initial ipc pending information                                */
    (pstrTCB->strIPCObjPend).enuIPCObjType = KER_enuIPC_OBJ_TYPE_UNKNOWN;
    (pstrTCB->strIPCObjPend).vidpIPCObject = KER_NULL;

#if KER_coIPC_MSGQ_EN > 0u
    /*!Pseudo: reset message receiver pointer                                 */
    pstrTCB->vidpMsg = KER_NULL;
    /*!Pseudo: reset message received size                                    */
    pstrTCB->u16MsgSize = (INT16U)0;
#endif

#if KER_coIPC_EVENT_EN > 0u
    /*!Pseudo: reset event receiver                                           */
    pstrTCB->evtEvent = KER_evtEVENT_NONE;

#endif

#if KER_coBUILT_IN_EVENT_EN > 0u

    #if KER_coBUILT_IN_EVENT_MASK_EN > 0u

    pstrTCB->evtBuiltInEvent = evtEventMask & evtInitEvent;
    pstrTCB->evtBuiltInEventMask = evtEventMask;

    #else

    KER_NOT_USED_ARGUMENT(evtEventMask);
    pstrTCB->evtBuiltInEvent = evtInitEvent;

    #endif

#endif

#if (KER_coBUILT_IN_EVENT_EN > 0u) || (KER_coIPC_EVENT_EN > 0u)
    /*!Pseudo: reset event receive method is OR with CLEAR                    */
    pstrTCB->enuEventRecvMethod = KER_enuEVT_RECV_OPT_UNKNOWN;
#endif

#if KER_coTASK_SUSPEND_EN > 0u
    /*!Pseudo: suspend nesting counter                                        */
    pstrTCB->u8SuspendNestCnt = (INT8U)0;
#endif

#if KER_coBUILT_IN_MSGQ_EN > 0u
    /*!Pseudo: set task built in message pool                                 */
    pstrTCB->pstrBuiltInMsgPool = pstrMsgPool;
#endif

#if KER_coBUILT_IN_SEM_EN > 0u
    pstrTCB->u32BuiltInSemValue = u32BuiltInSemInitValue;
#endif

    /*!Pseudo: reset user extend data                                         */
    pstrTCB->vidpUserExtData = vidpUserData;

    /*!Pseudo: kernel enter critical code section                             */
    KER_ENTER_CRITICAL();

#if KER_coHOOK_TASK_INIT_EN > 0u
    /*!Pseudo: call user hook function                                        */
    if (KER_NULL != KER_pfnHookTaskInit)
    {
        KER_pfnHookTaskInit(pstrTCB);
    }
#endif

    /*!Pseudo: insert to task TCB link list                                   */
    LIST_INSERT_TAIL(KER_strTCBLinkList, pstrTCB->strLinkList);

    /*!Pseudo: increase task counter                                          */
    KER_u32TaskCnt++;

    /*!Pseudo: insert to ready tcb list tail                                  */
    KER_vidRdyListInsertToHead(pstrTCB);

    /*!Pseudo: kernel leave critical code section                             */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: if kernel is running, trigger task schedule                    */
    if (KER_bKernelRunning)
    {
        KER_vidSched();
    }

    /*!Pseudo: task create successful                                         */
    return KER_enuERR_OK;
}

#if KER_coTASK_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete a task.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: delete task successful.
*           KER_enuERR_DELETE_IN_ISR: ISR not allow to delete a task.
*           KER_enuERR_DELETE_IDLE_TASK: not allow to delete idle task.
*           KER_enuERR_DELETE_TICK_TASK: not allow to delete tick task.
*           KER_enuERR_DELETE_TIMER_TASK: not allow to delete timer task.
*           KER_enuERR_DELETE_STKCHK_TASK: not allow to delete stack check task.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskDelete( KER_tstrTCB *pstrTCB )
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_DELETE_IN_ISR;
    }

    /*!Pseudo: not allow to delete idle task                                  */
    if (pstrTCB == &KER_strIdleTaskTcb)
    {
        return KER_enuERR_DELETE_IDLE_TASK;
    }

#if KER_coTICK_TASK_EN > 0u

    /*!Pseudo: not allow to delete tick task                                  */
    if (pstrTCB == &KER_strTickTaskTcb)
    {
        return KER_enuERR_DELETE_TICK_TASK;
    }

#endif

#if KER_coTIMER_TASK_EN > 0u

    /*!Pseudo: not allow to delete timer task                                 */
    if (pstrTCB == &KER_strTimerTaskTcb)
    {
        return KER_enuERR_DELETE_TIMER_TASK;
    }

#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))

    /*!Pseudo: not allow to suspend stack check task                          */
    if (pstrTCB == &KER_strStackChkTaskTcb)
    {
        return KER_enuERR_DELETE_STKCHK_TASK;
    }

#endif

#endif

    /*!Pseudo: check if delete task itself                                    */
    if (KER_NULL == pstrTCB)
    {
        pstrTCB = KER_pstrCurTCB;
    }

    /*!Pseudo: enter critical code section                                    */
    KER_ENTER_CRITICAL();

    switch (pstrTCB->u8State)
    {
        case KER_u8TSK_STAT_RDY:
            /*!Pseudo: remove current task from tcb ready list.               */
            KER_vidRdyListRemove(pstrTCB);
            break;

#if KER_coTASK_SUSPEND_EN > 0u
        case KER_u8TSK_STAT_SPD_DLY:
#endif
        case KER_u8TSK_STAT_DLY:
            /*!Pseudo: remove from tick wheel list                            */
            KER_vidTickListRemove(pstrTCB);
            break;

#if KER_coTASK_SUSPEND_EN > 0u
        case KER_u8TSK_STAT_SPD_BLK_DLY:
#endif
        case KER_u8TSK_STAT_BLK_DLY:
            /*!Pseudo: remove from tick wheel list                            */
            KER_vidTickListRemove(pstrTCB);
            /*!Comment: goto next case to treat block state                   */

#if KER_coTASK_SUSPEND_EN > 0u
        case KER_u8TSK_STAT_SPD_BLK:
#endif
        case KER_u8TSK_STAT_BLK:
            /*!Pseudo: remove from pending list                               */
            if (KER_NULL != (pstrTCB->strIPCObjPend).vidpIPCObject)
            {
                /*!Pseudo: remvoe pstrTCB node from pending list              */
                LIST_REMOVE_NODE(pstrTCB->strList);
            }
            break;

#if KER_coTASK_SUSPEND_EN > 0u
        case KER_u8TSK_STAT_SPD:
            break;
#endif
        default:
            KER_LEAVE_CRITICAL();
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            return KER_enuERR_TASK_STATE_INV;
    }

#if KER_coIPC_MUTEX_EN > 0u

    /*!Pseudo: release task holding mutexs                                    */
    if (!LIST_IS_EMPTY(pstrTCB->strMutexHoldList))
    {
        KER_tstrMutex *pstrMutex;
        KER_tstrList  *pstrNode = (pstrTCB->strMutexHoldList).pstrNext;
        KER_tstrList  *pstrPendNode;
        KER_tstrTCB   *pstrPendTCB;
        while (pstrNode != &(pstrTCB->strMutexHoldList))
        {
            /*!Pseudo: get mutex object pointer from mutex holding list       */
            pstrMutex = LIST_NODE_ENTRY_PTR(pstrNode,KER_tstrMutex,strHoldList);
            pstrNode = pstrNode->pstrNext;
            /*!Pseudo: remove this mutex from tcb mutex hold list             */
            LIST_REMOVE_NODE(pstrMutex->strHoldList);

            /*!Pseudo: if no task pending, release next holding mutex         */
            if (LIST_IS_EMPTY(pstrMutex->strPendList))
            {
                /*!Pseudo: release mutex, indicate no task hold this mutex    */
                pstrMutex->pstrOwnerTCB = KER_NULL;
            }
            else
            {

                /*!Pseudo: get head node from pending list                    */
                pstrPendNode = LIST_GET_HEAD(pstrMutex->strPendList);
                pstrPendTCB = LIST_NODE_ENTRY_PTR( pstrPendNode,
                                                   KER_tstrTCB,strList);

                /*!Pseudo: abort pending mutex object                         */
                KER_vidObjectPendAbort(pstrPendTCB, KER_enuERR_OK);

                /*!Pseudo: backup task orignal priority into mutex            */
                pstrMutex->u16OwnerOrignalPriority=pstrPendTCB->u16CurPriority;
                /*!Pseudo: set mutex is hold by the new task                  */
                pstrMutex->pstrOwnerTCB = pstrPendTCB;
                /*!Pseudo: reset the hold counter to one                      */
                pstrMutex->u8HoldCnt = (INT8U)1;

                /*!Pseudo: insert mutex object into tcb mutex hold list       */
                LIST_INSERT_HEAD( pstrPendTCB->strMutexHoldList,
                                  pstrMutex->strHoldList);
            }
        }
    }

#endif

    /*!Pseudo: remove link from tcb link list                                 */
    LIST_REMOVE_NODE(pstrTCB->strLinkList);

    /*!Pseudo: set task deleted state                                         */
    pstrTCB->u8State = KER_u8TSK_STAT_DEL;

    /*!Pseudo: increase task counter                                          */
    KER_u32TaskCnt--;

#if KER_coHOOK_TASK_DELETE_EN > 0u
    if (KER_NULL != KER_pfnHookTaskDelete)
    {
        /*!Pseudo: call user hook function                                    */
        KER_pfnHookTaskDelete(pstrTCB);
    }
#endif

    /*!Pseudo: leave critical code section                                    */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: schedule to new ready highest priority task                    */
    KER_vidSched();

    return KER_enuERR_OK;
}

#endif

#if KER_coIPC_MUTEX_EN > 0u
/*******************************************************************************
* @Brief  : change task mutex hold list orignal priority
* @Inputs : pstrList: hold list
* @Outputs: none
* @Returns: none
* @Note   : kernel internal function.
*******************************************************************************/
PRIVATE void vidTaskMutexHoldListOrignalPriorityChange( KER_tstrList *pstrList,
                                                        INT16U u16Priority)
{
    KER_tstrList *pstrNode;
    KER_tstrMutex  *pstrMutex;

    for (pstrNode = pstrList->pstrPrev;
         pstrNode != pstrList;
         pstrNode = pstrNode->pstrPrev)
    {
        pstrMutex = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrMutex, strHoldList);
        if (u16Priority < pstrMutex->u16OwnerOrignalPriority )
        {
            pstrMutex->u16OwnerOrignalPriority = u16Priority;
        }
        else
        {
            /*!Pseudo: previous mutex owner orignal priority is higher,
             *         not need change any more                               */
            break;
        }
    }

}
#endif


#if KER_coTASK_PRIO_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : change task priority.
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
PUBLIC KER_tenuERR KER_enuTaskPrioChange( KER_tstrTCB *pstrTCB,
                                          INT16U u16Priority)
{
    KER_CPU_SR_ALLOC();
    BOOL bPendListNeedChange = FALSE;

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseduo: check if tcb pointer is valid                                  */
    if (KER_NULL == pstrTCB)
    {
        return KER_enuERR_TCB_PTR_INV;
    }

    /* !Pseudo: check if new priority is legal,
     * should less than idle task priority                                    */
    if (u16Priority >= KER_u16TASK_PRIO_MAX-(INT16U)1)
    {
        return KER_enuERR_PRIORITY_INV;
    }

#endif

    KER_ENTER_CRITICAL();

    switch (pstrTCB->u8State)
    {
        case KER_u8TSK_STAT_RDY:
            KER_vidRdyListRemove(pstrTCB);
            pstrTCB->u16CurPriority = u16Priority;
            KER_vidRdyListInsertToHead(pstrTCB);

#if KER_coIPC_MUTEX_EN > 0u
            /*!Pseudo: change mutex hold list mutex's task orignal priority   */
            if (!LIST_IS_EMPTY(pstrTCB->strMutexHoldList))
            {
                vidTaskMutexHoldListOrignalPriorityChange(
                                                &(pstrTCB->strMutexHoldList),
                                                 u16Priority);
            }
#endif

#if KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u
            if ( KER_NULL != KER_pfnHookTaskPrioChange)
            {
                /*!Pseudo: call user hook function                            */
                KER_pfnHookTaskPrioChange(pstrTCB);
            }
#endif

            KER_LEAVE_CRITICAL();
            break;
        case KER_u8TSK_STAT_DLY:
#if KER_coTASK_SUSPEND_EN > 0u
        case KER_u8TSK_STAT_SPD:
        case KER_u8TSK_STAT_SPD_DLY:
#endif
            pstrTCB->u16CurPriority = u16Priority;
#if KER_coIPC_MUTEX_EN > 0u
            /*!Pseudo: change mutex hold list mutex's task orignal priority   */
            if (!LIST_IS_EMPTY(pstrTCB->strMutexHoldList))
            {
                vidTaskMutexHoldListOrignalPriorityChange(
                                                &(pstrTCB->strMutexHoldList),
                                                 u16Priority);
            }
#endif

#if KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u
            if ( KER_NULL != KER_pfnHookTaskPrioChange)
            {
                /*!Pseudo: call user hook function                            */
                KER_pfnHookTaskPrioChange(pstrTCB);
            }
#endif

            KER_LEAVE_CRITICAL();
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
            void  *vidpIPCObj = (pstrTCB->strIPCObjPend).vidpIPCObject;

            /*!Pseduo: replace node position in pending list                  */
            switch ((pstrTCB->strIPCObjPend).enuIPCObjType)
            {

#if KER_coIPC_MUTEX_EN > 0u
                case KER_enuIPC_OBJ_TYPE_MUTEX:
                    pstrListHead=&(((KER_tstrMutex *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrMutex *)vidpIPCObj)->enuIPCMethod;
                    bPendListNeedChange = TRUE;
                    break;
#endif

#if KER_coIPC_SEM_EN > 0u
                case KER_enuIPC_OBJ_TYPE_SEM:
                    pstrListHead=&(((KER_tstrSem *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrSem *)vidpIPCObj)->enuIPCMethod;
                    bPendListNeedChange = TRUE;
                    break;
#endif

#if KER_coIPC_MSGQ_EN > 0u
                case KER_enuIPC_OBJ_TYPE_MSG_Q:
                    pstrListHead=&(((KER_tstrMsgQ *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrMsgQ *)vidpIPCObj)->enuIPCMethod;
                    bPendListNeedChange = TRUE;
                    break;
#endif

#if KER_coIPC_EVENT_EN > 0u
                case KER_enuIPC_OBJ_TYPE_EVENT:
                    pstrListHead=&(((KER_tstrEvent *)vidpIPCObj)->strPendList);
                    enuIPCMethod=((KER_tstrEvent *)vidpIPCObj)->enuIPCMethod;
                    bPendListNeedChange = TRUE;
                    break;
#endif

#if KER_coBUILT_IN_MSGQ_EN > 0u
                case KER_enuIPC_OBJ_TYPE_BUILT_IN_MSG_Q:
                    /*!Pseudo: set new priority                               */
                    pstrTCB->u16CurPriority = u16Priority;
                    /*!Pseudo: return directly                                */
                    break;
#endif

#if KER_coBUILT_IN_EVENT_EN > 0u
                case KER_enuIPC_OBJ_TYPE_BUILT_IN_EVENT:
                    /*!Pseudo: set new priority                               */
                    pstrTCB->u16CurPriority = u16Priority;
                    /*!Pseudo: return directly                                */
                    break;
#endif

#if KER_coBUILT_IN_SEM_EN > 0u
                case KER_enuIPC_OBJ_TYPE_BUILT_IN_SEM:
                    /*!Pseudo: set new priority                               */
                    pstrTCB->u16CurPriority = u16Priority;
                    /*!Pseudo: return directly                                */
                    break;
#endif

                case KER_enuIPC_OBJ_TYPE_UNKNOWN:
                default:
                    KER_LEAVE_CRITICAL();
                    KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
                    return KER_enuERR_IPC_OBJ_TYPE_INV;
            }

            if (bPendListNeedChange)
            {
                /*!Pseudo: remove pstrTCB node from pending list              */
                LIST_REMOVE_NODE(pstrTCB->strList);
                /*!Pseudo: set new priority                                   */
                pstrTCB->u16CurPriority = u16Priority;
                /*!Pseudo: re-insert into pending list                        */
                KER_vidPendListInsert(pstrListHead, pstrTCB, enuIPCMethod);
            }

#if KER_coIPC_MUTEX_EN > 0u
            /*!Pseudo: change mutex hold list mutex's task orignal priority   */
            if (!LIST_IS_EMPTY(pstrTCB->strMutexHoldList))
            {
                vidTaskMutexHoldListOrignalPriorityChange(
                                                &(pstrTCB->strMutexHoldList),
                                                 u16Priority);
            }
#endif

#if KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u
            if ( KER_NULL != KER_pfnHookTaskPrioChange)
            {
                /*!Pseudo: call user hook function                            */
                KER_pfnHookTaskPrioChange(pstrTCB);
            }
#endif
            KER_LEAVE_CRITICAL();
            break;
        }
        default:
            KER_LEAVE_CRITICAL();
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            return KER_enuERR_TASK_STATE_INV;
    }

    /*!Pseudo: trigger scheduler, switch to the new highest prio ready task   */
    KER_vidSched();

    return KER_enuERR_OK;
}
#endif

#if KER_coTASK_YIELD_EN > 0u
/*******************************************************************************
* @Brief  : current task yield cpu.
* @Inputs : none
* @Outputs: none
* @Returns: KER_enuERR_OK: yield cpu ok(after task re-scheduled return).
*           KER_enuERR_NOK: yield cpu not ok.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskYield( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();

    if ((KER_pstrCurTCB->u8State == KER_u8TSK_STAT_RDY)
       &&
    ((KER_pstrCurTCB->strList).pstrNext != (KER_pstrCurTCB->strList).pstrPrev))
    {
        LIST_REMOVE_NODE(KER_pstrCurTCB->strList);

        LIST_INSERT_TAIL(KER_astrTaskRdyList[KER_pstrCurTCB->u16CurPriority],
                         KER_pstrCurTCB->strList);

#if KER_coHOOK_TASK_YIELD_EN > 0u
        if (KER_NULL != KER_pfnHookTaskYield)
        {
            /*!Pseudo: call user hook function                                */
            KER_pfnHookTaskYield(KER_pstrCurTCB);
        }
#endif

        KER_LEAVE_CRITICAL();

        KER_vidSched();

        return KER_enuERR_OK;
    }

    KER_LEAVE_CRITICAL();

    return KER_enuERR_NOK;
}
#endif

#if KER_coSWITCH_CONTEXT_CNT_EN > 0u
/*******************************************************************************
* @Brief  : set task switch context counter.
* @Inputs : pstrTCB: task tcb pointer.
*                    if pstrTCB is KER_NULL, set task self context switch counter,
*                    otherwise, set the specified task context switch counter.
*           u32Cnt: switch context counter value.
* @Outputs: none.
* @Returns: none
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidTaskSwCtxCntSet( KER_tstrTCB *pstrTCB, INT32U u32Cnt )
{
    KER_CPU_SR_ALLOC();

    /*!Pseudo: if pstTCB is KER_NULL, set task self switch context counter        */
    if ( KER_NULL == pstrTCB )
    {
        KER_ENTER_CRITICAL();
        KER_pstrCurTCB->u32SwCtxCnt = u32Cnt;
        KER_LEAVE_CRITICAL();
    }
    else
    {
        /*!Pseudo: set a specified task switch context counter                */
        KER_ENTER_CRITICAL();
        pstrTCB->u32SwCtxCnt = u32Cnt;
        KER_LEAVE_CRITICAL();
    }
}

#endif

#if KER_coSWITCH_CONTEXT_CNT_EN > 0u
/*******************************************************************************
* @Brief  : get task switch context counter.
* @Inputs : pstrTCB: task tcb pointer.
*                    if pstrTCB is KER_NULL, get task self context switch counter,
*                    otherwise, get the specified task context switch counter.
* @Outputs: none.
* @Returns: tasks switch context conter.
* @Note   : none.
*******************************************************************************/
PUBLIC INT32U KER_u32TaskSwCtxCntGet( KER_tstrTCB *pstrTCB )
{
    KER_CPU_SR_ALLOC();
    INT32U u32Cnt;

    /*!Pseudo: if pstTCB is KER_NULL, get task self switch context counter        */
    if ( KER_NULL == pstrTCB )
    {
        KER_ENTER_CRITICAL();
        u32Cnt = KER_pstrCurTCB->u32SwCtxCnt;
        KER_LEAVE_CRITICAL();
    }
    else
    {
        /*!Pseudo: get a specified task switch context counter                */
        KER_ENTER_CRITICAL();
        u32Cnt = pstrTCB->u32SwCtxCnt;
        KER_LEAVE_CRITICAL();
    }
    return u32Cnt;
}
#endif

#if (  (KER_coSWITCH_CONTEXT_CNT_EN > 0u) \
    || (KER_coCPULOAD_MODULE_EN > 0u) \
    || (KER_coHOOK_TASK_CTX_SW_EN > 0u))
/*******************************************************************************
* @Brief  : task switch context callback function.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : called before task switch context.
*******************************************************************************/
PUBLIC void KER_vidTaskSwCtxCallBack(void)
{

#if KER_coSWITCH_CONTEXT_CNT_EN > 0u
    /*!Pseudo: increase next task context switch counter                      */
    KER_pstrNextTCB->u32SwCtxCnt++;
#endif

#if KER_coCPULOAD_MODULE_EN > 0u
    /*!Pseudo: update CPU load related counter                                */
    KER_vidCPULoadPerformance();
#endif

#if (KER_coHOOK_EN > 0u) && (KER_coHOOK_TASK_CTX_SW_EN > 0u)
    if (KER_NULL != KER_pfnHookTaskSwCtx)
    {
        /*!Pseduo: here, call user hook function                              */
        KER_pfnHookTaskSwCtx(KER_pstrCurTCB, KER_pstrNextTCB);
    }
#endif
}
#endif

#if KER_coTASK_SUSPEND_EN > 0u
/*******************************************************************************
* @Brief  : suspend task
* @Inputs : pstrTCB: task tcb pointer. (KER_NULL task self)
* @Outputs: none
* @Returns: KER_enuERR_OK: suspend task successful.
*           KER_enuERR_SUSPEND_IN_ISR: not allow suspend task in ISR.
*           KER_enuERR_SUSPEND_IDLE_TASK: not allow suspend idle task.
*           KER_enuERR_SUSPEND_TICK_TASK: not allow suspend tick task.
*           KER_enuERR_SUSPEND_TIMER_TASK: not allow suspend tick task.
*           KER_enuERR_SUSPEND_STKCHK_TASK: not allow suspend stack check task.
*           KER_enuERR_SUSPEND_SELF_FAILED: suspend self not allow
*                                           when scheduler locked.
*           KER_enuERR_KERNEL_EXCEPTION: exception cause by task state unknown.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskSuspend(KER_tstrTCB *pstrTCB)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_SUSPEND_IN_ISR;
    }

    /*!Pseudo: not allow to suspend idle task                                 */
    if (pstrTCB == &KER_strIdleTaskTcb)
    {
        return KER_enuERR_SUSPEND_IDLE_TASK;
    }

#if KER_coTICK_TASK_EN > 0u

    /*!Pseudo: not allow to suspend tick task                                 */
    if (pstrTCB == &KER_strTickTaskTcb)
    {
        return KER_enuERR_SUSPEND_TICK_TASK;
    }

#endif

#if KER_coTIMER_TASK_EN > 0u

    /*!Pseudo: not allow to suspend timer task                                */
    if (pstrTCB == &KER_strTimerTaskTcb)
    {
        return KER_enuERR_SUSPEND_TIMER_TASK;
    }

#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))

    /*!Pseudo: not allow to suspend stack check task                          */
    if (pstrTCB == &KER_strStackChkTaskTcb)
    {
        return KER_enuERR_SUSPEND_STKCHK_TASK;
    }

#endif

#endif

    /*!Pseudo: check if suspend task self                                     */
    if (KER_NULL == pstrTCB)
    {
        if (KER_u8SchedLockNestCnt > (INT8U)0)
        {
            /*!Pseudo: task not allow suspend self when sched locked          */
            return KER_enuERR_SUSPEND_SELF_FAILED;
        }

        /*!Pseudo: get current tcb pointer                                    */
        pstrTCB = KER_pstrCurTCB;
    }

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: suspend task                                                   */
    switch (pstrTCB->u8State)
    {
        case KER_u8TSK_STAT_RDY:
            KER_vidRdyListRemove(pstrTCB);
            pstrTCB->u8State = KER_u8TSK_STAT_SPD;
            pstrTCB->u8SuspendNestCnt = (INT8U)1;

#if KER_coHOOK_TASK_SUSPEND_EN > 0u
            if (KER_NULL != KER_pfnHookTaskSuspend)
            {
                /*!Pseudo: call user hook function                            */
                KER_pfnHookTaskSuspend(pstrTCB);
            }
#endif

            KER_LEAVE_CRITICAL();
            break;
        case KER_u8TSK_STAT_DLY:
        case KER_u8TSK_STAT_BLK:
        case KER_u8TSK_STAT_BLK_DLY:
            pstrTCB->u8State |= KER_u8TSK_STAT_BIT_SPD;
            pstrTCB->u8SuspendNestCnt = (INT8U)1;

#if KER_coHOOK_TASK_SUSPEND_EN > 0u
            if (KER_NULL != KER_pfnHookTaskSuspend)
            {
                /*!Pseudo: call user hook function                            */
                KER_pfnHookTaskSuspend(pstrTCB);
            }
#endif

            KER_LEAVE_CRITICAL();
            break;
        case KER_u8TSK_STAT_SPD:
        case KER_u8TSK_STAT_SPD_DLY:
        case KER_u8TSK_STAT_SPD_BLK:
        case KER_u8TSK_STAT_SPD_BLK_DLY:
            (pstrTCB->u8SuspendNestCnt)++;

#if KER_coHOOK_TASK_SUSPEND_EN > 0u
            if (KER_NULL != KER_pfnHookTaskSuspend)
            {
                /*!Pseudo: call user hook function                            */
                KER_pfnHookTaskSuspend(pstrTCB);
            }
#endif

            KER_LEAVE_CRITICAL();
            break;
        default:
            KER_LEAVE_CRITICAL();
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            return KER_enuERR_KERNEL_EXCEPTION;
    }

    /*!Pseudo: if suspend task self, need do schedule to other task           */
    if (pstrTCB == KER_pstrCurTCB)
    {
        KER_vidSched();
    }

    return KER_enuERR_OK;
}
#endif

#if KER_coTASK_SUSPEND_EN > 0u
/*******************************************************************************
* @Brief  : resume task
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: resume task successful.
*           KER_enuERR_RESUME_IN_ISR: not allow resume task in ISR.
*           KER_enuERR_RESUME_SELF: not allow resume task self.
*           KER_enuERR_RESUME_IDLE_TASK: not allow resume idle task.
*           KER_enuERR_RESUME_TICK_TASK: not allow resume tick task.
*           KER_enuERR_RESUME_TIMER_TASK: not allow resume timer task.
*           KER_enuERR_RESUME_STKCHK_TASK: not allow resume stack check task.
*           KER_enuERR_RESUME_NOT_SUSPEND: resume a not suspend task.
*           KER_enuERR_KERNEL_EXCEPTION: exception cause by unknown task state.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskResume(KER_tstrTCB *pstrTCB)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: ISR not allow call this function                               */
    if (KER_u8IntrNestCnt > (INT8U)0)
    {
        return KER_enuERR_RESUME_IN_ISR;
    }

    /*!Pseudo: task not allow resume self                                     */
    if ((KER_NULL == pstrTCB) || (pstrTCB == KER_pstrCurTCB))
    {
        return KER_enuERR_RESUME_SELF;
    }

    /*!Pseudo: should not suspend idle task                                   */
    if (pstrTCB == &KER_strIdleTaskTcb)
    {
        return KER_enuERR_RESUME_IDLE_TASK;
    }

#if KER_coTICK_TASK_EN > 0u

    /*!Pseudo: not allow to resume tick task                                 */
    if (pstrTCB == &KER_strTickTaskTcb)
    {
        return KER_enuERR_RESUME_TICK_TASK;
    }

#endif

#if KER_coTIMER_TASK_EN > 0u

    /*!Pseudo: not allow to resume timer task                                */
    if (pstrTCB == &KER_strTimerTaskTcb)
    {
        return KER_enuERR_RESUME_TIMER_TASK;
    }

#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))

    /*!Pseudo: not allow to suspend stack check task                          */
    if (pstrTCB == &KER_strStackChkTaskTcb)
    {
        return KER_enuERR_RESUME_STKCHK_TASK;
    }

#endif

#endif

    KER_ENTER_CRITICAL();
    /*!Pseudo: resume task                                                    */
    switch (pstrTCB->u8State)
    {

        case KER_u8TSK_STAT_SPD:
            (pstrTCB->u8SuspendNestCnt)--;
            if ((INT8U)0 == (pstrTCB->u8SuspendNestCnt))
            {
                pstrTCB->u8State = KER_u8TSK_STAT_RDY;
                KER_vidRdyListInsertToHead(pstrTCB);
            }

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
            if (KER_NULL != KER_pfnHookTaskResume)
            {
                KER_pfnHookTaskResume(pstrTCB);
            }
#endif

            KER_LEAVE_CRITICAL();
            break;
        case KER_u8TSK_STAT_SPD_DLY:
        case KER_u8TSK_STAT_SPD_BLK:
        case KER_u8TSK_STAT_SPD_BLK_DLY:
            (pstrTCB->u8SuspendNestCnt)--;
            if ((INT8U)0 == (pstrTCB->u8SuspendNestCnt))
            {
                pstrTCB->u8State &= (INT8U)(~KER_u8TSK_STAT_BIT_SPD);
            }

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
            if (KER_NULL != KER_pfnHookTaskResume)
            {
                KER_pfnHookTaskResume(pstrTCB);
            }
#endif

            KER_LEAVE_CRITICAL();
            break;
        case KER_u8TSK_STAT_RDY:
        case KER_u8TSK_STAT_DLY:
        case KER_u8TSK_STAT_BLK:
        case KER_u8TSK_STAT_BLK_DLY:
            KER_LEAVE_CRITICAL();
            return KER_enuERR_RESUME_NOT_SUSPEND;
        default:
            KER_LEAVE_CRITICAL();
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            return KER_enuERR_KERNEL_EXCEPTION;
    }

    KER_vidSched();

    return KER_enuERR_OK;
}
#endif

/*******************************************************************************
* @Brief  : get task self tcb pointer
* @Inputs : none.
* @Outputs: none.
* @Returns: task self tcb pointer.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tstrTCB * KER_pstrTaskTCBPtrGet( void )
{
    return KER_pstrCurTCB;
}


