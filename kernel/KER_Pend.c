/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Pend.c
*Version    : V1.00
*Description: MRTOS kernel object pend management.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/30
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*******************************************************************************
* @Brief  : insert tcb to mutex pending list.
* @Inputs : pstrList: pend list pointer.
*           pstrTCB: tcb pointer.
*           enuIPCMethod: ipc pending list schedule strategy method.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidPendListInsert( KER_tstrList *pstrList,
                                   KER_tstrTCB *pstrTCB,
                                   KER_tenuIPCMethod enuIPCMethod)
{
    switch (enuIPCMethod)
    {
        case KER_enuIPC_METHOD_PRIO:
        {
            KER_tstrTCB  *pstrNextTCB;
            KER_tstrList *pstrNextNode;

            /*!Pseudo: find insert position                                   */
            for (pstrNextNode  = pstrList->pstrNext;
                 pstrNextNode != pstrList;
                 pstrNextNode  = pstrNextNode->pstrNext)
            {
                pstrNextTCB = LIST_NODE_ENTRY_PTR(pstrNextNode,
                                                  KER_tstrTCB,
                                                  strList);

                if (pstrTCB->u16CurPriority <= pstrNextTCB->u16CurPriority)
                {
                    /*!Pseudo: found insert position, break loop              */
                    break;
                }
            }

            /*!Pseudo: insert before current node,
             * A special case is not found in the loop end, or list is empty,
             * in this case, insert as a list tail node                       */
            LIST_INSERT_BEFORE_PTR(pstrNextNode, &(pstrTCB->strList));

            break;
        }
        case KER_enuIPC_METHOD_FIFO:
            /*!Pseudo: insert to pending list tail                            */
            LIST_INSERT_TAIL_PTR(pstrList, &(pstrTCB->strList));
            break;
        case KER_enuIPC_METHOD_LIFO:
            /*!Pseudo: insert to pending list tail                            */
            LIST_INSERT_HEAD_PTR(pstrList, &(pstrTCB->strList));
            break;
        case KER_enuIPC_METHOD_UNKNOWN:
        default:
            KER_vidRaiseException(__FILE__, (INT32U)__LINE__);
            break;
    }
}

/*******************************************************************************
* @Brief  : remove pend list all node.
* @Inputs : pstrList: list pointer.
*           enuError: set pending task error flag.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidPendListRemoveAll( KER_tstrList *pstrList,
                                      KER_tenuERR   enuError)
{
    KER_tstrList *pstrNode;
    KER_tstrTCB  *pstrTCB;

    /*!Pseudo: loop to remove mutex pend list                                 */
    pstrNode = pstrList->pstrNext;
    while (pstrNode != pstrList)
    {
        /*!Pseudo: remove task all pending list                               */
        pstrTCB = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrTCB, strList);
        pstrNode=pstrNode->pstrNext;
        LIST_REMOVE_NODE(pstrTCB->strList);

        /*!Pseudo: reset task pending object information                      */
        (pstrTCB->strIPCObjPend).enuIPCObjType=KER_enuIPC_OBJ_TYPE_UNKNOWN;
        (pstrTCB->strIPCObjPend).vidpIPCObject=KER_NULL;

        /*!Pseudo: clear task pending state                                   */
        pstrTCB->u8State     &= (INT8U)(~KER_u8TSK_STAT_BIT_BLK);

        /*!Pseudo: check if set pending timeout, remove from tick list        */
        if (pstrTCB->u8State & KER_u8TSK_STAT_BIT_DLY)
        {
                KER_vidTickListRemove(pstrTCB);
        }

        /*!Pseudo: if task is ready to running, insert to ready list head     */
        if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State )
        {
            /*!Pseudo: Insert the new ready task to ready list head           */
            KER_vidRdyListInsertToHead(pstrTCB);
        }

        /*!Pseudo: indicate task mutex object already deleted.                */
        pstrTCB->enuError = enuError;
    }
}

/*******************************************************************************
* @Brief  : pend ipc object
* @Inputs : vidpObj: ipc object pointer(also is the object pending list pointer)
*           enuObjType: ipc object type.
*           enuIPCMethod: ipc method.
*           u32TimeOut: timeout value.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidObjectPend(void *vidpObj,
                              KER_tenuIPCObjType enuObjType,
                              KER_tenuIPCMethod enuIPCMethod,
                              INT32U u32TimeOut)
{
    /*!Pseudo: remove current task from tcb ready list, for task blocking     */
    KER_vidRdyListRemove(KER_pstrCurTCB);

    /*!Pseudo: insert to pending list                                         */
    KER_vidPendListInsert((KER_tstrList *)vidpObj,
                           KER_pstrCurTCB,
                           enuIPCMethod);

    /*!Pseudo: set task blocking state                                        */
    KER_pstrCurTCB->u8State |= KER_u8TSK_STAT_BIT_BLK;

    /*!Pseudo: check if need set pending timeout                              */
    if (u32TimeOut)
    {
        /*!Pseudo: set pending timeout                                        */
        KER_vidTickListInsert(KER_pstrCurTCB, u32TimeOut);
    }

    /*!Pseudo: set task pending object information                            */
    (KER_pstrCurTCB->strIPCObjPend).enuIPCObjType = enuObjType;
    (KER_pstrCurTCB->strIPCObjPend).vidpIPCObject = vidpObj;

    /*!Pseduo: clear error flag, for the pending status update                */
    KER_pstrCurTCB->enuError = KER_enuERR_OK;
}

/*******************************************************************************
* @Brief  : pend built in object
* @Inputs : enuObjType: pending built in object type.
*           u32TimeOut: pending timeout value.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidBuiltInObjectPend( KER_tenuIPCObjType enuObjType,
                                      INT32U u32TimeOut)
{
    /*!Pseudo: remove current task from tcb ready list, for task blocking     */
    KER_vidRdyListRemove(KER_pstrCurTCB);

    /*!Pseudo: set task blocking state                                        */
    KER_pstrCurTCB->u8State |= KER_u8TSK_STAT_BIT_BLK;

    /*!Pseudo: check if need set pending timeout                              */
    if (u32TimeOut)
    {
        /*!Pseudo: set pending timeout                                        */
        KER_vidTickListInsert(KER_pstrCurTCB, u32TimeOut);
    }

    /*!Pseudo: set task pending object information                            */
    (KER_pstrCurTCB->strIPCObjPend).enuIPCObjType = enuObjType;
    (KER_pstrCurTCB->strIPCObjPend).vidpIPCObject = KER_NULL;

    /*!Pseduo: clear error flag, for the pending status update                */
    KER_pstrCurTCB->enuError = KER_enuERR_OK;
}

/*******************************************************************************
* @Brief  : abort pend object
* @Inputs : pstrTCB: task tcb pointer.
*           enuErrorFlag: error flag.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidObjectPendAbort(KER_tstrTCB *pstrTCB,
                                   KER_tenuERR enuErrorFlag)
{
    /*!Pseudo: remvoe pstrTCB node from pending list                          */
    LIST_REMOVE_NODE(pstrTCB->strList);

    /*!Pseudo: reset task pending object information                          */
    (pstrTCB->strIPCObjPend).enuIPCObjType = KER_enuIPC_OBJ_TYPE_UNKNOWN;
    (pstrTCB->strIPCObjPend).vidpIPCObject = KER_NULL;

    /*!Pseudo: clear task pending state                                       */
    pstrTCB->u8State &= (INT8U)(~KER_u8TSK_STAT_BIT_BLK);

    /*!Pseudo: check if set pending timeout, then remove from tick list       */
    if (pstrTCB->u8State & KER_u8TSK_STAT_BIT_DLY)
    {
        KER_vidTickListRemove(pstrTCB);
    }

    /*!Pseudo: indicate task pending object error flag                        */
    pstrTCB->enuError    = enuErrorFlag;

    /*!Pseudo: if task is ready to running, then insert to ready list head    */
    if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State )
    {
        /*!Pseudo: Insert the new ready task to corrsponding ready list head  */
        KER_vidRdyListInsertToHead(pstrTCB);
    }

}

/*******************************************************************************
* @Brief  : abort pend built in object
* @Inputs : pstrTCB: task tcb pointer.
*           enuErrorFlag: error flag.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidBuiltInObjectPendAbort(KER_tstrTCB *pstrTCB,
                                          KER_tenuERR enuErrorFlag)
{
    /*!Pseudo: reset task pending object information                          */
    (pstrTCB->strIPCObjPend).enuIPCObjType = KER_enuIPC_OBJ_TYPE_UNKNOWN;
    (pstrTCB->strIPCObjPend).vidpIPCObject = KER_NULL;

    /*!Pseudo: clear task pending state                                       */
    pstrTCB->u8State &= (INT8U)(~KER_u8TSK_STAT_BIT_BLK);

    /*!Pseudo: check if set pending timeout, then remove from tick list       */
    if (pstrTCB->u8State & KER_u8TSK_STAT_BIT_DLY)
    {
        KER_vidTickListRemove(pstrTCB);
    }

    /*!Pseudo: indicate task pending object error flag                        */
    pstrTCB->enuError    = enuErrorFlag;

    /*!Pseudo: if task is ready to running, then insert to ready list head    */
    if ( KER_u8TSK_STAT_RDY == pstrTCB->u8State )
    {
        /*!Pseudo: Insert the new ready task to corrsponding ready list head  */
        KER_vidRdyListInsertToHead(pstrTCB);
    }
}

/*******************************************************************************
* @Brief  : abort task pending built in object.
* @Inputs : pstrTCB: task tcb pointer.
*           enuOption: schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort task pending built in object successful.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending built in object.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTaskBuiltInObjPendAbort(
                                          KER_tstrTCB   *pstrTCB,
                                          KER_tenuIPCObjType enuBuiltInObjType,
                                          KER_tenuSchedOpt enuOption)
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

    /*!Pseudo: check if task already pending built in event object            */
    if ((pstrTCB->strIPCObjPend).enuIPCObjType
         != enuBuiltInObjType)

    {
        KER_LEAVE_CRITICAL();
        /*!Pseudo: return error not pending this object                       */
        return KER_enuERR_IPC_NOT_PEND_THIS;
    }

    /*!Pseudo: abort pending event object                                     */
    KER_vidBuiltInObjectPendAbort(pstrTCB, KER_enuERR_IPC_PEND_ABORT);

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


