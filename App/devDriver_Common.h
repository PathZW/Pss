/*******************************************************************************
 * @file    devDriver_Common.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/25
 * @brief   通用设备驱动框架
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/3/25 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DEV_DRIVER_COMMON_H__
#define __DEV_DRIVER_COMMON_H__
/******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
//lint -emacro(774, DEV_CMD_*, DEV_DRIVER_*, DEV_EVT_*)
//lint -emacro(845, DEV_CMD_*, DEV_DRIVER_*, DEV_EVT_*)
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Device_Driver
 * @{
 ******************************************************************************/
/**
 * 设备驱动
 */
typedef struct {
    int32_t   (*Send)(const void *data, size_t size, uint32_t opt); //!< 发送数据
    int32_t   (*Recv)(      void *data, size_t size, uint32_t opt); //!< 接收数据
    int32_t   (*Ctrl)(uint32_t cmd, uint32_t opt, ...);             //!< 设备控制
    int32_t   (*Init)(void);                                        //!< 初始化
    int32_t (*Uninit)(void);                                        //!< 反初始化
    const char *Name;                                               //!< 设备名
} const devDriver_t;

/**
 * 生成一个设备驱动名, 如 devDriver(ADC, 1) 将生成 _devDriver_ADC_1
 * @param[in]  name     设备名; 如 SPI ADC FLASH 等
 * @param[in]  n        设备号; 如 1 2 3
 */
#define  devDriver(name, n)     ATPASTE4(_devDriver_, name, _, n)

/**
 * 发送数据
 * @param[in]  data     发送缓存指针
 * @param[in]  size     发送缓存大小(以字节为单位)
 * @param[in]  opt      可选操作码
 * @retval    (>=0)     成功发送数据字节数
 * @retval    (< 0)     错误码
 * @note    1. 已配置超时, 与事件处理回调函数无关, 该操作为同步IO, 不能在中断中调用
 * @note    2. 未配置超时, 且事件处理回调函数为空, 该操作为同步IO, 不能在中断中调用
 * @note    3. 未配置超时, 且事件处理回调函数非空, 该操作为异步IO, 能够在中断中调用
 */
typedef int32_t (*devDriver_SEND)(const void *data, size_t size, uint32_t opt);

/**
 * 接收数据
 * @param[in]  data     接收缓存指针
 * @param[in]  size     接收缓存大小(以字节为单位)
 * @param[in]  opt      可选操作码
 * @retval    (>=0)     成功接收数据字节数
 * @retval    (< 0)     错误码
 * @note    1. 已配置超时, 与事件处理回调函数无关, 该操作为同步IO, 不能在中断中调用
 * @note    2. 未配置超时, 且事件处理回调函数为空, 该操作为同步IO, 不能在中断中调用
 * @note    3. 未配置超时, 且事件处理回调函数非空, 该操作为异步IO, 能够在中断中调用
 */
typedef int32_t (*devDriver_RECV)(void *data, size_t size, uint32_t opt);

/**
 * 设备控制, 能够在中断中调用
 * @param[in]  cmd      命令码
 * @param[in]  opt      可选操作码
 * @retval    (>=0)     命令返回值
 * @retval    (< 0)     错误码
 */
typedef int32_t (*devDriver_CTRL)(uint32_t cmd, uint32_t opt, ...);

/**
 * 初始化, 不能在中断中调用
 * @return     错误码
 */
typedef int32_t (*devDriver_INIT)(void);

/**
 * 反初始化, 不能在中断中调用
 * @return     错误码
 */
typedef int32_t (*devDriver_UNINIT)(void);

/**
 * flags 发布函数, 可能在中断中调用
 * @param[in]  opt      可选操作码
 * @retval     (>=0)    成功
 * @retval     (< 0)    失败
 */
typedef int32_t (*devDriver_POST)(uint32_t opt);

/**
 * flags 等待函数, Send() 与 Recv() 中调用
 * @param[in]  opt      可选操作码( Send(, , opt) or Recv(, , opt) )
 * @param[in]  timeout  超时, (0)不等待, (UINT32_MAX)无限等待
 * @retval     (>=0)    成功
 * @retval     (< 0)    失败
 */
typedef int32_t (*devDriver_WAIT)(uint32_t timeout, uint32_t opt);

/**
 * 事件处理回调函数, 可能在中断中调用
 * @param[in]  device   设备驱动指针
 * @param[in]  optsend  可选操作码(发送)
 * @param[in]  optrecv  可选操作码(接收)
 * @param[in]  evt      事件码
 */
typedef void (*devDriver_CB)(devDriver_t* device, uint32_t optsend, uint32_t optrecv, uint32_t evt, ...);


/*******************************************************************************
 *
 * @addtogroup              Event_Codes
 * @{
 ******************************************************************************/
// devDriver_CB()

#define DEV_EVT_ARG_MSK     (0x000000FFul)
#define DEV_EVT_POWER_CTRL     (1ul << 12)  //!< 事件: 电源控制; 参数ARG:(0/1/2)关闭/打开/复位
#define DEV_EVT_ERROE_SEND     (1ul << 13)  //!< 事件: 发送错误;     无参数
#define DEV_EVT_ERROE_RECV     (1ul << 14)  //!< 事件: 接收错误;     无参数
#define DEV_EVT_COMPLETE_SEND  (1ul << 15)  //!< 事件: 发送完成;     无参数
#define DEV_EVT_COMPLETE_RECV  (1ul << 16)  //!< 事件: 接收完成;     无参数
#define DEV_EVT_TIMEOUT_SEND   (1ul << 17)  //!< 事件: 发送超时;     无参数
#define DEV_EVT_TIMEOUT_RECV   (1ul << 18)  //!< 事件: 接收超时;     无参数
#define DEV_EVT_ABORT_SEND     (1ul << 19)  //!< 事件: 终止发送;     无参数
#define DEV_EVT_ABORT_RECV     (1ul << 20)  //!< 事件: 终止接收;     无参数
#define DEV_EVT_FLUSH_SEND     (1ul << 21)  //!< 事件: 清洗发送缓存; 无参数
#define DEV_EVT_FLUSH_RECV     (1ul << 22)  //!< 事件: 清洗接收缓存; 无参数
#define DEV_EVT_BIT_DEF(n)     (1ul <<(23 + (n)))
#define DEV_EVT_BIT_CHK(n)     (31u >=(23 + (n)))


// devDriver_CB()
/*******************************************************************************
 * @}
 * @addtogroup              Command_Codes
 * @{
 ******************************************************************************/
//lint -emacro(835, DEV_CMD_*)
//lint -emacro(845, DEV_CMD_*)
// devDriver_CTRL()

#define DEV_CMD_ARG_DEF(n)   (0x0000000Ful & (n))
#define DEV_CMD_ARG_MSK      (0x0000000Ful)
#define DEV_CMD_GRP_MSK      (0x0000FF00ul)
#define DEV_CMD_BIT_MSK      (0xFFFF0000ul)
#define DEV_CMD_GROUP_ECMD   (0x00000100ul) //!< 命令码扩展    ; 参数3:扩展命令码
#define DEV_CMD_GROUP_FUNC   (0x00000200ul) //!< 函数调用扩展  ; 参数3:函数指针
#define DEV_CMD_COUNT_SEND   (0x00000300ul) //!< 获取已发送字节; 无参数; 返回值:个数
#define DEV_CMD_COUNT_RECV   (0x00000400ul) //!< 获取已接收字节; 无参数; 返回值:个数
#define DEV_CMD_POWER_ON     (0x00000500ul) //!< 设备电源打开  ; 无参数; 无返回值
#define DEV_CMD_POWER_OFF    (0x00000600ul) //!< 设备电源关闭  ; 无参数; 无返回值
#define DEV_CMD_POWER_RST    (0x00000700ul) //!< 设备复位      ; 无参数; 无返回值
#define DEV_CMD_GRP______
#define DEV_CMD_GRP_DEF(n)   (0x00000800ul + ((n) << 8))
#define DEV_CMD_GRP_CHK(n)  ((0x00000800ul + ((n) << 8)) <= 0x0000FF00ul)

#define DEV_CMD_CALLBACK_FUN    (1ul << 16) //!< 事件配置; 参数2(opt): 未使用; 参数3:事件处理回调函数        ; 无返回值
#define DEV_CMD_TIMEOUT_SEND    (1ul << 17) //!< 超时配置; 参数2(opt):32b超时; 参数3:发布函数; 参数4:等待函数; 无返回值
#define DEV_CMD_TIMEOUT_RECV    (1ul << 18) //!< 超时配置; 参数2(opt):32b超时; 参数3:发布函数; 参数4:等待函数; 无返回值
#define DEV_CMD_ABORT_SEND      (1ul << 19) //!< 终止发送; 无参数; 无返回值
#define DEV_CMD_ABORT_RECV      (1ul << 20) //!< 终止接收; 无参数; 无返回值
#define DEV_CMD_FLUSH_SEND      (1ul << 21) //!< 清洗发送; 无参数; 无返回值
#define DEV_CMD_FLUSH_RECV      (1ul << 22) //!< 清洗接收; 无参数; 无返回值
#define DEV_CMD_BIT______
#define DEV_CMD_BIT_DEF(n)      (1ul <<(23 + (n)))
#define DEV_CMD_BIT_CHK(n)      (31u >=(23 + (n)))


// devDriver_CTRL()
/*******************************************************************************
 * @}
 * @addtogroup              Error_Codes
 * @{
 ******************************************************************************/

#define DEV_DRIVER_OK                0      //!< 操作成功
#define DEV_DRIVER_ERR_PARAMETER    -1      //!< 参数错误
#define DEV_DRIVER_ERR_UNSUPPORTED  -2      //!< 操作不支持
#define DEV_DRIVER_ERR_BUSY         -3      //!< 设备忙
#define DEV_DRIVER_ERR_TIMEOUT      -4      //!< 超时
#define DEV_DRIVER_ERR_ABORT        -5      //!< 终止发送
#define DEV_DRIVER_ERR_FLUSH        -6      //!< 清洗缓存
#define DEV_DRIVER_ERR_EMPTY        -7      //!< 缓存空
#define DEV_DRIVER_ERR_FULL         -8      //!< 缓存满
#define DEV_DRIVER_ERR(n)     (-(n) -9)     //!< 未知错误


/*******************************************************************************
 * @}
 * @addtogroup              Macros
 * @{
 ******************************************************************************/
/**
 * @brief DEV_CMD_GPR_BLOCK_BGN() : Group 命令块处理开始宏(不需使用";"语句结束符)
 * @brief DEV_CMD_GPR_BLOCK_END() : Group 命令块处理结束宏(不需使用";"语句结束符)
 * @brief DEV_CMD_GPR_CODE_BGN()  : Group 命令码处理开始宏(不需使用";"语句结束符)
 * @brief DEV_CMD_GPR_CODE_END()  : Group 命令码处理结束宏(不需使用";"语句结束符)
 */
#define DEV_CMD_GPR_BLOCK_BGN(cmd)      switch (cmd) { default: break;
#define DEV_CMD_GPR_CODE_BGN(code,cmd)  case(code): { (cmd) &= ~DEV_CMD_GRP_MSK;
#define DEV_CMD_GPR_CODE_END(code,cmd)  } break;
#define DEV_CMD_GPR_BLOCK_END(cmd)      }

/**
 * @brief DEV_CMD_BIT_BLOCK_BGN() : Bit 命令块处理开始宏(不需使用";"语句结束符)
 * @brief DEV_CMD_BIT_BLOCK_END() : Bit 命令块处理结束宏(不需使用";"语句结束符)
 * @brief DEV_CMD_BIT_CODE_BGN()  : Bit 命令码处理开始宏(不需使用";"语句结束符)
 * @brief DEV_CMD_BIT_CODE_END()  : Bit 命令码处理结束宏(不需使用";"语句结束符)
 */

#define DEV_CMD_BIT_BLOCK_BGN(cmd)      {
#define DEV_CMD_BIT_CODE_BGN(code,cmd)  if((cmd) & (code)) { (cmd) &= ~(code);
#define DEV_CMD_BIT_CODE_END(code,cmd)  }
#define DEV_CMD_BIT_BLOCK_END(cmd)      }


/**
 * DEV_EVT_VAR_ALLOC  : 分配事件变量
 * DEV_EVT_CODE_SET() : 设置事件码
 * DEV_EVT_FUN_CALL() : 调用事件处理函数
 */
#ifdef  DEV_DRIVER_INFO_USED
#define DEV_EVT_FUN_CALL(dev, ...)  if(((dev)->drvInfo->callback != NULL) && (evt_var != 0)) {             \
                                        (dev)->drvInfo->callback((dev)->device, (dev)->drvInfo->tx_opt     \
                                                                              , (dev)->drvInfo->rx_opt     \
                                                                              , evt_var                    \
                                                                              , ##__VA_ARGS__              \
                                                                );                                         \
                                    }
#define DEV_EVT_CODE_SET(code,arg)  BIT_MDF(evt_var, ((code) & ~DEV_EVT_ARG_MSK) |         DEV_EVT_ARG_MSK \
                                                   , ((code) & ~DEV_EVT_ARG_MSK) | (arg) & DEV_EVT_ARG_MSK \
                                           );
#define DEV_EVT_VAR_ALLOC           uint32_t  evt_var = 0;
#else
#define DEV_EVT_FUN_CALL(dev, ...)
#define DEV_EVT_CODE_SET(code,arg)
#define DEV_EVT_VAR_ALLOC
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif
/*****************************  END OF FILE  **********************************/

