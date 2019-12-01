/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Exception.c
*Version    : V1.00
*Description: MRTOS kernel running exception handler module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/28
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

#if KER_coEXCEPTION_EN > 0u
/*******************************************************************************
* @Brief  : raise kernel running exception
* @Inputs : ps8File: source file name string.
*           u32Line: exception line.
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidRaiseException( INT8S *ps8File,
                                   INT32U u32Line )
{

#if (KER_coHOOK_EN > 0u) && (KER_coHOOK_EXCEPTION_EN > 0u)

    KER_CPU_SR_ALLOC();

    KER_ENTER_CRITICAL();
    if (KER_NULL != KER_pfnHookException)
    {
        /*!Pseudo: call user hook function                                    */
        KER_pfnHookException(ps8File, u32Line);
    }
    KER_LEAVE_CRITICAL();

#endif

}
#endif

