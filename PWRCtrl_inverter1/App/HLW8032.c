#include "HLW8032.h"
void HLW8032_Get_NoCalibration(unsigned char Data[],AC_Para* AC_para)
{
    unsigned int V_para_REG;
    unsigned int I_para_REG;
    unsigned int P_para_REG;
    unsigned int V_REG;
    unsigned int I_REG;
    unsigned int P_REG;
    V_para_REG = Data[2]<<16|Data[3]<<8|Data[4];
    V_REG = Data[5]<<16|Data[6]<<8|Data[7];
    I_para_REG = Data[8]<<16| Data[9]<<8|Data[10];
    I_REG = Data[11]<<16|Data[12]<<8|Data[13];
    P_para_REG = Data[14]<<16|Data[15]<<8|Data[16];
    P_REG = Data[17]<<16|Data[18]<<8|Data[19];
    AC_para->V = (float)V_para_REG / (float)V_REG * (float)V_coef;
    AC_para->I = (float)I_para_REG / (float)I_REG * (float)I_coef;
    AC_para->AC_P = (float)P_para_REG / (float)P_REG * (float)I_coef * (float)V_coef;
    AC_para->AP_P = AC_para->I * AC_para->V;
    AC_para->F = AC_para->AC_P / AC_para->AP_P;
}
void HLW8032_Calibration(unsigned char Data[],AC_Para* AC_para, float v_cali, float i_cali, float p_cali)
{
    unsigned int V_cali_REG;
    unsigned int I_cali_REG;
    unsigned int P_cali_REG;
    V_cali_REG = Data[5]<<16|Data[6]<<8|Data[7];
    I_cali_REG = Data[11]<<16|Data[12]<<8|Data[13];
    P_cali_REG = Data[17]<<16|Data[18]<<8|Data[19];
    AC_para->V_cali = v_cali * V_cali_REG;
    AC_para->I_cali = i_cali * I_cali_REG;
    AC_para->P_cali = p_cali * P_cali_REG;
}
void HLW8032_Get_UseCalibration(unsigned char Data[],AC_Para* AC_para)
{
    unsigned int V_REG;
    unsigned int I_REG;
    unsigned int P_REG;
    V_REG = Data[5]<<16|Data[6]<<8|Data[7];
    I_REG = Data[11]<<16|Data[12]<<8|Data[13];
    P_REG = Data[17]<<16|Data[18]<<8|Data[19];
    AC_para->V = AC_para->V_cali / (float)V_REG;
    AC_para->I = AC_para->I_cali / (float)I_REG;
    AC_para->AC_P = AC_para->P_cali / (float)P_REG;
    AC_para->AP_P = AC_para->I * AC_para->V;
    AC_para->F = AC_para->AC_P / AC_para->AP_P;
}
