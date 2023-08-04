/**
 ******************************************************************************
 * @author  HEPENG
 * @version V1.0
 * @date    2019-05-29
 * @brief   �������������Ƽ����޹�˾ ��Ȩ���У�C��All rights reserved.
 ******************************************************************************
 * 
 * �� �� ����dp_para.h
 * ��    ����������������
 *  
 ******************************************************************************
**/ 

#include "system_type.h"

#ifndef _DP_PARA_H
#define _DP_PARA_H

#define RESET_FAULT_TIME        50000
#define DP_PWM_FREQUENCY        200e3         //����Ƶ��,��λHZ
#define VOLTAGE_FREQUENCY       2000          //��ѹ������
#define CURRENT_FREQUENCY       25000         //����������
#define DP_VOLTAGV_IN_MAX       52.0f         //��������ѹ,��λV  
#define DP_VOLTAGV_IN_MIN       15.0f         //��С�����ѹ,��λV  
#define DP_VOLTAGV_OUT_MAX      52.0f         //��������ѹ,��λV  
#define DP_CURRENT_IN_MAX       10.0f         //����������,��λA
#define DP_CURRENT_OUT_MAX      10.0f         //����������,��λA
#define DP_ADC_II_OFFSET        2048L         //����������1.65Vƫ�Ƶ�ѹ��Ӧ��ADC����ֵ
#define DP_ADC_IO_OFFSET        2048L         //����������1.65Vƫ�Ƶ�ѹ��Ӧ��ADC����ֵ
#define DP_VOLTAGE_IN_RATIO    (100.0f/4095)  //�����ѹ����
#define DP_VOLTAGE_OUT_RATIO   (100.0f/4095)  //�����ѹ����
#define DP_CURRENT_IN_RATIO    (33.00f/4095)  //����������� 3.3/0.005/20 = 33.00
#define DP_CURRENT_OUT_RATIO   (33.00f/4095)  //����������� 3.3/0.005/20 = 33.00

typedef struct {

  float32 x1;      //��һ�����ֵ
  float32 y1;      //��һ����ʵֵ
  float32 x2;      //�ڶ������ֵ
  float32 y2;      //�ڶ�����ʵֵ
  float32 Value;   //ʵʱֵ
  float32 Coeff;   //ADCֵ����ʵֵ�ı���ϵ��
  float32 Offset;  //���ƫ��ֵ
  
}ELEC_INFO_STRUCT;//���������ṹ��

typedef struct BASE_CMD_STRUCT_DEF {

  float  VoRefSet;   // �����ѹ�趨
  float  IoRefSet;   // ��������趨
  float  ViRefSet;   // �����ѹ�趨
  float  IiRefSet;   // ��������趨
  
}BASE_CMD_STRUCT;//ʵʱ�޸ĵ�����ṹ��

typedef union SYSTEM_FAULT{
   uint16 all;
   struct {
      uint16 iuvp:  1;//����Ƿѹ
      uint16 iovp:  1;//�����ѹ
      uint16 iocp:  1;//�������
      uint16 motp:  1;//����ģ�����
      uint16 oovp:  1;//�������
      uint16 oocp:  1;//�������
      uint16 oopp:  1;//�����ʹ���
      uint16 excu:  1;//����������Ư����
      uint16 cotp:  1;//CPU����
      uint16 ilrv:  1;//���뼫�Դ���
   }bit;
}SYSTEM_FAULT_STRUCT;

typedef union SYSTEM_STA{
   uint16 all;
   struct {  
            uint16 EN:     1; //ʹ��
            uint16 RUN:    1; //����
            uint16 RDY:    1; //����
            uint16 FAUT:   1; //����
            uint16 ALM:    1; //����
            uint16 COM:    1; //ͨ��������
            uint16 CC:     1; //����ģʽ
            uint16 CV:     1; //��ѹģʽ
            uint16 CW:     1;  //�㹦��ģʽ
            uint16 MPPT:   1; //MPPTģʽ
            uint16 CHARGE: 1; //��س��
            uint16 DISCHA: 1; //��طŵ�
            uint16 PWRUP:  1; //�������
   }bit;  
}SYSTEM_STA_STRUCT;

/*  ȫ�ֱ�������  */
extern BASE_CMD_STRUCT   gMainCmd;   //��ָ��

extern ELEC_INFO_STRUCT gVoltBatStr; //��ص�ѹ����    
extern ELEC_INFO_STRUCT gVoltOutStr; //�����ѹ����   
extern ELEC_INFO_STRUCT gCurrOutStr; //�����������   
extern ELEC_INFO_STRUCT gVoltInStr;  //�����ѹ����   
extern ELEC_INFO_STRUCT gCurrInStr;  //�����������   

extern volatile SYSTEM_FAULT_STRUCT gPSM_FAULT;
extern volatile SYSTEM_STA_STRUCT   gPSM_STA;

#endif
