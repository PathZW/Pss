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
 * 初始化裸机平台, 主要在 std 库前初始化内存以及CPU时钟
 *
 * @note  替代 CMSIS 环境 SystemInit() 函数以执行额外的操作
 * @note  硬件相关裸机初始化由 Board_BareInit() 完成
 */
void Platform_BareInit(void);

/**
 * 检查裸机平台初始化是否完成
 *
 * @retval  (== 0) 未完成
 * @retval  (!= 0) 已完成
 */
int Platform_SysCheck(void);

/**
 * 初始化平台, 使用 RTOS 时 RTOS 未加载
 *
 * @note  硬件相关初始化由 Board_Init() 完成
 */
void Platform_Init(void);

/**
 * 启动平台并执行 app_main() 函数, 使用 RTOS 时为 app_main() 创建线程
 *
 * @note  硬件相关启动由 Board_Start() 完成, 驱动与组件由 SystemEntry_Loader() 加载
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

