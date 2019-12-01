/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_StackCheck.c
*Version    : V1.00
*Description: task stack overflow check module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/10/11
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))
/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/

#if KER_coTASK_STACK_LIMIT_EN > 0u
PRIVATE void vidTaskStackOverflowHandler( KER_tstrTCB *pstrTCB );
#endif

PRIVATE CPU_STK_SIZE stkszStackMaxUsage( CPU_STK *pstkStackBase,
                                         CPU_STK_SIZE stkszStackSize );

PRIVATE void vidStackCheckTaskEntry( void *vidpParam );

#if KER_coTASK_STACK_LIMIT_EN > 0u
/*******************************************************************************
* @Brief  : task tack overflow handler.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidTaskStackOverflowHandler( KER_tstrTCB *pstrTCB )
{
#if KER_coHOOK_STK_OVERFLOW_EN > 0u
    if (KER_NULL != KER_pfnHookStackOverflow)
    {
        /*!Pseudo: call user hook function                                    */
        KER_pfnHookStackOverflow(pstrTCB);
    }
#else
    KER_NOT_USED_ARGUMENT(pstrTCB);
#endif
}
#endif

/*******************************************************************************
* @Brief  : calculate task max usage
* @Inputs : pstkStackBase: task stack base address.
*           stkszStackSize: task stack size(bytes).
* @Outputs: none
* @Returns: task stack maximum using size(bytes)
* @Note   : none
*******************************************************************************/
PRIVATE CPU_STK_SIZE stkszStackMaxUsage( CPU_STK *pstkStackBase,
                                         CPU_STK_SIZE stkszStackSize )
{
    CPU_REG INT8U u8TokenIdx;
    CPU_REG CPU_STK *pstkStack;

#if ((KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_FULL_DOWN) \
  || (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_EMPTY_DOWN))
    CPU_REG CPU_STK *pstkStackEnd;
#endif

    CPU_STK_SIZE stkszSize = (CPU_STK_SIZE)0;

#if ((KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_FULL_DOWN) \
  || (KER_u8STACK_GROWTH_CFG == KER_u8STACK_GROWTH_EMPTY_DOWN))

    /*!Pseudo: calculate set stack token start lowest address                 */
    pstkStack = (CPU_STK *)KER_ADDR_HI_ALIGN(pstkStackBase);
    pstkStackEnd = (CPU_STK *)KER_ADDR_LO_ALIGN((((INT8U *)pstkStackBase)
                                                        + stkszStackSize));
    /*!Pseduo: initial token index to access lowest cell of CPU_STK type      */
    u8TokenIdx = (INT8U)0;

    /*!Pseudo: scan task stack space stack token string                       */
    while (pstkStack < pstkStackEnd)
    {
        /*!Pseudo: compare is stack token                                     */
        if (*pstkStack != KER_kuniStackToken.astkToken[u8TokenIdx])
        {
            /*!Pseudo: not stack token, end the scan loop                     */
            break;
        }

        /*!Pseudo: increase stack free size                                   */
        stkszSize = stkszSize + sizeof(CPU_STK);

        /*!Pseudo: increase stack pointer to scan next position               */
        pstkStack++;

        /*!Pseudo: adjust token index                                         */
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

    /*!Pseudo: scan task stack space stack token string                       */
    while (pstkStack >= pstkStackBasePtr)
    {
        /*!Pseudo: compare is stack token                                     */
        if (*pstkStack != KER_kuniStackToken.astkToken[u8TokenIdx])
        {
            break;
        }

        /*!Pseudo: increase stack free size                                   */
        stkszSize = stkszSize + sizeof(CPU_STK);

        /*!Pseudo: decrease stack pointer to scan next position               */
        pstkStack--;

        /*!Pseudo: adjust token index                                         */
        u8TokenIdx--;
        u8TokenIdx = u8TokenIdx
                    & (INT8U)(KER_u8STACK_TOKEN_LENGTH / sizeof(CPU_STK) - 1u);
    }

#else
    #error "!Not supported stack growth configuration"
#endif

    /*!Pseudo: calculate stack maximum using size and return this value       */
    return stkszStackSize - stkszSize;
}

/*******************************************************************************
* @Brief  : stack check task entry
* @Inputs : vidpParam: task entry parameter pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidStackCheckTaskEntry( void *vidpParam )
{
    KER_CPU_SR_ALLOC();
    KER_tstrTCB *pstrTCB;
    KER_tstrList *pstrNode;
    CPU_STK *pstkStackBase;
    CPU_STK_SIZE stkszSize;

#if KER_coTASK_STACK_LIMIT_EN > 0u
    CPU_STK_SIZE stkszLimitSize;
#endif

    KER_NOT_USED_ARGUMENT(vidpParam);

    /*!Pseudo: enter critical section, for access tcb object                  */
    KER_ENTER_CRITICAL();

    /*!Pseudo: get link list head task node                                   */
    pstrNode = LIST_GET_HEAD(KER_strTCBLinkList);
    /*!Pseudo: get task tcb pointer                                           */
    pstrTCB = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrTCB, strLinkList);
    /*!Pseudo: get task stack base address                                    */
    pstkStackBase = pstrTCB->pstkStackBasePtr;
    /*!Pseudo: get task stack size                                            */
    stkszSize = pstrTCB->stkszStackSize;

#if KER_coTASK_STACK_LIMIT_EN > 0u
    /*!Pseudo: get stack limit size                                           */
    stkszLimitSize = pstrTCB->stkszStackLimitSize;
#endif

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    while (1)
    {

/*!Comment: if configured calculate stack usage, should always calculate      */
#if KER_coTASK_STACK_USAGE_EN > 0u

        /*!Pseudo: calculate task stack max usage                             */
        stkszSize = stkszStackMaxUsage(pstkStackBase, stkszSize);

/*!Comment: if only configured check stack limit usage                        */
#elif KER_coTASK_STACK_LIMIT_EN > 0u

        /*!Pseudo: calculate stack usage when task set the limit value        */
        if (stkszLimitSize > (CPU_STK_SIZE)0)
        {
            /*!Pseudo: calculate task stack max usage                         */
            stkszSize = stkszStackMaxUsage(pstkStackBase, stkszSize);
        }
#else
    #error "Stack check task is compiled without marcos control compiler"
#endif

        /*!Pseudo: enter critical section, for access tcb object              */
        KER_ENTER_CRITICAL();

        /*!Pseudo: if task is deleted, check from link list head task         */
        if ( KER_u8TSK_STAT_DEL == pstrTCB->u8State)
        {
            /*!Pseudo: get link list head task node                           */
            pstrNode = LIST_GET_HEAD(KER_strTCBLinkList);
        }
        else
        {
#if KER_coTASK_STACK_USAGE_EN > 0u
            /*!Pseudo: update task stack max usage information                */
            pstrTCB->stkszStackUsingMax = stkszSize;
#endif

#if KER_coTASK_STACK_LIMIT_EN > 0u
            /*!Pseudo: check if task stack overflow                           */
            if ((stkszLimitSize > (CPU_STK_SIZE)0)
                && (stkszSize > stkszLimitSize))
            {
                /*!Pseudo: task stack overflow treatment                      */
                vidTaskStackOverflowHandler(pstrTCB);
            }
            else
            {
                /*!Pseudo: do nothing!!!                                      */
            }
#endif
            /*!Pseudo: get next task node                                     */
            pstrNode = pstrNode->pstrNext;
            /*!Pseudo: skip list head                                         */
            if (pstrNode == &KER_strTCBLinkList)
            {
                pstrNode = pstrNode->pstrNext;
            }
            else
            {
                /*!Pseudo: do nothing!!!                                      */
            }
        }

        /*!Pseudo: get task tcb pointer                                       */
        pstrTCB = LIST_NODE_ENTRY_PTR(pstrNode, KER_tstrTCB, strLinkList);

        /*!Pseudo: get task stack base address and stack size                 */
        pstkStackBase = pstrTCB->pstkStackBasePtr;
        stkszSize = pstrTCB->stkszStackSize;

#if KER_coTASK_STACK_LIMIT_EN > 0u
        /*!Pseudo: get stack limit size                                       */
        stkszLimitSize = pstrTCB->stkszStackLimitSize;
#endif

#if (KER_coHOOK_STKCHK_EN > 0u) \
  && ((KER_coTASK_STACK_LIMIT_EN > 0u) || (KER_coTASK_STACK_USAGE_EN > 0u))
        if (KER_NULL != KER_pfnHookStackCheck)
        {
            /*!Pseudo: call user hook function                                */
            KER_pfnHookStackCheck();
        }
#endif

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: check one task per periodic, waiting for next periodic     */
        KER_vidDelay(KER_u32STACK_CHECK_TASK_PERIOD);
    }
}

/*******************************************************************************
* @Brief  : initial stack check task.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidStackCheckTaskInit( void )
{
    /*!Pseudo: initial kernel stack check task                                */
    KER_enuTaskInit( &KER_strStackChkTaskTcb,
                      "MRTOS->StackCheckTask",
                      vidStackCheckTaskEntry,
                      KER_NULL,
                      KER_NULL,
                      KER_astkStackChkTaskStack,
                      KER_stkszSTACK_CHECK_TASK_STKSZ,
                      (CPU_STK_SIZE)0,
                      KER_u16STACK_CHECK_TASK_PRIO,
                      (INT16U)10,
                      KER_NULL,
                     (INT32U)0,
                      KER_evtEVENT_NONE,
                      KER_evtMASK_ALL,
                      KER_NULL
                     );
}

#endif
