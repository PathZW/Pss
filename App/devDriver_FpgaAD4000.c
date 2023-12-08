/*******************************************************************************
 * @file    devDriver_FpgaAD4000.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   ͨ���豸���� - ADC - FPGA AD4000, ������"devDriver_cfg.h"����.\n
 *          ���á�extern devDriver_t devDriver(FpgaAD4000, x)�� xΪ 1 ~ 4 ����ʹ��.
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
//#define FpgaAD4000_INFO_USED    // ���� FpgaAD4000_INFO
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

//!< ״̬(Run-Time)
typedef struct {
    uint8_t                 busy;       //!< æ״̬,   ��ʾ����ִ�в�������
  //int8_t                  excpt;      //!< �쳣״̬, ��ʾ����ʱ�����쳣
  //uint8_t                 tx_busy;    //!< ���ڷ���, ��ʾ�����ڷ���״̬
  //uint8_t                 rx_busy;    //!< ���ڽ���, ��ʾ�����ڽ���״̬
    uint8_t                 rx_from;    //!< Recv() ����Դ; 1:У׼ֵFIFO; 2:ԭʼֵFIFO; ����:�����Ĵ���
    uint8_t                 reserved[2];
} FpgaAD4000_STAT;

#ifdef FpgaAD4000_INFO_USED
//!< ��Ϣ(Run-Time)
typedef struct {
    devDriver_CB            callback;   //!< �¼�����ص�����
    uint32_t                rx_opt;     //!< opt
    uint32_t                rx_timeout; //!< ��ʱʱ��
    devDriver_POST          rx_post;    //!< ��������
    devDriver_WAIT          rx_wait;    //!< �ȴ�����
    uint32_t                tx_opt;     //!< opt
    uint32_t                tx_timeout; //!< ��ʱʱ��
    devDriver_POST          tx_post;    //!< ��������
    devDriver_WAIT          tx_wait;    //!< �ȴ�����
} FpgaAD4000_INFO;
#endif

//!< ��Դ
typedef struct {
#ifdef FpgaAD4000_INFO_USED
    FpgaAD4000_INFO        *drvInfo;    //!< ��Ϣ(Run-Time)
#endif
    FpgaAD4000_STAT        *drvStat;    //!< ״̬
    FpgaAD4000_REGS        *devRegs;    //!< �Ĵ���
#ifdef FpgaAD4000_SYNC_BASE_ADDR
    FpgaAD4000_SYNC_REGS   *devSync;    //!< ͬ�����ƼĴ���
#endif
    devDriver_t            *device;     //!< ����
} const FpgaAD4000_RESOURCE;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * ��ʼ��, �������ж��е���
 */
static int32_t devInit(FpgaAD4000_RESOURCE *dev)
{
	UNUSED_VAR(dev);
	return( DEV_DRIVER_OK );
}

/**
 * ����ʼ��, �������ж��е���
 */
static int32_t devUninit(FpgaAD4000_RESOURCE *dev)
{
	UNUSED_VAR(dev);
	LOG_DEBUG("Unsupported on devDriver Uninit()!");
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * ��������, �޳�ʱ����¿����жϵ���
 * @note 1. dev->drvStat->busy ���账��, �Ѿ��� xxxx_n_Send() ����
 * @note 2. dev->drvStat �� dev->drvInfo ��Ҫ����
 */
static int32_t devSend(const void *data, size_t size, uint32_t opt, FpgaAD4000_RESOURCE *dev)
{  
	UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);
	LOG_DEBUG("Unsupported on devDriver Send()!");
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * ��������, �޳�ʱ����¿����жϵ���
 * @note 1. dev->drvStat->busy ���账��, �Ѿ��� xxxx_n_Recv() ����
 * @note 2. dev->drvStat �� dev->drvInfo ��Ҫ����
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
 * �豸����, �ܹ����ж��е���
 * @note 1. ��Ҫ���� DEV_DRIVER_ERR_UNSUPPORTED, �Ѿ��� xxxx_n_Ctrl() ����
 * @note 2. dev->drvStat->busy ���账��, �Ѿ��� xxxx_n_Ctrl() ����
 * @note 3. dev->drvStat �� dev->drvInfo ��Ҫ����
 */
static int32_t devCtrl(uint32_t *cmd, uint32_t opt, va_list args, FpgaAD4000_RESOURCE *dev)
{
	int32_t 	result = DEV_DRIVER_OK;
	DEV_EVT_VAR_ALLOC								//!< �����¼�����
	UNUSED_VAR(opt);  UNUSED_VAR(args);  UNUSED_VAR(dev);
	
	/***************************************************************************
	* �豸��� GRP ����
	*/
	DEV_CMD_GPR_BLOCK_BGN(*cmd)
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_START, *cmd)
	{	//---------------------- ����DAC -------------------------------
		return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_START, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_STOP, *cmd)
	{	//---------------------- ֹͣDAC -------------------------------
		return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_STOP, *cmd)
	
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_MODE_CFG, *cmd)
	{	//---------------------- ADC����ģʽ-------------------------------
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
	{	//---------------------- ADC�˲����� -------------------------------
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
	{	//---------------------- ADCͬ������ -------------------------------
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
	{	//---------------------- �����Ĵ���Ϊ Recv() ��Դ -------------------------------
		dev->drvStat->rx_from = DEV_AD4000RECV_FROM_SAMPREG;
		return (result);
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_DATA, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RECV_MEAR, *cmd)
	{	//---------------------- У׼ֵFIFOΪ Recv() ��Դ -------------------------------
		dev->drvStat->rx_from = DEV_AD4000RECV_FROM_CHKFIFO;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_MEAR, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_RECV_ORGN, *cmd)
	{	//---------------------- ԭʼֵFIFOΪ Recv() ��Դ -------------------------------
		dev->drvStat->rx_from = DEV_AD4000RECV_FROM_ORGFIFO;
		return( result );	   
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_RECV_ORGN, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_POINT_CFG, *cmd)
	{	//---------------------- ADC�������� -------------------------------
		dev->devRegs->mSamPoint = va_arg(args, uint32_t);
		return( result );	   
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_POINT_CFG, *cmd)
	
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_DELAY_CFG, *cmd)
	{	//---------------------- ADC������ʱ -------------------------------
		dev->devRegs->mSamDlyCyc = FpgaAD4000_NS_TO_CYC(va_arg(args, uint32_t));
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_DELAY_CFG, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_ADC_CALB_CFG, *cmd)
	{	//---------------------- ADCУ׼���� -------------------------------
		dev->devRegs->mCalKm  = va_arg(args, uint32_t);
		dev->devRegs->mCalBm  = va_arg(args, uint32_t);
		dev->devRegs->mCalM10 = va_arg(args, uint32_t);
		return( result );	   
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_ADC_CALB_CFG, *cmd)
	
	/***************************************************************************
	 * ��ܹ�ͬ GRP ����
	 */
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_RST, *cmd)
	{	//---------------------- �豸��λ -------------------------------
	  
		dev->devRegs->mFIFOreset = DEV_ADC_RESET_ORGFIFOREG | DEV_ADC_RESET_CHKFIFOREG;
		DEV_EVT_CODE_SET(DEV_EVT_POWER_CTRL, 2) 	//!< �����¼���
		return( result );
		
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_POWER_RST, *cmd)
	DEV_CMD_GPR_BLOCK_END(*cmd)

	/***************************************************************************
	 * �豸��� BIT ����
	 */
	DEV_CMD_BIT_BLOCK_BGN(*cmd)
	
	/***************************************************************************
	 * ��ܹ�ͬ BIT ����
	 */
#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_CALLBACK_FUN, *cmd)
	{	//---------------------- �¼����� -------------------------------
		dev->drvInfo->callback = va_arg(args, devDriver_CB);
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_CALLBACK_FUN, *cmd)
#endif

#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_SEND, *cmd)
	{	//---------------------- ��ʱ���� -------------------------------
		dev->drvInfo->tx_timeout = opt;
		dev->drvInfo->tx_post	 = va_arg(args, devDriver_POST);
		dev->drvInfo->tx_wait	 = va_arg(args, devDriver_WAIT);
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_SEND, *cmd)
#endif

#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_RECV, *cmd)
	{	//---------------------- ��ʱ���� -------------------------------
		dev->drvInfo->rx_timeout = opt;
		dev->drvInfo->rx_post	 = va_arg(args, devDriver_POST);
		dev->drvInfo->rx_wait	 = va_arg(args, devDriver_WAIT);
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_RECV, *cmd)
#endif

	DEV_CMD_BIT_CODE_BGN(DEV_CMD_FLUSH_RECV, *cmd)
	{	//---------------------- ��ϴ���� -------------------------------
		if(*cmd & DEV_ARG_ADC_FLUSH_MEAR)
		{
			dev->devRegs->mFIFOreset = DEV_ADC_RESET_CHKFIFOREG;  // ���ADCУ׼ֵFIFO
		}
		else if(*cmd & DEV_ARG_ADC_FLUSH_ORGN)
		{
			dev->devRegs->mFIFOreset = DEV_ADC_RESET_ORGFIFOREG;  // ���ADCУ׼ֵFIFO
		}
		else
		{
			dev->devRegs->mFIFOreset = DEV_ADC_RESET_CHKFIFOREG | DEV_ADC_RESET_ORGFIFOREG;
		}
		
		DEV_EVT_CODE_SET(DEV_EVT_FLUSH_RECV, 0) 	//!< �����¼���
		
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_FLUSH_RECV, *cmd)
	DEV_CMD_BIT_BLOCK_END(*cmd)

	DEV_EVT_FUN_CALL(dev)							//!< �����¼�������
	
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/*******************************************************************************
 * @}
 * @addtogroup              Chip_1
 * @{
 ******************************************************************************/
#ifdef  FpgaAD4000_1_BASE_ADDR

extern devDriver_t             _devDriver_FpgaAD4000_1;
static FpgaAD4000_STAT         _FpgaAD4000_1_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_1_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_1_res  =         //!< ��Դ
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
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
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
static FpgaAD4000_STAT         _FpgaAD4000_2_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_2_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_2_res  =         //!< ��Դ
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
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
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
static FpgaAD4000_STAT         _FpgaAD4000_3_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_3_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_3_res  =         //!< ��Դ
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
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
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
static FpgaAD4000_STAT         _FpgaAD4000_4_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef FpgaAD4000_INFO_USED
static FpgaAD4000_INFO         _FpgaAD4000_4_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD4000_RESOURCE     _FpgaAD4000_4_res  =         //!< ��Դ
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
        ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
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

