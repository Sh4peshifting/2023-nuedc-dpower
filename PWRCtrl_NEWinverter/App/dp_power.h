/**
 ******************************************************************************
 * @author  HEPENG
 * @version V1.0
 * @date    2019-05-29
 * @brief   �������������Ƽ����޹�˾ ��Ȩ���У�C��All rights reserved.
 ******************************************************************************
 * 
 * �� �� ����dp_power.h
 * ��    �������ֵ�Դ������ض���
 *  
 ******************************************************************************
**/ 

#ifndef _DP_POWER_H
#define _DP_POWER_H

#include "dp_para.h"

extern void   dp_PowerCtrlInit(void);
extern void   dp_PowerCmcCtrl(void);
extern void   dp_ExcursionCheck(void);
extern void   dp_ElecLineCorr(ELEC_INFO_STRUCT *p);
extern uint16 get_code_alm(void);
extern uint16 get_code_sta(void);
extern uint16 dp_vout_set(float32 ref);
extern uint16 dp_iout_set(float32 ref);


#endif




