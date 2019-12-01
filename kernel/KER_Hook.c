/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Hook.c
*Version    : V1.00
*Description: kernel hook functions implement.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2016/3/6
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coHOOK_EN > 0u

/*******************************************************************************
*!Comment: hook functions set/clear interface list
*******************************************************************************/
/*******************************************************************************
* @Brief  : clear all hook functions.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookFunctionsAllClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();

#if (KER_coHOOK_EXCEPTION_EN > 0u) && (KER_coEXCEPTION_EN > 0u)
    KER_pfnHookException = KER_NULL;
#endif

#if KER_coHOOK_TASK_CTX_SW_EN > 0u
    KER_pfnHookTaskSwCtx = KER_NULL;
#endif

#if KER_coHOOK_TASK_INIT_EN > 0u
    KER_pfnHookTaskInit = KER_NULL;
#endif

#if (KER_coHOOK_TASK_DELETE_EN > 0u) && (KER_coTASK_DELETE_EN > 0u)
    KER_pfnHookTaskDelete = KER_NULL;
#endif

#if (KER_coHOOK_TASK_SUSPEND_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
    KER_pfnHookTaskSuspend = KER_NULL;
#endif

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
    KER_pfnHookTaskResume = KER_NULL;
#endif

#if (KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u) && (KER_coTASK_PRIO_CHANGE_EN > 0u)
    KER_pfnHookTaskPrioChange = KER_NULL;
#endif

#if (KER_coHOOK_TASK_YIELD_EN > 0u) && (KER_coTASK_YIELD_EN > 0u)
    KER_pfnHookTaskYield = KER_NULL;
#endif

#if KER_coHOOK_IDLE_EN > 0u
    KER_pfnHookIdle = KER_NULL;
#endif

#if KER_coHOOK_TICK_EN > 0u
    KER_pfnHookTick = KER_NULL;
#endif

#if (KER_coHOOK_TIMER_EN > 0u) && ( KER_coTIMER_EN > 0u )
    KER_pfnHookTimer = KER_NULL;
#endif

#if (KER_coHOOK_STKCHK_EN > 0u) \
  && ((KER_coTASK_STACK_LIMIT_EN > 0u) || (KER_coTASK_STACK_USAGE_EN > 0u))
    KER_pfnHookStackCheck = KER_NULL;
#endif

#if (KER_coHOOK_STK_OVERFLOW_EN > 0u) && (KER_coTASK_STACK_LIMIT_EN > 0u)
    KER_pfnHookStackOverflow = KER_NULL;
#endif

    KER_LEAVE_CRITICAL();
}

#if (KER_coHOOK_EXCEPTION_EN > 0u) && (KER_coEXCEPTION_EN > 0u)
/*******************************************************************************
* @Brief  : set kernel running error exception user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookExceptionFuncSet( KER_tpfnHookException pfnFunc )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookException = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear kernel running error exception user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookExceptionFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookException = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if KER_coHOOK_TASK_CTX_SW_EN > 0u
/*******************************************************************************
* @Brief  : set task switch context user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskSwCtxFuncSet(KER_tpfnHookTaskSwCtx pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskSwCtx = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear task switch context user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskSwCtxFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskSwCtx = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if KER_coHOOK_TASK_INIT_EN > 0u
/*******************************************************************************
* @Brief  : set initial task user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskInitFuncSet(KER_tpfnHookTaskInit pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskInit = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear initial task user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskInitFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskInit = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_TASK_DELETE_EN > 0u) && (KER_coTASK_DELETE_EN > 0u)
/*******************************************************************************
* @Brief  : set task delete user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskDeleteFuncSet(KER_tpfnHookTaskDelete pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskDelete = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear task delete user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskDeleteFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskDelete = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_TASK_SUSPEND_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*******************************************************************************
* @Brief  : set task suspend user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskSuspendFuncSet(KER_tpfnHookTaskSuspend pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskSuspend = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear task suspend user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskSuspendFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskSuspend = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*******************************************************************************
* @Brief  : set task resume user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskResumeFuncSet(KER_tpfnHookTaskResume pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskResume = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear task resume user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskResumeFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskResume = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u) && (KER_coTASK_PRIO_CHANGE_EN > 0u)
/*******************************************************************************
* @Brief  : set task priority change user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskPrioChangeFuncSet(KER_tpfnHookTaskPrioChange pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskPrioChange = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear task priority change user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskPrioChangeFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskPrioChange = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_TASK_YIELD_EN > 0u) && (KER_coTASK_YIELD_EN > 0u)
/*******************************************************************************
* @Brief  : set task yield user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskYieldFuncSet(KER_tpfnHookTaskYield pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskYield = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear task yield user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTaskYieldFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTaskYield = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if KER_coHOOK_IDLE_EN > 0u
/*******************************************************************************
* @Brief  : set kernel idle user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookIdleFuncSet( KER_tpfnHookIdle pfnFunc )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookIdle = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear kernel idle user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookIdleFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookIdle = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if KER_coHOOK_TICK_EN > 0u
/*******************************************************************************
* @Brief  : set tick user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTickFuncSet( KER_tpfnHookTick pfnFunc )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTick = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear tick user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTickFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTick = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_TIMER_EN > 0u) && ( KER_coTIMER_EN > 0u )
/*******************************************************************************
* @Brief  : set timer user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTimerFuncSet( KER_tpfnHookTimer pfnFunc )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTimer = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear timer user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookTimerFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookTimer = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_STKCHK_EN > 0u) \
  && ((KER_coTASK_STACK_LIMIT_EN > 0u) || (KER_coTASK_STACK_USAGE_EN > 0u))
/*******************************************************************************
* @Brief  : set stack check user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookStackCheckFuncSet( KER_tpfnHookStackCheck pfnFunc )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookStackCheck = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear stack check user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookStackCheckFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookStackCheck = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#if (KER_coHOOK_STK_OVERFLOW_EN > 0u) && (KER_coTASK_STACK_LIMIT_EN > 0u)
/*******************************************************************************
* @Brief  : set stack check user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookStackOverflowFuncSet(KER_tpfnHookStackOverflow pfnFunc)
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookStackOverflow = pfnFunc;
    KER_LEAVE_CRITICAL();
}

/*******************************************************************************
* @Brief  : clear stack check user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
PUBLIC void KER_vidHookStackOverflowFuncClr( void )
{
    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    KER_pfnHookStackOverflow = KER_NULL;
    KER_LEAVE_CRITICAL();
}
#endif

#endif /* End of #if KER_coHOOK_EN > 0u                                       */

