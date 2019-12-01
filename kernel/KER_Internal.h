/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Internal.h
*Version    : V1.00
*Description: MRTOS kernel internal using interfaces header file.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/25
*******************************************************************************/
#ifndef  KER_INTERNAL_H
#define  KER_INTERNAL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <KER_Def.h>
#include <KER_Config.h>
#include <MRTOS.h>
#include <arch.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*!Comment: for stack check, define stack token length                        */
#define KER_u8STACK_TOKEN_LENGTH                                    ((INT8U)8)

/*!Comment: stack token union type definition                                 */
typedef union ker_stack_token
{
    /*!Comment: RW access stack token by byte unit                            */
    INT8U   au8Token[KER_u8STACK_TOKEN_LENGTH];
    /*!Comment: RW access stack token by CPU_STK type unit                    */
    CPU_STK astkToken[KER_u8STACK_TOKEN_LENGTH / sizeof(CPU_STK)];
}KER_tuniStackToken;

/*******************************************************************************
*!Comment: list operation macros by pass pointer
*******************************************************************************/
/*!Comment: initial a list by List pointer                                    */
#define LIST_INIT_PTR(pstrList) do { \
                                 (pstrList)->pstrPrev = (pstrList); \
                                 (pstrList)->pstrNext = (pstrList); \
                                }while (0)

/*!Comment: initial node, pointer to itself                                   */
#define LIST_NODE_INIT_PTR(pstrNode) do { \
                                      (pstrNode)->pstrPrev = (pstrNode); \
                                      (pstrNode)->pstrNext = (pstrNode); \
                                     } while (0)

/*!Comment: check if list is empty by head pointer                            */
#define LIST_IS_EMPTY_PTR(pstrList) ((pstrList)->pstrPrev == (pstrList))

/*!Comment: merge list2 to list1 tail                                         */
#define LIST_MERGE_PTR(pstrList1, pstrList2) do { \
                    (pstrList1)->pstrPrev->pstrNext = (pstrList2)->pstrNext; \
                    (pstrList2)->pstrNext->pstrPrev = (pstrList1)->pstrPrev; \
                    (pstrList1)->pstrPrev = (pstrList2)->pstrPrev; \
                    (pstrList2)->pstrPrev->pstrNext = (pstrList1); \
                    (pstrList2)->pstrPrev = (pstrList2); \
                    (pstrList2)->pstrNext = (pstrList2); \
                    }while (0)

/*!Comment: get node entry pointer                                            */
#define LIST_NODE_ENTRY_PTR(pstrNode, Type, Member) \
           ((Type *)((INT8U *)(pstrNode) - (CPU_ADDR)(&((Type *)0)->Member)))

/*!Comment: get list head node pointer                                        */
#define LIST_GET_HEAD_PTR(pstrList) ((pstrList)->pstrNext)

/*!Comment: get list tail node pointer                                        */
#define LIST_GET_TAIL_PTR(pstrList) ((pstrList)->pstrPrev)

/*!Comment: insert a node to a list by pointer                                */
#define LIST_INSERT_HEAD_PTR(pstrList, pstrNode) do { \
                               (pstrNode)->pstrPrev = (pstrList); \
                               (pstrNode)->pstrNext = (pstrList)->pstrNext; \
                               (pstrList)->pstrNext->pstrPrev = (pstrNode); \
                               (pstrList)->pstrNext = (pstrNode); \
                               }while (0)

/*!Comment: insert a node to a list tail by pointer                           */
#define LIST_INSERT_TAIL_PTR(pstrList, pstrNode) do { \
                               (pstrNode)->pstrNext = (pstrList); \
                               (pstrNode)->pstrPrev = (pstrList)->pstrPrev; \
                               (pstrList)->pstrPrev->pstrNext = (pstrNode); \
                               (pstrList)->pstrPrev = (pstrNode); \
                               }while (0)

/*!Comment: insert a node before another node by pointer                      */
#define LIST_INSERT_BEFORE_PTR(pstrAnother, pstrNode) do { \
                              (pstrNode)->pstrNext = (pstrAnother); \
                              (pstrNode)->pstrPrev = (pstrAnother)->pstrPrev; \
                              (pstrAnother)->pstrPrev->pstrNext = (pstrNode); \
                              (pstrAnother)->pstrPrev = (pstrNode); \
                              }while (0)

/*!Comment: insert a node after another node by pointer                       */
#define LIST_INSERT_AFTER_PTR(pstrAnother, pstrNode) do { \
                             (pstrNode)->pstrPrev = (pstrAnother); \
                             (pstrNode)->pstrNext = (pstrAnother)->pstrNext; \
                             (pstrAnother)->pstrNext->pstrPrev = (pstrNode); \
                             (pstrAnother)->pstrNext = (pstrNode); \
                             }while (0)

/*!Comment: remove a node from list                                           */
#define LIST_REMOVE_NODE_PTR(pstrNode) do { \
                    ((pstrNode)->pstrPrev->pstrNext = (pstrNode)->pstrNext); \
                    ((pstrNode)->pstrNext->pstrPrev = (pstrNode)->pstrPrev); \
                    (pstrNode)->pstrPrev = (pstrNode); \
                    (pstrNode)->pstrNext = (pstrNode); \
                    }while (0)


/*******************************************************************************
*!Comment: list operation macros by pass instance
*******************************************************************************/

/*!Comment: initial a list by list instance                                   */
#define LIST_INIT(strList) do { \
                            (strList).pstrPrev = &(strList); \
                            (strList).pstrNext = &(strList); \
                           }while (0)

/*!Comment: initial node, pointer to itself                                   */
#define LIST_NODE_INIT(strNode) do { \
                                 (strNode).pstrPrev = &(strNode); \
                                 (strNode).pstrNext = &(strNode); \
                                }while (0)

/*!Comment: check if list is empty by head instance                           */
#define LIST_IS_EMPTY(strList) ((strList).pstrPrev == &(strList))

/*!Comment: merge list2 to list1 tail                                         */
#define LIST_MERGE(strList1, strList2) do { \
                       (strList1).pstrPrev->pstrNext = (strList2).pstrNext; \
                       (strList2).pstrNext->pstrPrev = (strList1).pstrPrev; \
                       (strList1).pstrPrev = (strList2).pstrPrev; \
                       (strList2).pstrPrev->pstrNext = &(strList1); \
                       (strList2).pstrPrev = &(strList2); \
                       (strList2).pstrNext = &(strList2); \
                       }while (0)

/*!Comment: get node entry pointer                                            */
#define LIST_NODE_ENTRY(strNode, Type, Member) \
           ((Type *)((INT8U *)(&(strNode)) - (CPU_ADDR)(&((Type *)0)->Member)))

/*!Comment: get list head node pointer                                        */
#define LIST_GET_HEAD(strList) ((strList).pstrNext)

/*!Comment: get list tail node pointer                                        */
#define LIST_GET_TAIL(strList) ((strList).pstrPrev)

/*!Comment: insert a node to a list head by instance                          */
#define LIST_INSERT_HEAD(strList, strNode) do { \
                             (strNode).pstrPrev = &(strList); \
                             (strNode).pstrNext =  (strList).pstrNext; \
                             (strList).pstrNext->pstrPrev = &(strNode); \
                             (strList).pstrNext = &(strNode); \
                             }while (0)

/*!Comment: insert a node to a list tail by instance                          */
#define LIST_INSERT_TAIL(strList, strNode) do { \
                             (strNode).pstrNext = &(strList); \
                             (strNode).pstrPrev = (strList).pstrPrev; \
                             (strList).pstrPrev->pstrNext = &(strNode); \
                             (strList).pstrPrev = &(strNode); \
                             }while (0)

/*!Comment: insert a node before another node by instance                     */
#define LIST_INSERT_BEFORE(strAnother, strNode) do { \
                             (strNode).pstrNext = &(strAnother); \
                             (strNode).pstrPrev = (strAnother).pstrPrev; \
                             (strAnother).pstrPrev->pstrNext = &(strNode); \
                             (strAnother).pstrPrev = &(strNode); \
                             }while (0)

/*!Comment: insert a node after another node by instance                      */
#define LIST_INSERT_AFTER(strAnother, strNode) do { \
                             (strNode).pstrPrev = &(strAnother); \
                             (strNode).pstrNext =  (strAnother).pstrNext; \
                             (strAnother).pstrNext->pstrPrev = &(strNode); \
                             (strAnother).pstrNext = &(strNode); \
                             }while (0)

/*!Comment: remove a node from list                                           */
#define LIST_REMOVE_NODE(strNode) do { \
                        ((strNode).pstrPrev->pstrNext = (strNode).pstrNext); \
                        ((strNode).pstrNext->pstrPrev = (strNode).pstrPrev); \
                        (strNode).pstrPrev = &(strNode); \
                        (strNode).pstrNext = &(strNode); \
                        }while (0)

/*!Comment: check ipc methdo is valid                                         */
#define IPC_METHOD_IS_VALID(enuIPCMethod) \
                                 (((enuIPCMethod) == KER_enuIPC_METHOD_LIFO) \
                                ||((enuIPCMethod) == KER_enuIPC_METHOD_FIFO) \
                                ||((enuIPCMethod) == KER_enuIPC_METHOD_PRIO))

/*!Comment: check message queue recv method is valid                          */
#define MSG_Q_RECV_METHOD_IS_VALID(enuRecvMethod) \
                            (((enuRecvMethod) == KER_enuMSGQ_RECV_OPT_FRONT) \
                            ||((enuRecvMethod) == KER_enuMSGQ_RECV_OPT_REAR))

/*!Comment: check message queue send method is valid                          */
#define MSG_Q_SEND_METHOD_IS_VALID(enuSendMethod) \
                           (((enuSendMethod) == KER_enuMSGQ_SEND_OPT_FRONT) \
                           ||((enuSendMethod) == KER_enuMSGQ_SEND_OPT_REAR))

/*!Comment: check timer status is valid                                       */
#define TIMER_STATUS_IS_VALID(enuStatus) \
                               (((enuStatus) == KER_enuTIMER_STATUS_RUNNING) \
                              ||((enuStatus) == KER_enuTIMER_STATUS_STOP) \
                              ||((enuStatus) == KER_enuTIMER_STATUS_COMPLETED))

/*!Comment: check timer running method is valid                               */
#define TIMER_MODE_IS_VALID(enuMode) \
                                (((enuMode) == KER_enuTIMER_MODE_ONE_SHOT) \
                               ||((enuMode) == KER_enuTIMER_MODE_PERIODIC))

/*!Comment: cpu ram access low address align                                  */
#define KER_ADDR_LO_ALIGN(Addr) (((CPU_ADDR)(Addr)) \
                              & (~((CPU_ADDR)(RAM_ALIGN_SIZE - 1u))))

/*!Comment: cpu ram access high address align                                 */
#define KER_ADDR_HI_ALIGN(Addr) (((CPU_ADDR)(Addr) + RAM_ALIGN_SIZE - 1u) \
                              & (~((CPU_ADDR)(RAM_ALIGN_SIZE - 1u))))

/*!Comment: low align                                                         */
#define KER_LO_ALIGN(Addr) (((CPU_ADDR)(Addr)) \
                        & (~((CPU_ADDR)(RAM_ALIGN_SIZE - 1u))))
/*!Comment: high align                                                        */
#define KER_HI_ALIGN(Addr) (((CPU_ADDR)(Addr) + RAM_ALIGN_SIZE - 1u) \
                        & (~((CPU_ADDR)(RAM_ALIGN_SIZE - 1u))))

/*!Comment: avoid not used argument warning macro                             */
#define KER_NOT_USED_ARGUMENT(arg) ((arg) = (arg))

/*******************************************************************************
*!Comment: export kernel data to other kernel *.c files reference
*******************************************************************************/
#if KER_coCPU_CLZ_INSTRUCTION_EN == 0u
/*!Comment: priority bit map table                                            */
extern const INT8U KER_kau8PrioBitMap[256];
#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))
/*!Comment: stack token                                                       */
extern const KER_tuniStackToken KER_kuniStackToken;
#endif

#if KER_coCPULOAD_MODULE_EN > 0u
/*!Comment: CPU Load calcualte calculation variables definitions              */
/*!Comment: CPU Load (Unit:00.1%, Range: 0.0%~100.0%)                         */
extern INT32U  KER_u32CPULoad;
extern INT32U  KER_u32CPULoadISR;
/*!Comment: task CPU performance time counter                                 */
extern INT32U KER_u32CPUUsingCnt;
/*!Comment: ISR CPU using counter                                             */
extern INT32U KER_u32CPUISRCnt;
/*!Comment: CPU idle time counter                                             */
extern INT32U KER_u32CPUIdleCnt;
/*!Comment: interrupt nested level for CPU load calculate                     */
extern INT8U KER_u8CPULoadIntrNestedCnt;

/*!Comment: CPU time last counter record                                      */
#if KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_DECREASE
extern INT32U KER_u32CPULoadTimerLastCnt;
#elif KER_coCPULOAD_TIM_CNT_DIR == KER_coCPULOAD_TIM_CNT_INCREASE
extern INT32U KER_u32CPULoadTimerLastCnt;
#else
#error "Please configurate CPULoad timer counter direction!!!"
#endif

/*!Comment: calculate CPU Load period                                         */
extern INT32U KER_u32CPULoadCalcPeriod;

#endif


/*!Comment: cpu idle counter                                                  */
extern volatile INT32U KER_u32IdleCnt;

/*!Comment: idle task stack memory space                                      */
extern CPU_STK KER_astkIdleTaskStack[KER_stkszIDLE_TASK_STKSZ/sizeof(CPU_STK)];

/*!Comment: idle task tcb                                                     */
extern KER_tstrTCB  KER_strIdleTaskTcb;

/*!Comment: number of tasks                                                   */
extern volatile INT32U KER_u32TaskCnt;

/*!Comment: ticks counter                                                     */
extern volatile INT32U KER_u32TickCnt;

/*!Comment: interrupt nesting counter                                         */
extern volatile INT8U  KER_u8IntrNestCnt;

/*!Comment: scheduler lock nesting counter                                    */
extern volatile INT8U  KER_u8SchedLockNestCnt;

/*!Comment: kernel running flag                                               */
extern volatile BOOL KER_bKernelRunning;

/*!Comment: current running task tcb pointer                                  */
extern KER_tstrTCB *KER_pstrCurTCB;

/*!Comment: hightest priority ready task tcb pointer                          */
extern KER_tstrTCB *KER_pstrNextTCB;

/*!Comment: task ready state bit map                                          */
extern volatile CPU_WORD KER_awTaskRdyBitMap[KER_u16TASK_RDY_BITMAP_SIZE];

/*!Comment: task ready list                                                   */
extern KER_tstrList KER_astrTaskRdyList[KER_u16TASK_PRIO_MAX];

/*!Comment: task TCB link list                                                */
extern KER_tstrList KER_strTCBLinkList;

/*!Comment: tick wheel list                                                   */
extern KER_tstrList KER_astrTickWheelList[KER_u16TICK_WHEEL_SIZE];

#if KER_coTICK_TASK_EN > 0u

/*!Comment: tick task stack memory space                                      */
extern CPU_STK KER_astkTickTaskStack[KER_stkszTICK_TASK_STKSZ/sizeof(CPU_STK)];


/*!Comment: tick task tcb                                                     */
extern KER_tstrTCB  KER_strTickTaskTcb;

#endif

#if KER_coTIMER_EN > 0u

/*!Comment: timer tick wheel list                                             */
extern KER_tstrList KER_astrTimerWheelList[KER_u16TIMER_WHEEL_SIZE];

/*!Comment: timer tick counter                                                */
extern INT32U KER_u32TimerCnt;

#endif

#if (KER_coTIMER_EN > 0u) && (KER_coTIMER_TASK_EN > 0u)

/*!Comment: timer task stack memory space                                     */
extern CPU_STK KER_astkTimerTaskStack[KER_stkszTIMER_TASK_STKSZ
                                                            /sizeof(CPU_STK)];

/*!Comment: timer task tcb                                                    */
extern KER_tstrTCB  KER_strTimerTaskTcb;

#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))
/*!Comment: stack check task stack memory space                               */
extern CPU_STK KER_astkStackChkTaskStack[KER_stkszSTACK_CHECK_TASK_STKSZ
                                                            /sizeof(CPU_STK)];

/*!Comment: stack check task tcb                                              */
extern KER_tstrTCB  KER_strStackChkTaskTcb;

#endif

#if KER_coHOOK_EN > 0u
/*******************************************************************************
*!Comment: user hook function pointer list
*******************************************************************************/
#if KER_coHOOK_TASK_CTX_SW_EN > 0u

#if (KER_coHOOK_EXCEPTION_EN > 0u) && (KER_coEXCEPTION_EN > 0u)
/*!Comment: kernel running exception hook function pointer                    */
extern KER_tpfnHookException KER_pfnHookException;
#endif

/*!Comment: task switch context hook function pointer                         */
extern KER_tpfnHookTaskSwCtx  KER_pfnHookTaskSwCtx;
#endif

#if KER_coHOOK_TASK_INIT_EN > 0u
/*!Comment: initial task hook function pointer                                */
extern KER_tpfnHookTaskInit KER_pfnHookTaskInit;
#endif

#if (KER_coHOOK_TASK_DELETE_EN > 0u) && (KER_coTASK_DELETE_EN > 0u)
/*!Comment: task delete hook function pointer                                 */
extern KER_tpfnHookTaskDelete KER_pfnHookTaskDelete;
#endif

#if (KER_coHOOK_TASK_SUSPEND_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*!Comment: task suspend hook function pointer                                */
extern KER_tpfnHookTaskSuspend KER_pfnHookTaskSuspend;
#endif

#if (KER_coHOOK_TASK_RESUME_EN > 0u) && (KER_coTASK_SUSPEND_EN > 0u)
/*!Comment: task resume hook function pointer                                 */
extern KER_tpfnHookTaskResume KER_pfnHookTaskResume;
#endif

#if (KER_coHOOK_TASK_PRIO_CHANGE_EN > 0u) && (KER_coTASK_PRIO_CHANGE_EN > 0u)
/*!Comment: task priority change hook function type definition                */
extern KER_tpfnHookTaskPrioChange KER_pfnHookTaskPrioChange;
#endif

#if (KER_coHOOK_TASK_YIELD_EN > 0u) && (KER_coTASK_YIELD_EN > 0u)
/*!Comment: task yield hook function type definition                          */
extern KER_tpfnHookTaskYield KER_pfnHookTaskYield;
#endif

#if KER_coHOOK_IDLE_EN > 0u
/*!Comment: kernel idle hook function pointer                                 */
extern KER_tpfnHookIdle KER_pfnHookIdle;
#endif

#if KER_coHOOK_TICK_EN > 0u
/*!Comment: tick hook function pointer                                        */
extern KER_tpfnHookTick KER_pfnHookTick;
#endif

#if (KER_coHOOK_TIMER_EN > 0u) && ( KER_coTIMER_EN > 0u )
/*!Comment: timer hook function pointer                                       */
extern KER_tpfnHookTimer KER_pfnHookTimer;
#endif

#if (KER_coHOOK_STKCHK_EN > 0u) \
  && ((KER_coTASK_STACK_LIMIT_EN > 0u) || (KER_coTASK_STACK_USAGE_EN > 0u))
/*!Comment: stack check hook function pointer                                 */
extern KER_tpfnHookStackCheck KER_pfnHookStackCheck;
#endif

#if (KER_coHOOK_STK_OVERFLOW_EN > 0u) && (KER_coTASK_STACK_LIMIT_EN > 0u)
/*!Comment: stack overflow hook function pointer                              */
extern KER_tpfnHookStackOverflow KER_pfnHookStackOverflow;
#endif

#endif /* End of #if KER_coHOOK_EN > 0u                                       */

/*******************************************************************************
*!Comment: export kernel internal interfaces for kernel reference.
*******************************************************************************/

/*******************************************************************************
*!Comment: KER_Pend.c kernel internal interfaces
*******************************************************************************/
/*******************************************************************************
* @Brief  : insert tcb to mutex pending list.
* @Inputs : pstrList: pend list pointer.
*           pstrTCB: tcb pointer.
*           enuIPCMethod: ipc pending list schedule strategy method.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidPendListInsert( KER_tstrList *pstrList,
                                   KER_tstrTCB *pstrTCB,
                                   KER_tenuIPCMethod enuIPCMethod);

/*******************************************************************************
* @Brief  : remove pend list all node.
* @Inputs : pstrList: list pointer.
*           enuError: set pending task error flag.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidPendListRemoveAll( KER_tstrList *pstrList,
                                      KER_tenuERR   enuError);

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
extern void KER_vidObjectPend(void *vidpObj,
                              KER_tenuIPCObjType enuObjType,
                              KER_tenuIPCMethod enuIPCMethod,
                              INT32U u32TimeOut);

/*******************************************************************************
* @Brief  : pend built in object
* @Inputs : enuObjType: pending built in object type.
*           u32TimeOut: pending timeout value.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidBuiltInObjectPend( KER_tenuIPCObjType enuObjType,
                                      INT32U u32TimeOut);

/*******************************************************************************
* @Brief  : abort pend object
* @Inputs : pstrTCB: task tcb pointer.
*           enuErrorFlag: error flag.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidObjectPendAbort(KER_tstrTCB *pstrTCB,
                                   KER_tenuERR enuErrorFlag);

/*******************************************************************************
* @Brief  : abort pend built in object
* @Inputs : pstrTCB: task tcb pointer.
*           enuErrorFlag: error flag.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidBuiltInObjectPendAbort(KER_tstrTCB *pstrTCB,
                                          KER_tenuERR enuErrorFlag);

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
extern KER_tenuERR KER_enuTaskBuiltInObjPendAbort(
                                          KER_tstrTCB   *pstrTCB,
                                          KER_tenuIPCObjType enuBuiltInObjType,
                                          KER_tenuSchedOpt enuOption);



/*******************************************************************************
*!Comment: KER_Task.c kernel internal interfaces
*******************************************************************************/
/*******************************************************************************
* @Brief  : Task default exit execution.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : should be only called by kernel, user can't call it.
*******************************************************************************/
extern void KER_vidTaskDefaultExit( void );

/*******************************************************************************
*!Comment: KER_Sched.c kernel internal interfaces
*******************************************************************************/
/*******************************************************************************
* @Brief  : insert tcb to ready list tail.
* @Inputs : pstrTCB: tcb pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidRdyListInsertToTail( KER_tstrTCB *pstrTCB );

/*******************************************************************************
* @Brief  : insert tcb to ready list head.
* @Inputs : pstrTCB: tcb pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidRdyListInsertToHead( KER_tstrTCB *pstrTCB );

/*******************************************************************************
* @Brief  : remove a tcb node from a ready list.
* @Inputs : pstrTCB: tcb pointer
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidRdyListRemove(KER_tstrTCB *pstrTCB);

/*******************************************************************************
* @Brief  : task scheduler
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidSched( void );

#if KER_coSLICE_SCHED_EN > 0u
/*******************************************************************************
* @Brief  : slice schedule.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidSliceSched( void );
#endif

/*******************************************************************************
*!Comment: KER_Tick.c kernel internal interfaces
*******************************************************************************/
#if KER_coTICK_TASK_EN > 0u
/*******************************************************************************
* @Brief  : initial tick task
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidTickTaskInit( void );
#endif

/*******************************************************************************
* @Brief  : remove tcb from tick wheel list.
* @Inputs : pstrTCB: tcb pointer
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidTickListRemove( KER_tstrTCB *pstrTCB );

/*******************************************************************************
* @Brief  : insert tcb to tick wheel list.
* @Inputs : pstrTCB: tcb pointer
*           u32Ticks: delay ticks
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidTickListInsert(KER_tstrTCB *pstrTCB, INT32U u32Ticks );

/*******************************************************************************
*!Comment: KER_Priority.c kernel internal interfaces
*******************************************************************************/

/*******************************************************************************
* @Brief  : Set ready state bit
* @Inputs : u16Priority
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidSetRdyBitMap( INT16U u16Priority );

/*******************************************************************************
* @Brief  : Clear ready state bit
* @Inputs : u16Priority
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidClrRdyBitMap( INT16U u16Priority );

/*******************************************************************************
* @Brief  : Get highest priority ready task
* @Inputs : none
* @Outputs: none
* @Returns: Highest priority
* @Note   : none
*******************************************************************************/
extern INT16U KER_u16GetRdyHighestPriority( void );

/*******************************************************************************
*!Comment: KER_Idle.c kernel internal interfaces
*******************************************************************************/
/*******************************************************************************
* @Brief  : create kernel idle task.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidIdleTaskInit(void);

/*******************************************************************************
*!Comment: KER_Exception.c kernel internal interfaces
*******************************************************************************/

#if KER_coEXCEPTION_EN > 0u
/*******************************************************************************
* @Brief  : raise kernel running exception
* @Inputs : ps8File: source file name string.
*           u32Line: exception line.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidRaiseException( INT8S *ps8File,
                                   INT32U u32Line );
#else
    #define KER_vidRaiseException(ps8File, u32Line)
#endif

/*******************************************************************************
*!Comment: KER_Timer.c kernel internal interfaces
*******************************************************************************/
#if KER_coTIMER_EN > 0u
/*******************************************************************************
* @Brief  : initial timer task
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidTimerTaskInit( void );
#endif



/*******************************************************************************
*!Comment: KER_MsgPool.c kernel internal interfaces
*******************************************************************************/
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
extern void KER_vidMsgPoolInit(KER_tstrMsgPool *pstrMsgPoolObj,
                               void    *vidpMsgPool,
                               INT32U   u32MsgPoolSize,
                               INT16U   u16MsgSize);

/*******************************************************************************
* @Brief  : message copy
* @Inputs : vidpDst: destination message buffer pointer.
*           vidpSrc: source message buffer pointer.
*           u16Size: copy size.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidMsgPoolMsgCopy( void *vidpDst,
                                   void *vidpSrc,
                                   INT16U u16Size);

/*******************************************************************************
* @Brief  : query number of messages saved in message pool.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: number of messages saved in message pool.
* @Note   : none
*******************************************************************************/
extern INT16U KER_u16MsgPoolMsgNbrQuery( KER_tstrMsgPool *pstrMsgPoolObj );

/*******************************************************************************
* @Brief  : check message pool is empty.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: TRUE: is empty.
*           FALSE: not empty.
* @Note   : none
*******************************************************************************/
extern BOOL KER_bMsgPoolIsEmpty( KER_tstrMsgPool *pstrMsgPoolObj );

/*******************************************************************************
* @Brief  : check message pool is full.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: TRUE: is full.
*           FALSE: not full.
* @Note   : none
*******************************************************************************/
extern BOOL KER_bMsgPoolIsFull( KER_tstrMsgPool *pstrMsgPoolObj );

/*******************************************************************************
* @Brief  : get message pool one message buffer size.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: messge size.
* @Note   : none
*******************************************************************************/
extern INT16U KER_u16MsgPoolMsgSizeGet( KER_tstrMsgPool *pstrMsgPoolObj );

/*******************************************************************************
* @Brief  : get maximum number of messages can be saved in pool.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: maximum messages number.
* @Note   : none
*******************************************************************************/
extern INT16U KER_u16MsgPoolMsgsMaxGet( KER_tstrMsgPool *pstrMsgPoolObj );

/*******************************************************************************
* @Brief  : flush message pool.
* @Inputs : pstrMsgPoolObj: message pool object pointer.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidMsgPoolFlush( KER_tstrMsgPool *pstrMsgPoolObj );

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
extern KER_tenuERR KER_enuMsgPoolRead(KER_tstrMsgPool *pstrMsgPoolObj,
                                      void            *vidpMsg,
                                      INT16U          *pu16Size,
                                      KER_tenuMsgQRecvOpt  enuRecvMethod);

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
extern KER_tenuERR KER_enuMsgPoolWrite(KER_tstrMsgPool *pstrMsgPoolObj,
                                       void            *vidpMsg,
                                       INT16U           u16Size,
                                       KER_tenuMsgQSendOpt  enuSendMethod);

#endif

#if ((KER_coTASK_STACK_LIMIT_EN > 0u) \
  || (KER_coTASK_STACK_USAGE_EN > 0u))
/*******************************************************************************
* @Brief  : initial stack check task.
* @Inputs : none
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
extern void KER_vidStackCheckTaskInit( void );
#endif

#if KER_coCPULOAD_MODULE_EN > 0u
/*******************************************************************************
* @Brief  : update CPU using related counter.
* @Inputs : none.
* @Outputs: none.
* @Returns: none.
* @Note   : Only could be called by Task context switch procedure.
*******************************************************************************/
extern void KER_vidCPULoadPerformance( void );
#endif

#ifdef __cplusplus
}
#endif
#endif
