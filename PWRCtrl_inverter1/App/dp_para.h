/**
 ******************************************************************************
 * @author  HEPENG
 * @version V1.0
 * @date    2019-05-29
 * @brief   柳州启明电气科技有限公司 版权所有（C）All rights reserved.
 ******************************************************************************
 * 
 * 文 件 名：dp_para.h
 * 描    述：参数变量定义
 *  
 ******************************************************************************
**/ 

#include "system_type.h"

#ifndef _DP_PARA_H
#define _DP_PARA_H

#define RESET_FAULT_TIME        50000
#define DP_PWM_FREQUENCY        200e3         //开关频率,单位HZ
#define VOLTAGE_FREQUENCY       2000          //电压环带宽
#define CURRENT_FREQUENCY       25000         //电流环带宽
#define DP_VOLTAGV_IN_MAX       52.0f         //最大输入电压,单位V  
#define DP_VOLTAGV_IN_MIN       15.0f         //最小输入电压,单位V  
#define DP_VOLTAGV_OUT_MAX      52.0f         //最大输出电压,单位V  
#define DP_CURRENT_IN_MAX       10.0f         //最大输入电流,单位A
#define DP_CURRENT_OUT_MAX      10.0f         //最大输出电流,单位A
#define DP_ADC_II_OFFSET        2048L         //电流采样的1.65V偏移电压对应的ADC采样值
#define DP_ADC_IO_OFFSET        2048L         //电流采样的1.65V偏移电压对应的ADC采样值
#define DP_VOLTAGE_IN_RATIO    (100.0f/4095)  //输入电压比例
#define DP_VOLTAGE_OUT_RATIO   (100.0f/4095)  //输出电压比例
#define DP_CURRENT_IN_RATIO    (33.00f/4095)  //输出电流比例 3.3/0.005/20 = 33.00
#define DP_CURRENT_OUT_RATIO   (33.00f/4095)  //输出电流比例 3.3/0.005/20 = 33.00

typedef struct {

  float32 x1;      //第一点采样值
  float32 y1;      //第一点真实值
  float32 x2;      //第二点采样值
  float32 y2;      //第二点真实值
  float32 Value;   //实时值
  float32 Coeff;   //ADC值与真实值的比例系数
  float32 Offset;  //零点偏移值
  
}ELEC_INFO_STRUCT;//电气参数结构体

typedef struct BASE_CMD_STRUCT_DEF {

  float  VoRefSet;   // 输出电压设定
  float  IoRefSet;   // 输出电流设定
  float  ViRefSet;   // 输入电压设定
  float  IiRefSet;   // 输入电流设定
  
}BASE_CMD_STRUCT;//实时修改的命令结构体

typedef union SYSTEM_FAULT{
   uint16 all;
   struct {
      uint16 iuvp:  1;//输入欠压
      uint16 iovp:  1;//输入过压
      uint16 iocp:  1;//输入过流
      uint16 motp:  1;//功率模块过温
      uint16 oovp:  1;//输出过流
      uint16 oocp:  1;//输出过流
      uint16 oopp:  1;//过功率故障
      uint16 excu:  1;//电流采样零漂过大
      uint16 cotp:  1;//CPU过温
      uint16 ilrv:  1;//输入极性错误
   }bit;
}SYSTEM_FAULT_STRUCT;

typedef union SYSTEM_STA{
   uint16 all;
   struct {  
            uint16 EN:     1; //使能
            uint16 RUN:    1; //运行
            uint16 RDY:    1; //就绪
            uint16 FAUT:   1; //故障
            uint16 ALM:    1; //警告
            uint16 COM:    1; //通信连接上
            uint16 CC:     1; //恒流模式
            uint16 CV:     1; //恒压模式
            uint16 CW:     1;  //恒功率模式
            uint16 MPPT:   1; //MPPT模式
            uint16 CHARGE: 1; //电池充电
            uint16 DISCHA: 1; //电池放电
            uint16 PWRUP:  1; //启动完成
   }bit;  
}SYSTEM_STA_STRUCT;

/*  全局变量声明  */
extern BASE_CMD_STRUCT   gMainCmd;   //主指令

extern ELEC_INFO_STRUCT gVoltBatStr; //电池电压参数    
extern ELEC_INFO_STRUCT gVoltOutStr; //输出电压参数   
extern ELEC_INFO_STRUCT gCurrOutStr; //输出电流参数   
extern ELEC_INFO_STRUCT gVoltInStr;  //输入电压参数   
extern ELEC_INFO_STRUCT gCurrInStr;  //输入电流参数   

extern volatile SYSTEM_FAULT_STRUCT gPSM_FAULT;
extern volatile SYSTEM_STA_STRUCT   gPSM_STA;

#endif
