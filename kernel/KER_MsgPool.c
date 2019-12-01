/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_MsgPool.c
*Version    : V1.00
*Description: message pool manager module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/23
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if (KER_coIPC_MSGQ_EN > 0u) || (KER_coBUILT_IN_MSGQ_EN > 0u)

/*******************************************************************************
* @Brief  : initial message pool.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
*           vidpMsgPool: message store pool.
*           u32MsgPoolSize: message pool size(bytes).
*           u16MsgSize: message size.
* @Outputs: none
* @Returns: none
* @Note   : kernel internal fucntion.
*******************************************************************************/
PUBLIC void KER_vidMsgPoolInit(KER_tstrMsgPool *pstrMsgPoolObj,
                               void    *vidpMsgPool,
                               INT32U   u32MsgPoolSize,
                               INT16U   u16MsgSize)
{
    CPU_REG INT16U u16Idx;
    CPU_REG KER_tstrMsg *pstrMsg;
    CPU_REG INT16U u16MsgsMax;

    /*!Pseudo: record message pool base address                               */
    pstrMsgPoolObj->vidpMsgPool = vidpMsgPool;

    /*!Pseudo: reset message pool saved messages number                       */
    pstrMsgPoolObj->u16Entries = (INT16U)0;

    /*!Pseudo: calculate message valid size                                   */
    u16MsgSize = (INT16U)KER_LO_ALIGN((INT16U)(u16MsgSize+sizeof(KER_tstrMsg)));

    /*!Pseudo: record one message can be received maximum size                */
    pstrMsgPoolObj->u16MsgSize  = u16MsgSize - sizeof(KER_tstrMsg);

    /*!Pseudo: calculate message pool valid using size afte align             */
    u32MsgPoolSize = u32MsgPoolSize -
                     ((CPU_ADDR)(vidpMsgPool) - KER_ADDR_LO_ALIGN(vidpMsgPool));

    /*!Pseudo: calculate number of messages can be saved in queue             */
    u16MsgsMax = (INT16U)(u32MsgPoolSize/u16MsgSize);

    /*!Pseudo: record message pool can be save maximum number of messages     */
    pstrMsgPoolObj->u16MsgsMax  = u16MsgsMax;

    /*!Pseudo: initial message pool message store list                        */
    LIST_INIT(pstrMsgPoolObj->strMsgList);

    /*!Pseudo: initial message pool message free list                         */
    LIST_INIT(pstrMsgPoolObj->strFreeList);

    /*!Pseudo: get first message address after align pool address             */
    pstrMsg = (KER_tstrMsg *)KER_ADDR_LO_ALIGN(vidpMsgPool);

    /*!Pseudo: allocate message pool to free list                             */
    for (u16Idx = (INT16U)0; u16Idx < u16MsgsMax; u16Idx++)
    {
        LIST_INSERT_HEAD(pstrMsgPoolObj->strFreeList, pstrMsg->strList);
        pstrMsg = (KER_tstrMsg *)(((INT8U *)pstrMsg) + u16MsgSize);
    }
}

/*******************************************************************************
* @Brief  : message copy
* @Inputs : vidpDst: destination message buffer pointer.
*           vidpSrc: source message buffer pointer.
*           u16Size: copy size.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidMsgPoolMsgCopy( void *vidpDst,
                                   void *vidpSrc,
                                   INT16U u16Size)
{
    CPU_REG INT16U u16Idx;
    /*!Pseudo: loop copy source message to destination message buffer         */
    for (u16Idx = (INT16U)0; u16Idx < u16Size; u16Idx++)
    {
        *((INT8U *)(vidpDst) + u16Idx) = *((INT8U *)(vidpSrc) + u16Idx);
    }
}
/*******************************************************************************
* @Brief  : query number of messages saved in message pool.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: number of messages saved in message pool.
* @Note   : none
*******************************************************************************/
PUBLIC INT16U KER_u16MsgPoolMsgNbrQuery( KER_tstrMsgPool *pstrMsgPoolObj )
{
    return pstrMsgPoolObj->u16Entries;
}

/*******************************************************************************
* @Brief  : check message pool is empty.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: TRUE: is empty.
*           FALSE: not empty.
* @Note   : none
*******************************************************************************/
PUBLIC BOOL KER_bMsgPoolIsEmpty( KER_tstrMsgPool *pstrMsgPoolObj )
{
    return ((pstrMsgPoolObj->u16Entries) ? FALSE : TRUE);
}

/*******************************************************************************
* @Brief  : check message pool is full.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: TRUE: is full.
*           FALSE: not full.
* @Note   : none
*******************************************************************************/
PUBLIC BOOL KER_bMsgPoolIsFull( KER_tstrMsgPool *pstrMsgPoolObj )
{
    if (pstrMsgPoolObj->u16Entries >= pstrMsgPoolObj->u16MsgsMax)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* @Brief  : get message pool one message buffer size.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: messge size.
* @Note   : none
*******************************************************************************/
PUBLIC INT16U KER_u16MsgPoolMsgSizeGet( KER_tstrMsgPool *pstrMsgPoolObj )
{
    return pstrMsgPoolObj->u16MsgSize;
}

/*******************************************************************************
* @Brief  : get maximum number of messages can be saved in pool.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: maximum messages number.
* @Note   : none
*******************************************************************************/
PUBLIC INT16U KER_u16MsgPoolMsgsMaxGet( KER_tstrMsgPool *pstrMsgPoolObj )
{
    return pstrMsgPoolObj->u16MsgsMax;
}

/*******************************************************************************
* @Brief  : flush message pool.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidMsgPoolFlush( KER_tstrMsgPool *pstrMsgPoolObj )
{
    if (pstrMsgPoolObj->u16Entries > (INT16U)0)
    {
        /*!Pseudo: reset message pool saved messages number                   */
        pstrMsgPoolObj->u16Entries = (INT16U)0;
        /*!Pseudo: merge message message list to free list                    */
        LIST_MERGE(pstrMsgPoolObj->strFreeList, pstrMsgPoolObj->strMsgList);
    }
}

/*******************************************************************************
* @Brief  : read message queue.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
*           enuRecvMethod: read front or rear.
* @Outputs: vidpMsg: receiver buffer.
*           pu16Size: received size.
* @Returns: KER_enuERR_OK: received message.
*           KER_enuERR_NOK: not received message.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgPoolRead(KER_tstrMsgPool *pstrMsgPoolObj,
                                      void            *vidpMsg,
                                      INT16U          *pu16Size,
                                      KER_tenuMsgQRecvOpt  enuRecvMethod)
{

    if (pstrMsgPoolObj->u16Entries > (INT16U)0)
    {
        KER_tstrList *pstrNode;
        KER_tstrMsg  *pstrMsg;

        if ( KER_enuMSGQ_RECV_OPT_REAR != enuRecvMethod)
        {
            /*!Pseudo: get message queue head pointer                         */
            pstrNode = (pstrMsgPoolObj->strMsgList).pstrNext;
        }
        else
        {
            /*!Pseudo: get message queue tail pointer                         */
            pstrNode = (pstrMsgPoolObj->strMsgList).pstrPrev;
        }

        pstrMsg = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrMsg, strList);

        /*!Pseudo: copy message to receive buffer                             */
        KER_vidMsgPoolMsgCopy( vidpMsg, pstrMsg + 1, pstrMsg->u16MsgSize);

        /*!Pseudo: get message size                                           */
        *pu16Size = pstrMsg->u16MsgSize;

        /*!Pseudo: delete received message from message queue                 */
        LIST_REMOVE_NODE_PTR(pstrNode);

        /*!Pseudo: update free queue head pointer                             */
        LIST_INSERT_HEAD(pstrMsgPoolObj->strFreeList, *pstrNode);

        /*!Pseudo: update number of stored messages                           */
        pstrMsgPoolObj->u16Entries--;

        return KER_enuERR_OK;
    }

    return KER_enuERR_NOK;
}

/*******************************************************************************
* @Brief  : write message queue.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
*           vidpMsg: write message buffer.
*           u16Size: write message size.
*           enuSendMethod: send method.
* @Outputs: none
* @Returns: KER_enuERR_OK: write message ok.
*           KER_enuERR_NOK: message queue full.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuMsgPoolWrite(KER_tstrMsgPool *pstrMsgPoolObj,
                                       void            *vidpMsg,
                                       INT16U           u16Size,
                                       KER_tenuMsgQSendOpt  enuSendMethod)
{


    /*!Pseudo: check message queue is not full                                */
    if (pstrMsgPoolObj->u16Entries < pstrMsgPoolObj->u16MsgsMax)
    {
        KER_tstrMsg  *pstrMsg;

        /*!Comment: no task pending message queue, store message              */
        /*!Pseudo: get free message buffer                                    */
        pstrMsg = LIST_NODE_ENTRY_PTR( (pstrMsgPoolObj->strFreeList).pstrNext,
                                        KER_tstrMsg,
                                        strList);

        /*!Pseudo: remove from free list                                      */
        LIST_REMOVE_NODE(pstrMsg->strList);

        /*!Pseudo: copy message to message queue buffer                       */
        KER_vidMsgPoolMsgCopy(pstrMsg + 1, vidpMsg, u16Size);

        /*!Pseudo: set message size                                           */
        pstrMsg->u16MsgSize = u16Size;

        /*!Pseudo: insert to message queue                                    */
        if (KER_enuMSGQ_SEND_OPT_REAR != enuSendMethod)
        {
            /*!Pseudo: insert to message queue list head                      */
            LIST_INSERT_HEAD(pstrMsgPoolObj->strMsgList, pstrMsg->strList);
        }
        else
        {
            /*!Pseudo: insert to message queue list tail                      */
            LIST_INSERT_TAIL(pstrMsgPoolObj->strMsgList,  pstrMsg->strList);
        }

        /*!Pseudo: increase number of message entries                         */
        pstrMsgPoolObj->u16Entries++;

        return KER_enuERR_OK;
    }
    else
    {
        return KER_enuERR_NOK;
    }
}

#endif
