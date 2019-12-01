;*******************************************************************************
;*                   Copyright(c) 2015,Shiwei.LI
;*                     All rights reserved.
;*File   Name: arch_a.s
;*Version    : V1.00
;*Description: CPU architecture porting related assember code.
;*Author     : Shiwei.LI
;*E-Mail     : cfixsoft@163.com
;*Create Date: 2015/8/26
;*******************************************************************************
NVIC_INT_CTRL   EQU     0xE000ED04
NVIC_SYSPRI14   EQU     0xE000ED22
NVIC_PENDSV_PRI EQU           0xFF
NVIC_PENDSVSET  EQU     0x10000000


    EXPORT    CPU_vidDisableGlobalInterrupt
    EXPORT    CPU_vidEnableGlobalInterrupt
    EXPORT    CPU_wEnterCritical
    EXPORT    CPU_vidLeaveCritical
    EXPORT    CPU_vidStartHighRdyTask
    EXPORT    CPU_u32CntTailZeros
    EXPORT    PendSV_Handler
    IMPORT    __vector_table
    IMPORT    KER_pstrCurTCB
    IMPORT    KER_pstrNextTCB
    IMPORT    KER_vidTaskSwCtxCallBack

    RSEG arch_asm_code_segment:CODE:NOROOT(2)
    THUMB

;*******************************************************************************
;* @Brief  : Disable global interrupt
;* @Inputs : none
;* @Outputs: none
;* @Returns: none
;* @Note   : none
;*******************************************************************************
CPU_vidDisableGlobalInterrupt
    CPSID   I
    BX      LR

;*******************************************************************************
;* @Brief  : Enable global interrupt
;* @Inputs : none
;* @Outputs: none
;* @Returns: none
;* @Note   : none
;*******************************************************************************
CPU_vidEnableGlobalInterrupt
    CPSIE   I
    BX      LR

;*******************************************************************************
;* @Brief  : Kernel enter critical section, disable global interrupt
;* @Inputs : none
;* @Outputs: none
;* @Returns: CPU interrupt mask status CPU_REG
;* @Note   : none
;*******************************************************************************
CPU_wEnterCritical
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

;*******************************************************************************
;* @Brief  : Kernel exit critical section, restore last interrupt mask status
;* @Inputs : last enter critical saved cpu interrupt mask status CPU_REG
;* @Outputs: none
;* @Returns: none
;* @Note   : none
;*******************************************************************************
CPU_vidLeaveCritical
    MSR     PRIMASK, R0
    BX      LR

;*******************************************************************************
;* @Brief  : counter tail zeros
;* @Inputs : R0: 32bits unsigned int input
;* @Outputs: none
;* @Returns: number of 32bits unsigned int
;* @Note   : none
;*******************************************************************************
CPU_u32CntTailZeros
    RBIT R0, R0
    CLZ  R0, R0
    BX   LR

;*******************************************************************************
;* @Brief  : kernel start, trigger switch to highest priority task.
;* @Inputs : none
;* @Outputs: none
;* @Returns: none
;* @Note   : none
;*******************************************************************************
CPU_vidStartHighRdyTask
    LDR     R0, =NVIC_SYSPRI14
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVS    R0, #0
    MSR     PSP, R0

    LDR     R0, =__vector_table
    LDR     R1, [R0]
    MSR     MSP, R1

    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I
;should never execution next instruction.
__DeadLoop
    B       __DeadLoop

PendSV_Handler
    CPSID   I
    MRS     R0, PSP
    CBZ     R0, __PendSVHandler_nosave

    SUBS    R0, R0, #0x20
    STM     R0, {R4-R11}

    LDR     R1, =KER_pstrCurTCB
    LDR     R1, [R1]
    STR     R0, [R1]


__PendSVHandler_nosave
    PUSH    {LR}
    LDR     R0, =KER_vidTaskSwCtxCallBack
    BLX     R0
    POP     {LR}

    LDR     R0, =KER_pstrCurTCB
    LDR     R1, =KER_pstrNextTCB
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]
    LDM     R0, {R4-R11}
    ADDS    R0, R0, #0x20
    MSR     PSP, R0
    ORR     LR, LR, #0x04
    CPSIE   I
    BX      LR

    END