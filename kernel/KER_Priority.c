/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Priority.c
*Version    : V1.00
*Description: task priority manager module.
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/25
*******************************************************************************/
#include <KER_Internal.h>

#if KER_MAGIC_NUMBER != 0x12345678
    #error "Magic number not match error!!!"
#endif

/*******************************************************************************
* @Brief  : Set ready state bit
* @Inputs : u16Priority
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidSetRdyBitMap( INT16U u16Priority )
{

#if (KER_u8CPU_WORD_BITS == 8u)

    KER_awTaskRdyBitMap[u16Priority >> 3]
                         |= (((CPU_WORD)1) << (u16Priority & ((INT16U)0x07)));

#elif (KER_u8CPU_WORD_BITS == 16u)

    KER_awTaskRdyBitMap[u16Priority >> 4]
                         |= (((CPU_WORD)1) << (u16Priority & ((INT16U)0x0F)));

#elif (KER_u8CPU_WORD_BITS == 32u)

    KER_awTaskRdyBitMap[u16Priority >> 5]
                         |= (((CPU_WORD)1) << (u16Priority & ((INT16U)0x1F)));

#elif (KER_u8CPU_WORD_BITS == 64u)

    KER_awTaskRdyBitMap[u16Priority >> 6]
                         |= (((CPU_WORD)1) << (u16Priority & ((INT16U)0x3F)));

#else
    #error "Not supported cpu word bits! Please check KER_u8CPU_WORD_BITS."
#endif
}

/*******************************************************************************
* @Brief  : Clear ready state bit
* @Inputs : u16Priority
* @Outputs: none
* @Returns: none
* @Note   : none
*******************************************************************************/
PUBLIC void KER_vidClrRdyBitMap( INT16U u16Priority )
{

#if (KER_u8CPU_WORD_BITS == 8u)

    KER_awTaskRdyBitMap[u16Priority >> 3]
                         &= ~(((CPU_WORD)1) << (u16Priority & ((INT16U)0x07)));

#elif (KER_u8CPU_WORD_BITS == 16u)

    KER_awTaskRdyBitMap[u16Priority >> 4]
                         &= ~(((CPU_WORD)1) << (u16Priority & ((INT16U)0x0F)));

#elif (KER_u8CPU_WORD_BITS == 32u)

    KER_awTaskRdyBitMap[u16Priority >> 5]
                         &= ~(((CPU_WORD)1) << (u16Priority & ((INT16U)0x1F)));

#elif (KER_u8CPU_WORD_BITS == 64u)

    KER_awTaskRdyBitMap[u16Priority >> 6]
                         &= ~(((CPU_WORD)1) << (u16Priority & ((INT16U)0x3F)));

#else
    #error "Not supported cpu word bits! Please check KER_u8CPU_WORD_BITS."
#endif
}

#if KER_coCPU_CLZ_INSTRUCTION_EN > 0u
/*******************************************************************************
* @Brief  : Get highest priority ready task
* @Inputs : none
* @Outputs: none
* @Returns: Highest priority
* @Note   : none
*******************************************************************************/
PUBLIC INT16U KER_u16GetRdyHighestPriority( void )
{
    INT16U u16Prio = (INT16U)0;
    volatile CPU_WORD *pwBitMap = KER_awTaskRdyBitMap;

    while ( (CPU_WORD)0 == *pwBitMap )
    {
        pwBitMap++;
        u16Prio += KER_u8CPU_WORD_BITS;
    }

    u16Prio += (INT16U)KER_wCntTailZeros(*pwBitMap);
    return u16Prio;
}
#else
/*******************************************************************************
* @Brief  : Get highest priority ready task
* @Inputs : none
* @Outputs: none
* @Returns: Highest priority
* @Note   : none
*******************************************************************************/
PUBLIC INT16U KER_u16GetRdyHighestPriority( void )
{
    INT16U u16Prio = (INT16U)0;
    volatile CPU_WORD *pwBitMap = KER_awTaskRdyBitMap;

    while ( (CPU_WORD)0 == *pwBitMap )
    {
        pwBitMap++;
        u16Prio += KER_u8CPU_WORD_BITS;
    }

#if (KER_u8CPU_WORD_BITS == 8)

    u16Prio += KER_kau8PrioBitMap[*pwBitMap];

#elif (KER_u8CPU_WORD_BITS == 16)

    if ((*pwBitMap) & (CPU_WORD)0xFF)
    {
        u16Prio += KER_kau8PrioBitMap[(*pwBitMap) & (CPU_WORD)0xFF];
    }
    else
    {
        u16Prio += (INT16U)8;
        u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>8) & (CPU_WORD)0xFF];
    }

#elif (KER_u8CPU_WORD_BITS == 32)

    if ((*pwBitMap) & (CPU_WORD)0xFFFF)
    {
        if ((*pwBitMap) & (CPU_WORD)0xFF)
        {
            u16Prio += KER_kau8PrioBitMap[(*pwBitMap) & (CPU_WORD)0xFF];
        }
        else
        {
            u16Prio += (INT16U)8;
            u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>8) & (CPU_WORD)0xFF];
        }
    }
    else
    {
        if ((*pwBitMap) & ((CPU_WORD)0xFF << 16))
        {
            u16Prio += (INT16U)16;
            u16Prio += KER_kau8PrioBitMap[((*pwBitMap) >> 16) & (CPU_WORD)0xFF];
        }
        else
        {
            u16Prio += (INT16U)24;
            u16Prio += KER_kau8PrioBitMap[((*pwBitMap) >> 24) & (CPU_WORD)0xFF];
        }
    }

#elif (KER_u8CPU_WORD_BITS == 64)

    if ((*pwBitMap) & (CPU_WORD)0xFFFFFFFF)
    {
        if ((*pwBitMap) & (CPU_WORD)0xFFFF)
        {
            if ((*pwBitMap) & (CPU_WORD)0xFF)
            {
                u16Prio += KER_kau8PrioBitMap[(*pwBitMap) & (CPU_WORD)0xFF];
            }
            else
            {
                u16Prio += (INT16U)8;
                u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>8) & (CPU_WORD)0xFF];
            }
        }
        else
        {
            if ((*pwBitMap) & ((CPU_WORD)0xFF << 16))
            {
                u16Prio += (INT16U)16;
                u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>16) & (CPU_WORD)0xFF];
            }
            else
            {
                u16Prio += (INT16U)24;
                u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>24) & (CPU_WORD)0xFF];
            }
        }
    }
    else
    {
        if ((*pwBitMap) & ((CPU_WORD)0xFFFF << 32))
        {
            if ((*pwBitMap) & ((CPU_WORD)0xFF << 32))
            {
                u16Prio += (INT16U)32;
                u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>32) & (CPU_WORD)0xFF];
            }
            else
            {
                u16Prio += (INT16U)40;
                u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>40) & (CPU_WORD)0xFF];
            }
        }
        else
        {
            if ((*pwBitMap) & ((CPU_WORD)0xFF) << 48)
            {
                u16Prio += (INT16U)48;
                u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>48) & (CPU_WORD)0xFF];
            }
            else
            {
                u16Prio += (INT16U)56;
                u16Prio += KER_kau8PrioBitMap[((*pwBitMap)>>56) & (CPU_WORD)0xFF];
            }
        }
    }

#else
    #error "Not supported cpu word bits! Please check KER_u8CPU_WORD_BITS."
#endif

    return u16Prio;
}
#endif
