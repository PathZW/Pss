/*******************************************************************************
 * @file    devDriver_cfg.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/4/12
 * @brief   通用设备驱动配置文件
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/4/12 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DEV_DRIVER_CFG_H__
#define __DEV_DRIVER_CFG_H__
#ifndef ENABLE
#define ENABLE          1
#endif
#ifndef DISABLE
#define DISABLE         0
#endif
/******************************************************************************/
/**
 * FPGA AD4000 驱动配置
 */
#if ENABLE
#   define FpgaAD4000_1_BASE_ADDR      0x60001000   //!< FPGA AD4000 Chip1 地址
#   define FpgaAD4000_2_BASE_ADDR      0x60002000   //!< FPGA AD4000 Chip2 地址
#   define FpgaAD4000_3_BASE_ADDR      0x60003000   //!< FPGA AD4000 Chip3 地址
//# define FpgaAD4000_4_BASE_ADDR      0x60004000   //!< FPGA AD4000 Chip4 地址
#   define FpgaAD4000_SYNC_BASE_ADDR   0x60000000   //!< 多片 SYNC 地址(同步控制)
#endif


/**
 * FPGA AD5761 驱动配置
 */
#if ENABLE
#define FpgaAD5761_1_BASE_ADDR      0x60000000   //!< FPGA AD5761 Chip1 地址
#define FpgaAD5761_2_BASE_ADDR      0x60001000   //!< FPGA AD5761 Chip2 地址
#define FpgaAD5761_3_BASE_ADDR      0x60002000   //!< FPGA AD5761 Chip3 地址
#define FpgaAD5761_4_BASE_ADDR      0x60003000   //!< FPGA AD5761 Chip4 地址
#endif

/**
 * FPGA AD5683 驱动配置
 */
#if ENABLE
#define FpgaAD5683_1_BASE_ADDR      0x60000000   //!< FPGA AD5761 Chip1 地址
#define FpgaAD5683_2_BASE_ADDR      0x60001000   //!< FPGA AD5761 Chip2 地址
#define FpgaAD5683_3_BASE_ADDR      0x60002000   //!< FPGA AD5761 Chip3 地址
#define FpgaAD5683_4_BASE_ADDR      0x60003000   //!< FPGA AD5761 Chip4 地址
#endif

/**
 * FPGA AD1259 驱动配置
 */
#if ENABLE

#define   FpgaAD1259_1_BASE_ADDR        (0x60004000)
#define   FpgaAD1259_2_BASE_ADDR        (0x60005000)
#define   FpgaAD1259_3_BASE_ADDR        (0x60006000)
#endif


/*****************************  END OF FILE  **********************************/
#undef ENABLE
#undef DISABLE
#endif
/*****************************  END OF FILE  **********************************/

