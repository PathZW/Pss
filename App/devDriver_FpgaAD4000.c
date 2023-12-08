/*******************************************************************************
 * @file    devDriver_FpgaAD4000.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   通用设备驱动 - ADC - FPGA AD4000, 可以在"devDriver_cfg.h"配置.\n
 *          引用“extern devDriver_t devDriver(FpgaAD4000, x)” x为 1 ~ 4 即可使用.
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
#define  LOG_TAG   "FpgaAD4000"
#include "emb_log.h"
#include "devDriver_cfg.h"
#include "devDriver_ADC.h"
#include "devDriver_FpgaAD4000.h"

#include "defines.h"
#include "emb_delay.h"
#include "board.h"              // Device's defines


#if defined(FpgaAD4000_1_BASE_ADDR) || defined(FpgaAD4000_2_BASE_ADDR) \
 || defined(FpgaAD4000_3_BASE_ADDR) || defined(FpgaAD4000_4_BASE_ADDR)
//#define FpgaAD4000_INFO_USED    // 启用 FpgaAD4000_INFO
/******************************************************************************/
/**
 * @addtogroup Device_Driver
 * @{
 * @addtogroup ADC
 * @{
 * @addtogroup FpgaAD4000
 * @{
 * @addtogroup              Private_Defines
 * @{
 ******************************************************************************/
//lint -esym(754, FpgaAD4000_*, device)

#ifdef  FpgaAD4000_INFO_USED
#define EvtMDF(evt, clr, set)   BIT_MDF(evt, clr, set)
#define EvtCLR(evt, clr     )   BIT_CLR(evt, clr     )
#define EvtSET(evt,      set)   BIT_SET(evt,      set)
#else
#define EvtMDF(evt, clr, set)
#define EvtCLR(evt, clr     )
#define EvtSET(evt,      set)
#endif

//!< 状态(Run-Time)
typedef struct {
    uint8_t                 busy;       //!< 忙状态,   表示正在执行操作函数
  //int8_t                  excpt;      //!< 异常状态, 表示运行时发生异常
  //uint8_t                 tx_busy;    //!< 正在发送, 表示正处于发送状态
  //uint8_t                 rx_busy;    //!< 正在接收, 表示正处于接收状态
    uint8_t                 rx_from;    //!< Recv() 数据源; 1:校准值FIFO; 2:原始值FIFO; 其他:采样寄存器
    uint8_t                 reserved[2];
} FpgaAD4000_STAT;

#ifdef FpgaAD4000_INFO_USED
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
} FpgaAD4000_INFO;
#endif

//!< 资源
typedef struct {
#ifdef FpgaAD4000_INFO_USED
    FpgaAD4000_INFO        *drvInfo;    //!< 信息(Run-Time)
#endif
    FpgaAD4000_STAT        *drvStat;    //!< 状态
    FpgaAD4000_REGS        *devRegs;    //!< 寄存器
#ifdef FpgaAD4000_SYNC_BASE_ADDR
    FpgaAD4000_SYNC_REGS   *devSync;    //!< 同步控制寄存器
#endif
    devDriver_t            *device;     //!< 驱动
} const FpgaAD4000_RESOURCE;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * 初始化, 不能在中断中调用
 */
static int32_t devInit(FpgaAD4000_RESOURCE *dev)
{
	UNUSED_VAR(dev);
	return( DEV_DRIVER_OK );
}

/**
 * 反初始化, 不能在中断中调用
 */
static int32_t devUninit(FpgaAD4000_RESOURCE *dev)
{
	UNUSED_VAR(dev);
	LOG_DEBUG("Unsupported on devDriver Uninit()!");
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * 发送数据, 无超时情况下可在中断调用
 * @note 1. dev->drvStat->busy 不需处理, 已经被 xxxx_n_Send() 处理
 * @note 2. dev->drvStat 和 dev->drvInfo 需要处理
 */
static int32_t devSend(const void *data, size_t size, uint32_t opt, FpgaAD4000_RESOURCE *dev)
{  
	UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);
	LOG_DEBUG("Unsupported on devDriver Send()!");
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * 接收数据, 无超时情况下可在中断调用
 * @note 1. dev->drvStat->busy 不需处理, 已经被 xxxx_n_Recv() 处理
 * @note 2. dev->drvStat 和 dev->drvInfo 需要处理
 */
static int32_t devRecv(void *data, size_t size, uint32_t opt, FpgaAD4000_RESOURCE *dev)
{
	size_t      num;
  uint32_t   *buff = (uint32_t *)data;
	UNUSED_VAR(opt);
	switch(dev->drvStat->rx_from)
	{
	case DEV_AD4000RECV_FROM_SAMPREG:
		buff[0] = dev->devRegs->mSamOrgVal;
		return(sizeof(buff[0]));
	case DEV_AD4000RECV_FROM_CHKFIFO:
	{
		for(num = 0;  num < (size / sizeof(buff[0]));  num++)
		{
			if (DEV_AD4000RECV_CHKFIFO_EMPTY) {
					break;              
			}
			buff[num] = dev->devRegs->mFIFOmear; // * 2.5f / 32767.0f;
		}
		if (num != 0) {
			return( num * sizeof(buff[0]) ); 
		}			
	}
		return( DEV_DRIVER_ERR_EMPTY );
	case DEV_AD4000RECV_FROM_ORGFIFO:
	{
		for(num = 0;  num < (size / sizeof(buff[0]));  num++)
		{
				if (DEV_AD4000RECV_ORGFIFO_EMPTY) {
						break;              
				}
				buff[num] = dev->devRegs->mFIFOorgn; // * 2.5f / 32767.0f;
		}
		if (num != 0) {
				return( num * sizeof(buff[0]) ); //lint !e713
		}		
	}
		return( DEV_DRIVER_ERR_EMPTY );		
	default: break;
	}
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * 设备控制, 能够在中断中调用
 * @note 1. 不要返回 DEV_DRIVER_ERR_UNSUPPORTED, 已经被 xxxx_n_Ctrl() 处理
 * @note 2. dev->drvStat->busy 不需处理, 已经被 xxxx_n_Ctrl() 处理
 * @note 3. dev->drvStat 和 dev->drvInfo 需要处理
 */
static int32_t devCtrl(uint32_t *cmd, uint32_t opt, va_list args, FpgaAD4000_RESOURCE *dev)
{
	int32_t 	result = DEV_DRIVER_OK;
	DEV_EVT_VAR_ALLOC								//!< 分配事件变量
	UNUSED_VAR(opt);  UNUSED_VAR(args);  UNUSED_VAR(dev);
	
	/***************************************************************************
	* 设备相关 GRP 命令
	*/
	DEV_CMD_GPR_BLOCK_BGN(*cmd)
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_START, *cmd)
	{	//---------------------- 启动DAC -------------------------------
		return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_START, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_STOP, *cmd)
	{	//---------------------- 停止DAC -------------------------------
		return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_STOP, *cmd)
	
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_MODE_CFG, *cmd)
	{	//---------------------- ADC工作模式-------------------------------
		switch(va_arg(args, uint32_t))
		{
		default: break;
		case DEV_ADC_MODE_NORM:
			dev->devRegs->mSamMode = DEV_ADC_MODE_NORM_VALUE;
			break;
		case DEV_ADC_MODE_TRIG:
			dev->devRegs->mSamMode = DEV_ADC_MODE_TRIG_VALUE;
			break;
		}
		return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_MODE_CFG, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_FILT_CFG, *cmd)
	{	//---------------------- ADC滤波参数 -------------------------------
		switch(va_arg(args, uint32_t))
		{
		default: break;
		case DEV_ADC_FILT_NONE:
			dev->devRegs->mSamAvgNum = DEV_ADC_FILT_NONE_VALUE;
			break;
		case DEV_ADC_FILT_AVG:
			dev->devRegs->mSamAvgNum = va_arg(args, uint32_t);
			break;
		}
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_FILT_CFG, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_SYNC_CFG, *cmd)
	{	//---------------------- ADC同步参数 -------------------------------
		switch(va_arg(args, uint32_t))
		{
		default: break;
		case DEV_ADC_SYNC_NONE:
			return( DEV_DRIVER_ERR_PARAMETER );
		case DEV_ADC_SYNC_EN:
			dev->devSync->mSyncStart = va_arg(args, uint32_t);
			break;
		case DEV_ADC_SYNC_DIS:
			dev->devSync->mSyncStop = va_arg(args, uint32_t);
			break;
		case DEV_ADC_SYNC_INTV:
			return( DEV_DRIVER_ERR_PARAMETER );
		case DEV_ADC_SYNC_TXIV:
			dev->devSync->mTxInterval = FpgaAD4000_NS_TO_CYC(va_arg(args, uint32_t));
			break;
		}
		return( result );	   
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_SYNC_CFG, *cmd)
	
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RECV_DATA, *cmd)
	{	//---------------------- 采样寄存器为 Recv() 来源 -------------------------------
		dev->drvStat->rx_from = DEV_AD4000RECV_FROM_SAMPREG;
		return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_DATA, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RECV_MEAR, *cmd)
	{	//---------------------- 校准值FIFO为 Recv() 来源 -------------------------------
		dev->drvStat->rx_from = DEV_AD4000RECV_FROM_CHKFIFO;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_MEAR, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RECV_ORGN, *cmd)
	{	//---------------------- 原始值FIFO为 Recv() 来源 -------------------------------
		dev->drvStat->rx_from = DEV_AD4000RECV_FROM_ORGFIFO;
		return( result );	   
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_ORGN, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_POINT_CFG, *cmd)
	{	//---------------------- ADC采样点数 -------------------------------
		dev->devRegs->mSamPoint = va_arg(args, uint32_t);
		return( result );	   
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_POINT_CFG, *cmd)
	
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_DELAY_CFG, *cmd)
	{	//---------------------- ADC采样延时 -------------------------------
		dev->devRegs->mSamDlyCyc = FpgaAD4000_NS_TO_CYC(va_arg(args, uint32_t));
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_DELAY_CFG, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_CALB_CFG, *cmd)
	{	//---------------------- ADC校准参数 -------------------------------
		dev->devRegs->mCalKm  = va_arg(args, uint32_t);
		dev->devRegs->mCalBm  = va_arg(args, uint32_t);
		dev->devRegs->mCalM10 = va_arg(args, uint32_t);
		return( result );	   
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_CALB_CFG, *cmd)
	
	/***************************************************************************
	 * 框架共同 GRP 命令
	 */
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_RST, *cmd)
	{	//---------------------- 设备复位 -------------------------------
	  
		dev->devRegs->mFIFOreset = DEV_ADC_RESET_ORGFIFOREG | DEV_ADC_RESET_CHKFIFOREG;
		DEV_EVT_CODE_SET(DEV_EVT_POWER_CTRL, 2) 	//!< 设置事件码
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
	{	//---------------------- 事件配置 -------------------------------
		dev->drvInfo->callback = va_arg(args, devDriver_CB);
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_CALLBACK_FUN, *cmd)
#endif

#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_SEND, *cmd)
	{	//---------------------- 超时配置 -------------------------------
		dev->drvInfo->tx_timeout = opt;
		dev->drvInfo->tx_post	 = va_arg(args, devDriver_POST);
		dev->drvInfo->tx_wait	 = va_arg(args, devDriver_WAIT);
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_SEND, *cmd)
#endif

#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_RECV, *cmd)
	{	//---------------------- 超时配置 -------------------------------
		dev->drvInfo->rx_timeout = opt;
		dev->drvInfo->rx_post	 = va_arg(args, devDriver_POST);
		dev->drvInfo->rx_wait	 = va_arg(args, devDriver_WAIT);
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_RECV, *cmd)
#endif

	DEV_CMD_BIT_CODE_BGN(DEV_CMD_FLUSH_RECV, *cmd)
	{	//---------------------- 清洗接收 -------------------------------
		if(*cmd & DEV_ARG_ADC_FLUSH_MEAR)
		{
			dev->devRegs->mFIFOreset = DEV_ADC_RESET_CHKFIFOREG;  // 清除ADC校准值FIFO
		}
		else if(*cmd & DEV_ARG_ADC_FLUSH_ORGN)
		{
			dev->devRegs->mFIFOreset = DEV_ADC_RESET_ORGFIFOREG;  // 清除ADC校准值FIFO
		}
		else
		{
			dev->devRegs->mFIFOreset = DEV_ADC_RESET_CHKFIFOREG | DEV_ADC_RESET_ORGFIFOREG;
		}
		
		DEV_EVT_CODE_SET(DEV_EVT_FLUSH_RECV, 0) 	//!< 设置事件码
		
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_FLUSH_RECV, *cmd)
	DEV_CMD_BIT_BLOCK_END(*cmd)

	DEV_EVT_FUN_CALL(dev)							//!< 调用事件处理函数
	
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/*******************************************************************************
 * @}
 * @addtogroup              Chip_1
 * @{
 ******************************************************************************/
#ifdef  FpgaAD4000_1_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD4000_1;
static FpgaAD4000_STAT         _FpgaAD4000_1_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_1_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_1_res  =         //!< 资源
{
#ifdef FpgaAD4000_INFO_USED
    (FpgaAD4000_INFO      *)  &_FpgaAD4000_1_info,
#endif
    (FpgaAD4000_STAT      *)  &_FpgaAD4000_1_stat,
    (FpgaAD4000_REGS      *)    FpgaAD4000_1_BASE_ADDR,
#ifdef FpgaAD4000_SYNC_BASE_ADDR
    (FpgaAD4000_SYNC_REGS *)    FpgaAD4000_SYNC_BASE_ADDR,
#endif
    (devDriver_t          *)  &_devDriver_FpgaAD4000_1
};

static int32_t FpgaAD4000_1_Init(void) {
    return devInit(&_FpgaAD4000_1_res);
}
static int32_t FpgaAD4000_1_Uninit(void) {
    return devUninit(&_FpgaAD4000_1_res);
}
static int32_t FpgaAD4000_1_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_1_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD4000_1_res);
    _FpgaAD4000_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_1_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_1_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD4000_1_res);
    _FpgaAD4000_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_1_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD4000_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_1_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD4000_1_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD4000_1_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD4000_1 = {
    FpgaAD4000_1_Send
  , FpgaAD4000_1_Recv
  , FpgaAD4000_1_Ctrl
  , FpgaAD4000_1_Init
  , FpgaAD4000_1_Uninit
  , TO_STRING(_devDriver_FpgaAD4000_1)
};

#endif
/*******************************************************************************
 * @}
 * @addtogroup              Chip_2
 * @{
 ******************************************************************************/
#ifdef  FpgaAD4000_2_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD4000_2;
static FpgaAD4000_STAT         _FpgaAD4000_2_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_2_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_2_res  =         //!< 资源
{
#ifdef FpgaAD4000_INFO_USED
    (FpgaAD4000_INFO      *)  &_FpgaAD4000_2_info,
#endif
    (FpgaAD4000_STAT      *)  &_FpgaAD4000_2_stat,
    (FpgaAD4000_REGS      *)    FpgaAD4000_2_BASE_ADDR,
#ifdef FpgaAD4000_SYNC_BASE_ADDR
    (FpgaAD4000_SYNC_REGS *)    FpgaAD4000_SYNC_BASE_ADDR,
#endif
    (devDriver_t          *)  &_devDriver_FpgaAD4000_2
};

static int32_t FpgaAD4000_2_Init(void) {
    return devInit(&_FpgaAD4000_2_res);
}
static int32_t FpgaAD4000_2_Uninit(void) {
    return devUninit(&_FpgaAD4000_2_res);
}
static int32_t FpgaAD4000_2_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_2_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD4000_2_res);
    _FpgaAD4000_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_2_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_2_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD4000_2_res);
    _FpgaAD4000_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_2_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD4000_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_2_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD4000_2_res);

    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD4000_2_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD4000_2 = {
    FpgaAD4000_2_Send
  , FpgaAD4000_2_Recv
  , FpgaAD4000_2_Ctrl
  , FpgaAD4000_2_Init
  , FpgaAD4000_2_Uninit
  , TO_STRING(_devDriver_FpgaAD4000_2)
};

#endif
/*******************************************************************************
 * @}
 * @addtogroup              Chip_3
 * @{
 ******************************************************************************/
#ifdef  FpgaAD4000_3_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD4000_3;
static FpgaAD4000_STAT         _FpgaAD4000_3_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_3_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_3_res  =         //!< 资源
{
#ifdef FpgaAD4000_INFO_USED
    (FpgaAD4000_INFO      *)  &_FpgaAD4000_3_info,
#endif
    (FpgaAD4000_STAT      *)  &_FpgaAD4000_3_stat,
    (FpgaAD4000_REGS      *)    FpgaAD4000_3_BASE_ADDR,
#ifdef FpgaAD4000_SYNC_BASE_ADDR
    (FpgaAD4000_SYNC_REGS *)    FpgaAD4000_SYNC_BASE_ADDR,
#endif
    (devDriver_t          *)  &_devDriver_FpgaAD4000_3
};

static int32_t FpgaAD4000_3_Init(void) {
    return devInit(&_FpgaAD4000_3_res);
}
static int32_t FpgaAD4000_3_Uninit(void) {
    return devUninit(&_FpgaAD4000_3_res);
}
static int32_t FpgaAD4000_3_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_3_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD4000_3_res);
    _FpgaAD4000_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_3_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_3_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD4000_3_res);
    _FpgaAD4000_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_3_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD4000_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_3_res.drvStat->busy = 1;
    va_start(args, opt);
    
    ret = devCtrl(&cmd, opt, args, &_FpgaAD4000_3_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD4000_3_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD4000_3 = {
    FpgaAD4000_3_Send
  , FpgaAD4000_3_Recv
  , FpgaAD4000_3_Ctrl
  , FpgaAD4000_3_Init
  , FpgaAD4000_3_Uninit
  , TO_STRING(_devDriver_FpgaAD4000_3)
};

#endif
/*******************************************************************************
 * @}
 * @addtogroup              Chip_4
 * @{
 ******************************************************************************/
#ifdef  FpgaAD4000_4_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD4000_4;
static FpgaAD4000_STAT         _FpgaAD4000_4_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_4_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_4_res  =         //!< 资源
{
#ifdef FpgaAD4000_INFO_USED
    (FpgaAD4000_INFO      *)  &_FpgaAD4000_4_info,
#endif
    (FpgaAD4000_STAT      *)  &_FpgaAD4000_4_stat,
    (FpgaAD4000_REGS      *)    FpgaAD4000_4_BASE_ADDR,
#ifdef FpgaAD4000_SYNC_BASE_ADDR
    (FpgaAD4000_SYNC_REGS *)    FpgaAD4000_SYNC_BASE_ADDR,
#endif
    (devDriver_t          *)  &_devDriver_FpgaAD4000_4
};

static int32_t FpgaAD4000_4_Init(void) {
    return devInit(&_FpgaAD4000_4_res);
}
static int32_t FpgaAD4000_4_Uninit(void) {
    return devUninit(&_FpgaAD4000_4_res);
}
static int32_t FpgaAD4000_4_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_4_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD4000_4_res);
    _FpgaAD4000_4_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_4_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD4000_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_4_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD4000_4_res);
    _FpgaAD4000_4_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD4000_4_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD4000_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD4000_4_res.drvStat->busy = 1;
    va_start(args, opt);
    ret = devCtrl(&cmd, opt, args, &_FpgaAD4000_4_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD4000_4_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD4000_4 = {
    FpgaAD4000_4_Send
  , FpgaAD4000_4_Recv
  , FpgaAD4000_4_Ctrl
  , FpgaAD4000_4_Init
  , FpgaAD4000_4_Uninit
  , TO_STRING(_devDriver_FpgaAD4000_4)
};

#endif
/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 * @}
 * @}
 *****************************  END OF FILE  **********************************/
#endif

