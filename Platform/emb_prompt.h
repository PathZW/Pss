/*******************************************************************************
 * @file    emb_prompt.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/5/4
 * @brief   Embedded platform defines for the prompt of system status
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/5/4 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __EMB_PROMPT_H__
#define __EMB_PROMPT_H__
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Emb_Prompt
 * @{
 * @addtogroup          Exported_Functions
 * @{
 ******************************************************************************/

void SysPrompt_Assert (void);   //!< 系统提示: 致命故障 ---- 红色常亮
void SysPrompt_Booting(void);   //!< 系统提示: 正在引导 ---- 绿色常亮
void SysPrompt_Upgrade(void);   //!< 系统提示: 升级固件 ---- 黄色常亮
void SysPrompt_Error  (void);   //!< 系统提示: 错误指示 ---- 红色快闪
void SysPrompt_Warning(void);   //!< 系统提示: 警告指示 ---- 红色慢闪
void SysPrompt_Initing(void);   //!< 系统提示: 正在初始化 -- 绿色快闪
void SysPrompt_Normal (void);   //!< 系统提示: 正常运行 ---- 绿色慢闪

void SysPromptStateInit(void);  //!< 系统状态提示初始化
void SysPromptStatePoll(void);  //!< 系统状态提示轮询, 每100毫秒调用一次


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_PROMPT_H__
/*****************************  END OF FILE  **********************************/

