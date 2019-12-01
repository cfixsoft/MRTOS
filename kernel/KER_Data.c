/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Data.c
*Version    : V1.00
*Description: MRTOS kernel data definition
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/25
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coCPU_CLZ_INSTRUCTION_EN == 0u
/*!Comment: priority bit map table                                            */
PUBLIC const INT8U KER_kau8PrioBitMap[256] =
{
    (INT8U)0xFF, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 0 ~ 3    */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 4 ~ 7    */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 8  ~ 11  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 12 ~ 15  */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 16 ~ 19  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 20 ~ 23  */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 24 ~ 27  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 28 ~ 31  */

    (INT8U)0x05, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 32 ~ 35  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 36 ~ 39  */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 40 ~ 43  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 44 ~ 47  */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 48 ~ 51  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 52 ~ 56  */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 57 ~ 60  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 61 ~ 63  */

    (INT8U)0x06, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 64 ~ 67  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 68 ~ 71  */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 72 ~ 75  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 76 ~ 79  */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 80 ~ 83  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 84 ~ 87  */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 88 ~ 91  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 92 ~ 95  */

    (INT8U)0x05, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority: 96 ~ 99  */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:100 ~ 103 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:104 ~ 107 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:108 ~ 111 */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:112 ~ 115 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:116 ~ 119 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:120 ~ 123 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:124 ~ 127 */

    (INT8U)0x07, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:128 ~ 131 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:132 ~ 135 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:136 ~ 139 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:140 ~ 143 */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:144 ~ 147 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:148 ~ 151 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:152 ~ 155 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:156 ~ 159 */

    (INT8U)0x05, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:160 ~ 163 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:164 ~ 167 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:168 ~ 171 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:172 ~ 175 */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:176 ~ 179 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:180 ~ 183 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:184 ~ 187 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:188 ~ 191 */

    (INT8U)0x06, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:192 ~ 195 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:196 ~ 199 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:200 ~ 203 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:204 ~ 207 */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:208 ~ 211 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:212 ~ 215 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:216 ~ 219 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:220 ~ 223 */

    (INT8U)0x05, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:224 ~ 227 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:228 ~ 231 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:232 ~ 235 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:236 ~ 239 */

    (INT8U)0x04, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:240 ~ 243 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:244 ~ 247 */

    (INT8U)0x03, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00, /* Priority:248 ~ 251 */
    (INT8U)0x02, (INT8U)0x00, (INT8U)0x01, (INT8U)0x00  /* Priority:252 ~ 255 */
};
#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))
/*!Comment: stack token                                                       */
PUBLIC const KER_tuniStackToken KER_kuniStackToken
                          = {{'T','s','k','S','t','a','c','k'}};

#endif


#if KER_coCPULOAD_MODULE_EN > 0u
/*!Comment: CPU Load calcualte calculation variables definitions              */
/*!Comment: CPU Load (Unit:00.1%, Range: 0.0%~100.0%)                         */
PUBLIC INT32U  KER_u32CPULoad = (INT8U)0;
PUBLIC INT32U  KER_u32CPULoadISR = (INT8U)0;
/*!Comment: task CPU performance time counter                                 */
PUBLIC INT32U KER_u32CPUUsingCnt = (INT32U)1;
/*!Comment: ISR CPU using counter                                             */
PUBLIC INT32U KER_u32CPUISRCnt   = (INT32U)0;
/*!Comment: CPU idle time counter                                             */
PUBLIC INT32U KER_u32CPUIdleCnt  = (INT32U)0;
/*!Comment: Interrupt nested level for CPU load calculate                     */
PUBLIC INT8U KER_u8CPULoadIntrNestedCnt = (INT32U)0;

/*!Comment: CPU time last counter record                                      */
#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

PUBLIC INT32U KER_u32CPULoadTimerLastCnt = KER_u32CPULOAD_TIM_RELOAD_VAL;

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

PUBLIC INT32U KER_u32CPULoadTimerLastCnt = (INT32U)0;

#else
#error "Please configurate CPULoad timer counter direction!!!"
#endif

/*!Comment: calculate CPU Load period                                         */
PUBLIC INT32U KER_u32CPULoadCalcPeriod = KER_u32CPULOAD_CALC_PERIOD;

#endif


/*!Comment: cpu idle counter                                                  */
PUBLIC volatile INT32U KER_u32IdleCnt = (INT32U)0;

/*!Comment: idle task stack memory space                                      */
PUBLIC CPU_STK KER_astkIdleTaskStack[KER_stkszIDLE_TASK_STKSZ/sizeof(CPU_STK)];

/*!Comment: idle task tcb                                                     */
PUBLIC KER_tstrTCB  KER_strIdleTaskTcb;

/*!Comment: number of tasks                                                   */
PUBLIC volatile INT32U KER_u32TaskCnt = (INT32U)0;

/*!Comment: ticks counter                                                     */
PUBLIC volatile INT32U KER_u32TickCnt = (INT32U)0;

/*!Comment: interrupt nesting counter                                         */
PUBLIC volatile INT8U  KER_u8IntrNestCnt = (INT8U)0;

/*!Comment: scheduler lock nesting counter                                    */
PUBLIC volatile INT8U  KER_u8SchedLockNestCnt = (INT16U)0;

/*!Comment: kernel running flag                                               */
PUBLIC volatile BOOL KER_bKernelRunning = FALSE;

/*!Comment: current running task tcb pointer                                  */
PUBLIC KER_tstrTCB *KER_pstrCurTCB = KER_NULL;

/*!Comment: hightest priority ready task tcb pointer                          */
PUBLIC KER_tstrTCB *KER_pstrNextTCB = KER_NULL;

/*!Comment: task ready state bit map                                          */
PUBLIC volatile CPU_WORD KER_awTaskRdyBitMap[KER_u16TASK_RDY_BITMAP_SIZE];

/*!Comment: task ready list                                                   */
PUBLIC KER_tstrList KER_astrTaskRdyList[KER_u16TASK_PRIO_MAX];

/*!Comment: task TCB link list                                                */
PUBLIC KER_tstrList KER_strTCBLinkList;

/*!Comment: tick wheel list                                                   */
PUBLIC KER_tstrList KER_astrTickWheelList[KER_u16TICK_WHEEL_SIZE];

#if KER_coTICK_TASK_EN > 0u

/*!Comment: tick task stack memory space                                      */
PUBLIC CPU_STK KER_astkTickTaskStack[KER_stkszTICK_TASK_STKSZ/sizeof(CPU_STK)];

/*!Comment: tick task tcb                                                     */
PUBLIC KER_tstrTCB  KER_strTickTaskTcb;

#endif

#if KER_coTIMER_EN > 0u

/*!Comment: timer tick wheel list                                             */
PUBLIC KER_tstrList KER_astrTimerWheelList[KER_u16TIMER_WHEEL_SIZE];

/*!Comment: timer tick counter                                                */
PUBLIC INT32U KER_u32TimerCnt = (INT32U)0;

#endif

#if (KER_coTIMER_EN > 0u) && (KER_coTIMER_TASK_EN > 0u)

/*!Comment: timer task stack memory space                                     */
PUBLIC CPU_STK KER_astkTimerTaskStack[KER_stkszTIMER_TASK_STKSZ
                                                            /sizeof(CPU_STK)];


/*!Comment: timer task tcb                                                    */
PUBLIC KER_tstrTCB  KER_strTimerTaskTcb;

#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))

/*!Comment: stack check task stack memory space                               */
PUBLIC CPU_STK KER_astkStackChkTaskStack[KER_stkszSTACK_CHECK_TASK_STKSZ
                                                            /sizeof(CPU_STK)];

/*!Comment: stack check task tcb                                              */
PUBLIC KER_tstrTCB  KER_strStackChkTaskTcb;

#if KER_coHOOK_EN > 0u
/*******************************************************************************
*!Comment: user hook function pointer list
*******************************************************************************/
#if KER_coHOOK_TASK_CTX_SW_EN > 0u

#if (KER_coHOOK_EXCEPTION_EN > 0u) && (KER_coEXCEPTION_EN > 0u)
/*!Comment: kernel running exception hook function pointer                    */
PUBLIC KER_tpfnHookException KER_pfnHookException = KER_NULL;
#endif

/*!Comment: task switch context hook function pointer                         */
PUBLIC KER_tpfnHookTaskSwCtx  KER_pfnHookTaskSwCtx = KER_NULL;
#endif

#if KER_coHOOK_TASK_INIT_EN > 0u
/*!Comment: initial task hook function pointer                                */
PUBLIC KER_tpfnHookTaskInit KER_pfnHookTaskInit = KER_NULL;
#endif

#if (KER_coHOOK_TASK_DELETE_EN > 0u) && (KER_coTASK_DELETE_EN > 0u)
/*!Comment: task delete hook function pointer                                 */
PUBLIC KER_tpfnHookTaskDelete KER_pfnHookTaskDelete = KER_NULL;
#endif

#if (KER_coHOOK_TASK_SUSPEND_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*!Comment: task suspend hook function pointer                                */
PUBLIC KER_tpfnHookTaskSuspend KER_pfnHookTaskSuspend = KER_NULL;
#endif

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*!Comment: task resume hook function pointer                                 */
PUBLIC KER_tpfnHookTaskResume KER_pfnHookTaskResume = KER_NULL;
#endif

#if (KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u) && (KER_coTASK_PRIO_CHANGE_EN > 0u)
/*!Comment: task priority change hook function pointer                        */
PUBLIC KER_tpfnHookTaskPrioChange KER_pfnHookTaskPrioChange = KER_NULL;
#endif

#if (KER_coHOOK_TASK_YIELD_EN > 0u) && (KER_coTASK_YIELD_EN > 0u)
/*!Comment: task yield hook function pointer                                  */
PUBLIC KER_tpfnHookTaskYield KER_pfnHookTaskYield = KER_NULL;
#endif

#if KER_coHOOK_IDLE_EN > 0u
/*!Comment: kernel idle hook function pointer                                 */
PUBLIC KER_tpfnHookIdle KER_pfnHookIdle = KER_NULL;
#endif

#if KER_coHOOK_TICK_EN > 0u
/*!Comment: tick hook function pointer                                        */
PUBLIC KER_tpfnHookTick KER_pfnHookTick = KER_NULL;
#endif

#if (KER_coHOOK_TIMER_EN > 0u) && ( KER_coTIMER_EN > 0u )
/*!Comment: timer hook function pointer                                       */
PUBLIC KER_tpfnHookTimer KER_pfnHookTimer = KER_NULL;
#endif

#if (KER_coHOOK_STKCHK_EN > 0u) \
  && ((KER_coTASK_STACK_LIMIT_EN > 0u) || (KER_coTASK_STACK_USAGE_EN > 0u))
/*!Comment: stack check hook function pointer                                 */
PUBLIC KER_tpfnHookStackCheck KER_pfnHookStackCheck = KER_NULL;
#endif

#if (KER_coHOOK_STK_OVERFLOW_EN > 0u) && (KER_coTASK_STACK_LIMIT_EN > 0u)
/*!Comment: stack overflow hook function pointer                              */
PUBLIC KER_tpfnHookStackOverflow KER_pfnHookStackOverflow = KER_NULL;
#endif

#endif /* End of #if KER_coHOOK_EN > 0u                                       */

#endif

