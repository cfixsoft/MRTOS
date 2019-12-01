/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Config.h
*Version    : V1.00
*Description: MRTOS kernel configuration control macros definition.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/25
*******************************************************************************/
#ifndef KER_CONFIG_H
#define KER_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
*!Comment: generated magic number
*******************************************************************************/
#define KER_MAGIC_NUMBER                                          (0x12345678)

/*******************************************************************************
*!Comment: public macros
*******************************************************************************/
/*******************************************************************************
*!Comment: CPU architecture configure macros(Note: user should not change)
*******************************************************************************/
/*!Comment: ARM Cortex-M3 CPU architecture                                    */
#define KER_u32CPU_ARCH_CORTEX_M3                                         (0u)

/*!Comment: ARM7TDMI CPU architecture                                         */
#define KER_u32CPU_ARCH_ARM7TDMI                                          (1u)

/*******************************************************************************
*!Comment: CPU architecture selection, for user to select a cpu architecture.
*******************************************************************************/
#define KER_u32CPU_ARCH                              KER_u32CPU_ARCH_CORTEX_M3

/*******************************************************************************
*!Comment: CPU architecture configure macros
*******************************************************************************/
/*******************************************************************************
*!Comment: Define stack growth direction (Note: user should not change)
*******************************************************************************/
#define KER_u8STACK_GROWTH_FULL_DOWN                                      (0u)
#define KER_u8STACK_GROWTH_FULL_UP                                        (1u)
#define KER_u8STACK_GROWTH_EMPTY_DOWN                                     (2u)
#define KER_u8STACK_GROWTH_ENPTY_UP                                       (3u)

/*!Comment: select stack growth configuration                                 */
#define KER_u8STACK_GROWTH_CFG                     KER_u8STACK_GROWTH_FULL_DOWN

/*!Comment: Define cpu word bits                                              */
#define KER_u8CPU_WORD_BITS                                              (32u)

/*!Comment: check KER_u8CPU_WORD_BITS configure support cpu word              */
#if ((KER_u8CPU_WORD_BITS != 64u) && (KER_u8CPU_WORD_BITS != 32u) \
    && (KER_u8CPU_WORD_BITS != 16u) && (KER_u8CPU_WORD_BITS != 8u))

    #error "KER_Config.h: KER_u8CPU_WORD_BITS macro configure error!!!"
#endif

/*!Comment: Define stack data bits                                            */
#define KER_u8CPU_STK_BITS                                               (32u)
/*!Comment: Configure max task priority                                       */
#define KER_u16TASK_PRIO_MAX                                      ((INT16U)32)

/*!Comment: Task ready state bit map                                          */
#define KER_u16TASK_RDY_BITMAP_SIZE        ((KER_u16TASK_PRIO_MAX - (INT8U)1) \
                                            / KER_u8CPU_WORD_BITS + (INT8U)1)

/*!Comment: kernel idle task stack size (bytes)                               */
#define KER_stkszIDLE_TASK_STKSZ                           ((CPU_STK_SIZE)160)

/*!Comment: define interrupt nesting level limit                              */
#define KER_u8INTR_NESTING_LEVEL_LIMIT                            ((INT8U)255)

/*!Comment: define task slice default reload ticks                            */
#define KER_u16RELOAD_TICKS_DEFAULT_VALUE                          ((INT16U)2)

/*!Comment: This macro option use to enable kernel self initialize kernel data
*           0--disable, !0--enable                                            */
#define KER_coKERNEL_DATA_SELF_INIT_EN                                    (1u)

/*!Comment: kernel interface paramters check configuration macro
*           0--disable, !0--enable                                            */
#define KER_coPARAM_CHECK_EN                                              (1u)

/*!Comment: task slice scheduler configuration
*           0--disable, !0--enable                                            */
#define KER_coSLICE_SCHED_EN                                              (1u)

/*!Comment: task priority change using configuration
*           0--disable, !0--enable                                            */
#define KER_coTASK_PRIO_CHANGE_EN                                         (1u)

/*!Comment: task yield using configuration
*           0--disable, !0--enable                                            */
#define KER_coTASK_YIELD_EN                                               (1u)

/*!Comment: task suspend using configuration
*           0--disable, !0--enable                                            */
#define KER_coTASK_SUSPEND_EN                                             (1u)

/*!Comment: kernel task delete using configuration
*           0--disable, !0--enable                                            */
#define KER_coTASK_DELETE_EN                                              (1u)

/*!Comment: kernel exception module using configuration
*           0--disable, !0--enable                                            */
#define KER_coEXCEPTION_EN                                                (1u)

/*!Comment: real time memmory manager module using configuration
*           0--disable, !0--enable                                            */
#define KER_coRT_MEM_EN                                                   (1u)

/*!Comment: switch context counter using configuration
*           0--disable, !0--enable                                            */
#define KER_coSWITCH_CONTEXT_CNT_EN                                       (1u)

/*!Comment: use cpu counter leading zeros instruction to calculate prority
*           0--disable, !0--enable                                            */
#define KER_coCPU_CLZ_INSTRUCTION_EN                                      (1u)



/*******************************************************************************
*!Comment: tick module using configuration
*******************************************************************************/
/*!Comment: configure tick counter number of ticks per second                 */
#define KER_u32TICKS_PER_SECOND                                  ((INT32U)200)

/*!Comment: Tick wheel size                                                   */
#define KER_u16TICK_WHEEL_SIZE                                    ((INT16U)11)

/*!Comment: timer module interfaces detail using configuration
*           0--disable, !0--enable                                            */
#define KER_coTICK_TASK_EN                                                (1u)

/*!Comment: KER_vidDelayHMSM interface using
*           0--disable, !0--enable                                            */
#define KER_coDELAY_HMSM_EN                                               (1u)

#if KER_coTICK_TASK_EN > 0u
    /*!Comment: tcik task stack size (bytes)                                  */
    #define KER_stkszTICK_TASK_STKSZ                       ((CPU_STK_SIZE)160)

    /*!Comment: tick task priority                                            */
    #define KER_u16TICK_TASK_PRIO                                  ((INT16U)3)
#endif

/*******************************************************************************
*!Comment: timer module using configuration
*******************************************************************************/
/*!Comment: kernel timer module using configuration
*           if enable this macro, below basic functions force to using
*           private: vidTimerWheelListInsert
*           public: KER_enuTimerInit
*                   KER_vidTimerHandler
*           0--disable, !0--enable                                            */
#define KER_coTIMER_EN                                                    (1u)

#if KER_coTIMER_EN > 0u

/*!Comment: timer feature configurations                                      */
    /*!Comment: configure timer counter number of ticks per second            */
    #define KER_u32TIMER_TICKS_PER_SECOND                        ((INT32U)200)
    /*!Comment: Timer wheel size                                              */
    #define KER_u16TIMER_WHEEL_SIZE                               ((INT16U)11)

/*!Comment: using timer task configuration
*           0--disable, !0--enable                                            */
    #define KER_coTIMER_TASK_EN                                           (1u)

    #if KER_coTIMER_TASK_EN > 0u
        /*!Comment: timer task stack size (bytes)                             */
        #define KER_stkszTIMER_TASK_STKSZ                  ((CPU_STK_SIZE)160)

        /*!Comment: timer task priority                                       */
        #define KER_u16TIMER_TASK_PRIO                             ((INT16U)4)
    #endif

/*!Comment: timer module interfaces detail using configuration                */

    /*!Comment: KER_enuTimerHMSMInit interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coTIMER_HMSM_INIT_EN                                      (1u)

    /*!Comment: KER_enuTimerStart interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coTIMER_START_EN                                          (1u)

    /*!Comment: KER_enuTimerStop interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coTIMER_STOP_EN                                           (1u)

    /*!Comment: KER_enuTimerModeChange interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coTIMER_MODE_CHANGE_EN                                    (1u)

    /*!Comment: KER_enuTimerPeriodChange interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coTIMER_PERIOD_CHANGE_EN                                  (1u)

    /*!Comment: KER_enuTimerActionChange interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coTIMER_ACTION_CHANGE_EN                                  (1u)

    /*!Comment: KER_enuTimerHMSMChange interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coTIMER_HMSM_CHANGE_EN                                    (1u)

#endif

/*******************************************************************************
*!Comment: stack check task configuration
*******************************************************************************/
/*!Comment: task stack limit check feature using configuration
*           0--disable, !0--enable                                            */
#define KER_coTASK_STACK_LIMIT_EN                                         (1u)

/*!Comment: task stack usage calculation feature using configuration
*           0--disable, !0--enable                                            */
#define KER_coTASK_STACK_USAGE_EN                                         (1u)

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))

    /*!Comment: stack check task stack size (bytes)                           */
    #define KER_stkszSTACK_CHECK_TASK_STKSZ                ((CPU_STK_SIZE)160)

    /*!Comment: stack check task priority                                     */
    #define KER_u16STACK_CHECK_TASK_PRIO    (KER_u16TASK_PRIO_MAX - (INT16U)2)

    /*!Comment: stack check task running period, 1 second                     */
    #define KER_u32STACK_CHECK_TASK_PERIOD      (KER_u32TICKS_PER_SECOND * 1u)


#endif

/*******************************************************************************
*!Comment: calculate CPU load module using configuration
*******************************************************************************/
/*!Comment: calculate CPU load module using configuration
*           0--disable, !0--enable                                            */
#define KER_coCPULOAD_MODULE_EN                                           (1u)

/*!Comment: counter right shift divide                                        */
#define KER_u8CPULOAD_CNT_RSH_DIV                                   ((INT8U)6)

/*!Comment: calaculate CPU Load period (1 second)                             */
#define KER_u32CPULOAD_CALC_PERIOD                   (KER_u32TICKS_PER_SECOND)

/*!Comment: CPU load counter timer decrease counter direction option          */
#define KER_coCPULOAD_TIM_CNT_DECREASE                                    (0u)

/*!Comment: CPU load counter timer increase counter direction option          */
#define KER_coCPULOAD_TIM_CNT_INCREASE                                    (1u)

/*!Comment: CPU load counter timer counter direction configuration            */
#define KER_coCPULOAD_TIM_CNT_DIR              KER_coCPULOAD_TIM_CNT_DECREASE

#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE

    /*!Comment: CPU load counter timer counter decrease to zero reload value  */
    #define KER_u32CPULOAD_TIM_RELOAD_VAL (((INT32U)120000 * 5u) \
                                                 >> KER_u8CPULOAD_CNT_RSH_DIV)

#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE

    /*!Comment: CPU load counter timer counter increase expired value         */
    #define KER_u32CPULOAD_TIM_EXPIRED_VAL                ((INT32U)0xFFFFFFFF)

#else
    #error "Please configurate CPULoad timer counter direction!!!"
#endif


/*******************************************************************************
*!Comment: ipc event module using configuration
*******************************************************************************/
/*!Comment: ipc event module using configuration
*           if enable this marco, below basic functions force to using
*           KER_enuEventInit
*           KER_enuEventPend
*           KER_enuEventPost
*           0--disable, !0--enable                                            */
#define KER_coIPC_EVENT_EN                                                (1u)

/*!Comment: ipc event module interfaces detail using configuration            */
#if KER_coIPC_EVENT_EN > 0u

    /*!Comment: using event mask feature
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_EVENT_MASK_EN                                       (1u)

    /*!Comment: evnet mask operation interfaces using detail configuration    */
    #if KER_coIPC_EVENT_MASK_EN > 0u

        /*!Comment: KER_enuEventMaskSet interface using configuration
         *          0--disable, !0--enable                                    */
        #define KER_coIPC_EVENT_MASK_SET_EN                               (1u)

        /*!Comment: KER_enuEventMaskClear interface using configuration
         *          0--disable, !0--enable                                    */
        #define KER_coIPC_EVENT_MAKS_CLEAR_EN                             (1u)

        /*!Comment: KER_enuEventMaskGet interface using configuration
         *          0--disable, !0--enable                                    */
        #define KER_coIPC_EVENT_MASK_GET_EN                               (1u)

    #endif

    /*!Comment: KER_enuEventTryPend interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_EVENT_TRY_PEND_EN                                   (1u)

    /*!Comment: KER_enuEventPendAbort interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_EVENT_PEND_ABORT_EN                                 (1u)

    /*!Comment: KER_enuEventPendAbortAll interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_EVENT_PEND_ABORT_ALL_EN                             (1u)

    /*!Comment: KER_enuEventDelete interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_EVENT_DELETE_EN                                     (1u)

#endif

/*******************************************************************************
*!Comment: ipc mutex module using configuration
*******************************************************************************/
/*!Comment: ipc mutex module using configuration
*           if enable this marco, below basic functions force to using
*           KER_enuMutexInit
*           KER_enuMutexPend
*           KER_enuMutexPost
*           0--disable, !0--enable                                            */
#define KER_coIPC_MUTEX_EN                                                (1u)

/*!Comment: ipc mutex module interfaces detail using configuration            */
#if KER_coIPC_MUTEX_EN > 0u

    /*!Comment: KER_enuMutexTryPend interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MUTEX_TRY_PEND_EN                                   (1u)

    /*!Comment: KER_enuMutexPendAbort interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MUTEX_ABORT_EN                                      (1u)

    /*!Comment: KER_enuMutexPendAbortAll interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MUTEX_ABORT_ALL_EN                                  (1u)

    /*!Comment: KER_enuMutexDelete interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MUTEX_DELETE_EN                                     (1u)

#endif

/*******************************************************************************
*!Comment: ipc semaphore module using configuration
*******************************************************************************/
/*!Comment: ipc semaphore module using configuration
*           if enable this marco, below basic functions force to using
*           KER_enuSemInit
*           KER_enuSemPend
*           KER_enuSemPost
*           0--disable, !0--enable                                            */
#define KER_coIPC_SEM_EN                                                  (1u)

/*!Comment: ipc semaphore module interfaces detail using configuration        */
#if KER_coIPC_SEM_EN > 0u

    /*!Comment: KER_enuSemTryPend interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_SEM_TRY_PEND_EN                                     (1u)

    /*!Comment: KER_enuSemPendAbort interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_SEM_ABORT_EN                                        (1u)

    /*!Comment: KER_enuSemPendAbortAll interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_SEM_ABORT_ALL_EN                                    (1u)

    /*!Comment: KER_enuSemDelete interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_SEM_DELETE_EN                                       (1u)

#endif


/*******************************************************************************
*!Comment: ipc message queue module using configuration
*******************************************************************************/
/*!Comment: ipc message queue module using configuration
*           if enable this marco, below basic functions force to using
*           KER_enuMsgQInit
*           KER_enuMsgQPend
*           KER_enuMsgQPost
*           0--disable, !0--enable                                            */
#define KER_coIPC_MSGQ_EN                                                 (1u)

/*!Comment: ipc semaphore module interfaces detail using configuration        */
#if KER_coIPC_MSGQ_EN > 0u

    /*!Comment: KER_enuMsgQTryPend interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MSGQ_TRY_PEND_EN                                    (1u)

    /*!Comment: KER_enuMsgQPendAbort interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MSGQ_ABORT_EN                                       (1u)

    /*!Comment: KER_enuMsgQPendAbortAll interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MSGQ_ABORT_ALL_EN                                   (1u)

    /*!Comment: KER_enuMsgQPendDelete interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coIPC_MSGQ_DELETE_EN                                      (1u)

#endif

/*******************************************************************************
*!Comment: task built in message queue module using configuration
*******************************************************************************/
/*!Comment: task built in message queue module using configuration
*           if enable this marco, below basic functions force to using
*           KER_enuTaskMsgQInit
*           KER_enuTaskMsgQPend
*           KER_enuTaskMsgQPost
*           0--disable, !0--enable                                            */
#define KER_coBUILT_IN_MSGQ_EN                                            (1u)

/*!Comment: task built in message queue module detail using configuration     */
#if KER_coBUILT_IN_MSGQ_EN > 0u

    /*!Comment: KER_enuTaskMsgQTryPend interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_MSGQ_TRY_PEND_EN                               (1u)

    /*!Comment: KER_enuTaskMsgQPendAbort interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_MSGQ_ABORT_EN                                  (1u)

#endif

/*******************************************************************************
*!Comment: task built in event module using configuration
*******************************************************************************/
/*!Comment: task built in event module using configuration
*           if enable this marco, below basic functions force to using
*           KER_enuTaskEventPend
*           KER_enuTaskEventPost
*           0--disable, !0--enable                                            */
#define KER_coBUILT_IN_EVENT_EN                                           (1u)

/*!Comment: task built in event module detail using configuration             */
#if KER_coBUILT_IN_EVENT_EN > 0u

    /*!Comment: using event mask feature
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_EVENT_MASK_EN                                  (1u)

    #if KER_coBUILT_IN_EVENT_MASK_EN > 0u

        /*!Comment: KER_enuTaskEventMaskSet interface using configuration
         *          0--disable, !0--enable                                    */
        #define KER_coBUILT_IN_EVENT_MASK_SET_EN                          (1u)

        /*!Comment: KER_enuTaskEventMaskClear interface using configuration
         *          0--disable, !0--enable                                    */
        #define KER_coBUILT_IN_EVENT_MASK_CLR_EN                          (1u)

        /*!Comment: KER_enuTaskEventMaskGet interface using configuration
         *          0--disable, !0--enable                                    */
        #define KER_coBUILT_IN_EVENT_MASK_GET_EN                          (1u)

    #endif

    /*!Comment: KER_enuTaskEventTryPend interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_EVENT_TRY_PEND_EN                              (1u)

    /*!Comment: KER_enuTaskEventPendAbort interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_EVENT_ABORT_EN                                 (1u)

    /*!Comment: KER_enuTaskEventMaskGet interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_EVENT_RESET_EN                                 (1u)

#endif

/*******************************************************************************
*!Comment: task built in semaphore module using configuration
*******************************************************************************/
/*!Comment: task built in semaphore module using configuration
*           if enable this marco, below basic functions force to using
*           KER_enuTaskSemPend
*           KER_enuTaskSemPost
*           0--disable, !0--enable                                            */
#define KER_coBUILT_IN_SEM_EN                                             (1u)

/*!Comment: task built in semaphore module detail using configuration         */
#if KER_coBUILT_IN_SEM_EN > 0u

    /*!Comment: KER_enuTaskSemTryPend interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_SEM_TRY_PEND_EN                                (1u)

    /*!Comment: KER_enuTaskSemPendAbort interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_SEM_ABORT_EN                                   (1u)

    /*!Comment: KER_enuTaskSemPendReset interface using configuration
     *          0--disable, !0--enable                                        */
    #define KER_coBUILT_IN_SEM_RESET_EN                                   (1u)

#endif

/*******************************************************************************
*!Comment: hook functions configuration
*******************************************************************************/
/*!Comment: hook function module using configuration
*           0--disable, !0--enable                                            */
#define KER_coHOOK_EN                                                     (1u)

#if KER_coHOOK_EN > 0u

    /*!Comment: kernel running error exception hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_EXCEPTION_EN                                       (1u)

    /*!Comment: task context switch hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TASK_CTX_SW_EN                                     (1u)

    /*!Comment: initial task hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TASK_INIT_EN                                       (1u)

    /*!Comment: task delete hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TASK_DELETE_EN                                     (1u)

    /*!Comment: task suspend hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TASK_SUSPEND_EN                                    (1u)

    /*!Comment: task resume hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TASK_RESUME_EN                                     (1u)

    /*!Comment: task priority change hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TASK_PRIO_CHANGE_EN                                (1u)

    /*!Comment: task yield hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TASK_YIELD_EN                                      (1u)

    /*!Comment: kernel idle hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_IDLE_EN                                            (1u)

    /*!Comment: tick hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TICK_EN                                            (1u)

    /*!Comment: tiemr hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_TIMER_EN                                           (1u)

    /*!Comment: stack check hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_STKCHK_EN                                          (1u)

    /*!Comment: stack overflow hook function configuration
     *           0--disable, !0--enable                                       */
    #define KER_coHOOK_STK_OVERFLOW_EN                                    (1u)

#endif

#ifdef __cplusplus
}
#endif
#endif
