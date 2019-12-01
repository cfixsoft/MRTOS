/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_RTMem.c
*Version    : V1.00
*Description: MRTOS real time memmory manager module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/9/29
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coRT_MEM_EN > 0u

/*******************************************************************************
* @Brief  : initial real time memmory module.
* @Inputs : pstrRTMem: memmory management block structure pointer.
*           vidpMemBaseAddr: memmory base address.
*           u32MemSize: memmory pool size.
*           u16BlockSize: block size.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial memmory pool ok.
*           KER_enuERR_RTM_BLK_SZ_INV: block size invalid.
*           KER_enuERR_RTM_POOL_SZ_INV: memmory pool size invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuRTMemInit( KER_tstrRTMem *pstrRTMem,
                                     void *vidpMemBaseAddr,
                                     INT32U u32MemSize,
                                     INT16U u16BlockSize)
{
    INT16U u16Idx;

#if KER_coPARAM_CHECK_EN > 0u

    /*!Pseudo: check if memmory pool structure pointer valid                  */
    if ( KER_NULL == pstrRTMem)
    {
        return KER_enuERR_RTM_POOL_OBJ_INV;
    }

    /*!Pseudo: check if memmory base address valid                            */
    if (KER_NULL == vidpMemBaseAddr)
    {
        /*Pseudo: memmory pool address invalid                                */
        return KER_enuERR_RTM_POOL_ADDR_INV;
    }

    /*!Pseudo: check if block size is valid                                   */
    if ((u16BlockSize < sizeof(KER_tstrList))
        || (KER_LO_ALIGN(u16BlockSize) != u16BlockSize))
    {
        /*!Pseudo: block size invalid                                         */
        return KER_enuERR_RTM_BLK_SZ_INV;
    }

    /*!Pseudo: check if memmory size is valid                                 */
    if (u32MemSize < u16BlockSize)
    {
        /*!Pseudo: memmory pool size is invalid                               */
        return KER_enuERR_RTM_POOL_SZ_INV;
    }

    /*!Pseudo: check if memmory size is valid                                 */
    if (KER_LO_ALIGN(u32MemSize) != u32MemSize)
    {
        /*!Pseudo: memmory pool size is invalid                               */
        return KER_enuERR_RTM_POOL_SZ_INV;
    }

#endif

    /*!Pseudo: set block size                                                 */
    pstrRTMem->u16BlockSize     = u16BlockSize;
    /*!Pseudo: calculate free blocks                                          */
    pstrRTMem->u16NbrFreeBlocks = (INT16U)(u32MemSize/u16BlockSize);
    /*!Pseudo: memmory total size                                             */
    pstrRTMem->u32MemPoolSize   = u32MemSize;
    /*!Pseudo: memmory base address                                           */
    pstrRTMem->vidpMemBaseAddr  = vidpMemBaseAddr;
    /*!Pseudo: initial free block list                                        */
    LIST_INIT(pstrRTMem->strFreeList);
    /*!Pseudo: link free blocks                                               */
    for (u16Idx = (INT16U)0; u16Idx < pstrRTMem->u16NbrFreeBlocks; u16Idx++)
    {
        LIST_INSERT_HEAD_PTR(&(pstrRTMem->strFreeList),
           ((KER_tstrList*)((INT8U *)vidpMemBaseAddr + u16Idx * u16BlockSize)));
    }

    return KER_enuERR_OK;

}

/*******************************************************************************
* @Brief  : get memmory block.
* @Inputs : pstrRTMem: memmory management block structure pointer.
* @Outputs: none.
* @Returns: memmory block address: get memmory block successful.
*           KER_NULL: get memmory block failed.
* @Note   : none.
*******************************************************************************/
PUBLIC void *KER_vidpRTMemGet(KER_tstrRTMem *pstrRTMem)
{
    KER_CPU_SR_ALLOC();
    KER_tstrList *pstrNode;

#if KER_coPARAM_CHECK_EN > 0u

    /*!Psuedo: check if memmory management block structure pointer valid      */
    if ( KER_NULL == pstrRTMem)
    {
        return KER_NULL;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: check if has free blocks to allocate                           */
    if (pstrRTMem->u16NbrFreeBlocks)
    {
        /*!Pseudo: decrease free blocks number                                */
        pstrRTMem->u16NbrFreeBlocks--;

        /*!Pseudo: get free block                                             */
        pstrNode = LIST_GET_HEAD(pstrRTMem->strFreeList);

        /*!Pseudo: remove from free block list                                */
        LIST_REMOVE_NODE_PTR(pstrNode);

        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return free block address                                  */
        return (void *)pstrNode;
    }
    else
    {
        /*!Pseudo: leave critical section                                     */
        KER_LEAVE_CRITICAL();

        /*!Pseudo: return KER_NULL, indicate allocate memmory block failed.       */
        return KER_NULL;
    }
}

/*******************************************************************************
* @Brief  : release memmory block.
* @Inputs : pstrRTMem: memmory management block structure pointer.
*           vidpBlock: block address.
* @Outputs: none
* @Returns: KER_enuERR_OK: release block successful.
*           KER_enuERR_RTM_POOL_OBJ_INV: memmory management object invalid.
*           KER_enuERR_RTM_BLK_ADDR_INV: memmory block address invalid.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuRTMemPut( KER_tstrRTMem *pstrRTMem,
                                    void *vidpBlock)
{
    KER_CPU_SR_ALLOC();

#if KER_coPARAM_CHECK_EN > 0u

    /*!Psuedo: check if memmory management block structure pointer valid      */
    if ( KER_NULL == pstrRTMem)
    {
        return KER_enuERR_RTM_POOL_OBJ_INV;
    }

    /*!Psuedo: check if memmory address valid                                 */
    if ( KER_NULL == vidpBlock)
    {
        return KER_enuERR_RTM_BLK_ADDR_INV;
    }

#endif

    /*!Pseudo: enter critical section                                         */
    KER_ENTER_CRITICAL();

    /*!Pseudo: increase free blocks number                                    */
    pstrRTMem->u16NbrFreeBlocks++;

    /*!Pseudo: insert to free block list tail                                 */
    LIST_INSERT_TAIL_PTR(&(pstrRTMem->strFreeList), (KER_tstrList *)vidpBlock);

    /*!Pseudo: leave critical section                                         */
    KER_LEAVE_CRITICAL();

    /*!Pseudo: retun with release memmory ok                                  */
    return KER_enuERR_OK;

}

#endif

