/*******************************************************************************
 * @file    board.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/4/26
 * @brief   Device's defines for the hardware board
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/4/26 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __BOARD_H__
#define __BOARD_H__
/******************************************************************************/

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#include <stdint.h>
#include "Board_Device.h"


/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/**
 * @addtogroup Board
 * @{
 * @addtogroup              Exported_Macros
 * @{
 ******************************************************************************/

#ifndef ROM_MAIN_ADDR
#define ROM_MAIN_ADDR       0       //!< (C) 主ROM块 地址
#endif
#ifndef ROM_MAIN_SIZE
#define ROM_MAIN_SIZE       0       //!< (C) 主ROM块 大小
#endif
#ifndef ROM_RESV_SIZE
#define ROM_RESV_SIZE       0       //!< (C) 主ROM块 保留大小(BootLoader)
#endif

#ifndef RAM_MAIN_ADDR
#define RAM_MAIN_ADDR       0       //!< (C) 主RAM块 地址
#endif
#ifndef RAM_MAIN_SIZE
#define RAM_MAIN_SIZE       0       //!< (C) 主RAM块 大小
#endif

/*----------------------------------------------------------------------------*/
#ifndef ROM_LINK_START
#define ROM_LINK_START      0       //!< (C) 链接器已使用的 默认ROM块 开始地址
#endif
#ifndef ROM_LINK_LIMIT
#define ROM_LINK_LIMIT      0       //!< (C) 链接器已使用的 默认ROM块 结束地址
#endif

#ifndef RAM_LINK_START
#define RAM_LINK_START      0       //!< (C) 链接器已使用的 默认RAM块 开始地址
#endif
#ifndef RAM_LINK_LIMIT
#define RAM_LINK_LIMIT      0       //!< (C) 链接器已使用的 默认RAM块 结束地址
#endif

#ifndef HEAP_EXT_BASE
#define HEAP_EXT_BASE          RAM_LINK_LIMIT         //!< (C) std Heap 扩展内存地址
#endif
#ifndef HEAP_EXT_SIZE
#define HEAP_EXT_SIZE (RAM_MAIN_SIZE - RAM_LINK_SIZE) //!< (C) std Heap 扩展内存大小
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * 裸机硬件初始化, 一般初始化 CPU时钟与内存, 以便 C runtime 加载
 */
void Board_BareInit(void);

/**
 * 硬件初始化, RTOS 未初始化
 */
void Board_Init(void);

/**
 * 硬件启动, RTOS 未初始化
 */
void Board_Start(void);

/**
 * 硬件复位, 整个系统重启
 */
void Board_Reset(void);


/*----------------------------------------------------------------------------*/
/**
 * 硬件延时初始化或重新初始化. (内核时钟 和 C runtime 未初始化)
 */
void Board_DelayInit(void);

/** 
 * 获取系统运行时间硬件计数(毫秒单位), 注意能够在 ISR 中调用
 */
uint32_t Board_GetMsec(void);

/** 
 * 微秒延时, 注意能够在 ISR 中调用
 */
void Board_DelayUS(uint16_t uSec);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __BOARD_H__
/*****************************  END OF FILE  **********************************/

