/*******************************************************************************
 * @file    devDriver_FpgaAD5761.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   通用设备驱动 - DAC - FPGA AD5761, 可以在"devDriver_cfg.h"配置.\n
 *          引用“extern devDriver_t devDriver(FpgaAD5761, x)” x为 1 ~ 4 即可使用.
 *******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) Wuhan Precise Co.,Ltd</center></h2>
 *
 * The information and source code contained herein is the exclusive
 * property of Wuhan Precise Electronic Technology Co.,Ltd. And may
 * not be disclosed, examined or reproduced in whole or in part without
 * explicit written authorization from the company.
 *
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/3/26 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                        // Repeated include file 'xxxx'
//lint -e750                        // local macro 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro((740), va_start, va_arg)
//lint ++d"va_start(ap, parmN) = ( *((char**)&(ap)) = ((char*)&(parmN) + sizeof(parmN)) )"
//lint ++d"va_arg(ap, type)    = ( *((*((type**)&(ap)))++) )"
//lint ++d"va_end(ap)          = ( (void)(ap) )"

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG   "FpgaAD5761"
#include "emb_log.h"
#include "devDriver_cfg.h"
#include "devDriver_DAC.h"
#include "devDriver_FpgaAD5761.h"
#undef   DEV_DRIVER_INFO_USED   // 禁用 xxxx_INFO

#include "defines.h"
#include "emb_delay.h"
#include "board.h"              // Device's defines
#include "os_abstract.h"

#if defined(FpgaAD5761_1_BASE_ADDR) || defined(FpgaAD5761_2_BASE_ADDR) \
 || defined(FpgaAD5761_3_BASE_ADDR) || defined(FpgaAD5761_4_BASE_ADDR)
/******************************************************************************/
/**
 * @addtogroup Device_Driver
 * @{
 * @addtogroup DAC
 * @{
 * @addtogroup FpgaAD5761
 * @{
 * @addtogroup              Private_Defines
 * @{
 ******************************************************************************/
//lint -esym(754, FpgaAD5761_*)

//!< 状态(Run-Time)
typedef struct {
 //   int8_t                  excpt;      //!< 异常状态, 表示运行时发生异常
    uint8_t                 busy;       //!< 忙状态,   表示正在执行操作函数
 // uint8_t                 tx_busy;    //!< 正在发送, 表示正处于发送状态
 // uint8_t                 rx_busy;    //!< 正在接收, 表示正处于接收状态
		uint8_t                 tx_to;      // 0表示发送到cmd_reg,1表示发送到FIFO
    uint8_t                 reserved[2];
} FpgaAD5761_STAT;

#ifdef DEV_DRIVER_INFO_USED
//!< 信息(Run-Time)
typedef struct {
    devDriver_CB            callback;   //!< 事件处理回调函数
    uint32_t                rx_opt;     //!< opt
    uint32_t                rx_timeout; //!< 超时时间
    devDriver_POST          rx_post;    //!< 发布函数
    devDriver_WAIT          rx_wait;    //!< 等待函数
    uint32_t                tx_opt;     //!< opt
    uint32_t                tx_timeout; //!< 超时时间
    devDriver_POST          tx_post;    //!< 发布函数
    devDriver_WAIT          tx_wait;    //!< 等待函数
} FpgaAD5761_INFO;
#endif

//!< 资源
typedef struct {
#ifdef DEV_DRIVER_INFO_USED
    FpgaAD5761_INFO        *drvInfo;    //!< 信息(Run-Time)
#endif
    FpgaAD5761_STAT        *drvStat;    //!< 状态
    FpgaAD5761_REGS        *devRegs;    //!< 寄存器
    devDriver_t            *device;     //!< 驱动
} const FpgaAD5761_RESOURCE;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * 初始化, 不能在中断中调用
 *
 * @return     错误码
 */
static int32_t devInit(FpgaAD5761_RESOURCE *dev)
{
	uint16_t cfg = 0;
	PSS_MCU_DRVS_AD5761_PARA_T para = {0};
  UNUSED_VAR(dev);
	
	para.ovr = PSS_MCU_DRVS_AD5761_CTRL_OVR_DI;
	para.b2c = PSS_MCU_DRVS_AD5761_CTRL_B2Z_SB;
	para.ets = PSS_MCU_DRVS_AD5761_CTRL_ETS_ON;
	para.pv  = PSS_MCU_DRVS_AD5761_CTRL_PV_ZERO;
	para.ra  = PSS_MCU_DRVS_AD5761_CTRL_RA_0_P5;
	para.cv  = PSS_MCU_DRVS_AD5761_CTRL_CV_ZERO;
	cfg |= (para.ra&0x7);
	cfg |= (para.pv&0x3)<<PSS_MCU_DRVS_AD5761_CTRL_PV_OFFSET;
	cfg |= para.ets<<PSS_MCU_DRVS_AD5761_CTRL_ETS_OFFSET;
	cfg |= para.b2c<<PSS_MCU_DRVS_AD5761_CTRL_B2Z_OFFSET;
	cfg |= para.ovr<<PSS_MCU_DRVS_AD5761_CTRL_OVR_OFFSET;
	cfg |= (para.cv&0x3)<<PSS_MCU_DRVS_AD5761_CTRL_CV_OFFSET;
	cfg |= _CMD_WRITE_CONTROL_REG_<<16;
	cfg |= (PSS_MCU_DRVS_AD5761_0_MASK << PSS_MCU_DRVS_AD5761_NUM_CTRL_OFFSET);
	cfg |= (PSS_MCU_DRVS_AD5761_1_MASK << PSS_MCU_DRVS_AD5761_NUM_CTRL_OFFSET);
	cfg |= (PSS_MCU_DRVS_AD5761_2_MASK << PSS_MCU_DRVS_AD5761_NUM_CTRL_OFFSET);
	cfg |= 0x00498000; 
	dev->devRegs->mCmdData = cfg;
	
  return( DEV_DRIVER_OK );
}


/**
 * 反初始化, 不能在中断中调用
 *
 * @return     错误码
 */
static int32_t devUninit(FpgaAD5761_RESOURCE *dev)
{
    UNUSED_VAR(dev);
    LOG_DEBUG("Unsupported on devDriver Uninit()!");
    return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * 发送数据
 *
 * @param[in]  data     发送缓存指针
 * @param[in]  size     发送缓存大小(以字节为单位)
 * @param[in]  opt      可选操作码
 *
 * @retval    (>=0)     成功发送数据字节数
 * @retval    (< 0)     错误码
 *
 * @note    1. 已配置超时, 与事件处理回调函数无关, 该操作为同步IO, 不能在中断中调用
 * @note    2. 未配置超时, 且事件处理回调函数为空, 该操作为同步IO, 不能在中断中调用
 * @note    3. 未配置超时, 且事件处理回调函数非空, 该操作为异步IO, 能够在中断中调用
 * @note    4. 不要返回 DEV_DRIVER_ERR_BUSY, 已经被 xxxx_n_Ctrl() 处理
 */
static int32_t devSend(const void *data, size_t size, uint32_t opt, FpgaAD5761_RESOURCE *dev)
{ 
	uint32_t  num;
  float   *buff = (float *)data;
	uint16_t setVal=0;
  UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);

	switch(dev->drvStat->tx_to)
	{
	default: break;
	case DEV_AD5761Send_TO_CMD_REG:
	{
		for(num = 0;  num < (size / sizeof(buff[0]));  num++)
		{
			setVal = DEV_AD5761_VAL_ESTIMATE(buff[num]);
			DEV_AD5761_WRITE_INPUTREG_UPDATA(opt,setVal,dev->devRegs->mCmdData);
		}
		if (num != 0) {
				return( num * sizeof(buff[0]) ); 
		}		
	}
		break;
	case DEV_AD5761Send_TO_FIFO:
	{
		for(num = 0;  num < (size / sizeof(buff[0]));  num++)
		{
				if (DEV_AD5761_FIFOISFULL || DEV_AD5761_FIFOISBUSY) {
						break;              
				}
			setVal = DEV_AD5761_VAL_ESTIMATE(buff[num]);
			DEV_AD5761_WRITE_INPUTREG_UPDATA(opt,setVal,dev->devRegs->mFIFOout);
		}
		if (num != 0) {
			return( num * sizeof(buff[0]) ); 
		}
	}
		break;
	}

    return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * 接收数据
 *
 * @param[in]  data     接收缓存指针
 * @param[in]  size     接收缓存大小(以字节为单位)
 * @param[in]  opt      可选操作码
 *
 * @retval    (>=0)     成功接收数据字节数
 * @retval    (< 0)     错误码
 *
 * @note    1. 已配置超时, 与事件处理回调函数无关, 该操作为同步IO, 不能在中断中调用
 * @note    2. 未配置超时, 且事件处理回调函数为空, 该操作为同步IO, 不能在中断中调用
 * @note    3. 未配置超时, 且事件处理回调函数非空, 该操作为异步IO, 能够在中断中调用
 * @note    4. 不要返回 DEV_DRIVER_ERR_BUSY, 已经被 xxxx_n_Ctrl() 处理
 */
static int32_t devRecv(void *data, size_t size, uint32_t opt, FpgaAD5761_RESOURCE *dev)
{   
	UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);
	LOG_DEBUG("Unsupported on devDriver Send()!");
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}


/**
 * 设备控制, 能够在中断中调用
 *
 * @param[in]  cmd      命令码
 * @param[in]  opt      可选操作码 
 *
 * @retval    (>=0)     命令返回值
 * @retval    (< 0)     错误码
 *
 * @note 1. 不要返回 DEV_DRIVER_ERR_BUSY, 已经被 xxxx_n_Ctrl() 处理
 * @note 2. 不要返回 DEV_DRIVER_ERR_UNSUPPORTED, 已经被 xxxx_n_Ctrl() 处理
 */
static int32_t devCtrl(uint32_t *cmd, uint32_t opt, va_list args, FpgaAD5761_RESOURCE *dev)
{
  int32_t     result = DEV_DRIVER_OK;
	DEV_EVT_VAR_ALLOC                               //!< 分配事件变量
	UNUSED_VAR(opt);  UNUSED_VAR(args);  UNUSED_VAR(dev);

	/***************************************************************************
	 * 设备相关 GRP 命令
	 */
	DEV_CMD_GPR_BLOCK_BGN(*cmd)
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_START, *cmd)
	{   //---------------------- 启动DAC -------------------------------
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_START, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_STOP, *cmd)
	{   //---------------------- 停止DAC -------------------------------
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_STOP, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_MODE_CFG, *cmd)
	{   //---------------------- 配置DAC工作模式 ------------------------
		switch(va_arg(args, uint32_t))
		{
		default: break;
		case DEV_DAC_MODE_TRIG:
		return( result );
		}
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_MODE_CFG, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_DATA, *cmd)
	{   //---------------------- 数据寄存器为 Send() 目标 ---------------
		dev->drvStat->tx_to = DEV_AD5761Send_TO_CMD_REG;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_DATA, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_ORGN, *cmd)
	{   //---------------------- 原始值FIFO为 Send() 目标 ---------------
		dev->drvStat->tx_to = DEV_AD5761Send_TO_FIFO;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_ORGN, *cmd)

	/***************************************************************************
	 * 框架共同 GRP 命令
	 */
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_RST, *cmd)
	{   //---------------------- 设备复位 -------------------------------
		
		dev->devRegs->mFIFOreset = DEV_AD5761_RESET_FIFO_FLG;
		DEV_EVT_CODE_SET(DEV_EVT_POWER_CTRL, 2)     //!< 设置事件码
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_POWER_RST, *cmd)
	DEV_CMD_GPR_BLOCK_END(*cmd)

	/***************************************************************************
	 * 设备相关 BIT 命令
	 */
	DEV_CMD_BIT_BLOCK_BGN(*cmd)
	
	/***************************************************************************
	 * 框架共同 BIT 命令
	 */
	#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_CALLBACK_FUN, *cmd)
	{   //---------------------- 事件配置 -------------------------------
			dev->drvInfo->callback = va_arg(args, devDriver_CB);
	return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_CALLBACK_FUN, *cmd)
	#endif

	#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_SEND, *cmd)
	{   //---------------------- 超时配置 -------------------------------
			dev->drvInfo->tx_timeout = opt;
			dev->drvInfo->tx_post    = va_arg(args, devDriver_POST);
			dev->drvInfo->tx_wait    = va_arg(args, devDriver_WAIT);
	return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_SEND, *cmd)
	#endif

	#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_RECV, *cmd)
	{   //---------------------- 超时配置 -------------------------------
			dev->drvInfo->rx_timeout = opt;
			dev->drvInfo->rx_post    = va_arg(args, devDriver_POST);
			dev->drvInfo->rx_wait    = va_arg(args, devDriver_WAIT);
	return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_RECV, *cmd)
	#endif

	DEV_CMD_BIT_CODE_BGN(DEV_CMD_FLUSH_SEND, *cmd)
	{   //---------------------- 清洗发送 -------------------------------
		dev->devRegs->mFIFOreset = DEV_AD5761_RESET_FIFO_FLG; //写入任何值
		DEV_EVT_CODE_SET(DEV_EVT_FLUSH_SEND, 0)     //!< 设置事件码
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_FLUSH_SEND, *cmd)
	DEV_CMD_BIT_BLOCK_END(*cmd)

	DEV_EVT_FUN_CALL(dev)                           //!< 调用事件处理函数

	return( DEV_DRIVER_ERR_UNSUPPORTED );
}


/*******************************************************************************
 * @}
 * @addtogroup              Chip_1
 * @{
 ******************************************************************************/
#ifdef  FpgaAD5761_1_BASE_ADDR
extern devDriver_t             _devDriver_FpgaAD5761_1;     //!< 导出 devDriver 对象

static FpgaAD5761_STAT         _FpgaAD5761_1_stat = { 0 };  //!< 状态(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_1_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_1_res  = {       //!< 资源
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_1_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_1_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_1_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_1
};

static int32_t FpgaAD5761_1_Init(void) {
    return devInit(&_FpgaAD5761_1_res);                     //!< 初始化
}
static int32_t FpgaAD5761_1_Uninit(void) {
    return devUninit(&_FpgaAD5761_1_res);                   //!< 反初始化
}
static int32_t FpgaAD5761_1_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 发送数据
    if (_FpgaAD5761_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_1_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_1_res);
    _FpgaAD5761_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_1_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 接收数据
    if (_FpgaAD5761_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_1_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_1_res);
    _FpgaAD5761_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_1_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< 设备控制
    va_list  args;
    if (_FpgaAD5761_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_1_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_1_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_1_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_1 = {                   //!< 导出 devDriver 对象
    FpgaAD5761_1_Send
  , FpgaAD5761_1_Recv
  , FpgaAD5761_1_Ctrl
  , FpgaAD5761_1_Init
  , FpgaAD5761_1_Uninit
  , TO_STRING(_devDriver_FpgaAD5761_1)
};
#endif
/*******************************************************************************
 * @}
 * @addtogroup              Chip_2
 * @{
 ******************************************************************************/
#ifdef  FpgaAD5761_2_BASE_ADDR
extern devDriver_t             _devDriver_FpgaAD5761_2;     //!< 导出 devDriver 对象

static FpgaAD5761_STAT         _FpgaAD5761_2_stat = { 0 };  //!< 状态(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_2_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_2_res  = {       //!< 资源
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_2_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_2_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_2_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_2
};

static int32_t FpgaAD5761_2_Init(void) {
    return devInit(&_FpgaAD5761_2_res);                     //!< 初始化
}
static int32_t FpgaAD5761_2_Uninit(void) {
    return devUninit(&_FpgaAD5761_2_res);                   //!< 反初始化
}
static int32_t FpgaAD5761_2_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 发送数据
    if (_FpgaAD5761_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_2_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_2_res);
    _FpgaAD5761_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_2_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 接收数据
    if (_FpgaAD5761_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_2_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_2_res);
    _FpgaAD5761_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_2_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< 设备控制
    va_list  args;
    if (_FpgaAD5761_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_2_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_2_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_2_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_2 = {                   //!< 导出 devDriver 对象
    FpgaAD5761_2_Send
  , FpgaAD5761_2_Recv
  , FpgaAD5761_2_Ctrl
  , FpgaAD5761_2_Init
  , FpgaAD5761_2_Uninit
  , TO_STRING(_devDriver_FpgaAD5761_2)
};
#endif
/*******************************************************************************
 * @}
 * @addtogroup              Chip_3
 * @{
 ******************************************************************************/
#ifdef  FpgaAD5761_3_BASE_ADDR
extern devDriver_t             _devDriver_FpgaAD5761_3;     //!< 导出 devDriver 对象

static FpgaAD5761_STAT         _FpgaAD5761_3_stat = { 0 };  //!< 状态(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_3_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_3_res  = {       //!< 资源
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_3_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_3_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_3_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_3
};

static int32_t FpgaAD5761_3_Init(void) {
    return devInit(&_FpgaAD5761_3_res);                     //!< 初始化
}
static int32_t FpgaAD5761_3_Uninit(void) {
    return devUninit(&_FpgaAD5761_3_res);                   //!< 反初始化
}
static int32_t FpgaAD5761_3_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 发送数据
    if (_FpgaAD5761_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_3_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_3_res);
    _FpgaAD5761_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_3_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 接收数据
    if (_FpgaAD5761_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_3_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_3_res);
    _FpgaAD5761_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_3_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< 设备控制
    va_list  args;
    if (_FpgaAD5761_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_3_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_3_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_3_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_3 = {                   //!< 导出 devDriver 对象
    FpgaAD5761_3_Send
  , FpgaAD5761_3_Recv
  , FpgaAD5761_3_Ctrl
  , FpgaAD5761_3_Init
  , FpgaAD5761_3_Uninit
  , TO_STRING(_devDriver_FpgaAD5761_3)
};
#endif
/*******************************************************************************
 * @}
 * @addtogroup              Chip_4
 * @{
 ******************************************************************************/
#ifdef  FpgaAD5761_4_BASE_ADDR
extern devDriver_t             _devDriver_FpgaAD5761_4;     //!< 导出 devDriver 对象

static FpgaAD5761_STAT         _FpgaAD5761_4_stat = { 0 };  //!< 状态(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_4_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_4_res  = {       //!< 资源
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_4_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_4_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_4_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_4
};

static int32_t FpgaAD5761_4_Init(void) {
    return devInit(&_FpgaAD5761_4_res);                     //!< 初始化
}
static int32_t FpgaAD5761_4_Uninit(void) {
    return devUninit(&_FpgaAD5761_4_res);                   //!< 反初始化
}
static int32_t FpgaAD5761_4_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 发送数据
    if (_FpgaAD5761_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_4_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_4_res);
    _FpgaAD5761_4_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_4_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< 接收数据
    if (_FpgaAD5761_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_4_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_4_res);
    _FpgaAD5761_4_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_4_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< 设备控制
    va_list  args;
    if (_FpgaAD5761_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_4_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_4_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_4_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_4 = {                   //!< 导出 devDriver 对象
    FpgaAD5761_4_Send
  , FpgaAD5761_4_Recv
  , FpgaAD5761_4_Ctrl
  , FpgaAD5761_4_Init
  , FpgaAD5761_4_Uninit
  , TO_STRING(_devDriver_FpgaAD5761_4)
};
#endif
/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 * @}
 * @}
 *****************************  END OF FILE  **********************************/
#endif

