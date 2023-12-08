/*******************************************************************************
 * @file    emb_platform.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/5/10
 * @brief   Embedded platform bootstarp and startup
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/5/10 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __EMB_PLATFORM_H__
#define __EMB_PLATFORM_H__
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Emb_Platform
 * @{
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * ��ʼ�����ƽ̨, ��Ҫ�� std ��ǰ��ʼ���ڴ��Լ�CPUʱ��
 *
 * @note  ��� CMSIS ���� SystemInit() ������ִ�ж���Ĳ���
 * @note  Ӳ����������ʼ���� Board_BareInit() ���
 */
void Platform_BareInit(void);

/**
 * ������ƽ̨��ʼ���Ƿ����
 *
 * @retval  (== 0) δ���
 * @retval  (!= 0) �����
 */
int Platform_SysCheck(void);

/**
 * ��ʼ��ƽ̨, ʹ�� RTOS ʱ RTOS δ����
 *
 * @note  Ӳ����س�ʼ���� Board_Init() ���
 */
void Platform_Init(void);

/**
 * ����ƽ̨��ִ�� app_main() ����, ʹ�� RTOS ʱΪ app_main() �����߳�
 *
 * @note  Ӳ����������� Board_Start() ���, ����������� SystemEntry_Loader() ����
 */
void Platform_Start(void *arg);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_PLATFORM_H__
/*****************************  END OF FILE  **********************************/

