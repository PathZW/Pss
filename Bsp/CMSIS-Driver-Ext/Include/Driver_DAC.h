/*******************************************************************************
 * @file    Driver_DAC.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/8/21
 * @brief
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/8/21 -- Linghu -- the first version
 ******************************************************************************/

#include "stddef.h"
#include "stdint.h"     // #include "Driver_Common.h"

/******************************************************************************/
#ifndef __DRIVER_DAC_H__
#define __DRIVER_DAC_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/

#ifndef ARM_DRIVER_VERSION_MAJOR_MINOR
#define ARM_DRIVER_VERSION_MAJOR_MINOR(major,minor)   (((major) << 8) | (minor))

/**
\brief Driver Version
*/
typedef struct _ARM_DRIVER_VERSION {
  uint16_t api;                         //!< API version
  uint16_t drv;                         //!< Driver version
} ARM_DRIVER_VERSION;

/* General return codes, execution_status */
#define ARM_DRIVER_OK                 0 //!< Operation succeeded 
#define ARM_DRIVER_ERROR             -1 //!< Unspecified error
#define ARM_DRIVER_ERROR_BUSY        -2 //!< Driver is busy
#define ARM_DRIVER_ERROR_TIMEOUT     -3 //!< Timeout occurred
#define ARM_DRIVER_ERROR_UNSUPPORTED -4 //!< Operation not supported
#define ARM_DRIVER_ERROR_PARAMETER   -5 //!< Parameter error
#define ARM_DRIVER_ERROR_SPECIFIC    -6 //!< Start of driver specific errors 

/**
\brief General power states
*/ 
typedef enum _ARM_POWER_STATE {
  ARM_POWER_OFF,                        //!< Power off: no operation possible
  ARM_POWER_LOW,                        //!< Low Power mode: retain state, detect and signal wake-up events
  ARM_POWER_FULL                        //!< Power on: full operation at maximum performance
} ARM_POWER_STATE;
#endif//ARM_DRIVER_VERSION_MAJOR_MINOR


/******************************************************************************/
//lint -emacro(835, ARM_DAC_API_VERSION, ARM_DAC_DRV_VERSION)

#define ARM_DAC_API_VERSION  ARM_DRIVER_VERSION_MAJOR_MINOR(1,0) // API version

#define  ARM_Driver_DAC_(n)     _ARM_Driver_DAC_(n)
#define _ARM_Driver_DAC_(n)          Driver_DAC##n

#define ARM_DAC_CHANNEL_ALL     (0xFFFFFFFFul)      //!< DAC 所有通道

/*-------------------------- DAC_Event ---------------------------------------*/
#define ARM_DAC_EVENT_COMPLETE      (1ul)           //!< 转换完成
#define ARM_DAC_EVENT_ERROR         (3ul)           //!< 检测到错误

/*----------------------------------------------------------------------------*/
/**
 * @brief DAC Status
 */
typedef volatile struct _ARM_DAC_STATUS {
    uint32_t    busy        : 1;    //!< 繁忙标志
    uint32_t    error       : 1;    //!< 错误标志(下次操作开始时清除)
    uint32_t    reserved    : 30;
} ARM_DAC_STATUS;

/**
 * @brief DAC Driver Capabilities.
 */
typedef struct _ARM_DAC_CAPABILITIES {
    uint32_t    channels    : 10;   //!< DAC 通道数, 1 ~ 512
    uint32_t    resolution  : 6;    //!< DAC 分辨率, 1 ~ 32
    uint32_t    event       : 1;    //!< 是否支持事件模式
    uint32_t    trigger     : 1;    //!< 是否支持触发模式
    uint32_t    reserved    : 14;   //!< Reserved (must be zero)
} ARM_DAC_CAPABILITIES;

/**
 * Pointer to \ref ARM_DAC_SignalEvent : 信号处理回调函数.
 */
typedef void (*ARM_DAC_SignalEvent_t)(uint32_t event, uint32_t arg);

/**
 * @brief Access structure of the DAC Driver
 */
typedef struct _ARM_DRIVER_DAC {
    ARM_DRIVER_VERSION   (*GetVersion)     (void);                                      //!< Pointer to \ref ARM_DAC_GetVersion      : 获取驱动程序版本信息.
    ARM_DAC_CAPABILITIES (*GetCapabilities)(void);                                      //!< Pointer to \ref ARM_DAC_GetCapabilities : 获取驱动程序功能信息(与实现相关).
    int32_t              (*Initialize)     (ARM_DAC_SignalEvent_t cb_event);            //!< Pointer to \ref ARM_DAC_Initialize      : 初始化
    int32_t              (*Uninitialize)   (void);                                      //!< Pointer to \ref ARM_DAC_Uninitialize    : 取消初始化.
    int32_t              (*PowerControl)   (ARM_POWER_STATE state);                     //!< Pointer to \ref ARM_DAC_PowerControl    : 控制 DAC 电源.
    int32_t              (*SetValue)       (uint32_t ch, uint32_t    val);              //!< Pointer to \ref ARM_DAC_SetValue        : 设置 DAC 输出值(直接输出模式).
    int32_t              (*SetData)        (uint32_t ch, const void *data, size_t num); //!< Pointer to \ref ARM_DAC_SetData         : 设置 DAC 转换值(触发输出模式).
    int32_t              (*Trigger)        (uint32_t ch);                               //!< Pointer to \ref ARM_DAC_Trigger         : 触发 DAC 转换.
    int32_t              (*Abort)          (uint32_t ch);                               //!< Pointer to \ref ARM_DAC_Abort           : 终止 DAC 转换.
    int32_t              (*GetDataCount)   (uint32_t ch);                               //!< Pointer to \ref ARM_DAC_GetDataCount    : 获取 DAC 已转换的数据条目数
    ARM_DAC_STATUS       (*GetStatus)      (void);                                      //!< Pointer to \ref ARM_DAC_GetStatus       : 获取 DAC 状态.
} const ARM_DRIVER_DAC;


/*------------------ Function documentation ----------------------------------*/
/**
 * @fn      ARM_DRIVER_VERSION ARM_DAC_GetVersion(void)
 * @brief   获取驱动程序版本信息(与实现相关).
 *
 * @return  \ref ARM_DRIVER_VERSION
 */

/**
 * @fn      ARM_DAC_CAPABILITIES ARM_DAC_GetCapabilities(void)
 * @brief   获取驱动程序功能信息(与实现相关).
 *
 * @return  \ref ARM_DAC_CAPABILITIES
 */

/**
 * @fn      int32_t ARM_DAC_Initialize(ARM_DAC_SignalEvent_t cb_event)
 * @brief   初始化
 *
 * @param[in]  cb_event   Pointer to \ref ARM_DAC_SignalEvent
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_Uninitialize(void)
 * @brief   取消初始化.
 *
 * @return  \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_PowerControl(ARM_POWER_STATE state)
 * @brief   控制 DAC 电源
 *
 * @param[in]  state   电源状态
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_SetValue(uint32_t ch, uint32_t val)
 * @brief   设置 DAC 输出值(直接输出模式).
 *
 * @param[in]  ch    DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 * @param[in]  val   将要转换的值, 对齐方式:
 *                   @arg 分辨率(01 ~~ 08):  8bit 左对齐
 *                   @arg 分辨率(09 ~~ 16): 16bit 左对齐
 *                   @arg 分辨率(17 ~~ 32): 32bit 左对齐
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_SetData(uint32_t ch, const void *data, size_t num)
 * @brief   设置 DAC 转换值(触发输出模式).
 *
 * @param[in]  ch     DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 * @param[in]  data   输出数据条目缓存, NULL表示无输出数据, 类型与对齐方式参考 \ref ARM_DAC_SetValue
 * @param[in]  num    输出数据条目数(非 data 字节数)
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_Trigger(uint32_t ch)
 * @brief   触发 DAC 转换.
 *
 * @param[in]  ch   DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_Abort(uint32_t ch)
 * @brief   终止 DAC 转换.
 *
 * @param[in]  ch   DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_GetDataCount(uint32_t ch)
 * @brief   获取 DAC 已转换的数据条目数.
 *
 * @param[in]  ch   DAC 通道, 最小通道为0
 *
 * @return     已转换的数据条目数量或, 小于(0)为错误状态码 \ref execution_status
 */

/**
 * @fn      ARM_DAC_STATUS ARM_DAC_GetStatus(void)
 * @brief   获取 DAC 状态.
 *
 * @return  DAC status \ref ARM_DAC_STATUS
 */

/**
 * @fn      void ARM_DAC_SignalEvent(uint32_t event, uint32_t arg)
 * @brief   信号处理回调函数, 处理驱动程序操作期间发生的一个或多个信号.
 *
 * @param[in]  event   事件通知码, \ref DAC_Event
 * @param[in]  arg     DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 */


/*****************************  END OF FILE  **********************************/
#ifdef  __cplusplus
}
#endif
#endif  // __DRIVER_DAC_H__
/*****************************  END OF FILE  **********************************/

