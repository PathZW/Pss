/********************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaAd4000.h
 * 负责人  :  zwj
 * 创建日期:  2021.10.18
 * 版本号  :  v1.0
 * 文件描述： PSS AD4000 接口
 * 其    他:  无
 * 修改日志:  无
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_DRVS_FPGA_AD4000_H_
#define _PSS_MCU_DRVS_FPGA_AD4000_H_

/************************************ 头文件 ***********************************/
#include <stdint.h>
#include <stdbool.h>
/************************************ 宏定义 ***********************************/
#define PSS_MCU_DRVS_AD4000_1_BASE_ADDR                         (0x60001000)
#define PSS_MCU_DRVS_AD4000_2_BASE_ADDR                         (0x60002000)
#define PSS_MCU_DRVS_AD4000_3_BASE_ADDR                         (0x60003000)

#define PSS_MCU_DRVS_AD4000_SYNCHRONOUS_CTRL_BASE_ADDR          (0x60000000)
/*********************************** 类型定义 **********************************/
typedef struct
{
    volatile uint32_t c0Adc;                     /* 1.c0 adc系数 */
    volatile uint32_t c1Adc;                     /* 2.c1 adc系数 */
    volatile uint32_t outpStableDelay;           /* 3.输出稳定延时 */
    volatile uint32_t startMode;                 /* 4.启动采样方式 */
    volatile uint32_t sampleCycTimes;            /* 5.组合循环次数 */
    volatile uint32_t sampleGroupNum;            /* 6.采样组合个数 */
    volatile uint32_t sampleFrequency;           /* 7.采样频率 */
    volatile uint32_t extractionRate;            /* 8.抽取率 */
    volatile uint32_t sampleNum;                 /* 9.采样点数 */
    volatile uint32_t configFinish;              /* 10.配置完成 */
    volatile uint32_t rangeInfo;                 /* 11.量程信息 */
    volatile uint32_t clear;                     /* 12.清除配置 */
    volatile uint32_t trigConfig;                /* 13.触发配置 */
    volatile uint32_t fpgaSampleStatus;          /* 14.Fpga采样状态 */
    volatile uint32_t interrupt;                 /* 15.中断脚,写1清除中断状态 */
    volatile uint32_t voltFlaot;                 /* 16.电压浮值 */
    volatile uint32_t maxVal;                    /* 17.最大值 */
    volatile uint32_t stableVal;                 /* 18.稳定值 */
    volatile uint32_t adValFifo;                 /* 19.AD原始值 */
    volatile uint32_t calValFifo;                /* 20.校准之后值 */
    volatile uint32_t fifoState;                 /* 21.Fifo状态1:原始值fifo空, 2:原始值fifo满, 4:校准值fifo空, 8:校准值fifo满 */
    volatile uint32_t clearFifo;                 /* 22.清除Fifo状态 1:清除原始值fifo, 2:清除校准值fifo*/
    volatile uint32_t vfdCtrl;                   /* 23.vfd控制寄存器,用于MiniLed消除数据采集板对模拟板的影响 */
    volatile uint32_t realVal;                   /* 24.实时寄存器 */
} Ad4000_TypeDef;

typedef struct
{
    volatile uint32_t adSynchronousStart;        /* 1.Ad4000同步启动采样 */
    volatile uint32_t adSynchronousStop;         /* 2.Ad4000同步关闭采样 */
    volatile uint32_t sampleCtrl;                /* 3.数据发送间隔 */
} Ad4000_Synchronous_Ctrl_Typedef;

typedef struct
{
    uint16_t trigLine;
    uint8_t trigMode;
    bool isNeedSign;
    uint16_t signNum;
} TrigConfig;

typedef struct _PSS_MCU_DRVS_AD4000_TAG_
{
    void            *phy;                   /* 底层驱动 或 物理层 */
    bool            isSynchronousCtrAd;     /* 同步控制Ad? */
} PSS_MCU_DRVS_AD4000_T;                    /* AD4000驱动 */
/*--------------------------------- 接口声明区 --------------------------------*/

/*********************************** 全局变量 **********************************/

/*********************************** 接口函数 **********************************/
int PssMcuDrvsAd4000Init(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t addr);
int PssMcuDrvsAd4000SyncInit(void);

int PssMcuDrvsAd4000SetSampleGroupNum(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t num);
int PssMcuDrvsAd4000SetSamplePara(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t sampleFrequency, uint32_t extractionRate, uint32_t sampleNum);
int PssMcuDrvsAd4000SetConfigFinish(PSS_MCU_DRVS_AD4000_T *ad4000);
int PssMcuDrvsAd4000SetTrig(PSS_MCU_DRVS_AD4000_T *ad4000, TrigConfig *trig);
int PssMcuDrvsAd4000SetStartMode(PSS_MCU_DRVS_AD4000_T *ad4000, bool isTrig);
int PssMcuDrvsAd4000SetSampleCycTimes(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t val);

int PssMcuDrvsAd4000SetCalPara(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t c0, uint32_t c1);
int PssMcuDrvsAd4000SetWaitSampleTime(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t timeNs);
int PssMcuDrvsAd4000SetSampleNum(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t sampleNum);
int PssMcuDrvsAd4000SetClearConfig(PSS_MCU_DRVS_AD4000_T *ad4000);
int PssMcuDrvsAd4000SetRangeInfo(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t val);

bool    PssMcuDrvsIsAd4000IntArrived(PSS_MCU_DRVS_AD4000_T *ad4000);
int PssMcuDrvsAd4000ClearInt(PSS_MCU_DRVS_AD4000_T *ad4000);
int PssMcuDrvsAd4000SetVoltFlaot(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t val);
uint32_t PssMcuDrvsAd4000GetMaxVal(PSS_MCU_DRVS_AD4000_T *ad4000);
uint32_t PssMcuDrvsAd4000GetStableVal(PSS_MCU_DRVS_AD4000_T *ad4000);
float PssMcuDrvsAd4000GetAdVal(PSS_MCU_DRVS_AD4000_T *ad4000);
uint32_t PssMcuDrvsAd4000GetCalVal(PSS_MCU_DRVS_AD4000_T *ad4000);
void PssMcuDrvsAd4000ClearFifo(PSS_MCU_DRVS_AD4000_T *ad4000);
float PssMcuDrvsAd4000GetRealAdVal(PSS_MCU_DRVS_AD4000_T *ad4000);

int PssMcuDrvsAd4000SetStartSample(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t type);
int PssMcuDrvsAd4000SetStopSample(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t type);
#endif // #ifndef _PSS_MCU_DRVS_FPGA_AD4000_H_

#ifdef __cplusplus
}
#endif

