/*******************************************************************************
*                   Copyright(c) 2015,Shiwei.LI
*                     All rights reserved.
*File   Name: KER_Def.h
*Version    : V1.00
*Description: Define common basic data type
*Author     : Shiwei.LI
*E-Mail     : cfixsoft@163.com
*Create Date: 2015/8/23
*******************************************************************************/
#ifndef KER_DEF_H
#define KER_DEF_H

/*******************************************************************************
* !Comment: Define common basic data type.
*******************************************************************************/
typedef  unsigned char       BOOL;
typedef  unsigned char       INT8U;
typedef  unsigned short      INT16U;
typedef  unsigned int        INT32U;
typedef  char                INT8S;
typedef  short               INT16S;
typedef  int                 INT32S;
typedef  unsigned int        CPU_WORD;
typedef  unsigned int        CPU_ADDR;
typedef  unsigned int        CPU_STK;
typedef  unsigned int        CPU_STK_SIZE;
typedef  unsigned int        EVENT_TYPE;
typedef  unsigned long long  INT64U;

#define CPU_REG              register

#define  RAM_ALIGN_SIZE      (4u)

#ifdef TRUE
#undef TRUE
#endif
#define  TRUE                ((BOOL)1u)

#ifdef FALSE
#undef FALSE
#endif
#define  FALSE               ((BOOL)0u)

#ifndef  KER_NULL
    #define  KER_NULL            ((void *)0)
#endif

#define PUBLIC
#define PRIVATE              static
#ifndef KER_INLINE
    #define KER_INLINE       static inline
#endif

#endif






