/**
 ******************************************************************************
 * @author  何。小P
 * @version V1.0
 * @date    2018-08-22
 * @brief   柳州启明电气科技有限公司 版权所有（C）All rights reserved.
 ******************************************************************************
 * 
 *  文件名：system_typedef.h
 *  功  能：数据类型定义
 *
 ******************************************************************************
**/ 


#ifndef __SYSTEM_TYPEDEF_H
#define __SYSTEM_TYPEDEF_H

//===========数据类型定义=========//
#ifndef USER_DATA_TYPES
#define USER_DATA_TYPES

typedef enum
{
    FALSE = 0x00,
    TRUE  = 0x01,
} ENUM_FALSE_TRUE;


typedef unsigned char   BOOL;

typedef char            int8;
typedef short           int16;
typedef int             int32;
typedef long            int64;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef unsigned long   uint64;
typedef float           float32;
typedef double          float64;

#endif


#define FreqToCycle(X)     ((float32)(1.0f/X))//频率转周期，频率单位HZ，周期单位S
#define CycleToFreq(X)     ((float32)(1.0f/X))//周期转频率，频率单位HZ，周期单位S

#endif


/********************************** END OF FILE **********************************/
