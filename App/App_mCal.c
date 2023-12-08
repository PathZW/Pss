#include "App_mcal.h"

static rtcfg_double_t sDefaultCalPara[PSS_BIN_RANGE_CAL_INDEX_MAX][2] =
{
/*  校准系数索引            c1         c0 */
    /* 3组电压量程系数 */
    {{"3V_A",        1},     {"3V_B",        0}},
    {{"30V_A",       1},     {"30V_B",       0}},
    {{"100V_A",      1},     {"100V_B",      0}},
    /* 光功率系数 */
    {{"0_1MW_A",     1},     {"0_1MW_B",     0}},
    {{"1_0MW_A",     1},     {"1_0MW_B",     0}},
    {{"10MW_A",      1},     {"10MW_B",      0}},
    /* 12组分板系数 */
    {{"I1_5A_1_A",   0.2},     {"I1_5A_1_B",   0}},
    {{"I1_10A_2_A",  0.2},     {"I1_10A_2_B",  0}},
    {{"I1_15A_3_A",  0.2},     {"I1_15A_3_B",  0}},
    {{"I1_20A_4_A",  0.2},     {"I1_20A_4_B",  0}},

    {{"I2_5A_1_A",   1},     {"I2_5A_1_B",   0}},
    {{"I2_10A_2_A",  1},     {"I2_10A_2_B",  0}},
    {{"I2_15A_3_A",  1},     {"I2_15A_3_B",  0}},
    {{"I2_20A_4_A",  1},     {"I2_20A_4_B",  0}},
    {{"I2_25A_5_A",  1},     {"I2_25A_5_B",  0}},
    {{"I2_30A_6_A",  1},     {"I2_30A_6_B",  0}},
    {{"I2_35A_7_A",  1},     {"I2_35A_7_B",  0}},
    {{"I2_40A_8_A",  1},     {"I2_40A_8_B",  0}},
    {{"I2_45A_9_A",  1},     {"I2_45A_9_B",  0}},
    {{"I2_50A_10_A", 1},     {"I2_50A_10_B", 0}},
    {{"I2_55A_11_A", 1},     {"I2_55A_11_B", 0}},
    {{"I2_60A_12_A", 1},     {"I2_60A_12_B", 0}},

    {{"I3_50A_1_A",   1},     {"I3_50A_1_B",   0}},
    {{"I3_100A_2_A",  1},     {"I3_100A_2_B",  0}},
    {{"I3_150A_3_A",  1},     {"I3_150A_3_B",  0}},
    {{"I3_200A_4_A",  1},     {"I3_200A_4_B",  0}},
    {{"I3_250A_5_A",  1},     {"I3_250A_5_B",  0}},
    {{"I3_300A_6_A",  1},     {"I3_300A_6_B",  0}},
    {{"I3_350A_7_A",  1},     {"I3_350A_7_B",  0}},
    {{"I3_400A_8_A",  1},     {"I3_400A_8_B",  0}},
    {{"I3_450A_9_A",  1},     {"I3_450A_9_B",  0}},
    {{"I3_500A_10_A", 1},     {"I3_500A_10_B", 0}},
    {{"I3_550A_11_A", 1},     {"I3_550A_11_B", 0}},
    {{"I3_600A_12_A", 1},     {"I3_600A_12_B", 0}},
    /* 12组分板系数 回读 */
    {{"R1_5A_1_A",   1},      {"R1_5A_1_B",   0}},
    {{"R1_10A_2_A",  1},      {"R1_10A_2_B",  0}},
    {{"R1_15A_3_A",  1},      {"R1_15A_3_B",  0}},
    {{"R1_20A_4_A",  1},      {"R1_20A_4_B",  0}},

    {{"R2_5A_1_A",   1},     {"R2_5A_1_B",   0}},
    {{"R2_10A_2_A",  1},     {"R2_10A_2_B",  0}},
    {{"R2_15A_3_A",  1},     {"R2_15A_3_B",  0}},
    {{"R2_20A_4_A",  1},     {"R2_20A_4_B",  0}},
    {{"R2_25A_5_A",  1},     {"R2_25A_5_B",  0}},
    {{"R2_30A_6_A",  1},     {"R2_30A_6_B",  0}},
    {{"R2_35A_7_A",  1},     {"R2_35A_7_B",  0}},
    {{"R2_40A_8_A",  1},     {"R2_40A_8_B",  0}},
    {{"R2_45A_9_A",  1},     {"R2_45A_9_B",  0}},
    {{"R2_50A_10_A", 1},     {"R2_50A_10_B", 0}},
    {{"R2_55A_11_A", 1},     {"R2_55A_11_B", 0}},
    {{"R2_60A_12_A", 1},     {"R2_60A_12_B", 0}},

    {{"R3_50A_1_A",   1},     {"R3_50A_1_B",   0}},
    {{"R3_100A_2_A",  1},     {"R3_100A_2_B",  0}},
    {{"R3_150A_3_A",  1},     {"R3_150A_3_B",  0}},
    {{"R3_200A_4_A",  1},     {"R3_200A_4_B",  0}},
    {{"R3_250A_5_A",  1},     {"R3_250A_5_B",  0}},
    {{"R3_300A_6_A",  1},     {"R3_300A_6_B",  0}},
    {{"R3_350A_7_A",  1},     {"R3_350A_7_B",  0}},
    {{"R3_400A_8_A",  1},     {"R3_400A_8_B",  0}},
    {{"R3_450A_9_A",  1},     {"R3_450A_9_B",  0}},
    {{"R3_500A_10_A", 1},     {"R3_500A_10_B", 0}},
    {{"R3_550A_11_A", 1},     {"R3_550A_11_B", 0}},
    {{"R3_600A_12_A", 1},     {"R3_600A_12_B", 0}},
    {{"PT100:A1_K",   1},     {"PT100:A1_B",   0}},
    {{"PT100:A2_K",   1},     {"PT100:A2_B",   0}},
    {{"PT100:A3_K",   1},     {"PT100:A3_B",   0}},
    {{"PT100:A4_K",   1},     {"PT100:A4_B",   0}},
    {{"PT100:A5_K",   1},     {"PT100:A5_B",   0}},
    {{"PT100:A6_K",   1},     {"PT100:A6_B",   0}},
    {{"PT100:A7_K",   1},     {"PT100:A7_B",   0}},
    {{"PT100:A8_K",   1},     {"PT100:A8_B",   0}},
    {{"PT100:A9_K",   1},     {"PT100:A9_B",   0}},
    {{"PT100:A10_K",  1},     {"PT100:A10_B",  0}},
    {{"PT100:A11_K",  1},     {"PT100:A11_B",  0}},
    {{"PT100:A12_K",  1},     {"PT100:A12_B",  0}},
    {{"PT100:A13_K",  1},     {"PT100:A13_B",  0}},
    {{"PT100:A14_K",  1},     {"PT100:A14_B",  0}},
    {{"PT100:A15_K",  1},     {"PT100:A15_B",  0}},
    {{"PT100:A16_K",  1},     {"PT100:A16_B",  0}},
    {{"PT100:B1_K",   1},     {"PT100:B1_B",   0}},
    {{"PT100:B2_K",   1},     {"PT100:B2_B",   0}},
    {{"PT100:B3_K",   1},     {"PT100:B3_B",   0}},
    {{"PT100:B4_K",   1},     {"PT100:B4_B",   0}},
    {{"PT100:B5_K",   1},     {"PT100:B5_B",   0}},
    {{"PT100:B6_K",   1},     {"PT100:B6_B",   0}},
    {{"PT100:B7_K",   1},     {"PT100:B7_B",   0}},
    {{"PT100:B8_K",   1},     {"PT100:B8_B",   0}},
    {{"PT100:B9_K",   1},     {"PT100:B9_B",   0}},
    {{"PT100:B10_K",  1},     {"PT100:B10_B",  0}},
    {{"PT100:B11_K",  1},     {"PT100:B11_B",  0}},
    {{"PT100:B12_K",  1},     {"PT100:B12_B",  0}},
    {{"PT100:B13_K",  1},     {"PT100:B13_B",  0}},
    {{"PT100:B14_K",  1},     {"PT100:B14_B",  0}},
    {{"PT100:B15_K",  1},     {"PT100:B15_B",  0}},
    {{"PT100:B16_K",  1},     {"PT100:B16_B",  0}},
};


int PssMcuCalInit(PSS_MCU_CAL_T* cal)
{
    /* 使用默认校准系数 */
    for(int i = 0; i < PSS_BIN_RANGE_CAL_INDEX_MAX; i ++)
    {
        cal->calPara[i].mC1 = rtcfg_double_read(&sDefaultCalPara[i][0]);
        cal->calPara[i].mC0 = rtcfg_double_read(&sDefaultCalPara[i][1]);
    }
    return 0;
}

double Load2CalVal(PSS_MCU_CAL_T *cal, double load, uint32_t index)
{
    index = (PSS_MCU_RANGE_CAL_INDEX_T)index;
    double c1=0, c0=0;
    c1 = cal->calPara[index].mC1;
    c0 = cal->calPara[index].mC0;
    return (load*c1+c0);
}

void PssMcuCalFire(PSS_MCU_CAL_T *cal)
{
    for(int index =0; index < PSS_BIN_RANGE_CAL_INDEX_MAX; index++)
    {
        rtcfg_double_write(&sDefaultCalPara[index][0], cal->calPara[index].mC1);
        rtcfg_double_write(&sDefaultCalPara[index][1], cal->calPara[index].mC0);
    }
}

void PssMcuCalSet(PSS_MCU_CAL_T *cal, uint32_t index, double c1, double c0)
{
    index = (PSS_MCU_RANGE_CAL_INDEX_T)index;
    cal->calPara[index].mC1 = c1;
    cal->calPara[index].mC0 = c0;
}

void PssAnlgCalReset(PSS_MCU_CAL_T *cal, uint32_t index)
{
    index = (PSS_MCU_RANGE_CAL_INDEX_T)index;
    cal->calPara[index].mC1 = sDefaultCalPara[index][0].dft;
    cal->calPara[index].mC0 = sDefaultCalPara[index][1].dft;
    PssMcuCalFire(cal);
}

double PssMcuCalReadC1(PSS_MCU_CAL_T *cal, uint32_t index)
{
    index = (PSS_MCU_RANGE_CAL_INDEX_T)index;
    return cal->calPara[index].mC1;
}

double PssMcuCalReadC0(PSS_MCU_CAL_T *cal, uint32_t index)
{
    index = (PSS_MCU_RANGE_CAL_INDEX_T)index;
    return cal->calPara[index].mC0;
}
