/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Idle.c
*Version    : V1.00
*Description: MRTOS idle task.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/28
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*******************************************************************************
*!Comment: private function prototype list
*******************************************************************************/
PRIVATE void vidIdleTaskEntry(void *vidpParam);

/*******************************************************************************
* @Brief  : kernel idle task entry.
* @Inputs : vidpParam: not used.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PRIVATE void vidIdleTaskEntry(void *vidpParam)
{
    KER_CPU_SR_ALLOC();

    for ( ; ; )
    {
        KER_ENTER_CRITICAL();
        KER_u32IdleCnt++;

#if KER_coHOOK_IDLE_EN > 0u
        if (KER_NULL != KER_pfnHookIdle)
        {
            /*!Pseudo: call user hook function                                */
            KER_pfnHookIdle();
        }
#endif

        KER_LEAVE_CRITICAL();
    }
}

/*******************************************************************************
* @Brief  : create kernel idle task.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidIdleTaskInit(void)
{
    /*!Pseudo: create kernel idle task                                        */
    KER_enuTaskInit( &KER_strIdleTaskTcb,
                      "MRTOS->IdleTask",
                      vidIdleTaskEntry,
                      KER_NULL,
                      KER_NULL,
                      KER_astkIdleTaskStack,
                      KER_stkszIDLE_TASK_STKSZ,
                      (CPU_STK_SIZE)0,
                      KER_u16TASK_PRIO_MAX - (INT16U)1,
                      (INT16U)10,
                      KER_NULL,
                     (INT32U)0,
                      KER_evtEVENT_NONE,
                      KER_evtMASK_ALL,
                      KER_NULL
                     );
}

/*******************************************************************************
* @Brief  : get cpu idle counter.
* @Inputs : none
* @Outputs: none
* @Returns: cpu idle counter
* @Note   : none
*******************************************************************************/
PUBLIC INT32U KER_u32IdleCntGet( void )
{
    KER_CPU_SR_ALLOC();
    INT32U u32Counter;

    /*!Pseduo: make sure in 8/16Bits cpu access consistency                   */
    KER_ENTER_CRITICAL();
    u32Counter = KER_u32IdleCnt;
    KER_LEAVE_CRITICAL();

    return u32Counter;
}