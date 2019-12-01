/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: arch.h
*Version    : V1.00
*Description: export CPU architecture porting related interfaces.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/26
*******************************************************************************/
#ifndef  ARCH_H
#define  ARCH_H
#ifdef __cplusplus
extern "C" {
#endif

#include <KER_Def.h>
#include <KER_Config.h>

extern CPU_WORD CPU_wEnterCritical(void);
extern void CPU_vidLeaveCritical(CPU_WORD cpu_sr);
extern void CPU_vidStartHighRdyTask(void);
extern CPU_WORD CPU_u32CntTailZeros(CPU_WORD n);
#ifndef  NVIC_INT_CTRL
    #define  NVIC_INT_CTRL                           *((CPU_WORD *)0xE000ED04)
#endif

#ifndef  NVIC_PENDSVSET
    #define  NVIC_PENDSVSET                             (CPU_WORD)(0x10000000)
#endif

#define KER_TASK_SW()                           NVIC_INT_CTRL = NVIC_PENDSVSET
#define KER_INTR_SW()                           NVIC_INT_CTRL = NVIC_PENDSVSET
#define KER_CPU_SR_ALLOC()                      CPU_REG CPU_WORD cpu_sr
#define KER_ENTER_CRITICAL()                   (cpu_sr = CPU_wEnterCritical())
#define KER_LEAVE_CRITICAL()                    CPU_vidLeaveCritical(cpu_sr)
#define KER_vidStartHighRdyTask()               CPU_vidStartHighRdyTask()
#define KER_wCntTailZeros(n)                    CPU_u32CntTailZeros((n))

/*******************************************************************************
* @Brief  : Initial task stack
* @Inputs : pstkStack: before initial stack top pointer.
*           pfEntry: task entry pointer.
*           vidpParam: task entry parameter pointer.
* @Outputs: none
* @Returns: after initial stack top pointer.
* @Note   : only called by create task function.
*******************************************************************************/
extern CPU_STK *KER_pstkInitTaskStack ( CPU_STK    *pstkStack,
                                        void      (*pfnEntry)(void *vidpParam),
                                        void       *vidpParam );

#if KER_coCPULOAD_MODULE_EN > 0u
/*******************************************************************************
* @Brief  : get CPU Load calculation timer current counter.
* @Inputs : none.
* @Outputs: none.
* @Returns: timer current counter.
* @Note   : none.
*******************************************************************************/
extern INT32U KER_u32CPULoadTimerCounterGet( void );
#endif

#ifdef __cplusplus
}
#endif
#endif
