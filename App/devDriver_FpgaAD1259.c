/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaAds1259.c
 * 负责人  ： myk
 * 创建日期： 20211008
 * 版本号  ： v1.0
 * 文件描述： PSS ADS1259驱动实现
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
#include <string.h>
#include "devDriver_FpgaAD1259.h"
#include "devDriver_cfg.h"
#include "devDriver_ADC.h"
#include "defines.h"
#include "emb_delay.h"
#include "board.h"              // Device's defines

/*----------------------------------- 声明区 ----------------------------------*/
//!< 状态(Run-Time)
typedef struct {
    uint8_t                 busy;       //!< 忙状态,   表示正在执行操作函数
  //int8_t                  excpt;      //!< 异常状态, 表示运行时发生异常
  //uint8_t                 tx_busy;    //!< 正在发送, 表示正处于发送状态
  //uint8_t                 rx_busy;    //!< 正在接收, 表示正处于接收状态
    uint8_t                 rx_from;    //!< Recv() 数据源; 1:校准值FIFO; 2:原始值FIFO; 其他:采样寄存器
    uint8_t                 reserved[2];
} FpgaAD1259_STAT;

#ifdef FpgaAD1259_INFO_USED
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
} FpgaAD1259_INFO;
#endif

//!< 资源
typedef struct {
#ifdef FpgaAD1259_INFO_USED
    FpgaAD1259_INFO        *drvInfo;    //!< 信息(Run-Time)
#endif
    FpgaAD1259_STAT        *drvStat;    //!< 状态
    FpgaAD1259_REGS        *devRegs;    //!< 寄存器
    devDriver_t            *device;     //!< 驱动
} const FpgaAD1259_RESOURCE;


/********************************** 变量声明区 *********************************/


/********************************** 函数声明区 *********************************/


/********************************** 变量实现区 *********************************/
/********************************** 函数实现区 *********************************/


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
 /*整形转浮点型*/
static void BitsToF32(float *val, uint32_t Data)
{
#if 1
    uint8_t msb = (uint8_t)(Data >> 24);
    uint8_t mid = (uint8_t)(Data >> 16);
    uint8_t lsb = (uint8_t)(Data >> 8);
    uint8_t chksum = (uint8_t)(Data);
    int i32Val = 0;
    int i32Temp = 0;
    uint8_t chksumCompute = 0;
    uint8_t chksumWithoutOOR = 0;

    /* 校验 */
    chksumCompute = msb + mid + lsb + 0x9B;
    /* 处理out-of-range bit */
    chksumCompute &= 0x7f;
    chksumWithoutOOR = 0x7f & chksum;
    if(chksumWithoutOOR != chksumCompute) /* 校验失败 */
    {
        *val = 0.0f;
        return;
    }
    /* 转换为浮点数 */
    i32Temp |= lsb;
    i32Temp |= mid << 8;
    i32Temp |= msb << 16;

    /* 符号扩展 */
    if(0x80 == (0x80 & msb)) /* 符号位为1 */
    {
        i32Temp |= 0xff000000;
    }

    /* 无需大小端转换 */
    i32Val = i32Temp;
    *val = i32Val * _I32_TO_F32_STEP_;
	#endif
}

static void dev_AD1259_sendToADC(uint32_t cmd,FpgaAD1259_RESOURCE *dev)
{
	dev->devRegs->cmdReg = cmd; 
	while(!(dev->devRegs->SpiSendReg & _SPI_SEND_FLAG)); 
	dev->devRegs->ClearSpiReg &= (~_SPI_SEND_FLAG); 
}

static float  dev_AD1259_ReadFromDataReg(FpgaAD1259_RESOURCE *dev)
{
	uint32_t u32_AD1259_OriginData = 0;
	float    f32_int2float_val= 0;
	dev_AD1259_sendToADC(_CMD_START_,dev);
	while(!(dev->devRegs->SpiSendReg & _SPI_RECV_FLAG)); 
	u32_AD1259_OriginData = (uint32_t)dev->devRegs->realDataReg; 
	dev->devRegs->ClearSpiReg &= (~_SPI_RECV_FLAG); 
	BitsToF32(&f32_int2float_val, u32_AD1259_OriginData); 
	return f32_int2float_val;
}

static float  dev_AD1259_ReadFromFifoReg(FpgaAD1259_RESOURCE *dev)
{
	uint32_t u32_AD1259_OriginData = 0;
	float    f32_int2float_val= 0;
	dev_AD1259_sendToADC(_CMD_START_,dev);
	while(!(dev->devRegs->SpiSendReg & _SPI_RECV_FLAG)); 
	u32_AD1259_OriginData = (uint32_t)dev->devRegs->dataReg; 
	dev->devRegs->ClearSpiReg &= (~_SPI_RECV_FLAG); 
	BitsToF32(&f32_int2float_val, u32_AD1259_OriginData); 
	return f32_int2float_val;
}

/**
 * 初始化, 不能在中断中调用
 */
static int32_t devInit(FpgaAD1259_RESOURCE *dev)
{
	UNUSED_VAR(dev);
	 _PARA_T_ wrPara = {0};
	
	uint8_t op = 0; 											 /* 操作数固定为0 表示只读一个寄存器的值*/
	uint8_t cfg0 = 0x00;
	uint8_t cfg1 = 0x00;
	uint8_t cfg2 = 0x00;
	uint8_t rCfg = 0;
	/* step0: 给定芯片的配置 */
	wrPara.rbias  = _REG_CFG0_RBIAS_DISABLE_;
	wrPara.spi	  = _REG_CFG0_SPI_TO_ENABLE_;
	wrPara.flag   = _REG_CFG1_FLAG_ENABLE_;
	wrPara.chksum = _REG_CFG1_CHKSUM_ENABLE_;
	wrPara.sinc2  = _REG_CFG1_SINC2_DISABLE_;
	wrPara.extref = _REG_CFG1_EXTREF_EXTERNAL_;
	wrPara.delay  = _REG_CFG1_DELAY_NO_DELAY_;
	wrPara.syncout= _REG_CFG2_SYNCOUT_DISABLE_;
	wrPara.pulse  = _REG_CFG2_MODE_PULSE_;
	wrPara.dr	  = _REG_CFG2_DATA_Rate_10PSP;

	dev_AD1259_sendToADC(_CMD_RESET_<<DEV_AD_1259_SEND_CMD_OFFSET,dev);
	dev_AD1259_sendToADC(_CMD_SDATAC_<<DEV_AD_1259_SEND_CMD_OFFSET,dev);

	//Config0 register
	cfg0 |= wrPara.rbias  << _REG_CFG0_RBIAS_OFFSET_;
	cfg0 |= wrPara.spi	  << _REG_CFG0_SPI_TO_OFFSET_;
	dev_AD1259_sendToADC(_CMD_WREG_0<<DEV_AD_1259_SEND_CMD_OFFSET | op<<8 | cfg0 ,dev);
	//Readback Config0 register
	dev->devRegs->cmdReg = _CMD_RREG_0<<16 | op<<8 | rCfg;
	while(!(dev->devRegs->SpiSendReg & _SPI_RECV_FLAG));
	rCfg = (uint8_t)(dev->devRegs->ReadBackReg);
	dev->devRegs->ClearSpiReg &= (~_SPI_RECV_FLAG);
	if(rCfg != cfg0)
	{
			return (DEV_DRIVER_ERR_PARAMETER);
	}	

	//Config1 register
	cfg1 |= wrPara.flag   << _REG_CFG1_FLAG_OFFSET_;
	cfg1 |= wrPara.chksum << _REG_CFG1_CHKSUM_OFFSET_;
	cfg1 |= wrPara.sinc2  << _REG_CFG1_SINC2_OFFSET_;
	cfg1 |= wrPara.extref << _REG_CFG1_EXTREF_OFFSET_;
	cfg1 |= wrPara.delay  << _REG_CFG1_DELAY_OFFSET_;
	dev_AD1259_sendToADC(_CMD_WREG_1<<DEV_AD_1259_SEND_CMD_OFFSET | op<<8 |cfg1,dev);
	//Readback Config1 register
	dev->devRegs->cmdReg = _CMD_RREG_1<<16 | op<<8 | rCfg;
	while(!(dev->devRegs->SpiSendReg & _SPI_RECV_FLAG));
	rCfg = (uint8_t)(dev->devRegs->ReadBackReg);
	dev->devRegs->ClearSpiReg &= (~_SPI_RECV_FLAG);
	if(rCfg != cfg1)
	{
			return (DEV_DRIVER_ERR_PARAMETER);
	}	

	//Config2 register
	cfg2 |= wrPara.syncout<< _REG_CFG2_SYNCOUT_OFFSET_;
	cfg2 |= wrPara.pulse  << _REG_CFG2_MODE_OFFSET_;
	cfg2 |= wrPara.dr	  << _REG_CFG2_DR_OFFSET_;
	cfg2 |= _REG_CFG2_DATA_NREADY_;
	dev_AD1259_sendToADC(_CMD_WREG_2<<DEV_AD_1259_SEND_CMD_OFFSET | op<<8 | cfg2,dev);
	//Readback Config2 register
	dev->devRegs->cmdReg = _CMD_RREG_2<<16 | op<<8 | rCfg;
	while(!(dev->devRegs->SpiSendReg & _SPI_RECV_FLAG));
	rCfg = (uint8_t)(dev->devRegs->ReadBackReg);
	dev->devRegs->ClearSpiReg &= (~_SPI_RECV_FLAG);
	if(rCfg != cfg2)
	{
			return (DEV_DRIVER_ERR_PARAMETER);
	}		
	dev->devRegs->InitSuccessReg = 1;
	return( DEV_DRIVER_OK );
}

/**
 * 反初始化, 不能在中断中调用
 */
static int32_t devUninit(FpgaAD1259_RESOURCE *dev)
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
static int32_t devSend(const void *data, size_t size, uint32_t opt, FpgaAD1259_RESOURCE *dev)
{  
   UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);
   LOG_DEBUG("Unsupported on devDriver Send()!");
   return( DEV_DRIVER_ERR_UNSUPPORTED );
}

static int32_t devRecv(void *data, size_t size, uint32_t opt, FpgaAD1259_RESOURCE *dev)
{
	size_t      num;
  float   *buff = data;
  UNUSED_VAR(opt);

  switch(dev->drvStat->rx_from)
  {
  default: break;
	case DEV_AD1259RECV_FROM_SAMPREG:
	{
		buff[0] = dev_AD1259_ReadFromDataReg(dev);
		return( sizeof(buff[0]));		
	}
	case DEV_AD1259RECV_FROM_CHKFIFO:
	{
		for(num = 0;  num < (size / sizeof(buff[0]));  num++)
		{
				if (DEV_AD1259RECV_CHKFIFO_NULL) {
						break;              
				}
				buff[num] = dev_AD1259_ReadFromFifoReg(dev); // * 2.5f / 32767.0f;
		}
		if (num != 0) {
				return( num * sizeof(buff[0]) ); 
		}
		return( DEV_DRIVER_ERR_EMPTY );		
	}
  }
  return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * 设备控制, 能够在中断中调用
 * @note 1. 不要返回 DEV_DRIVER_ERR_UNSUPPORTED, 已经被 xxxx_n_Ctrl() 处理
 * @note 2. dev->drvStat->busy 不需处理, 已经被 xxxx_n_Ctrl() 处理
 * @note 3. dev->drvStat 和 dev->drvInfo 需要处理
 */
static int32_t devCtrl(uint32_t *cmd, uint32_t opt, va_list args, FpgaAD1259_RESOURCE *dev)
{
	int32_t     result = DEV_DRIVER_OK;
	DEV_EVT_VAR_ALLOC                               //!< 分配事件变量
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

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RATE_CFG, *cmd)
	{	//---------------------- ADC采样速率 -------------------------------
		dev_AD1259_sendToADC(_CMD_WREG_2 << DEV_AD_1259_SEND_CMD_OFFSET \
		                   | (va_arg(args, uint32_t) << _REG_CFG2_DR_OFFSET_),dev);
	    return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RATE_CFG, *cmd)
	
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RECV_DATA, *cmd)
	{	//---------------------- 采样寄存器为 Recv() 来源 -------------------------------
		dev->drvStat->rx_from = DEV_AD1259RECV_FROM_SAMPREG;
	    return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_DATA, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RECV_MEAR, *cmd)
	{	//---------------------- 校准值FIFO为 Recv() 来源 -------------------------------
		dev->drvStat->rx_from = DEV_AD1259RECV_FROM_CHKFIFO;
	    return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_MEAR, *cmd)
	
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_DELAY_CFG, *cmd)
	{	//---------------------- ADC采样延时 -------------------------------
		dev_AD1259_sendToADC(_CMD_WREG_1 << DEV_AD_1259_SEND_CMD_OFFSET \
	                       | (va_arg(args, uint32_t)<<_REG_CFG1_DELAY_OFFSET_),dev);
	    return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_DELAY_CFG, *cmd)

	/***************************************************************************
	 * 框架共同 GRP 命令
	 */
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_RST, *cmd)
	{   //---------------------- 设备复位 -------------------------------
		
		dev_AD1259_sendToADC(_CMD_RESET_ << DEV_AD_1259_SEND_CMD_OFFSET,dev);
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
	
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_ABORT_RECV, *cmd)
	{	//---------------------- 清洗接收 -------------------------------
		dev->devRegs->fifoReset = DEV_AD_1259_CLEAR_FIFO_FLAG; 
		DEV_EVT_CODE_SET(DEV_CMD_ABORT_RECV, 0) 	//!< 设置事件码
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_ABORT_RECV, *cmd)
	DEV_CMD_BIT_BLOCK_END(*cmd)
	
	DEV_EVT_FUN_CALL(dev)							//!< 调用事件处理函数
	return( DEV_DRIVER_ERR_UNSUPPORTED );

}



/*******************************************************************************
 * @}
 * @addtogroup              Chip_1
 * @{
 ******************************************************************************/
#ifdef  FpgaAD1259_1_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD1259_1;
static FpgaAD1259_STAT         _FpgaAD1259_1_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD1259_INFO_USED
static FpgaAD1259_INFO         _FpgaAD1259_1_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD1259_RESOURCE     _FpgaAD1259_1_res  =         //!< 资源
{
#ifdef FpgaAD1259_INFO_USED
    (FpgaAD1259_INFO      *)  &_FpgaAD1259_1_info,
#endif
    (FpgaAD1259_STAT      *)  &_FpgaAD1259_1_stat,
    (FpgaAD1259_REGS      *)    FpgaAD1259_1_BASE_ADDR,
    (devDriver_t          *)  &_devDriver_FpgaAD1259_1
};

static int32_t FpgaAD1259_1_Init(void) {
    return devInit(&_FpgaAD1259_1_res);
}
static int32_t FpgaAD1259_1_Uninit(void) {
    return devUninit(&_FpgaAD1259_1_res);
}
static int32_t FpgaAD1259_1_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD1259_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_1_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD1259_1_res);
    _FpgaAD1259_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD1259_1_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD1259_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_1_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD1259_1_res);
    _FpgaAD1259_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD1259_1_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD1259_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_1_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD1259_1_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD1259_1_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD1259_1 = {
    FpgaAD1259_1_Send
  , FpgaAD1259_1_Recv
  , FpgaAD1259_1_Ctrl
  , FpgaAD1259_1_Init
  , FpgaAD1259_1_Uninit
  , TO_STRING(_devDriver_FpgaAD1259_1)
};

#endif

/*******************************************************************************
 * @}
 * @addtogroup              Chip_2
 * @{
 ******************************************************************************/
#ifdef  FpgaAD1259_2_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD1259_2;
static FpgaAD1259_STAT         _FpgaAD1259_2_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD1259_INFO_USED
static FpgaAD1259_INFO         _FpgaAD1259_2_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD1259_RESOURCE     _FpgaAD1259_2_res  =         //!< 资源
{
#ifdef FpgaAD1259_INFO_USED
    (FpgaAD1259_INFO      *)  &_FpgaAD1259_2_info,
#endif
    (FpgaAD1259_STAT      *)  &_FpgaAD1259_2_stat,
    (FpgaAD1259_REGS      *)    FpgaAD1259_2_BASE_ADDR,
    (devDriver_t          *)  &_devDriver_FpgaAD1259_2
};

static int32_t FpgaAD1259_2_Init(void) {
    return devInit(&_FpgaAD1259_2_res);
}
static int32_t FpgaAD1259_2_Uninit(void) {
    return devUninit(&_FpgaAD1259_2_res);
}
static int32_t FpgaAD1259_2_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD1259_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_2_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD1259_2_res);
    _FpgaAD1259_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD1259_2_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD1259_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_2_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD1259_2_res);
    _FpgaAD1259_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD1259_2_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD1259_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_2_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD1259_2_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD1259_2_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD1259_2 = {
    FpgaAD1259_2_Send
  , FpgaAD1259_2_Recv
  , FpgaAD1259_2_Ctrl
  , FpgaAD1259_2_Init
  , FpgaAD1259_2_Uninit
  , TO_STRING(_devDriver_FpgaAD1259_2)
};

#endif

/*******************************************************************************
 * @}
 * @addtogroup              Chip_3
 * @{
 ******************************************************************************/
#ifdef  FpgaAD1259_3_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD1259_3;
static FpgaAD1259_STAT         _FpgaAD1259_3_stat = { 0 };  //!< 状态(Run-Time)
#ifdef FpgaAD1259_INFO_USED
static FpgaAD1259_INFO         _FpgaAD1259_3_info = { 0 };  //!< 信息(Run-Time)
#endif
static FpgaAD1259_RESOURCE     _FpgaAD1259_3_res  =         //!< 资源
{
#ifdef FpgaAD1259_INFO_USED
    (FpgaAD1259_INFO      *)  &_FpgaAD1259_3_info,
#endif
    (FpgaAD1259_STAT      *)  &_FpgaAD1259_3_stat,
    (FpgaAD1259_REGS      *)    FpgaAD1259_3_BASE_ADDR,
    (devDriver_t          *)  &_devDriver_FpgaAD1259_3
};

static int32_t FpgaAD1259_3_Init(void) {
    return devInit(&_FpgaAD1259_3_res);
}
static int32_t FpgaAD1259_3_Uninit(void) {
    return devUninit(&_FpgaAD1259_3_res);
}
static int32_t FpgaAD1259_3_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD1259_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_3_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD1259_3_res);
    _FpgaAD1259_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD1259_3_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;
    if (_FpgaAD1259_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_3_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD1259_3_res);
    _FpgaAD1259_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD1259_3_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;
    va_list  args;
    if (_FpgaAD1259_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD1259_3_res.drvStat->busy = 1;
    va_start(args, opt);

    ret = devCtrl(&cmd, opt, args, &_FpgaAD1259_3_res);
    
    if((ret == DEV_DRIVER_OK) && (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK))) {
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // 存在未处理 GRP or BIT 命令
        LOG_DEBUG("Unsupported on devDriver Ctrl()!");
    }
    va_end(args);
    _FpgaAD1259_3_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD1259_3 = {
    FpgaAD1259_3_Send
  , FpgaAD1259_3_Recv
  , FpgaAD1259_3_Ctrl
  , FpgaAD1259_3_Init
  , FpgaAD1259_3_Uninit
  , TO_STRING(_devDriver_FpgaAD1259_3)
};

#endif


