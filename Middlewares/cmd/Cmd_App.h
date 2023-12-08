/*******************************************************************************
 * @file    Cmd_App.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/6/23
 * @brief   
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/6/23 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __CMD_APP_H__
#define __CMD_APP_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup Cmd_App
 * @{
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * 初始化命令接口应用
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int Cmd_AppInit(void);

//lint -esym(522, Cmd_ForwardRemIF)
/**
 * 移去命令转发接口(所有索引)
 *
 * @param[in]  cli  命令行接口
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int Cmd_ForwardRemIF(int cli);

//lint -esym(522, Cmd_ForwardSetIF)
/**
 * 设置命令转发接口(指定索引)
 *
 * @param[in]  idx  转发接口索引, (0)为默认接口
 * @param[in]  cli  命令行接口
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int Cmd_ForwardSetIF(unsigned idx, int cli);

//lint -esym(522, Cmd_ForwardGetIF)
/**
 * 获取命令转发接口(指定索引)
 *
 * @param[in]  idx  转发接口索引, (0)为默认接口
 *
 * @retval  ( > 0) CLI 接口
 * @retval  (<= 0) 无效的索引
 */
int Cmd_ForwardGetIF(unsigned idx);

//lint -esym(522, Cmd_ForwardToIdx)
/**
 * 命令手动转发(注意:未注册命令会自动转发到默认转发接口)
 *
 * @param[in]  idx    转发到的接口索引, (0)为默认转发接口, (0xFFFF)为所有转发接口
 * @param[in]  cli    转发的源命令行接口
 * @param[in]  argc   命令字符串个数
 * @param[in]  argv   命令字符串数组, 包括命令名, 包括命令结束符('\n')
 *
 * @retval  ( > 0) 成功, 返回转发的命令字符数
 * @retval  (== 0) 参数错误或目的接口无效
 * @retval  ( < 0) 设备操作异常
 */
int Cmd_ForwardToIdx(unsigned idx, int cli, int argc, const char * const argv[]);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __CMD_APP_H__
/*****************************  END OF FILE  **********************************/

