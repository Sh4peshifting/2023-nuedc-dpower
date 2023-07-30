/**
 ******************************************************************************
 * @author  HEPENG
 * @version V1.0
 * @date    2019-05-29
 * @brief   柳州启明电气科技有限公司 版权所有（C）All rights reserved.
 ******************************************************************************
 * 
 * 文 件 名：dp_power.h
 * 描    述：数字电源控制相关定义
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




