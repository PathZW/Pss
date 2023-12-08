/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsAd5683GoWIN_M1.c
 * 负责人  ： zwj
 * 创建日期： 20200827
 * 版本号  ： v1.0
 * 文件描述： PSS AD5683驱动
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
/************************************ 头文件 ***********************************/
#include <string.h>
#include <math.h>
#include "devDriver_cfg.h"
#include "devDriver_DAC.h"
#include "devDriver_FpgaAD5683.h"
#include "emb_log.h"

#include "defines.h"
#include "emb_delay.h"
#include "board.h"              // Device's defines

/*----------------------------------- 声明区 ----------------------------------*/

/********************************** 变量声明区 *********************************/
//!< 状态(Run-Time)
typedef struct {
 //  int8_t                  excpt;      //!< 异常状态, 表示运行时发生异常
    uint8_t                 busy;       //!< 忙状态,   表示正在执行操作函数
 // uint8_t                 tx_busy;    //!< 正在发送, 表示正处于发送状态
 // uint8_t                 rx_busy;    //!< 正在接收, 表示正处于接收状态
  	uint8_t                 tx_to;      // 0表示发送到cmd_reg,1表示发送到FIFO
    uint8_t                 reserved[2];
} FpgaAD5683_STAT;

//!< 信息(Run-Time)
#ifdef FpgaAD5683_INFO_USED
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
} FpgaAD5683_INFO;
#endif
//!< 资源
typedef struct {
#ifdef FpgaAD5683_INFO_USED
    FpgaAD5683_INFO        *drvInfo;    //!< 信息(Run-Time)
#endif
    FpgaAD5683_STAT        *drvStat;    //!< 状态
    FpgaAD5683_REGS        *devRegs;    //!< 寄存器
    devDriver_t            *device;     //!< 驱动
} const FpgaAD5683_RESOURCE;



/********************************** 函数声明区 *********************************/

/********************************** 变量实现区 *********************************/

/********************************** 函数实现区 *********************************/
/**
 * 初始化, 不能在中断中调用
 *
 * @return     错误码
 */
static int32_t devInit(FpgaAD5683_RESOURCE *dev)
{
  UNUSED_VAR(dev);
	dev->devRegs->cmdReg  |= (PSS_MCU_DRVS_AD5683_0_MASK << PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET);
	dev->devRegs->cmdReg  |= (PSS_MCU_DRVS_AD5683_1_MASK << PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET);
	dev->devRegs->cmdReg  |= (PSS_MCU_DRVS_AD5683_2_MASK << PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET);
	dev->devRegs->cmdReg  |= 0x00498000; 
	return( DEV_DRIVER_OK );
}

/**
 * 反初始化, 不能在中断中调用
 *
 * @return     错误码
 */
static int32_t devUninit(FpgaAD5683_RESOURCE *dev)
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
static int32_t devSend(const void *data, size_t size, uint32_t opt, FpgaAD5683_RESOURCE *dev)
{ 
	uint32_t  num;
	float	*buff = (float*) data;
	uint16_t setVal = 0;
	UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);	UNUSED_VAR(dev);

	switch(dev->drvStat->tx_to)
	{
	default: break;
	case DEV_AD5683Send_TO_CMD_REG:
	{
		for(num = 0;  num < (size / sizeof(buff[0]));  num++)
		{
			setVal = (uint16_t)DEV_AD5683_VAL_ESTIMATE(buff[num]);
			DEV_AD5683_WRITE_INPUTREG_UPDATA(opt,setVal,dev->devRegs->cmdReg);
		}
		if (num != 0) {
			return( num * sizeof(buff[0]) ); 
		}	
	}
		break;
	case DEV_AD5683Send_TO_FIFO:
	{
		for(num = 0;  num < (size / sizeof(buff[0]));  num++)
		{
			if (DEV_AD5683_FIFOISFULL || DEV_AD5683_FIFOISBUSY) {
				break;				
			}
			setVal = (uint16_t)DEV_AD5683_VAL_ESTIMATE(buff[num]);
			DEV_AD5683_WRITE_INPUTREG_UPDATA(opt,setVal,dev->devRegs->fifoReg);
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
static int32_t devRecv(void *data, size_t size, uint32_t opt, FpgaAD5683_RESOURCE *dev)
{   
	UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);
	
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}


/**
 * 设备控制, 能够在中断中调用
 * @note 1. 不要返回 DEV_DRIVER_ERR_UNSUPPORTED, 已经被 xxxx_n_Ctrl() 处理
 * @note 2. dev->drvStat->busy 不需处理, 已经被 xxxx_n_Ctrl() 处理
 * @note 3. dev->drvStat 和 dev->drvInfo 需要处理
 */
static int32_t devCtrl(uint32_t *cmd, uint32_t opt, va_list args, FpgaAD5683_RESOURCE *dev)
{
	int32_t 	result = DEV_DRIVER_OK;
	DEV_EVT_VAR_ALLOC								//!< 分配事件变量
	UNUSED_VAR(opt);  UNUSED_VAR(args);  UNUSED_VAR(dev);

	/***************************************************************************
	 * 设备相关 GRP 命令
	 */
	DEV_CMD_GPR_BLOCK_BGN(*cmd)
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_START, *cmd)
	{	//---------------------- 启动DAC -------------------------------
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_START, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_STOP, *cmd)
	{	//---------------------- 停止DAC -------------------------------
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_STOP, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_MODE_CFG, *cmd)
	{	//---------------------- 配置DAC工作模式 ------------------------
		switch(va_arg(args, uint32_t))
		{
		default: break;
		case DEV_DAC_MODE_TRIG:
			return( result );
		}
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_MODE_CFG, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_DATA, *cmd)
	{	//---------------------- 数据寄存器为 Send() 目标 ---------------
		dev->drvStat->tx_to = DEV_AD5683Send_TO_CMD_REG;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_DATA, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_REAL, *cmd)
	{	//---------------------- 实际值FIFO为 Send() 目标 ---------------
		dev->drvStat->tx_to = DEV_AD5683Send_TO_FIFO;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_REAL, *cmd)

	/***************************************************************************
	 * 框架共同 GRP 命令
	 */
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_RST, *cmd)
	{	//---------------------- 设备复位 -------------------------------
		dev->devRegs->resetReg = 1; //写入任何值
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
	
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_FLUSH_SEND, *cmd)
	{	//---------------------- 清洗发送 -------------------------------
		dev->devRegs->resetReg = 1; 
		DEV_EVT_CODE_SET(DEV_EVT_FLUSH_SEND, 0) 	//!< 设置事件码
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_FLUSH_SEND, *cmd)
	DEV_CMD_BIT_BLOCK_END(*cmd)

	DEV_EVT_FUN_CALL(dev)							//!< 调用事件处理函数

	return( DEV_DRIVER_ERR_UNSUPPORTED );	
}



/*******************************************************************************
 * @}
 * @addtogroup              Chip_1
 * @{
 ******************************************************************************/
 #ifdef  FpgaAD5683_1_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD5683_1;
static FpgaAD5683_STAT         _FpgaAD5683_1_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD5683_INFO_USED
static FpgaAD5683_INFO         _FpgaAD5683_1_info = { 0 };  //!< 信息(Run-Time)
#endif

static FpgaAD5683_RESOURCE     _FpgaAD5683_1_res  =         //!< 资源
{
#ifdef FpgaAD5683_INFO_USED
	(FpgaAD5683_INFO	  *)  &_FpgaAD5683_1_info,
#endif
	(FpgaAD5683_STAT	  *)  &_FpgaAD5683_1_stat,

    (FpgaAD5683_REGS      *)    FpgaAD5683_1_BASE_ADDR,
    (devDriver_t          *)  &_devDriver_FpgaAD5683_1
};

static int32_t FpgaAD5683_1_Init(void) {
    return devInit(&_FpgaAD5683_1_res);
}
static int32_t FpgaAD5683_1_Uninit(void) {
    return devUninit(&_FpgaAD5683_1_res);
}
static int32_t FpgaAD5683_1_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD5683_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_1_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5683_1_res);
    _FpgaAD5683_1_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_1_Recv(void *data, size_t size, uint32_t opt)
{
	int32_t  ret;
    if (_FpgaAD5683_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_1_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5683_1_res);
    _FpgaAD5683_1_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_1_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD5683_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_1_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD5683_1_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD5683_1_res.drvStat->busy = 0;
    return( ret );

}

devDriver_t   _devDriver_FpgaAD5683_1 = {
    FpgaAD5683_1_Send
  , FpgaAD5683_1_Recv
  , FpgaAD5683_1_Ctrl
  , FpgaAD5683_1_Init
  , FpgaAD5683_1_Uninit
  , TO_STRING(_devDriver_FpgaAD5683_1)
};	

#endif

/*******************************************************************************
 * @}
 * @addtogroup              Chip_2
 * @{
 ******************************************************************************/
 #ifdef  FpgaAD5683_1_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD5683_2;
static FpgaAD5683_STAT         _FpgaAD5683_2_stat = { 0 };  //!< 状态(Run-Time)

#ifdef FpgaAD5683_INFO_USED
static FpgaAD5683_INFO         _FpgaAD5683_2_info = { 0 };  //!< 信息(Run-Time)
#endif

static FpgaAD5683_RESOURCE     _FpgaAD5683_2_res  =         //!< 资源
{
#ifdef FpgaAD5683_INFO_USED
	(FpgaAD5683_INFO	  *)  &_FpgaAD5683_2_info,
#endif
	(FpgaAD5683_STAT	  *)  &_FpgaAD5683_2_stat,

    (FpgaAD5683_REGS      *)    FpgaAD5683_2_BASE_ADDR,
    (devDriver_t          *)  &_devDriver_FpgaAD5683_2
};

static int32_t FpgaAD5683_2_Init(void) {
    return devInit(&_FpgaAD5683_2_res);
}
static int32_t FpgaAD5683_2_Uninit(void) {
    return devUninit(&_FpgaAD5683_2_res);
}
static int32_t FpgaAD5683_2_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD5683_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_2_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5683_2_res);
    _FpgaAD5683_2_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_2_Recv(void *data, size_t size, uint32_t opt)
{
	int32_t  ret;
    if (_FpgaAD5683_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_2_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5683_2_res);
    _FpgaAD5683_2_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_2_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD5683_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_2_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD5683_2_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD5683_2_res.drvStat->busy = 0;
    return( ret );

}

devDriver_t   _devDriver_FpgaAD5683_2 = {
    FpgaAD5683_2_Send
  , FpgaAD5683_2_Recv
  , FpgaAD5683_2_Ctrl
  , FpgaAD5683_2_Init
  , FpgaAD5683_2_Uninit
  , TO_STRING(_devDriver_FpgaAD5683_2)
};	

#endif

/*******************************************************************************
 * @}
 * @addtogroup              Chip_3
 * @{
 ******************************************************************************/
 #ifdef  FpgaAD5683_1_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD5683_3;
static FpgaAD5683_STAT         _FpgaAD5683_3_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD5683_INFO_USED
static FpgaAD5683_INFO         _FpgaAD5683_3_info = { 0 };  //!< 信息(Run-Time)
#endif

static FpgaAD5683_RESOURCE     _FpgaAD5683_3_res  =         //!< 资源
{
#ifdef FpgaAD5683_INFO_USED
	(FpgaAD5683_INFO	  *)  &_FpgaAD5683_3_info,
#endif
	(FpgaAD5683_STAT	  *)  &_FpgaAD5683_3_stat,

    (FpgaAD5683_REGS      *)    FpgaAD5683_3_BASE_ADDR,
    (devDriver_t          *)  &_devDriver_FpgaAD5683_3
};

static int32_t FpgaAD5683_3_Init(void) {
    return devInit(&_FpgaAD5683_3_res);
}
static int32_t FpgaAD5683_3_Uninit(void) {
    return devUninit(&_FpgaAD5683_3_res);
}
static int32_t FpgaAD5683_3_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD5683_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_3_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5683_3_res);
    _FpgaAD5683_3_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_3_Recv(void *data, size_t size, uint32_t opt)
{
	int32_t  ret;
    if (_FpgaAD5683_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_3_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5683_3_res);
    _FpgaAD5683_3_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_3_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD5683_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_3_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD5683_3_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD5683_3_res.drvStat->busy = 0;
    return( ret );

}

devDriver_t   _devDriver_FpgaAD5683_3 = {
    FpgaAD5683_3_Send
  , FpgaAD5683_3_Recv
  , FpgaAD5683_3_Ctrl
  , FpgaAD5683_3_Init
  , FpgaAD5683_3_Uninit
  , TO_STRING(_devDriver_FpgaAD5683_3)
};	

#endif

/*******************************************************************************
 * @}
 * @addtogroup              Chip_4
 * @{
 ******************************************************************************/
 #ifdef  FpgaAD5683_1_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD5683_4;
static FpgaAD5683_STAT         _FpgaAD5683_4_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD5683_INFO_USED
static FpgaAD5683_INFO         _FpgaAD5683_4_info = { 0 };  //!< 信息(Run-Time)
#endif

static FpgaAD5683_RESOURCE     _FpgaAD5683_4_res  =         //!< 资源
{
#ifdef FpgaAD5683_INFO_USED
	(FpgaAD5683_INFO	  *)  &_FpgaAD5683_4_info,
#endif
	(FpgaAD5683_STAT	  *)  &_FpgaAD5683_4_stat,

    (FpgaAD5683_REGS      *)    FpgaAD5683_4_BASE_ADDR,
    (devDriver_t          *)  &_devDriver_FpgaAD5683_4
};

static int32_t FpgaAD5683_4_Init(void) {
    return devInit(&_FpgaAD5683_4_res);
}
static int32_t FpgaAD5683_4_Uninit(void) {
    return devUninit(&_FpgaAD5683_4_res);
}
static int32_t FpgaAD5683_4_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD5683_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_4_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5683_4_res);
    _FpgaAD5683_4_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_4_Recv(void *data, size_t size, uint32_t opt)
{
	int32_t  ret;
    if (_FpgaAD5683_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_4_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5683_4_res);
    _FpgaAD5683_4_res.drvStat->busy = 0;
    return( ret );

}
static int32_t FpgaAD5683_4_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD5683_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5683_4_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD5683_4_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD5683_4_res.drvStat->busy = 0;
    return( ret );

}

devDriver_t   _devDriver_FpgaAD5683_4 = {
    FpgaAD5683_4_Send
  , FpgaAD5683_4_Recv
  , FpgaAD5683_4_Ctrl
  , FpgaAD5683_4_Init
  , FpgaAD5683_4_Uninit
  , TO_STRING(_devDriver_FpgaAD5683_4)
};	

#endif




