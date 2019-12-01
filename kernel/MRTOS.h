/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: MRTOS.h
*Version    : V1.00
*Description: MRTOS export kernel interfaces header file.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/24
*******************************************************************************/
#ifndef MRTOS_H
#define MRTOS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <KER_Def.h>
#include <KER_Config.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*!Comment: MRTOS Version: 1.00                                               */
#define KER_u8MRTOS_MAJOR_VERSION                                ((INT8U)0x01)
#define KER_u8MRTOS_MINOR_VERSION                                ((INT8U)0x00)

/*!Comment: define task state bit                                             */
/*!Comment: task delay state bit                                              */
#define KER_u8TSK_STAT_BIT_DLY                                   ((INT8U)0x01)
/*!Comment: task block state bit                                              */
#define KER_u8TSK_STAT_BIT_BLK                                   ((INT8U)0x02)
/*!Comment: task suspend state bit                                            */
#define KER_u8TSK_STAT_BIT_SPD                                   ((INT8U)0x04)

/*!Comment: define task state                                                 */
/*!Comment: ready state                                                       */
#define KER_u8TSK_STAT_RDY                                       ((INT8U)0x00)
/*!Comment: delay state                                                       */
#define KER_u8TSK_STAT_DLY                                       ((INT8U)0x01)
/*!Comment: block state                                                       */
#define KER_u8TSK_STAT_BLK                                       ((INT8U)0x02)
/*!Comment: block & delay state                                               */
#define KER_u8TSK_STAT_BLK_DLY                                   ((INT8U)0x03)
/*!Comment: suspend state                                                     */
#define KER_u8TSK_STAT_SPD                                       ((INT8U)0x04)
/*!Comment: suspend & delay state                                             */
#define KER_u8TSK_STAT_SPD_DLY                                   ((INT8U)0x05)
/*!Comment: suspend & block state                                             */
#define KER_u8TSK_STAT_SPD_BLK                                   ((INT8U)0x06)
/*!Comment: suspend & block & delay state                                     */
#define KER_u8TSK_STAT_SPD_BLK_DLY                               ((INT8U)0x07)
/*!Comment: deleted state                                                     */
#define KER_u8TSK_STAT_DEL                                       ((INT8U)0xFF)

/*******************************************************************************
*!Comment: general event & event mask definition
*******************************************************************************/
#define KER_evtEVENT_NONE                                      ((EVENT_TYPE)0)
#define KER_evtMASK_ALL                                   (~KER_evtEVENT_NONE)

/*******************************************************************************
*!Comment: event receive method option definition.
*******************************************************************************/
typedef enum
{
    KER_enuEVT_RECV_OPT_UNKNOWN   = (INT8U)0x00,
    KER_enuEVT_RECV_OPT_OR        = (INT8U)0x01,
    KER_enuEVT_RECV_OPT_AND       = (INT8U)0x02,
    KER_enuEVT_RECV_OPT_OR_CLR    = (INT8U)0x03,
    KER_enuEVT_RECV_OPT_AND_CLR   = (INT8U)0x04
}KER_tenuEvtRecvOpt;

/*******************************************************************************
*!Comment: kernel function return error code definition
*******************************************************************************/
typedef enum
{
    /*!Comment: kernel general error code definition                          */
    KER_enuERR_OK                    = (INT8U)0x00,
    KER_enuERR_NOK                   = (INT8U)0x01,
    KER_enuERR_TCB_PTR_INV           = (INT8U)0x02,
    KER_enuERR_TASK_ENTRY_INV        = (INT8U)0x03,
    KER_enuERR_STACK_PTR_INV         = (INT8U)0x04,
    KER_enuERR_PRIORITY_INV          = (INT8U)0x05,
    KER_enuERR_ISR_FAULT_USING       = (INT8U)0x06,
    KER_enuERR_SCHED_LOCK_PENDING    = (INT8U)0x07,
    KER_enuERR_TASK_STATE_INV        = (INT8U)0x08,
    KER_enuERR_STK_SET_LMT_INV       = (INT8U)0x09,

    KER_enuERR_KERNEL_EXCEPTION      = (INT8U)0x0A,

    /*!Comment: suspend & resume task error code definition                   */
    KER_enuERR_SUSPEND_IN_ISR        = (INT8U)0x20,
    KER_enuERR_SUSPEND_IDLE_TASK     = (INT8U)0x21,
    KER_enuERR_SUSPEND_TICK_TASK     = (INT8U)0x22,
    KER_enuERR_SUSPEND_TIMER_TASK    = (INT8U)0x23,
    KER_enuERR_SUSPEND_STKCHK_TASK   = (INT8U)0x24,
    KER_enuERR_SUSPEND_SELF_FAILED   = (INT8U)0x25,
    KER_enuERR_RESUME_IN_ISR         = (INT8U)0x26,
    KER_enuERR_RESUME_SELF           = (INT8U)0x27,
    KER_enuERR_RESUME_IDLE_TASK      = (INT8U)0x28,
    KER_enuERR_RESUME_TICK_TASK      = (INT8U)0x29,
    KER_enuERR_RESUME_TIMER_TASK     = (INT8U)0x2A,
    KER_enuERR_RESUME_STKCHK_TASK    = (INT8U)0x2B,
    KER_enuERR_RESUME_NOT_SUSPEND    = (INT8U)0x2C,

    /*!Comment: delete task operation error code definition                   */
    KER_enuERR_DELETE_IN_ISR         = (INT8U)0x30,
    KER_enuERR_DELETE_IDLE_TASK      = (INT8U)0x31,
    KER_enuERR_DELETE_TICK_TASK      = (INT8U)0x32,
    KER_enuERR_DELETE_TIMER_TASK     = (INT8U)0x33,
    KER_enuERR_DELETE_STKCHK_TASK    = (INT8U)0x34,

    /*!Comment: real time memmory manager operation error code definition     */
    KER_enuERR_RTM_BLK_SZ_INV        = (INT8U)0x40,
    KER_enuERR_RTM_POOL_SZ_INV       = (INT8U)0x41,
    KER_enuERR_RTM_POOL_ADDR_INV     = (INT8U)0x42,
    KER_enuERR_RTM_BLK_ADDR_INV      = (INT8U)0x43,
    KER_enuERR_RTM_POOL_OBJ_INV      = (INT8U)0x44,

    /*!Comment: timer error code definition                                   */
    KER_enuERR_TIMER_PTR_INV         = (INT8U)0x70,
    KER_enuERR_TIMER_ACTION_INV      = (INT8U)0x71,
    KER_enuERR_TIEMR_ZERO_PERIOD     = (INT8U)0x72,
    KER_enuERR_TIMER_HMSM_INV        = (INT8U)0x73,
    KER_enuERR_TIMER_STATUS_INV      = (INT8U)0x74,
    KER_enuERR_TIEMR_MODE_INV        = (INT8U)0x75,
    KER_enuERR_TIMER_IS_RUNNING      = (INT8U)0x76,
    KER_enuERR_TIMER_IS_STOP         = (INT8U)0x77,
    KER_enuERR_TIMER_MODE_SAME       = (INT8U)0x78,

    /*!Comment: IPC related error code definition                             */
    KER_enuERR_IPC_OBJ_INV           = (INT8U)0x80,
    KER_enuERR_IPC_OBJ_TYPE_INV      = (INT8U)0x81,
    KER_enuERR_IPC_METHOD_INV        = (INT8U)0x82,
    KER_enuERR_IPC_NESTING_HOLD      = (INT8U)0x83,
    KER_enuERR_IPC_NOT_OWNER         = (INT8U)0x84,
    KER_enuERR_IPC_PEND_TIMEOUT      = (INT8U)0x85,
    KER_enuERR_IPC_PEND_ABORT        = (INT8U)0x86,
    KER_enuERR_IPC_NOT_PEND_THIS     = (INT8U)0x87,
    KER_enuERR_IPC_OBJ_DELETED       = (INT8U)0x88,

    KER_enuERR_EVT_SET_INV           = (INT8U)0x90,
    KER_enuERR_EVT_RECV_PTR_INV      = (INT8U)0x91,
    KER_enuERR_EVT_RECV_OPT_UNKNOWN  = (INT8U)0x92,
    KER_enuERR_EVT_MASK_INV          = (INT8U)0x93,

    KER_enuERR_MQ_POOL_INV           = (INT8U)0xA0,
    KER_enuERR_MQ_POOL_SZ_INV        = (INT8U)0xA1,
    KER_enuERR_MQ_MSG_SZ_INV         = (INT8U)0xA2,
    KER_enuERR_MQ_MSG_BUF_PTR_INV    = (INT8U)0xA3,
    KER_enuERR_MQ_MSG_SZ_PTR_INV     = (INT8U)0xA4,
    KER_enuERR_MQ_FULL               = (INT8U)0xA5,
    KER_enuERR_MQ_RECV_OPT_UNKNOWN   = (INT8U)0xA6,
    KER_enuERR_MQ_SEND_OPT_UNKNOWN   = (INT8U)0xA7,
    KER_enuERR_MQ_NO_BUILT_IN_MQ     = (INT8U)0xA8

}KER_tenuERR;

/*******************************************************************************
*!Comment: kernel ipc object pend list operation method macros definition
*******************************************************************************/
typedef enum
{
    KER_enuIPC_METHOD_UNKNOWN        = (INT8U)0x00,
    KER_enuIPC_METHOD_PRIO           = (INT8U)0x01,
    KER_enuIPC_METHOD_FIFO           = (INT8U)0x02,
    KER_enuIPC_METHOD_LIFO           = (INT8U)0x03
}KER_tenuIPCMethod;

/*******************************************************************************
*!Comment: ipc object type definition.
*******************************************************************************/
typedef enum
{
    KER_enuIPC_OBJ_TYPE_UNKNOWN        = (INT8U)0x00,

#if KER_coIPC_MUTEX_EN > 0u
    KER_enuIPC_OBJ_TYPE_MUTEX          = (INT8U)0x01,
#endif

#if KER_coIPC_SEM_EN > 0u
    KER_enuIPC_OBJ_TYPE_SEM            = (INT8U)0x02,
#endif

#if KER_coIPC_MSGQ_EN > 0u
    KER_enuIPC_OBJ_TYPE_MSG_Q          = (INT8U)0x03,
#endif

#if KER_coIPC_EVENT_EN > 0u
    KER_enuIPC_OBJ_TYPE_EVENT          = (INT8U)0x04,
#endif

#if KER_coBUILT_IN_MSGQ_EN > 0u
    KER_enuIPC_OBJ_TYPE_BUILT_IN_MSG_Q = (INT8U)0x05,
#endif

#if KER_coBUILT_IN_EVENT_EN > 0u
    KER_enuIPC_OBJ_TYPE_BUILT_IN_EVENT = (INT8U)0x06,
#endif

#if KER_coBUILT_IN_SEM_EN > 0u
    KER_enuIPC_OBJ_TYPE_BUILT_IN_SEM   = (INT8U)0x07
#endif

}KER_tenuIPCObjType;

/*******************************************************************************
*!Comment: kernel operation option definition
*******************************************************************************/
typedef enum
{
    KER_enuSCHED_OPT_NO_NEED      = (INT8U)0x00,
    KER_enuSCHED_OPT_NEED         = (INT8U)0x01
}KER_tenuSchedOpt;

/*******************************************************************************
*!Comment: message queue receive method option definition
*******************************************************************************/
typedef enum
{
    KER_enuMSGQ_RECV_OPT_FRONT = (INT8U)0,
    KER_enuMSGQ_RECV_OPT_REAR  = (INT8U)1
}KER_tenuMsgQRecvOpt;

/*******************************************************************************
*!Comment: message queue send method option definition
*******************************************************************************/
typedef enum
{
    KER_enuMSGQ_SEND_OPT_FRONT = (INT8U)0,
    KER_enuMSGQ_SEND_OPT_REAR  = (INT8U)1
}KER_tenuMsgQSendOpt;

/*!Comment: timer timeout action function type definition                     */
typedef void (*KER_tpfnTimerAction)(void *vidParam);

/*!Comment: timer status definition                                           */
typedef enum
{
    KER_enuTIMER_STATUS_INV   = (INT8U)0x00,
    KER_enuTIMER_STATUS_RUNNING   = (INT8U)0x01,
    KER_enuTIMER_STATUS_STOP      = (INT8U)0x02,
    KER_enuTIMER_STATUS_COMPLETED = (INT8U)0x03
}KER_tenuTimerStatus;

/*!Comment: timer running method definition                                   */
typedef enum
{
    KER_enuTIMER_MODE_ONE_SHOT = (INT8U)0x00,
    KER_enuTIMER_MODE_PERIODIC = (INT8U)0x01
}KER_tenuTimerMode;

/*******************************************************************************
* !Comment: kernel object list structure definition
*******************************************************************************/
typedef struct ker_list
{
    /*!Comment: pointer to previous object                                    */
    struct ker_list *pstrPrev;
    /*!Comment: pointer to next object                                        */
    struct ker_list *pstrNext;
}KER_tstrList;

/*******************************************************************************
* !Comment: ipc object pending information structure definition
*******************************************************************************/
typedef struct ker_ipc_pend
{
    /*!Comment: pending ipc object type                                       */
    KER_tenuIPCObjType enuIPCObjType;
    /*!Comment: pending ipc object pointer                                    */
    void *vidpIPCObject;
}KER_tstrIPCPend;

/*******************************************************************************
*!Comment: message pool structure definition
*******************************************************************************/
typedef struct ker_msg_pool
{
    /*!Comment: message entries                                               */
    INT16U u16Entries;
    /*!Comment: max messages                                                  */
    INT16U u16MsgsMax;
    /*!Comment: message size                                                  */
    INT16U u16MsgSize;
    /*!Comment: stored messages queue list                                    */
    KER_tstrList strMsgList;
    /*!Comment: message free list                                             */
    KER_tstrList strFreeList;
    /*!Comment: message pool                                                  */
    void *vidpMsgPool;
}KER_tstrMsgPool;

/*******************************************************************************
*!Comment: memmory management block structure definition
*******************************************************************************/
typedef struct ker_rt_mem
{
    /*!Comment: block size                                                    */
    INT16U u16BlockSize;
    /*!Comment: free blocks                                                   */
    INT16U u16NbrFreeBlocks;
    /*!Comment: memmory pool size                                             */
    INT32U u32MemPoolSize;
    /*!Comment: memmory base address                                          */
    void  *vidpMemBaseAddr;
    /*!Comment: free block list                                               */
    KER_tstrList strFreeList;

}KER_tstrRTMem;

/*******************************************************************************
*!Comment: task control block structure definition
*******************************************************************************/
typedef struct ker_tcb
{
    /*!Comment: Task stack pointer                                            */
    CPU_STK *pstkStackPtr;

#if KER_coTASK_STACK_LIMIT_EN > 0u
    /*!Comment: Task stack limit size                                         */
    CPU_STK_SIZE stkszStackLimitSize;
#endif
#if KER_coTASK_STACK_USAGE_EN > 0u
    /*!Comment: Task stack usage                                              */
    CPU_STK_SIZE stkszStackUsingMax;
#endif
    /*!Comment: Task stack base pointer                                       */
    CPU_STK *pstkStackBasePtr;
    /*!Comment: Task stack size(bytes)                                        */
    CPU_STK_SIZE stkszStackSize;
    /*!Comment: Task current state                                            */
    INT8U   u8State;
    /*!Comment: Task error flag                                               */
    KER_tenuERR  enuError;
    /*!Comment: Task Current running priority                                 */
    INT16U  u16CurPriority;
    /*!Comment: Task initial priority, priority range 0~65535                 */
    INT16U  u16InitPriority;

#if KER_coSLICE_SCHED_EN > 0u
    /*!Comment: Task remain running ticks, range 0~65535                      */
    INT16U  u16RemainTicks;
    /*!Comment: Task reload initial running ticks, range 0~65535              */
    INT16U  u16ReloadTicks;
#endif

    /*!Comment: tick wheel index                                              */
    INT16U  u16TickWheelIdx;
    /*!Comment: Task delay or wait a object timeout time                      */
    INT32U  u32TimeoutMatch;

#if KER_coSWITCH_CONTEXT_CNT_EN > 0u
    /*!Comment: Task switch context counter                                   */
    INT32U  u32SwCtxCnt;
#endif

    /*!Comment: Task name                                                     */
    INT8S   *ps8TaskName;
    /*!Comment: Task entry function pointer                                   */
    void  (*pfEntry)(void *pvParam);
    /*!Comment: Task exit cleanup function pointer                            */
    void  (*pfExit)(INT8U u8ExitArg);
    /*!Comment: Created Task Link List                                        */
    KER_tstrList strLinkList;
    /*!Comment: tick wheel queue list                                         */
    KER_tstrList strTickList;
    /*!Comment: ready/block list node                                         */
    KER_tstrList strList;

#if KER_coIPC_MUTEX_EN > 0u
    /*!Comment: mutext holding list                                           */
    KER_tstrList strMutexHoldList;
#endif

    /*!Comment: task current pending ipc object information                   */
    KER_tstrIPCPend strIPCObjPend;

#if KER_coIPC_MSGQ_EN > 0u
    /*!Comment: message receiver pointer                                      */
    void *vidpMsg;
    /*!Comment: message received size                                         */
    INT16U u16MsgSize;
#endif

#if KER_coTASK_SUSPEND_EN > 0u
    /*!Comment: suspend nesting counter                                       */
    INT8U       u8SuspendNestCnt;
#endif

#if (KER_coBUILT_IN_EVENT_EN > 0u) || (KER_coIPC_EVENT_EN > 0u)
    /*!Comment: event receive method                                          */
    KER_tenuEvtRecvOpt enuEventRecvMethod;
    /*!Comment: event receiver                                                */
    EVENT_TYPE   evtEvent;
#endif

#if KER_coBUILT_IN_EVENT_EN > 0u
    EVENT_TYPE  evtBuiltInEvent;
    #if KER_coBUILT_IN_EVENT_MASK_EN > 0u
    EVENT_TYPE  evtBuiltInEventMask;
    #endif
#endif

#if KER_coBUILT_IN_MSGQ_EN > 0u
    /*!Comment: task built in message pool                                    */
    KER_tstrMsgPool *pstrBuiltInMsgPool;
#endif

#if KER_coBUILT_IN_SEM_EN > 0u
    INT32U  u32BuiltInSemValue;
#endif

    /*!Comment: User extend data                                              */
    void *vidpUserExtData;

}KER_tstrTCB;

/*******************************************************************************
*!Comment: mutex object structure definition
*******************************************************************************/
typedef struct ker_mutex
{
    /*!Comment: pending list                                                  */
    KER_tstrList        strPendList;
    /*!Comment: task nesting hold mutex counter                               */
    INT8U               u8HoldCnt;
    /*!Comment: pending list schedule strategy                                */
    KER_tenuIPCMethod   enuIPCMethod;
    /*!Comment: owner orignal priority                                        */
    INT16U              u16OwnerOrignalPriority;
    /*!Comment: owner tcb pointer                                             */
    KER_tstrTCB        *pstrOwnerTCB;
    /*!Comment: several mutexs holding by the same task list                  */
    KER_tstrList        strHoldList;
    /*!Comment: mutex object name                                             */
    INT8S              *ps8Name;
}KER_tstrMutex;

/*******************************************************************************
* !Comment: kernel event object structure definition
*******************************************************************************/
typedef struct ker_event
{
    /*!Comment: pending list                                                  */
    KER_tstrList       strPendList;
    /*!Comment: valid event bit map                                           */
    EVENT_TYPE         evtEvent;

#if KER_coIPC_EVENT_MASK_EN > 0u
    /*!Comment: event mask to filter the invalid event                        */
    EVENT_TYPE         evtMask;
#endif

    /*!Comment: pending list schedule strategy                                */
    KER_tenuIPCMethod  enuIPCMethod;
    /*!Comment: event object name                                             */
    INT8S             *ps8Name;
}KER_tstrEvent;

/*******************************************************************************
* !Comment: kernel semaphore object structure definition
*******************************************************************************/
typedef struct ker_sem
{
    /*!Comment: pending list                                                  */
    KER_tstrList       strPendList;
    /*!Comment: semaphore value                                               */
    INT32U             u32Value;
    /*!Comment: pending list schedule strategy                                */
    KER_tenuIPCMethod  enuIPCMethod;
    /*!Comment: semaphore object name                                         */
    INT8S             *ps8Name;
}KER_tstrSem;

/*******************************************************************************
* !Comment: message structure definition
*******************************************************************************/
typedef struct ker_msg
{
    /*!Comment:  message queue list                                           */
    KER_tstrList    strList;
    /*!Comment: message size                                                  */
    INT16U          u16MsgSize;
}KER_tstrMsg;

/*******************************************************************************
* !Comment: kernel message queue object structure definition
*******************************************************************************/
typedef struct ker_msgq
{
    /*!Comment: receive pending list                                          */
    KER_tstrList strPendList;
    /*!Comment: pending list schedule strategy                                */
    KER_tenuIPCMethod  enuIPCMethod;
    /*!Comment: message pool                                                  */
    KER_tstrMsgPool strMsgPool;
    /*!Comment: message queue object name                                     */
    INT8S *ps8Name;
}KER_tstrMsgQ;

/*******************************************************************************
* !Comment: kernel timer structure definition
*******************************************************************************/
typedef struct
{
    KER_tstrList         strList;
    KER_tpfnTimerAction  pfnAction;
    void                *vidpParam;
    INT32U               u32MatchTicks;
    INT32U               u32Period;
    KER_tenuTimerStatus  enuStatus;
    KER_tenuTimerMode    enuMode;
    INT8S               *ps8Name;
}KER_tstrTimer;

#if KER_coHOOK_EN > 0u
/*******************************************************************************
*!Comment: hook function type definition list
*******************************************************************************/
#if KER_coHOOK_TASK_CTX_SW_EN > 0u

#if (KER_coHOOK_EXCEPTION_EN > 0u) && (KER_coEXCEPTION_EN > 0u)
/*!Comment: kernel running exception hook function type definition            */
typedef void (*KER_tpfnHookException)( INT8S *ps8File,
                                       INT32U u32Line);
#endif

/*!Comment: task switch context hook function type definition                 */
typedef void (*KER_tpfnHookTaskSwCtx)( KER_tstrTCB *pstrCurTCB,
                                       KER_tstrTCB *pstrNextTCB);
#endif

#if KER_coHOOK_TASK_INIT_EN > 0u
/*!Comment: initial task hook function type definition                        */
typedef void (*KER_tpfnHookTaskInit)( KER_tstrTCB *pstrTCB );
#endif

#if (KER_coHOOK_TASK_DELETE_EN > 0u) && (KER_coTASK_DELETE_EN > 0u)
/*!Comment: task delete hook function type definition                         */
typedef void (*KER_tpfnHookTaskDelete)( KER_tstrTCB *pstrTCB );
#endif

#if (KER_coHOOK_TASK_SUSPEND_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*!Comment: task suspend hook function type definition                        */
typedef void (*KER_tpfnHookTaskSuspend)( KER_tstrTCB *pstrTCB );
#endif

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*!Comment: task resume hook function type definition                         */
typedef void (*KER_tpfnHookTaskResume)( KER_tstrTCB *pstrTCB );
#endif

#if (KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u) && (KER_coTASK_PRIO_CHANGE_EN > 0u)
/*!Comment: task priority change hook function type definition                */
typedef void (*KER_tpfnHookTaskPrioChange)( KER_tstrTCB *pstrTCB );
#endif

#if (KER_coHOOK_TASK_YIELD_EN > 0u) && (KER_coTASK_YIELD_EN > 0u)
/*!Comment: task yield hook function type definition                          */
typedef void (*KER_tpfnHookTaskYield)( KER_tstrTCB *pstrTCB );
#endif

#if KER_coHOOK_IDLE_EN > 0u
/*!Comment: kernel idle hook function type definition                         */
typedef void (*KER_tpfnHookIdle)(void);
#endif

#if KER_coHOOK_TICK_EN > 0u
/*!Comment: tick hook function type definition                                */
typedef void (*KER_tpfnHookTick)(void);
#endif

#if (KER_coHOOK_TIMER_EN > 0u) && ( KER_coTIMER_EN > 0u )
/*!Comment: timer hook function type definition                               */
typedef void (*KER_tpfnHookTimer)(void);
#endif

#if (KER_coHOOK_STKCHK_EN > 0u) \
  && ((KER_coTASK_STACK_LIMIT_EN > 0u) || (KER_coTASK_STACK_USAGE_EN > 0u))
/*!Comment: stack check hook function type definition                         */
typedef void (*KER_tpfnHookStackCheck)(void);
#endif

#if (KER_coHOOK_STK_OVERFLOW_EN > 0u) && (KER_coTASK_STACK_LIMIT_EN > 0u)
/*!Comment: stack overflow hook function type definition                      */
typedef void (*KER_tpfnHookStackOverflow)(KER_tstrTCB *pstrTCB);
#endif

#endif /* End of #if KER_coHOOK_EN > 0u                                       */

/*******************************************************************************
*!Comment: export MRTOS interfaces
*******************************************************************************/
/*******************************************************************************
* @Brief  : kernel initialise
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidInitialise( void );

/*******************************************************************************
* @Brief  : MRTOS start
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : called after kernel initialized and user static task created.
*******************************************************************************/
extern void KER_vidStart( void );

/*******************************************************************************
* @Brief  : lock scheduler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidSchedLock( void );

/*******************************************************************************
* @Brief  : unlock scheduler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidSchedUnlock( void );

/*******************************************************************************
* @Brief  : initial task.
* @Inputs :
* @Outputs:
* @Returns:
* @Note   :
*******************************************************************************/
extern KER_tenuERR KER_enuTaskInit( KER_tstrTCB *pstrTCB,
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
                                    );

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
extern KER_tenuERR KER_enuTaskDelete( KER_tstrTCB *pstrTCB );
#endif

#if KER_coTASK_SUSPEND_EN > 0u
/*******************************************************************************
* @Brief  : suspend task
* @Inputs : pstrTCB: task tcb pointer. (NULL task self)
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
extern KER_tenuERR KER_enuTaskSuspend(KER_tstrTCB *pstrTCB);
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
extern KER_tenuERR KER_enuTaskResume(KER_tstrTCB *pstrTCB);
#endif

#if KER_coSWITCH_CONTEXT_CNT_EN > 0u
/*******************************************************************************
* @Brief  : set task switch context counter.
* @Inputs : pstrTCB: task tcb pointer.
*                    if pstrTCB is null, set task self context switch counter,
*                    otherwise, set the specified task context switch counter.
*           u32Cnt: switch context counter value.
* @Outputs: none.
* @Returns: none
* @Note   : none.
*******************************************************************************/
extern void KER_vidTaskSwCtxCntSet( KER_tstrTCB *pstrTCB, INT32U u32Cnt );
#endif

#if KER_coSWITCH_CONTEXT_CNT_EN > 0u
/*******************************************************************************
* @Brief  : get task switch context counter.
* @Inputs : pstrTCB: task tcb pointer.
*                    if pstrTCB is null, get task self context switch counter,
*                    otherwise, get the specified task context switch counter.
* @Outputs: none.
* @Returns: tasks switch context conter.
* @Note   : none.
*******************************************************************************/
extern INT32U KER_u32TaskSwCtxCntGet( KER_tstrTCB *pstrTCB );
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
extern KER_tenuERR KER_enuTaskPrioChange( KER_tstrTCB *pstrTCB,
                                          INT16U u16Priority);
#endif

/*******************************************************************************
* @Brief  : get task self tcb pointer
* @Inputs : none.
* @Outputs: none.
* @Returns: task self tcb pointer.
* @Note   : none
*******************************************************************************/
extern KER_tstrTCB * KER_pstrTaskTCBPtrGet( void );

/*******************************************************************************
* @Brief  : get cpu idle counter.
* @Inputs : none
* @Outputs: none
* @Returns: cpu idle counter
* @Note   : none
*******************************************************************************/
extern INT32U KER_u32IdleCntGet( void );

/*******************************************************************************
* @Brief  : trigger tick expired.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : for hardware timer ISR calling to trigger tick expired.
*******************************************************************************/
extern void KER_vidTickTrigger( void );

/*******************************************************************************
* @Brief  : get current tick counter
* @Inputs : none
* @Outputs: none
* @Returns: current tick counter
* @Note   : none
*******************************************************************************/
extern INT32U KER_u32TickGet( void );

/*******************************************************************************
* @Brief  : task delay
* @Inputs : u32Ticks: delay ticks
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidDelay( INT32U u32Ticks );

#if KER_coDELAY_HMSM_EN > 0u
/*******************************************************************************
* @Brief  : HH:MM:SS:MS format delay.
* @Inputs : u16Hours: hours
*           u8Min: minute
*           u8Sec: second
*           u16Ms: millisecond.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidDelayHMSM( INT16U u16Hours,
                              INT8U u8Min,
                              INT8U u8Sec,
                              INT16U u16Ms );
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
extern KER_tenuERR KER_enuTaskYield( void );
#endif

/*!Comment: using mutex module compiler control                               */
#if KER_coIPC_MUTEX_EN > 0u

/*******************************************************************************
* @Brief  : initialize mutex object.
* @Inputs : pstrMutex: Mutex object pointer.
*           enuIPCMethod: mutex pend method.
*           psName: Mutex object name pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: Initail mutex object is ok.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: ipc object is invalid.
*           KER_enuERR_IPC_METHOD_INV: ipc pending method is invalid.
* @Note   : should not call by interrupt service routine!!!
*******************************************************************************/
extern KER_tenuERR KER_enuMutexInit( KER_tstrMutex *pstrMutex,
                                     KER_tenuIPCMethod enuIPCMethod,
                                     INT8S *ps8Name);

/*******************************************************************************
* @Brief  : pend a mutex object.
* @Inputs : pstrMutex: mutex object valid pointer.
*           u32TimeOut: pending timeout value. if equal zero, pending until
*                       mutex is released. otherwise, pending until timeout.
* @Outputs: none
* @Returns: KER_enuERR_OK: pend mutex successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: IPC object pointer invalid.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_IPC_NESTING_HOLD: mutex nesting hold.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMutexPend(KER_tstrMutex *pstrMutex,INT32U u32TimeOut);

#if KER_coIPC_MUTEX_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend a mutex object.
* @Inputs : pstrMutex: mutex object valid pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: pend mutex successful.
*           KER_enuERR_NOK: try to pend mutex failed.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: IPC object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
*           KER_enuERR_IPC_NESTING_HOLD: mutex nesting hold.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMutexTryPend(KER_tstrMutex *pstrMutex);
#endif /* End of #if KER_coIPC_MUTEX_TRY_PEND_EN > 0u                         */

/*******************************************************************************
* @Brief  : post mutex
* @Inputs : pstrMutex: mutex object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: post with none error.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: mutex object invalid.
*           KER_enuERR_IPC_NOT_OWNER: poster not the mutex owner.
*           KER_enuERR_IPC_NESTING_HOLD: nesting hold, should nesting post.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMutexPost( KER_tstrMutex *pstrMutex );

#if KER_coIPC_MUTEX_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pend the specify mutex.
* @Inputs : pstrMutex: mutex object pointer.
*           pstrTCB: tcb pointer.
*           enuOption: schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: Mutex object invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMutexPendAbort( KER_tstrMutex *pstrMutex,
                                          KER_tstrTCB   *pstrTCB,
                                          KER_tenuSchedOpt enuOption );
#endif /* End of #if KER_coIPC_MUTEX_ABORT_EN > 0u                            */

#if KER_coIPC_MUTEX_ABORT_ALL_EN > 0u
/*******************************************************************************
* @Brief  : abort all tasks pend the specify mutex.
* @Inputs : pstrMutex: mutex object pointer.
*           enuOption: schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort all pending tasks successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: Mutex object invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object already deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMutexPendAbortAll( KER_tstrMutex *pstrMutex,
                                             KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coIPC_MUTEX_ABORT_ALL_EN > 0u                        */

#if KER_coIPC_MUTEX_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete mutex object
* @Inputs : pstrMutex: mutex object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: mutex object delete successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: mutex object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: mutex object pointer deleted.
*           KER_enuERR_TASK_STATE_INV: pending task state invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMutexDelete( KER_tstrMutex *pstrMutex);

#endif /* End of #if KER_coIPC_MUTEX_DELETE_EN > 0u                           */

#endif /* End of #if KER_coIPC_MUTEX_EN > 0u                                  */

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
extern KER_tenuERR KER_enuEventInit( KER_tstrEvent    *pstrEvent,
                                     EVENT_TYPE        evtEventMask,
                                     EVENT_TYPE        evtInitEvent,
                                     KER_tenuIPCMethod enuIPCMethod,
                                     INT8S            *ps8Name);

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
extern KER_tenuERR KER_enuEventMaskSet( KER_tstrEvent *pstrEvent,
                                        EVENT_TYPE evtMask );
#endif /* End of #if KER_coIPC_EVENT_MASK_SET_EN > 0u                         */

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
extern KER_tenuERR KER_enuEventMaskClear( KER_tstrEvent *pstrEvent,
                                          EVENT_TYPE evtMask);
#endif /* End of #if KER_coIPC_EVENT_MAKS_CLEAR_EN > 0u                       */

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
extern KER_tenuERR KER_enuEventMaskGet( KER_tstrEvent *pstrEvent,
                                        EVENT_TYPE    *pevtMask );
#endif /* End of #if KER_coIPC_EVENT_MASK_GET_EN > 0u                         */

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
extern KER_tenuERR KER_enuEventPend(KER_tstrEvent          *pstrEvent,
                                    EVENT_TYPE              evtMask,
                                    KER_tenuEvtRecvOpt enuRecvMethod,
                                    INT32U                  u32TimeOut,
                                    EVENT_TYPE             *pevtEvent);

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
extern KER_tenuERR KER_enuEventTryPend(KER_tstrEvent          *pstrEvent,
                                       EVENT_TYPE              evtMask,
                                       KER_tenuEvtRecvOpt enuRecvMethod,
                                       EVENT_TYPE             *pevtEvent);
#endif /* End of #if KER_coIPC_EVENT_TRY_PEND_EN > 0u                         */

/*******************************************************************************
* @Brief  : post events.
* @Inputs : pstrEvent: event object pointer.
*           evtEvent: set event.
* @Outputs: none
* @Returns: KER_enuERR_OK: post event successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_EVT_SET_INV: post invalid event(event masked).
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: excetpion, unknown recv option.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuEventPost(KER_tstrEvent *pstrEvent,
                                    EVENT_TYPE     evtEvent);

#if KER_coIPC_EVENT_PEND_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort a task pending the specify event object.
* @Inputs : pstrEvent: event object pointer.
*           pstrTCB: task tcb pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending task successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending this event object.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuEventPendAbort(KER_tstrEvent *pstrEvent,
                                         KER_tstrTCB   *pstrTCB,
                                         KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coIPC_EVENT_PEND_ABORT_EN > 0u                       */

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
extern KER_tenuERR KER_enuEventPendAbortAll(KER_tstrEvent *pstrEvent,
                                            KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coIPC_EVENT_PEND_ABORT_ALL_EN > 0u                   */

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
extern KER_tenuERR KER_enuEventDelete( KER_tstrEvent *pstrEvent );
#endif /* End of #if KER_coIPC_EVENT_DELETE_EN > 0u                           */

#endif /* End of #if KER_coIPC_EVENT_EN > 0u                                  */

/*!Comment: using semaphore module compiler control                           */
#if KER_coIPC_SEM_EN > 0u
/*******************************************************************************
* @Brief  : initial semaphore object.
* @Inputs : pstrSem: semaphore object pointer.
*           u32Value: semaphore initial value.
*           enuIPCmethod: pending list schedule strategy.
*           ps8Name: semaphore object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial successful.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_METHOD_INV: ipc method invalid.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuSemInit( KER_tstrSem      *pstrSem,
                                   INT32U            u32Value,
                                   KER_tenuIPCMethod enuIPCMethod,
                                   INT8S            *ps8Name);

/*******************************************************************************
* @Brief  : pend semaphore.
* @Inputs : pstrSem: semaphore object pointer.
*           u32TimeOut: pending timeout time.
* @Outputs: none
* @Returns: KER_enuERR_OK: pend semaphore successful.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semaphore object deleted.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuSemPend( KER_tstrSem *pstrSem,
                                   INT32U u32TimeOut );

#if KER_coIPC_SEM_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend semaphore.
* @Inputs : none
* @Outputs: none
* @Returns: KER_enuERR_OK: try to pend semaphore successful.
*           KER_enuERR_NOK: try to pend semaphore failed.
*           KER_enuERR_IPC_OBJ_INV: semahpre object pointer invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuSemTryPend( KER_tstrSem *pstrSem);
#endif /* End of #if KER_coIPC_SEM_TRY_PEND_EN > 0u                           */

/*******************************************************************************
* @Brief  : post semaphore.
* @Inputs : pstrSem: semaphore object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: post semaphore successful.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semaphore object deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuSemPost( KER_tstrSem *pstrSem );

#if KER_coIPC_SEM_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pending semaphore object.
* @Inputs : pstrSem: semaphore object pointer.
*           pstrTCB: task tcb pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending semaphore successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
*           KER_enuERR_IPC_OBJ_INV: semaphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semaphore object deleted.
*           KER_enuERR_IPC_NOT_PEND_THIS: Not pending this semaphore object.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuSemPendAbort( KER_tstrSem *pstrSem,
                                        KER_tstrTCB *pstrTCB,
                                        KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coIPC_SEM_ABORT_EN > 0u                              */

#if KER_coIPC_SEM_ABORT_ALL_EN > 0u
/*******************************************************************************
* @Brief  : abort all tasks pending the specify semaphore object.
* @Inputs : pstrSem: semaphore object pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort all pending tasks successful.
*           KER_enuERR_IPC_OBJ_INV: event object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: event object already deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuSemPendAbortAll( KER_tstrSem *pstrSem,
                                           KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coIPC_SEM_ABORT_ALL_EN > 0u                          */

#if KER_coIPC_SEM_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete a semaphore object.
* @Inputs : pstrEvent: semaphore object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: deleted successful.
*           KER_enuERR_IPC_OBJ_INV: semphore object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: semphore object already deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuSemDelete( KER_tstrSem *pstrSem );
#endif /* End of #if KER_coIPC_SEM_DELETE_EN > 0u                             */

#endif /* End of #if KER_coIPC_SEM_EN > 0u                                    */

/*!Comment: using message queue module compiler control                       */
#if KER_coIPC_MSGQ_EN > 0u

/*******************************************************************************
* @Brief  : initial message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
*           enuIPCMethod: ipc pending list method.
*           vidpMsgPool: message pool pointer.
*           u32MsgPoolSize: message pool size.
*           u16MsgSize: each message size.
*           ps8Name: message queue object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial message queue successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_METHOD_INV: ipc method invalid.
*           KER_enuERR_MQ_POOL_INV: message pool invalid.
*           KER_enuERR_MQ_POOL_SZ_INV: message pool size invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMsgQInit(KER_tstrMsgQ      *pstrMsgQ,
                                   KER_tenuIPCMethod  enuIPCMethod,
                                   void              *vidpMsgPool,
                                   INT32U             u32MsgPoolSize,
                                   INT16U             u16MsgSize,
                                   INT8S             *ps8Name);

/*******************************************************************************
* @Brief  : pend message queue message.
* @Inputs : pstrMsgQ: message queue object.
*           enuRecvMethod: receive from message queue front or rear.
*           u32TimeOut: pending timeout value.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: message received size.
* @Returns: KER_enuERR_OK: pending message successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: receive buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: message queue object deleted.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMsgQPend( KER_tstrMsgQ *pstrMsgQ,
                                    void *vidpMsg,
                                    INT16U *pu16Size,
                                    KER_tenuMsgQRecvOpt enuRecvMethod,
                                    INT32U u32TimeOut);

#if KER_coIPC_MSGQ_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend message queue message.
* @Inputs : pstrMsgQ: message queue object.
*           enuRecvMethod: receive from message queue front or rear.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: message received size.
* @Returns: KER_enuERR_OK: pending message successful.
*           KER_enuERR_NOK: message queue is empty, not received message.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: receive buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: message queue object deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMsgQTryPend( KER_tstrMsgQ *pstrMsgQ,
                                       void *vidpMsg,
                                       INT16U *pu16Size,
                                       KER_tenuMsgQRecvOpt enuRecvMethod);
#endif /* End of #if KER_coIPC_MSGQ_TRY_PEND_EN > 0u                          */

/*******************************************************************************
* @Brief  : post one message to message queue.
* @Inputs : pstrMsgQ: message queue object pointer.
*           vidpMsg: message send buffer.
*           u16Size: send size.
*           enuSendMethod: send method.
* @Outputs: none
* @Returns: KER_enuERR_OK: send message successful.
*           KER_enuERR_IPC_OBJ_INV: message queue object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: message queue object deleted.
*           KER_enuERR_MQ_FULL: message queue is full.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_SEND_OPT_UNKNOWN: send method invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMsgQPost( KER_tstrMsgQ *pstrMsgQ,
                                    void *vidpMsg,
                                    INT16U u16Size,
                                    KER_tenuMsgQSendOpt enuSendMethod);

#if KER_coIPC_MSGQ_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort a task pending the specify message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
*           pstrTCB: task tcb pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending task successful.
*           KER_enuERR_IPC_OBJ_INV: object pointer invalid.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: object already deleted.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending this object.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMsgQPendAbort(KER_tstrMsgQ        *pstrMsgQ,
                                        KER_tstrTCB        *pstrTCB,
                                        KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coIPC_MSGQ_ABORT_EN > 0u                             */

#if KER_coIPC_MSGQ_ABORT_ALL_EN > 0u
/*******************************************************************************
* @Brief  : abort all tasks pending the specify message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
*           enuOption: need schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort all pending tasks successful.
*           KER_enuERR_IPC_OBJ_INV: object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: object already deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMsgQPendAbortAll(KER_tstrMsgQ        *pstrMsgQ,
                                           KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coIPC_MSGQ_ABORT_ALL_EN > 0u                         */

#if KER_coIPC_MSGQ_DELETE_EN > 0u
/*******************************************************************************
* @Brief  : delete a message queue object.
* @Inputs : pstrMsgQ: message queue object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: deleted successful.
*           KER_enuERR_IPC_OBJ_INV: object pointer invalid.
*           KER_enuERR_IPC_OBJ_DELETED: object already deleted.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuMsgQDelete( KER_tstrMsgQ *pstrMsgQ );
#endif /* End of #if KER_coIPC_MSGQ_DELETE_EN > 0u                            */

#endif /* End of #if KER_coIPC_MSGQ_EN > 0u                                   */

#if KER_coTIMER_EN > 0u
/*******************************************************************************
* @Brief  : intial timer object
* @Inputs : pstrTimer: timer object pointer.
*           pfnAction: timeout callback function pointer.
*           vidpParam: timeout callback function private parameter pointer.
*           u32Period: timer period.
*           enuInitStatus: timer initial status.
*           enuMode: timer running mode.
*           ps8Name: timer object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial timer is ok.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_ACTION_INV: timeout action pointer invalid.
*           KER_enuERR_TIEMR_ZERO_PERIOD: period set to zero error.
*           KER_enuERR_TIMER_STATUS_INV: invalid status.
*           KER_enuERR_TIEMR_MODE_INV: invalid running mode.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTimerInit( KER_tstrTimer *pstrTimer,
                                     KER_tpfnTimerAction pfnAction,
                                     void *vidpParam,
                                     INT32U u32Period,
                                     KER_tenuTimerStatus enuInitStatus,
                                     KER_tenuTimerMode enuMode,
                                     INT8S *ps8Name
                                     );

/*******************************************************************************
* @Brief  : trigger timer expired.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : for hardware timer ISR calling to trigger software timer expired.
*******************************************************************************/
extern void KER_vidTimerTrigger( void );

#if KER_coTIMER_HMSM_INIT_EN > 0u
/*******************************************************************************
* @Brief  : intial timer object (HMSM format)
* @Inputs : pstrTimer: timer object pointer.
*           pfnAction: timeout callback function pointer.
*           vidpParam: timeout callback function private parameter pointer.
*           u16Hours: hours.
*           u8Min: minute.
*           u8Sec: second.
*           u16Ms: millisecond.
*           enuInitStatus: timer initial status.
*           enuMode: timer running mode.
*           ps8Name: timer object name.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial timer is ok.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_ACTION_INV: timeout action pointer invalid.
*           KER_enuERR_TIEMR_ZERO_PERIOD: period set to zero error.
*           KER_enuERR_TIMER_STATUS_INV: invalid status.
*           KER_enuERR_TIEMR_MODE_INV: invalid running mode.
*           KER_enuERR_TIMER_HMSM_INV: HMSM paramter exceed limit value.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerHMSMInit( KER_tstrTimer *pstrTimer,
                                         KER_tpfnTimerAction pfnAction,
                                         void *vidpParam,
                                         INT16U u16Hours,
                                         INT8U u8Min,
                                         INT8U u8Sec,
                                         INT16U u16Ms,
                                         KER_tenuTimerStatus enuInitStatus,
                                         KER_tenuTimerMode enuMode,
                                         INT8S *ps8Name
                                       );
#endif /* End of #if KER_coTIMER_HMSM_INIT_EN > 0u                            */

#if KER_coTIMER_START_EN > 0u
/*******************************************************************************
* @Brief  : start a not running timer
* @Inputs : pstrTimer
* @Outputs: none
* @Returns: KER_enuERR_OK: start timer successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_IS_RUNNING: duplicate request timer to running.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTimerStart( KER_tstrTimer *pstrTimer );
#endif /* End of #if KER_coTIMER_START_EN > 0u                                */

#if KER_coTIMER_STOP_EN > 0u
/*******************************************************************************
* @Brief  : stop timer
* @Inputs : pstrTimer: timer object pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: stop timer successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_IS_STOP: duplicate request timer to stop.
* @Note   : none
*******************************************************************************/
PUBLIC KER_tenuERR KER_enuTimerStop( KER_tstrTimer *pstrTimer );
#endif /* End of #if KER_coTIMER_STOP_EN > 0u                                 */

#if KER_coTIMER_MODE_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : change timer mode
* @Inputs : pstrTimer: timer object pointer.
*           enuMode: timer new mode.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTimerModeChange( KER_tstrTimer *pstrTimer,
                                           KER_tenuTimerMode enuMode);
#endif /* End of #if KER_coTIMER_MODE_CHANGE_EN > 0u                          */

#if KER_coTIMER_PERIOD_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : change timer period
* @Inputs : pstrTimer: timer object pointer.
*           u32Period: timer period
* @Outputs: none
* @Returns: KER_enuERR_OK: change timer period successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIEMR_ZERO_PERIOD: set zero period error.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTimerPeriodChange( KER_tstrTimer *pstrTimer,
                                             INT32U u32Period);
#endif /* End of #if KER_coTIMER_PERIOD_CHANGE_EN > 0u                        */

#if KER_coTIMER_HMSM_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : Timer period (HMSM format) change
* @Inputs : pstrTimer: timer object pointer.
*           u16Hours: hours
*           u8Min: minute
*           u8Sec: second
*           u16Ms: millisecond.
* @Outputs: none
* @Returns: KER_enuERR_OK: change timer period successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_HMSM_INV: HMSM paramter exceed limit value.
*           KER_enuERR_TIEMR_ZERO_PERIOD: period set to zero error.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTimerHMSMChange( KER_tstrTimer *pstrTimer,
                                           INT16U u16Hours,
                                           INT8U u8Min,
                                           INT8U u8Sec,
                                           INT16U u16Ms );
#endif /* End of #if KER_coTIMER_HMSM_CHANGE_EN > 0u                          */

#if KER_coTIMER_ACTION_CHANGE_EN > 0u
/*******************************************************************************
* @Brief  : change timer timeout action function
* @Inputs : pstrTimer: timer object pointer.
*           pfnAction: timeout action function pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: change successful.
*           KER_enuERR_TIMER_PTR_INV: timer object pointer invalid.
*           KER_enuERR_TIMER_ACTION_INV: timer action invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTimerActionChange( KER_tstrTimer *pstrTimer,
                                             KER_tpfnTimerAction pfnAction);
#endif /* End of #if KER_coTIMER_ACTION_CHANGE_EN > 0u                        */

#endif /* End of #if KER_coTIMER_EN > 0u                                      */

#if KER_coBUILT_IN_MSGQ_EN > 0u
/*******************************************************************************
* @Brief  : initial task built in message queue.
* @Inputs : pstrMsgPoolObj: message queue pool object.
*           vidpMsgPool: message queue buffer pool.
*           u32MsgPoolSize: message buffer pool total size.
*           u16MsgSize: set one message maximum size.
* @Outputs: none
* @Returns: KER_enuERR_OK: initial ok.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_POOL_INV: message pool invalid.
*           KER_enuERR_MQ_POOL_SZ_INV: message pool size invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
* @Note   : if task use built in message queue, should be call this function to
*           initial built in message queue object before create the task.
*******************************************************************************/
extern KER_tenuERR KER_enuTaskMsgQInit(KER_tstrMsgPool *pstrMsgPoolObj,
                                       void            *vidpMsgPool,
                                       INT32U           u32MsgPoolSize,
                                       INT16U           u16MsgSize);

/*******************************************************************************
* @Brief  : pending task built in message queue.
* @Inputs : enuRecvMethod: receive message queue front or rear.
*           u32TimeOut: pending timeout value.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: received message size output pointer.
* @Returns: KER_enuERR_OK: pending built in message queue successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_NO_BUILT_IN_MQ: task not have built in MSG_Q.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskMsgQPend( void *vidpMsg,
                                        INT16U *pu16Size,
                                        KER_tenuMsgQRecvOpt enuRecvMethod,
                                        INT32U u32TimeOut);

#if KER_coBUILT_IN_MSGQ_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend task built in message queue.
* @Inputs : enuRecvMethod: receive message queue front or rear.
* @Outputs: vidpMsg: message receive buffer.
*           pu16Size: received message size output pointer.
* @Returns: KER_enuERR_OK: pending built in message queue successful.
*           KER_enuERR_NOK: built in message queue empty, not received message.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_MQ_NO_BUILT_IN_MQ: task not have built in MSG_Q.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_PTR_INV: receive size pointer invalid.
*           KER_enuERR_MQ_RECV_OPT_UNKNOWN: receive method invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskMsgQTryPend(void *vidpMsg,
                                          INT16U *pu16Size,
                                          KER_tenuMsgQRecvOpt enuRecvMethod);
#endif /* End of #if KER_coBUILT_IN_MSGQ_TRY_PEND_EN > 0u                    */

/*******************************************************************************
* @Brief  : post message to task.
* @Inputs : pstrTCB: task tcb pointer invalid.
*           vidpMsg: message buffer.
*           u16Size: message size.
*           enuSendMethod: post to message queue front or rear.
* @Outputs: none
* @Returns: KER_enuERR_OK: post message to task successful.
*           KER_enuERR_TCB_PTR_INV: task tcb invalid.
*           KER_enuERR_MQ_MSG_BUF_PTR_INV: message buffer invalid.
*           KER_enuERR_MQ_MSG_SZ_INV: message size invalid.
*           KER_enuERR_MQ_SEND_OPT_UNKNOWN: send method invalid.
*           KER_enuERR_MQ_FULL: message queue buffer full.
*           KER_enuERR_MQ_NO_BUILT_IN_MQ: task not have built in MSG_Q.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskMsgQPost( KER_tstrTCB *pstrTCB,
                                        void *vidpMsg,
                                        INT16U u16Size,
                                        KER_tenuMsgQSendOpt enuSendMethod);

#if KER_coBUILT_IN_MSGQ_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort pending built in message queue.
* @Inputs : none
* @Outputs: none
* @Returns: KER_enuERR_OK: abort pending successful.
*           KER_enuERR_TCB_PTR_INV: tcb pointer invalid.
*           KER_enuERR_IPC_NOT_PEND_THIS: not pending built in message queue.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskMsgQPendAbort ( KER_tstrTCB        *pstrTCB,
                                              KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coBUILT_IN_MSGQ_ABORT_EN > 0u                        */

#endif /* End of #if KER_coBUILT_IN_MSGQ_EN > 0u                              */

#if KER_coBUILT_IN_EVENT_EN > 0u
/*******************************************************************************
* @Brief  : pending task self built in events.
* @Inputs : evtMask: mask expected received events.
*           enuRecvMethod: receive method front or rear.
*           u32TimeOut: pending timeout value.
* @Outputs: pevtEvent: event receiver pointer.
* @Returns: KER_enuERR_OK: pend event successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_EVENT_RECV_PTR_INV: event receive pointer invalid
*           KER_enuERR_SCHED_LOCK_PENDING: scheduler locked, not allow pending.
*           KER_enuERR_IPC_OBJ_DELETED: event object deleted.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: unknown receive method option.
*           KER_enuERR_EVT_MASK_INV: event mask invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventPend( EVENT_TYPE             *pevtEvent,
                                         EVENT_TYPE              evtMask,
                                         KER_tenuEvtRecvOpt enuRecvMethod,
                                         INT32U                  u32TimeOut);

#if KER_coBUILT_IN_EVENT_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend task self built in events.
* @Inputs : evtMask: mask expected received events.
*           enuRecvMethod: receive method front or rear.
* @Outputs: pevtEvent: event receiver pointer.
* @Returns: KER_enuERR_OK: pend event successful.
*           KER_enuERR_NOK: no event.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_IPC_EVENT_RECV_PTR_INV: event receive pointer invalid
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: unknown receive method option.
*           KER_enuERR_EVT_MASK_INV: event mask invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventTryPend(
                                        EVENT_TYPE             *pevtEvent,
                                        EVENT_TYPE              evtMask,
                                        KER_tenuEvtRecvOpt enuRecvMethod);
#endif /* End of #if KER_coBUILT_IN_EVENT_TRY_PEND_EN > 0u                    */

/*******************************************************************************
* @Brief  : post events to task built in event object.
* @Inputs : pstrTCB: task tcb pointer.
*           evtEvent: post events.
* @Outputs: none
* @Returns: KER_enuERR_OK: post event successful.
*           KER_enuERR_TCB_PTR_INV:
*           KER_enuERR_IPC_EVENT_SET_INV: initial event set masked.
*           KER_enuERR_EVT_RECV_OPT_UNKNOWN: excetpion, unknown recv option
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventPost(KER_tstrTCB *pstrTCB,
                                        EVENT_TYPE   evtEvent);

#if KER_coBUILT_IN_EVENT_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pending built in event object.
* @Inputs : pstrTCB: task tcb pointer.
*           enuOption: schedule option.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort task pending built in event object successful.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending built in event.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventPendAbort(KER_tstrTCB   *pstrTCB,
                                             KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coBUILT_IN_EVENT_ABORT_EN > 0u                       */

#if KER_coBUILT_IN_EVENT_MASK_SET_EN > 0u
/*******************************************************************************
* @Brief  : set task built in event reception mask.
* @Inputs : pstrTCB: task tcb pointer.
*           evtMask: related bit is set, corresponding bit event is receptible.
* @Outputs: none
* @Returns: KER_enuERR_OK: set event mask successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventMaskSet(KER_tstrTCB   *pstrTCB,
                                           EVENT_TYPE     evtMask);
#endif /* End of #if KER_coBUILT_IN_EVENT_MASK_SET_EN > 0u                    */

#if KER_coBUILT_IN_EVENT_MASK_CLR_EN > 0u
/*******************************************************************************
* @Brief  : clear task built in event reception mask.
* @Inputs : pstrTCB: task tcb pointer.
*           evtMask: related bit is set, corresponding bit event not receptible.
* @Outputs: none
* @Returns: KER_enuERR_OK: clear event mask successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventMaskClear(KER_tstrTCB   *pstrTCB,
                                             EVENT_TYPE     evtMask);
#endif /* End of #if KER_coBUILT_IN_EVENT_MASK_CLR_EN > 0u                    */

#if KER_coBUILT_IN_EVENT_MASK_GET_EN > 0u
/*******************************************************************************
* @Brief  : get task built in event mask.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: pevtMask: current event mask.
* @Returns: KER_enuERR_OK: get event mask successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventMaskGet(KER_tstrTCB   *pstrTCB,
                                           EVENT_TYPE    *pevtMask);
#endif /* End of #if KER_coBUILT_IN_EVENT_MASK_GET_EN > 0u                    */

#if KER_coBUILT_IN_EVENT_RESET_EN > 0u
/*******************************************************************************
* @Brief  : reset task built in event
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: reset event successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskEventReset(KER_tstrTCB *pstrTCB);
#endif /* End of #if KER_coBUILT_IN_EVENT_RESET_EN > 0u                       */

#endif /* End of #if KER_coBUILT_IN_EVENT_EN > 0u                             */

#if KER_coBUILT_IN_SEM_EN > 0u
/*******************************************************************************
* @Brief  : pend task built in semaphore.
* @Inputs : u32TimeOut: pending timeout value.
* @Outputs: none
* @Returns: KER_enuERR_OK: pending semaphore successful.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
*           KER_enuERR_SCHED_LOCK_PENDING: schedule locked, not allow pending.
*           KER_enuERR_IPC_PEND_TIMEOUT: pending timeout.
*           KER_enuERR_IPC_PEND_ABORT: pending abort.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskSemPend(INT32U u32TimeOut);

/*******************************************************************************
* @Brief  : post task built in semaphore.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: post semaphore to a task successful.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer invalid.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskSemPost(KER_tstrTCB *pstrTCB);

#if KER_coBUILT_IN_SEM_TRY_PEND_EN > 0u
/*******************************************************************************
* @Brief  : try to pend task built in semaphore.
* @Inputs : none
* @Outputs: none
* @Returns: KER_enuERR_OK: try to pend semaphore successful.
*           KER_enuERR_NOK: try to pend semaphore failed.
*           KER_enuERR_ISR_FAULT_USING: ISR not allow call this function.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskSemTryPend( void );
#endif /* End of #if KER_coBUILT_IN_SEM_TRY_PEND_EN > 0u                      */

#if KER_coBUILT_IN_SEM_ABORT_EN > 0u
/*******************************************************************************
* @Brief  : abort task pending semaphore.
* @Inputs : pstrTCB: task tcb pointer.
* @Outputs: none
* @Returns: KER_enuERR_OK: abort task pending built in object successful.
*           KER_enuERR_IPC_NOT_PEND_THIS: task not pending built in object.
*           KER_enuERR_TCB_PTR_INV: task tcb pointer.
* @Note   : none
*******************************************************************************/
extern KER_tenuERR KER_enuTaskSemPendAbort( KER_tstrTCB *pstrTCB,
                                            KER_tenuSchedOpt enuOption);
#endif /* End of #if KER_coBUILT_IN_SEM_ABORT_EN > 0u                         */

#if KER_coBUILT_IN_SEM_RESET_EN > 0u
/*******************************************************************************
* @Brief  : reset a task's built in semaphore.
* @Inputs : pstrTCB: task tcb pointer(NULL reset task self)
* @Outputs: none
* @Returns: KER_enuERR_OK: reset successful.
*           KER_enuERR_ISR_FAULT_USING: ISR should specify tcb pointer.
* @Note   : Note: if ISR reset one task's built in semaphore, it should specify
*                 the task's tcb pointer. otherwise return NOK.
*******************************************************************************/
extern KER_tenuERR KER_enuTaskSemReset( KER_tstrTCB *pstrTCB );
#endif /* End of #if KER_coBUILT_IN_SEM_RESET_EN > 0u                         */

#endif /* End of #if KER_coBUILT_IN_SEM_EN > 0u                               */


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
extern KER_tenuERR KER_enuRTMemInit( KER_tstrRTMem *pstrRTMem,
                                     void *vidpMemBaseAddr,
                                     INT32U u32MemSize,
                                     INT16U u16BlockSize);

/*******************************************************************************
* @Brief  : get memmory block.
* @Inputs : pstrRTMem: memmory management block structure pointer.
* @Outputs: none.
* @Returns: memmory block address: get memmory block successful.
*           NULL: get memmory block failed.
* @Note   : none.
*******************************************************************************/
extern void *KER_vidpRTMemGet(KER_tstrRTMem *pstrRTMem);

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
extern KER_tenuERR KER_enuRTMemPut( KER_tstrRTMem *pstrRTMem,
                                    void *vidpBlock);

#endif /* End of #if KER_coRT_MEM_EN > 0u                                     */

#if KER_coCPULOAD_MODULE_EN > 0u
/*******************************************************************************
* @Brief  : CPU Load calculation timer ISR.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidCPULoadTimer( void );

/*******************************************************************************
* @Brief  : get CPULoad value.
* @Inputs : none.
* @Outputs: none.
* @Returns: CPULoad: Unit:0.1%
* @Note   : none.
*******************************************************************************/
extern INT32U KER_u32CPULoadGet( void );

/*******************************************************************************
* @Brief  : get ISR CPULoad value.
* @Inputs : none.
* @Outputs: none.
* @Returns: CPULoad: Unit:0.1%
* @Note   : none.
*******************************************************************************/
extern INT32U KER_u32CPULoadISRGet( void );

/*******************************************************************************
* @Brief  : update CPU load related counter at ISR entry.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : should only called by category 1 interrupt service routine.
*******************************************************************************/
extern void KER_vidCPULoadISRStart( void );

/*******************************************************************************
* @Brief  : update CPU load related counter at ISR end.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : should only called by category 1 interrupt service routine.
*******************************************************************************/
extern void KER_vidCPULoadISREnd( void );
#endif /* End of #if KER_coCPULOAD_MODULE_EN > 0u                             */

/*******************************************************************************
* @Brief  : called at the begin by interrupt routine service functions.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : should only called by category 2 interrupt service routine.
*******************************************************************************/
extern void KER_vidInterruptEnter( void );

/*******************************************************************************
* @Brief  : called at the end by interrupt routine service functions.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : should only called by category 2 interrupt service routine.
*******************************************************************************/
extern void KER_vidInterruptExit( void );

#if ((KER_coSWITCH_CONTEXT_CNT_EN > 0u) \
    || (KER_coCPULOAD_MODULE_EN > 0u) \
    || (KER_coHOOK_TASK_CTX_SW_EN > 0u))
/*******************************************************************************
* @Brief  : task switch context callback function.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : called before task switch context.
*******************************************************************************/
PUBLIC void KER_vidTaskSwCtxCallBack(void);
#endif

#if KER_coHOOK_EN > 0u
/*******************************************************************************
* @Brief  : clear all hook functions.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookFunctionsAllClr( void );

#if (KER_coHOOK_EXCEPTION_EN > 0u) && (KER_coEXCEPTION_EN > 0u)
/*******************************************************************************
* @Brief  : set kernel running error exception user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookExceptionFuncSet( KER_tpfnHookException pfnFunc );
/*******************************************************************************
* @Brief  : clear kernel running error exception user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookExceptionFuncClr( void );
#endif /* Enf of #if KER_coHOOK_EXCEPTION_EN > 0u                             */

#if KER_coHOOK_TASK_CTX_SW_EN > 0u
/*******************************************************************************
* @Brief  : set task switch context user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskSwCtxFuncSet(KER_tpfnHookTaskSwCtx pfnFunc);

/*******************************************************************************
* @Brief  : clear task switch context user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskSwCtxFuncClr( void );
#endif /* End of #if KER_coHOOK_TASK_CTX_SW_EN > 0u                           */

#if KER_coHOOK_TASK_INIT_EN > 0u
/*******************************************************************************
* @Brief  : set initial task user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskInitFuncSet(KER_tpfnHookTaskInit pfnFunc);

/*******************************************************************************
* @Brief  : clear initial task user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskInitFuncClr( void );
#endif /* End of #if KER_coHOOK_TASK_INIT_EN > 0u                             */

#if (KER_coHOOK_TASK_DELETE_EN > 0u) && (KER_coTASK_DELETE_EN > 0u)
/*******************************************************************************
* @Brief  : set task delete user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskDeleteFuncSet(KER_tpfnHookTaskDelete pfnFunc);

/*******************************************************************************
* @Brief  : clear task delete user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskDeleteFuncClr( void );
#endif /* End of #if KER_coHOOK_TASK_DELETE_EN > 0u                           */

#if (KER_coHOOK_TASK_SUSPEND_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*******************************************************************************
* @Brief  : set task suspend user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskSuspendFuncSet(KER_tpfnHookTaskSuspend pfnFunc);

/*******************************************************************************
* @Brief  : clear task suspend user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskSuspendFuncClr( void );
#endif /* End of #if KER_coHOOK_TASK_SUSPEND_EN > 0u                          */

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*******************************************************************************
* @Brief  : set task resume user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskResumeFuncSet(KER_tpfnHookTaskResume pfnFunc);

/*******************************************************************************
* @Brief  : clear task resume user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskResumeFuncClr( void );
#endif /* End of #if KER_coHOOK_TASK_RESUME_EN > 0u                           */

#if (KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u) && (KER_coTASK_PRIO_CHANGE_EN > 0u)
/*******************************************************************************
* @Brief  : set task priority change user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskPrioChangeFuncSet(
                                          KER_tpfnHookTaskPrioChange pfnFunc);

/*******************************************************************************
* @Brief  : clear task priority change user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskPrioChangeFuncClr( void );
#endif /* End of #if KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u                      */

#if (KER_coHOOK_TASK_YIELD_EN > 0u) && (KER_coTASK_YIELD_EN > 0u)
/*******************************************************************************
* @Brief  : set task yield user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskYieldFuncSet(KER_tpfnHookTaskYield pfnFunc);

/*******************************************************************************
* @Brief  : clear task yield user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTaskYieldFuncClr( void );
#endif /* End of #if KER_coHOOK_TASK_YIELD_EN > 0u                            */

#if KER_coHOOK_IDLE_EN > 0u
/*******************************************************************************
* @Brief  : set kernel idle user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookIdleFuncSet( KER_tpfnHookIdle pfnFunc );
/*******************************************************************************
* @Brief  : clear kernel idle user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookIdleFuncClr( void );
#endif /* Enf of #if KER_coHOOK_IDLE_EN > 0u                                  */

#if KER_coHOOK_TICK_EN > 0u
/*******************************************************************************
* @Brief  : set tick user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTickFuncSet( KER_tpfnHookTick pfnFunc );

/*******************************************************************************
* @Brief  : clear tick user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTickFuncClr( void );
#endif /* Enf of #if KER_coHOOK_TICK_EN > 0u                                  */

#if (KER_coHOOK_TIMER_EN > 0u) && ( KER_coTIMER_EN > 0u )
/*******************************************************************************
* @Brief  : set timer user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTimerFuncSet( KER_tpfnHookTimer pfnFunc );

/*******************************************************************************
* @Brief  : clear timer user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookTimerFuncClr( void );
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
extern void KER_vidHookStackCheckFuncSet( KER_tpfnHookStackCheck pfnFunc );

/*******************************************************************************
* @Brief  : clear stack check user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookStackCheckFuncClr( void );
#endif

#if (KER_coHOOK_STK_OVERFLOW_EN > 0u) && (KER_coTASK_STACK_LIMIT_EN > 0u)
/*******************************************************************************
* @Brief  : set stack check user hook function.
* @Inputs : pfnFunc: user hook function pointer.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookStackOverflowFuncSet(KER_tpfnHookStackOverflow pfnFunc);

/*******************************************************************************
* @Brief  : clear stack check user hook function.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : none.
*******************************************************************************/
extern void KER_vidHookStackOverflowFuncClr( void );
#endif /* End of #if KER_coHOOK_STK_OVERFLOW_EN > 0u                          */

#endif /* End of #if KER_coHOOK_EN > 0u                                       */

#ifdef __cplusplus
}
#endif
#endif
