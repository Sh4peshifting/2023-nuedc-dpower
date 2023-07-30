#ifndef __HLW8032_H__
#define __HLW8032_H__
#define V_coef 0.18f
#define I_coef 0.25f
typedef struct 
{
    float V;
    float I;
    float AC_P;
    float AP_P;
    float F;
    float V_cali;
    float I_cali;
    float P_cali;
}AC_Para; 
void HLW8032_Get_NoCalibration(unsigned char Data[],AC_Para* AC_para);
void HLW8032_Calibration(unsigned char Data[],AC_Para* AC_para, float v_cali, float i_cali, float p_cali);
void HLW8032_Get_UseCalibration(unsigned char Data[],AC_Para* AC_para);
#endif
