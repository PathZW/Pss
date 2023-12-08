/*******************************************************************************
 * @file    emb_delay.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/4/27
 * @brief   Embedded platform defines for the delay and sleep
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/8/2 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __EMB_DELAY_H__
#define __EMB_DELAY_H__
/******************************************************************************/
#include <stdint.h>
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * 获取系统运行时间计数(毫秒单位), 注意能够在 ISR 中使用
 */
uint32_t GetSysMsec(void);

/**
 * 延时到指定系统运行时间计数(毫秒单位)时刻, 注意不能在 ISR 中使用
 */
void DelayTO(uint32_t mCnt);
void SleepTO(uint32_t mCnt);

/**
 * 毫秒延时, 注意不能在 ISR 中使用
 */
void DelayMS(uint32_t mSec);
void SleepMS(uint32_t mSec);

/**
 * 微秒延时, 注意能够在 ISR 中使用
 */
void DelayUS(uint16_t uSec);


/*****************************  END OF FILE  **********************************/
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_DELAY_H__
/*****************************  END OF FILE  **********************************/

