/*******************************************************************************
 * @file    devDriver_FpgaAD5761.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   ͨ���豸���� - DAC - FPGA AD5761, ������"devDriver_cfg.h"����.\n
 *          ���á�extern devDriver_t devDriver(FpgaAD5761, x)�� xΪ 1 ~ 4 ����ʹ��.
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
#undef   DEV_DRIVER_INFO_USED   // ���� xxxx_INFO

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

//!< ״̬(Run-Time)
typedef struct {
 //   int8_t                  excpt;      //!< �쳣״̬, ��ʾ����ʱ�����쳣
    uint8_t                 busy;       //!< æ״̬,   ��ʾ����ִ�в�������
 // uint8_t                 tx_busy;    //!< ���ڷ���, ��ʾ�����ڷ���״̬
 // uint8_t                 rx_busy;    //!< ���ڽ���, ��ʾ�����ڽ���״̬
		uint8_t                 tx_to;      // 0��ʾ���͵�cmd_reg,1��ʾ���͵�FIFO
    uint8_t                 reserved[2];
} FpgaAD5761_STAT;

#ifdef DEV_DRIVER_INFO_USED
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
} FpgaAD5761_INFO;
#endif

//!< ��Դ
typedef struct {
#ifdef DEV_DRIVER_INFO_USED
    FpgaAD5761_INFO        *drvInfo;    //!< ��Ϣ(Run-Time)
#endif
    FpgaAD5761_STAT        *drvStat;    //!< ״̬
    FpgaAD5761_REGS        *devRegs;    //!< �Ĵ���
    devDriver_t            *device;     //!< ����
} const FpgaAD5761_RESOURCE;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * ��ʼ��, �������ж��е���
 *
 * @return     ������
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
 * ����ʼ��, �������ж��е���
 *
 * @return     ������
 */
static int32_t devUninit(FpgaAD5761_RESOURCE *dev)
{
    UNUSED_VAR(dev);
    LOG_DEBUG("Unsupported on devDriver Uninit()!");
    return( DEV_DRIVER_ERR_UNSUPPORTED );
}

/**
 * ��������
 *
 * @param[in]  data     ���ͻ���ָ��
 * @param[in]  size     ���ͻ����С(���ֽ�Ϊ��λ)
 * @param[in]  opt      ��ѡ������
 *
 * @retval    (>=0)     �ɹ����������ֽ���
 * @retval    (< 0)     ������
 *
 * @note    1. �����ó�ʱ, ���¼�����ص������޹�, �ò���Ϊͬ��IO, �������ж��е���
 * @note    2. δ���ó�ʱ, ���¼�����ص�����Ϊ��, �ò���Ϊͬ��IO, �������ж��е���
 * @note    3. δ���ó�ʱ, ���¼�����ص������ǿ�, �ò���Ϊ�첽IO, �ܹ����ж��е���
 * @note    4. ��Ҫ���� DEV_DRIVER_ERR_BUSY, �Ѿ��� xxxx_n_Ctrl() ����
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
 * ��������
 *
 * @param[in]  data     ���ջ���ָ��
 * @param[in]  size     ���ջ����С(���ֽ�Ϊ��λ)
 * @param[in]  opt      ��ѡ������
 *
 * @retval    (>=0)     �ɹ����������ֽ���
 * @retval    (< 0)     ������
 *
 * @note    1. �����ó�ʱ, ���¼�����ص������޹�, �ò���Ϊͬ��IO, �������ж��е���
 * @note    2. δ���ó�ʱ, ���¼�����ص�����Ϊ��, �ò���Ϊͬ��IO, �������ж��е���
 * @note    3. δ���ó�ʱ, ���¼�����ص������ǿ�, �ò���Ϊ�첽IO, �ܹ����ж��е���
 * @note    4. ��Ҫ���� DEV_DRIVER_ERR_BUSY, �Ѿ��� xxxx_n_Ctrl() ����
 */
static int32_t devRecv(void *data, size_t size, uint32_t opt, FpgaAD5761_RESOURCE *dev)
{   
	UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);
	LOG_DEBUG("Unsupported on devDriver Send()!");
	return( DEV_DRIVER_ERR_UNSUPPORTED );
}


/**
 * �豸����, �ܹ����ж��е���
 *
 * @param[in]  cmd      ������
 * @param[in]  opt      ��ѡ������ 
 *
 * @retval    (>=0)     �����ֵ
 * @retval    (< 0)     ������
 *
 * @note 1. ��Ҫ���� DEV_DRIVER_ERR_BUSY, �Ѿ��� xxxx_n_Ctrl() ����
 * @note 2. ��Ҫ���� DEV_DRIVER_ERR_UNSUPPORTED, �Ѿ��� xxxx_n_Ctrl() ����
 */
static int32_t devCtrl(uint32_t *cmd, uint32_t opt, va_list args, FpgaAD5761_RESOURCE *dev)
{
  int32_t     result = DEV_DRIVER_OK;
	DEV_EVT_VAR_ALLOC                               //!< �����¼�����
	UNUSED_VAR(opt);  UNUSED_VAR(args);  UNUSED_VAR(dev);

	/***************************************************************************
	 * �豸��� GRP ����
	 */
	DEV_CMD_GPR_BLOCK_BGN(*cmd)
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_START, *cmd)
	{   //---------------------- ����DAC -------------------------------
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_START, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_STOP, *cmd)
	{   //---------------------- ֹͣDAC -------------------------------
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_STOP, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_MODE_CFG, *cmd)
	{   //---------------------- ����DAC����ģʽ ------------------------
		switch(va_arg(args, uint32_t))
		{
		default: break;
		case DEV_DAC_MODE_TRIG:
		return( result );
		}
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_MODE_CFG, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_DATA, *cmd)
	{   //---------------------- ���ݼĴ���Ϊ Send() Ŀ�� ---------------
		dev->drvStat->tx_to = DEV_AD5761Send_TO_CMD_REG;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_DATA, *cmd)

	DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_ORGN, *cmd)
	{   //---------------------- ԭʼֵFIFOΪ Send() Ŀ�� ---------------
		dev->drvStat->tx_to = DEV_AD5761Send_TO_FIFO;
		return( result );
	}
	DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_ORGN, *cmd)

	/***************************************************************************
	 * ��ܹ�ͬ GRP ����
	 */
	DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_RST, *cmd)
	{   //---------------------- �豸��λ -------------------------------
		
		dev->devRegs->mFIFOreset = DEV_AD5761_RESET_FIFO_FLG;
		DEV_EVT_CODE_SET(DEV_EVT_POWER_CTRL, 2)     //!< �����¼���
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
	{   //---------------------- �¼����� -------------------------------
			dev->drvInfo->callback = va_arg(args, devDriver_CB);
	return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_CALLBACK_FUN, *cmd)
	#endif

	#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_SEND, *cmd)
	{   //---------------------- ��ʱ���� -------------------------------
			dev->drvInfo->tx_timeout = opt;
			dev->drvInfo->tx_post    = va_arg(args, devDriver_POST);
			dev->drvInfo->tx_wait    = va_arg(args, devDriver_WAIT);
	return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_SEND, *cmd)
	#endif

	#ifdef DEV_DRIVER_INFO_USED
	DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_RECV, *cmd)
	{   //---------------------- ��ʱ���� -------------------------------
			dev->drvInfo->rx_timeout = opt;
			dev->drvInfo->rx_post    = va_arg(args, devDriver_POST);
			dev->drvInfo->rx_wait    = va_arg(args, devDriver_WAIT);
	return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_RECV, *cmd)
	#endif

	DEV_CMD_BIT_CODE_BGN(DEV_CMD_FLUSH_SEND, *cmd)
	{   //---------------------- ��ϴ���� -------------------------------
		dev->devRegs->mFIFOreset = DEV_AD5761_RESET_FIFO_FLG; //д���κ�ֵ
		DEV_EVT_CODE_SET(DEV_EVT_FLUSH_SEND, 0)     //!< �����¼���
		return( result );
	}
	DEV_CMD_BIT_CODE_END(DEV_CMD_FLUSH_SEND, *cmd)
	DEV_CMD_BIT_BLOCK_END(*cmd)

	DEV_EVT_FUN_CALL(dev)                           //!< �����¼�������

	return( DEV_DRIVER_ERR_UNSUPPORTED );
}


/*******************************************************************************
 * @}
 * @addtogroup              Chip_1
 * @{
 ******************************************************************************/
#ifdef  FpgaAD5761_1_BASE_ADDR
extern devDriver_t             _devDriver_FpgaAD5761_1;     //!< ���� devDriver ����

static FpgaAD5761_STAT         _FpgaAD5761_1_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_1_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_1_res  = {       //!< ��Դ
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_1_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_1_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_1_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_1
};

static int32_t FpgaAD5761_1_Init(void) {
    return devInit(&_FpgaAD5761_1_res);                     //!< ��ʼ��
}
static int32_t FpgaAD5761_1_Uninit(void) {
    return devUninit(&_FpgaAD5761_1_res);                   //!< ����ʼ��
}
static int32_t FpgaAD5761_1_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_1_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_1_res);
    _FpgaAD5761_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_1_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_1_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_1_res);
    _FpgaAD5761_1_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_1_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< �豸����
    va_list  args;
    if (_FpgaAD5761_1_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_1_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_1_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_1_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_1 = {                   //!< ���� devDriver ����
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
extern devDriver_t             _devDriver_FpgaAD5761_2;     //!< ���� devDriver ����

static FpgaAD5761_STAT         _FpgaAD5761_2_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_2_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_2_res  = {       //!< ��Դ
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_2_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_2_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_2_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_2
};

static int32_t FpgaAD5761_2_Init(void) {
    return devInit(&_FpgaAD5761_2_res);                     //!< ��ʼ��
}
static int32_t FpgaAD5761_2_Uninit(void) {
    return devUninit(&_FpgaAD5761_2_res);                   //!< ����ʼ��
}
static int32_t FpgaAD5761_2_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_2_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_2_res);
    _FpgaAD5761_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_2_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_2_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_2_res);
    _FpgaAD5761_2_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_2_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< �豸����
    va_list  args;
    if (_FpgaAD5761_2_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_2_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_2_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_2_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_2 = {                   //!< ���� devDriver ����
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
extern devDriver_t             _devDriver_FpgaAD5761_3;     //!< ���� devDriver ����

static FpgaAD5761_STAT         _FpgaAD5761_3_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_3_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_3_res  = {       //!< ��Դ
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_3_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_3_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_3_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_3
};

static int32_t FpgaAD5761_3_Init(void) {
    return devInit(&_FpgaAD5761_3_res);                     //!< ��ʼ��
}
static int32_t FpgaAD5761_3_Uninit(void) {
    return devUninit(&_FpgaAD5761_3_res);                   //!< ����ʼ��
}
static int32_t FpgaAD5761_3_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_3_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_3_res);
    _FpgaAD5761_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_3_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_3_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_3_res);
    _FpgaAD5761_3_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_3_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< �豸����
    va_list  args;
    if (_FpgaAD5761_3_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_3_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_3_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_3_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_3 = {                   //!< ���� devDriver ����
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
extern devDriver_t             _devDriver_FpgaAD5761_4;     //!< ���� devDriver ����

static FpgaAD5761_STAT         _FpgaAD5761_4_stat = { 0 };  //!< ״̬(Run-Time)
#ifdef DEV_DRIVER_INFO_USED
static FpgaAD5761_INFO         _FpgaAD5761_4_info = { 0 };  //!< ��Ϣ(Run-Time)
#endif
static FpgaAD5761_RESOURCE     _FpgaAD5761_4_res  = {       //!< ��Դ
#ifdef DEV_DRIVER_INFO_USED
    (FpgaAD5761_INFO *)       &_FpgaAD5761_4_info,
#endif
    (FpgaAD5761_STAT *)       &_FpgaAD5761_4_stat,
    (FpgaAD5761_REGS *)         FpgaAD5761_4_BASE_ADDR,
    (devDriver_t     *)       &_devDriver_FpgaAD5761_4
};

static int32_t FpgaAD5761_4_Init(void) {
    return devInit(&_FpgaAD5761_4_res);                     //!< ��ʼ��
}
static int32_t FpgaAD5761_4_Uninit(void) {
    return devUninit(&_FpgaAD5761_4_res);                   //!< ����ʼ��
}
static int32_t FpgaAD5761_4_Send(const void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_4_res.drvStat->busy = 1;
    ret = devSend(data, size, opt, &_FpgaAD5761_4_res);
    _FpgaAD5761_4_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_4_Recv(void *data, size_t size, uint32_t opt)
{   int32_t  ret;                                           //!< ��������
    if (_FpgaAD5761_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_4_res.drvStat->busy = 1;
    ret = devRecv(data, size, opt, &_FpgaAD5761_4_res);
    _FpgaAD5761_4_res.drvStat->busy = 0;
    return( ret );
}
static int32_t FpgaAD5761_4_Ctrl(uint32_t cmd, uint32_t opt, ...)
{   int32_t  ret;                                           //!< �豸����
    va_list  args;
    if (_FpgaAD5761_4_res.drvStat->busy) {
        return( DEV_DRIVER_ERR_BUSY );
    }
    _FpgaAD5761_4_res.drvStat->busy = 1;
    va_start(args, opt);
    if((ret = devCtrl(&cmd, opt, args, &_FpgaAD5761_4_res)) == DEV_DRIVER_OK) {
        if (cmd & (DEV_CMD_GRP_MSK | DEV_CMD_BIT_MSK)) {
            ret  = DEV_DRIVER_ERR_UNSUPPORTED;  // ����δ���� GRP or BIT ����
            LOG_DEBUG("Unsupported on devDriver Ctrl()!");
        }
    }
    va_end(args);
    _FpgaAD5761_4_res.drvStat->busy = 0;
    return( ret );
}

devDriver_t   _devDriver_FpgaAD5761_4 = {                   //!< ���� devDriver ����
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

