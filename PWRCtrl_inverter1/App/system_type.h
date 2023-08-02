/**
 ******************************************************************************
 * @author  �Ρ�СP
 * @version V1.0
 * @date    2018-08-22
 * @brief   �������������Ƽ����޹�˾ ��Ȩ���У�C��All rights reserved.
 ******************************************************************************
 * 
 *  �ļ�����system_typedef.h
 *  ��  �ܣ��������Ͷ���
 *
 ******************************************************************************
**/ 


#ifndef __SYSTEM_TYPEDEF_H
#define __SYSTEM_TYPEDEF_H

//===========�������Ͷ���=========//
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


#define FreqToCycle(X)     ((float32)(1.0f/X))//Ƶ��ת���ڣ�Ƶ�ʵ�λHZ�����ڵ�λS
#define CycleToFreq(X)     ((float32)(1.0f/X))//����תƵ�ʣ�Ƶ�ʵ�λHZ�����ڵ�λS

#endif


/********************************** END OF FILE **********************************/
