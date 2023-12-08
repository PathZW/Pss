/******************************************************************************
 *
 * 文件名  ： Drv_Ad5683.h
 * 负责人  ： zwj
 * 创建日期： 20200827
 * 版本号  ： v1.0
 * 文件描述： PSS AD5683驱动
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_DRVS_AD5683_H_
#define _PSS_MCU_DRVS_AD5683_H_

/************************************ 头文件 ***********************************/
#include <stdint.h>
#include <stdbool.h>

/************************************ 宏定义 ***********************************/
#define PSS_MCU_DRVS_AD5683_CMD_BASE                         (0x60000000)

#define PSS_MCU_DRVS_AD5683_CMD_FIFO                         (PSS_MCU_DRVS_AD5683_CMD_BASE + 0x00000000)
#define PSS_MCU_DRVS_AD5683_CMD_FIFO_STATE                   (PSS_MCU_DRVS_AD5683_CMD_BASE + 0x00000004)
#define PSS_MCU_DRVS_AD5683_CMD                              (PSS_MCU_DRVS_AD5683_CMD_BASE + 0x00000008)
#define PSS_MCU_DRVS_AD5683_CMD_RESET                        (PSS_MCU_DRVS_AD5683_CMD_BASE + 0x0000000C)

/* 5683控制寄存器 */
#define PSS_MCU_DRVS_AD5683_CTRL_REG_OFFSET         (16)
#define PSS_MCU_DRVS_AD5683_CTRL_REG_ADDR           (0x04)
#define PSS_MCU_DRVS_AD5683_WRITE_REG_ADDR          (0x01)

#define PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET         (28)
#define PSS_MCU_DRVS_AD5683_0_MASK                  (0x01)
#define PSS_MCU_DRVS_AD5683_1_MASK                  (0x02)
#define PSS_MCU_DRVS_AD5683_2_MASK                  (0x04)
#define PSS_MCU_DRVS_AD5683_ALL_MASK                (PSS_MCU_DRVS_AD5683_0_MASK | PSS_MCU_DRVS_AD5683_1_MASK | PSS_MCU_DRVS_AD5683_2_MASK)

#define PSS_MCU_DRVS_AD5683_CMD_FIFO_FULL           (0x01)
#define PSS_MCU_DRVS_AD5683_CMD_FIFO_EMPTY          (0x02)
#define PSS_MCU_DRVS_AD5683_CMD_FIFO_BUSY           (0x04)

/* 读写位 */
#define PSS_MCU_DRVS_AD5683_CTRL_CV_OFFSET          (9)
#define PSS_MCU_DRVS_AD5683_CTRL_CV_MASK            (0x03)
#define PSS_MCU_DRVS_AD5683_CTRL_CV_ZERO            (0x00)
#define PSS_MCU_DRVS_AD5683_CTRL_CV_MID             (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_CV_FULL            (0x02)
#define PSS_MCU_DRVS_AD5683_CTRL_OVR_OFFSET         (8)
#define PSS_MCU_DRVS_AD5683_CTRL_OVR_MASK           (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_OVR_DI             (0x00)
#define PSS_MCU_DRVS_AD5683_CTRL_OVR_EN             (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_B2Z_OFFSET         (7)
#define PSS_MCU_DRVS_AD5683_CTRL_B2Z_MASK           (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_B2Z_SB             (0x00)
#define PSS_MCU_DRVS_AD5683_CTRL_B2Z_CB             (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_ETS_OFFSET         (6)
#define PSS_MCU_DRVS_AD5683_CTRL_ETS_MASK           (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_ETS_OFF            (0x00)
#define PSS_MCU_DRVS_AD5683_CTRL_ETS_ON             (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_PV_OFFSET          (3)
#define PSS_MCU_DRVS_AD5683_CTRL_PV_MASK            (0x03)
#define PSS_MCU_DRVS_AD5683_CTRL_PV_ZERO            (0x00)
#define PSS_MCU_DRVS_AD5683_CTRL_PV_MID             (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_PV_FULL            (0x02)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_OFFSET          (0)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_MASK            (0x07)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_N10_P10         (0x00)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_0_P10           (0x01)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_N5_P5           (0x02)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_0_P5            (0x03)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_N2E5_N7E5       (0x04)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_N3_P3           (0x05)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_0_P16           (0x06)
#define PSS_MCU_DRVS_AD5683_CTRL_RA_0_P20           (0x07)

#define PSS_MCU_DRVS_AD5683_RESET_FLAG              (0x01)

//#define iowrite32(data, addr) (*(volatile unsigned int *)(addr) = (data))
/*********************************** 类型定义 **********************************/
typedef struct
{
    volatile  uint32_t fifoReg;               /* FIFO寄存器 */
    volatile  uint32_t fifoStateReg;          /* FIFO状态寄存器 */
    volatile  uint32_t cmdReg;                /* 命令寄存器 */
    volatile  uint32_t resetReg;              /* 复位寄存器 */
}Ad5683_TypeDef;

typedef struct _PSS_MCU_DRVS_AD5683_PARA_TAG_
{
    /* 对应芯片数据手册中控制寄存器 */
    /* 只读位 */
    uint8_t sc;                          /* 短路状态 */
    uint8_t bo;                          /* 低压状态 */

    /* 读写位 */
    uint8_t cv;                          /* CLEAR电平 */
    uint8_t ovr;                         /* OVR使能 */
    uint8_t b2c;                         /* 数据极性格式 */
    uint8_t ets;                         /* 150度过温保护 */
    uint8_t pv;                          /* 启动电压 */
    uint8_t ra;                          /* 输出电压范围 */
} PSS_MCU_DRVS_AD5683_PARA_T;       /* 普塞斯 AD5683驱动 */

typedef struct _PSS_MCU_DRVS_AD5683_TAG_
{
    const char*        name;        /* ad5683名字 */
    Ad5683_TypeDef     *ad5683Hw;   /* ad5683 */
} PSS_MCU_DRVS_AD5683_T;            /* 普塞斯 AD5683驱动 */

/*--------------------------------- 接口声明区 --------------------------------*/

/*********************************** 全局变量 **********************************/

/*********************************** 接口函数 **********************************/
void PssMcuDrvsAd5683Init(PSS_MCU_DRVS_AD5683_T *ad5683);

void PssMcuDrvsAd5683SetVal(PSS_MCU_DRVS_AD5683_T *ad5683, bool isPulse, double val);
void PssMcuDrvsChoseAd5683SetVal(PSS_MCU_DRVS_AD5683_T *ad5683, uint8_t chipNum, bool isPulse, double val);

void PssMcuDrvsAd5683Reset(PSS_MCU_DRVS_AD5683_T *ad5683);
#endif // #ifndef _PSS_MCU_DRVS_AD5683_H_

#ifdef __cplusplus
}
#endif

