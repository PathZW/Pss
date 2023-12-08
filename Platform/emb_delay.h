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
 * ��ȡϵͳ����ʱ�����(���뵥λ), ע���ܹ��� ISR ��ʹ��
 */
uint32_t GetSysMsec(void);

/**
 * ��ʱ��ָ��ϵͳ����ʱ�����(���뵥λ)ʱ��, ע�ⲻ���� ISR ��ʹ��
 */
void DelayTO(uint32_t mCnt);
void SleepTO(uint32_t mCnt);

/**
 * ������ʱ, ע�ⲻ���� ISR ��ʹ��
 */
void DelayMS(uint32_t mSec);
void SleepMS(uint32_t mSec);

/**
 * ΢����ʱ, ע���ܹ��� ISR ��ʹ��
 */
void DelayUS(uint16_t uSec);


/*****************************  END OF FILE  **********************************/
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_DELAY_H__
/*****************************  END OF FILE  **********************************/

