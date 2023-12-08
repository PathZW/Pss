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
#define  DEV_DRIVER_INFO_USED   // ���� xxxx_INFO

#include "defines.h"
#include "emb_delay.h"
#include "board.h"              // Device's defines


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
    int8_t                  excpt;      //!< �쳣״̬, ��ʾ����ʱ�����쳣
    uint8_t                 busy;       //!< æ״̬,   ��ʾ����ִ�в�������
 // uint8_t                 tx_busy;    //!< ���ڷ���, ��ʾ�����ڷ���״̬
 // uint8_t                 rx_busy;    //!< ���ڽ���, ��ʾ�����ڽ���״̬
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
    UNUSED_VAR(dev);
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
{   // Todo: DEV_EVT_ERROE_SEND
    //       DEV_EVT_COMPLETE_SEND
    //       DEV_EVT_TIMEOUT_SEND
    UNUSED_VAR(data);  UNUSED_VAR(size);  UNUSED_VAR(opt);  UNUSED_VAR(dev);
    LOG_DEBUG("Unsupported on devDriver Send()!");
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
{   // Todo: DEV_EVT_ERROE_RECV
    //       DEV_EVT_COMPLETE_RECV
    //       DEV_EVT_TIMEOUT_RECV
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
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_START, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_STOP, *cmd)
    {   //---------------------- ֹͣDAC -------------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_STOP, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_MODE_CFG, *cmd)
    {   //---------------------- ����DAC����ģʽ ------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_MODE_CFG, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_RATE_CFG, *cmd)
    {   //---------------------- ����DACת������ ------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_RATE_CFG, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SYNC_CFG, *cmd)
    {   //---------------------- ����DACͬ������ ------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SYNC_CFG, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_DATA, *cmd)
    {   //---------------------- ���ݼĴ���Ϊ Send() Ŀ�� ---------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_DATA, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_REAL, *cmd)
    {   //---------------------- ʵ��ֵFIFOΪ Send() Ŀ�� ---------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_REAL, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_SEND_ORGN, *cmd)
    {   //---------------------- ԭʼֵFIFOΪ Send() Ŀ�� ---------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_SEND_ORGN, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_DAC_CALB_CFG, *cmd)
    {   //---------------------- ����DACУ׼���� ------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_DAC_CALB_CFG, *cmd)

    /***************************************************************************
     * ��ܹ�ͬ GRP ����
     */
    DEV_CMD_GPR_CODE_BGN(DEV_CMD_GROUP_ECMD, *cmd)
    {   //---------------------- ��������չ -----------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_GROUP_ECMD, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_GROUP_FUNC, *cmd)
    {   //---------------------- ����������չ ---------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_GROUP_FUNC, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_COUNT_SEND, *cmd)
    {   //---------------------- ��ȡ�ѷ����ֽ� -------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_COUNT_SEND, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_COUNT_RECV, *cmd)
    {   //---------------------- ��ȡ�ѽ����ֽ� -------------------------
        // Todo:
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_COUNT_RECV, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_ON, *cmd)
    {   //---------------------- �豸��Դ�� ---------------------------
        // Todo:

        DEV_EVT_CODE_SET(DEV_EVT_POWER_CTRL, 1)     //!< �����¼���
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_POWER_ON, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_OFF, *cmd)
    {   //---------------------- �豸��Դ�ر� ---------------------------
        // Todo:

        DEV_EVT_CODE_SET(DEV_EVT_POWER_CTRL, 0)     //!< �����¼���
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_POWER_OFF, *cmd)

    DEV_CMD_GPR_CODE_BGN(DEV_CMD_POWER_RST, *cmd)
    {   //---------------------- �豸��λ -------------------------------
        // Todo:

        DEV_EVT_CODE_SET(DEV_EVT_POWER_CTRL, 2)     //!< �����¼���
    }
    DEV_CMD_GPR_CODE_END(DEV_CMD_POWER_RST, *cmd)
    DEV_CMD_GPR_BLOCK_END(*cmd)

    /***************************************************************************
     * �豸��� BIT ����
     */
    DEV_CMD_BIT_BLOCK_BGN(*cmd)
//  DEV_CMD_BIT_CODE_BGN(DEV_CMD_DAC_xxxx, *cmd)
//  {   // Todo:
//  }
//  DEV_CMD_BIT_CODE_END(DEV_CMD_DAC_xxxx, *cmd)

    /***************************************************************************
     * ��ܹ�ͬ BIT ����
     */
    #ifdef DEV_DRIVER_INFO_USED
    DEV_CMD_BIT_CODE_BGN(DEV_CMD_CALLBACK_FUN, *cmd)
    {   //---------------------- �¼����� -------------------------------
        dev->drvInfo->callback = va_arg(args, devDriver_CB);
    }
    DEV_CMD_BIT_CODE_END(DEV_CMD_CALLBACK_FUN, *cmd)
    #endif

    #ifdef DEV_DRIVER_INFO_USED
    DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_SEND, *cmd)
    {   //---------------------- ��ʱ���� -------------------------------
        dev->drvInfo->tx_timeout = opt;
        dev->drvInfo->tx_post    = va_arg(args, devDriver_POST);
        dev->drvInfo->tx_wait    = va_arg(args, devDriver_WAIT);
    }
    DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_SEND, *cmd)
    #endif

    #ifdef DEV_DRIVER_INFO_USED
    DEV_CMD_BIT_CODE_BGN(DEV_CMD_TIMEOUT_RECV, *cmd)
    {   //---------------------- ��ʱ���� -------------------------------
        dev->drvInfo->rx_timeout = opt;
        dev->drvInfo->rx_post    = va_arg(args, devDriver_POST);
        dev->drvInfo->rx_wait    = va_arg(args, devDriver_WAIT);
    }
    DEV_CMD_BIT_CODE_END(DEV_CMD_TIMEOUT_RECV, *cmd)
    #endif

    DEV_CMD_BIT_CODE_BGN(DEV_CMD_ABORT_SEND, *cmd)
    {   //---------------------- ��ֹ���� -------------------------------
        // Todo:

        DEV_EVT_CODE_SET(DEV_EVT_ABORT_SEND, 0)     //!< �����¼���
    }
    DEV_CMD_BIT_CODE_END(DEV_CMD_ABORT_SEND, *cmd)

    DEV_CMD_BIT_CODE_BGN(DEV_CMD_ABORT_RECV, *cmd)
    {   //---------------------- ��ֹ���� -------------------------------
        // Todo:

        DEV_EVT_CODE_SET(DEV_EVT_ABORT_RECV, 0)     //!< �����¼���
    }
    DEV_CMD_BIT_CODE_END(DEV_CMD_ABORT_RECV, *cmd)

    DEV_CMD_BIT_CODE_BGN(DEV_CMD_FLUSH_SEND, *cmd)
    {   //---------------------- ��ϴ���� -------------------------------
        // Todo:

        DEV_EVT_CODE_SET(DEV_EVT_FLUSH_SEND, 0)     //!< �����¼���
    }
    DEV_CMD_BIT_CODE_END(DEV_CMD_FLUSH_SEND, *cmd)

    DEV_CMD_BIT_CODE_BGN(DEV_CMD_FLUSH_RECV, *cmd)
    {   //---------------------- ��ϴ���� -------------------------------
        // Todo:

        DEV_EVT_CODE_SET(DEV_EVT_FLUSH_RECV, 0)     //!< �����¼���
    }
    DEV_CMD_BIT_CODE_END(DEV_CMD_FLUSH_RECV, *cmd)
    DEV_CMD_BIT_BLOCK_END(*cmd)

    DEV_EVT_FUN_CALL(dev)                           //!< �����¼�������
    return( result );
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

