/*******************************************************************************
 * @file    DAC_SGM5348.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/6/22
 * @brief   DAC Device Driver for SGM5348 (SPI Bus)
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/6/22 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro(835, ARM_SPI_EVENT_DATA_LOST, ARM_SPI_EVENT_MODE_FAULT, ARM_SPI_EVENT_TRANSFER_COMPLETE)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#include "defines.h"            // Used: __WEAK, __INLINE, UNUSED_VAR()
#include "Driver_SPI.h"
#include "Driver_DAC.h"


/******************************************************************************/
/**
 * @addtogroup __Config
 * @{
 * @addtogroup              DAC_SGM5348
 * @{
 ******************************************************************************/

#ifndef SGM5348_DRV_NUM
#define SGM5348_DRV_NUM          10         //!< (C) SGM5348 驱动号
#endif

#ifndef SGM5348_COUNT
#define SGM5348_COUNT            4          //!< (C) SGM5348 芯片数量
#endif

#ifndef SGM5348_SPI_NUM
#define SGM5348_SPI_NUM          1          //!< (C) SPI driver number
#endif

#ifndef SGM5348_SPI_SPEED
#define SGM5348_SPI_SPEED     36000000      //!< (C) SPI bus speed
#endif

#ifndef SGM5348_SYNC_CTRL
#define SGM5348_SYNC_CTRL  SGM5348_CtrlSYNC //!< (C) SGM5348 SYNC 控制
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup DAC_SGM5348
 * @{
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/

#define ARM_DAC_DRV_VERSION     ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)
#define DAC_DRV_NUM             SGM5348_DRV_NUM

// DAC Driver Capabilities
#define DAC_CHIPS               SGM5348_COUNT   // DAC 芯片数
#define DAC_CHANNELS                8           // DAC 通道数
#define DAC_RESOLUTION              12          // DAC 分辨率
#define DAC_MODE_EVENT              1           // 是否支持事件模式
#define DAC_MODE_TRIGGER            0           // 是否支持触发模式

// DAC Driver Flags
#define DAC_FLAG_INIT             (0x01U)
#define DAC_FLAG_POWER            (0x02U)
#define DAC_FLAG_CLR_BUSY         (0x04U)


/*******************************************************************************
 * @}
 * @addtogroup              Private_Types
 * @{
 ******************************************************************************/

// DAC data type, left-align
#  if (DAC_RESOLUTION <= 8)
typedef uint8_t             DAC_DATA;
#elif (DAC_RESOLUTION <= 16)
typedef uint16_t            DAC_DATA;
#else
typedef uint32_t            DAC_DATA;
#endif

// // DAC channel Information (Run-Time)
// typedef struct _DAC_CHANNEL {
//     uint32_t                num;    // 通道待输出数据项总数
//     uint32_t                cnt;    // 通道已输出数据项计数
//     DAC_DATA                val;    // 通道输出值(输出单数据时使用)
//     const DAC_DATA         *data;   // 通道待输出数据项指针
// } DAC_CHANNEL;

// DAC Information (Run-time)
typedef struct _DAC_INFO
{
  #if (DAC_MODE_EVENT != 0)
    ARM_DAC_SignalEvent_t   cb_event;   // Event Callback
  #endif
    ARM_DAC_STATUS          status;     // status
    uint8_t                 flags;      // flags
//  DAC_CHANNEL  chnl[DAC_CHIPS][DAC_CHANNELS]; // 所有通道
} DAC_INFO;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

static const ARM_DRIVER_VERSION  _DAC_DriverVersion = {         // Driver Version
    ARM_DAC_API_VERSION,                                        // API version
    ARM_DAC_DRV_VERSION                                         // Driver version
};

static const ARM_DAC_CAPABILITIES  _DAC_DriverCapabilities = {  // Driver Capabilities
    DAC_CHANNELS * DAC_CHIPS,                                   // DAC 通道数
    DAC_RESOLUTION,                                             // DAC 分辨率
    DAC_MODE_EVENT,                                             // 是否支持事件模式
    DAC_MODE_TRIGGER,                                           // 是否支持触发模式
    0
};

static DAC_INFO     _DAC_Info = {0};    // DAC Information (Run-time)


/*******************************************************************************
 * @}
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/

static __INLINE void DAC_EventComplete(void);
static __INLINE void DAC_EventError   (void);


/*******************************************************************************
 * @}
 * @addtogroup              SPI_Interface
 * @{
 ******************************************************************************/
//lint -emacro((835), SPI_BUS_CFG, SPI_BUS_SPEED)
//lint -emacro((845), SPI_BUS_CFG, SPI_BUS_SPEED)
/**
 * SPI Driver (ptrSPI)
 */
#define _SPI_Driver_(n)      Driver_SPI##n
#define  SPI_Driver_(n)      _SPI_Driver_(n)
extern ARM_DRIVER_SPI         SPI_Driver_(SGM5348_SPI_NUM);
#define ptrSPI              (&SPI_Driver_(SGM5348_SPI_NUM))

// SPI BUS config
#define SPI_BUS_CFG         (  ARM_SPI_MODE_MASTER          \
                             | ARM_SPI_CPOL0_CPHA1          \
                             | ARM_SPI_DATA_BITS(16)        \
                             | ARM_SPI_MSB_LSB              \
                             | ARM_SPI_SS_MASTER_UNUSED     \
                            )

// SPI Bus Speed
#define SPI_BUS_SPEED       ((uint32_t)SGM5348_SPI_SPEED)

// SPI BUS NSS control
#define SPI_NSS_CTRL        SGM5348_SYNC_CTRL

//lint -esym(522, SPI_NSS_CTRL, SGM5348_SYNC_CTRL, SGM5348_CtrlSYNC)
/**
 * 控制 SPI NSS 信号
 *
 * @param[in]  bitmap   Chips CS Pin signal level(bitN -- Chip N)
 */
__WEAK void SPI_NSS_CTRL(uint32_t bitmap)
{
    UNUSED_VAR(bitmap);
}

#if (DAC_MODE_EVENT != 0)
/**
 * Signal SPI Events
 */
static void SPI_SignalEvent(uint32_t event)
{
    if (event & ARM_SPI_EVENT_DATA_LOST)
    {   // Data lost: Receive overflow / Transmit underflow
        DAC_EventError();
    }

    if (event & ARM_SPI_EVENT_MODE_FAULT)
    {   // Master Mode Fault (SS deactivated when Master)
        DAC_EventError();
    }

    if (event & ARM_SPI_EVENT_TRANSFER_COMPLETE)
    {   // Data Transfer completed
        DAC_EventComplete();
    }
}
#endif

static __INLINE int32_t SPI_Initialize(void)
{   // succeeded to return: ARM_DRIVER_OK
  #if (DAC_MODE_EVENT != 0)
    return( ptrSPI->Initialize(SPI_SignalEvent) );
  #else
    return( ptrSPI->Initialize(NULL) );
  #endif
}

static __INLINE int32_t SPI_Uninitialize(void)
{   // succeeded to return: ARM_DRIVER_OK
    return( ptrSPI->Uninitialize() );
}

static __INLINE int32_t SPI_PowerON(void)
{   // succeeded to return: ARM_DRIVER_OK
    int32_t  status;

    if((status = ptrSPI->PowerControl(ARM_POWER_FULL)) != ARM_DRIVER_OK) {
        return( status );
    }
    return( ptrSPI->Control(SPI_BUS_CFG, SPI_BUS_SPEED) );
}

static __INLINE int32_t SPI_PowerOFF(void)
{   // succeeded to return: ARM_DRIVER_OK
    return( ptrSPI->PowerControl(ARM_POWER_OFF) );
}

static __INLINE int32_t SPI_SendStart(uint32_t data)
{   // succeeded to return: ARM_DRIVER_OK
    static DAC_DATA  item;

    item = (DAC_DATA)data;
    return( ptrSPI->Send(&item, 1) );
}

static __INLINE void SPI_SendWaiting(void)
{
    while(ptrSPI->GetStatus().busy) ;
}


/*******************************************************************************
 * @}
 * @addtogroup              Device_Interface
 * @{
 ******************************************************************************/
/**
 * Initialize DAC device
 */
static __INLINE int32_t Device_Init(void)
{   // succeeded to return: ARM_DRIVER_OK
    unsigned    chip, chnl;
    int32_t     status = ARM_DRIVER_OK;

    for(chip = 0;  (chip < DAC_CHIPS) && (status == ARM_DRIVER_OK);  chip++)
    {
        SPI_NSS_CTRL( ~(1u << chip) );      // Valid (chip) SPI NSS
        status = SPI_SendStart( 0x9000 );   // SGM5348
        SPI_SendWaiting();
        SPI_NSS_CTRL( ~(0u) );              // Invalid all SPI NSS

        for(chnl = 0;  (chnl < DAC_CHANNELS) && (status == ARM_DRIVER_OK);  chnl++)
        {
            SPI_NSS_CTRL( ~(1u << chip) );  // Valid (chip) SPI NSS
            status = SPI_SendStart( (chnl << 12) ); // SGM5348
            SPI_SendWaiting();
            SPI_NSS_CTRL( ~(0u) );          // Invalid all SPI NSS
        }
    }

    return( status );
}

/**
 * Set DAC device output value
 */
static __INLINE int32_t Device_Output(unsigned chip, unsigned chnl, uint32_t val)
{   // succeeded to return: ARM_DRIVER_OK
    int32_t  status;

    SPI_NSS_CTRL( ~(1u << chip) );  // Valid (chip) SPI NSS
    status = SPI_SendStart( (chnl << 12) | ((val >> 4) & 0x0FFF) ); // SGM5348

#if (DAC_MODE_EVENT == 0)
    SPI_SendWaiting();
    if (status == ARM_DRIVER_OK) {
        DAC_EventComplete();
    } else {
        DAC_EventError();
    }
 // SPI_NSS_CTRL( ~(0u) );          // Invalid all SPI NSS
#endif

    return( status );
}


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/

static __INLINE void DAC_EventComplete(void)
{
    SPI_NSS_CTRL( ~(0u) );          // Invalid all SPI NSS
    _DAC_Info.status.busy  = 0;
 // _DAC_Info.status.error = 0;

  #if (DAC_MODE_EVENT != 0)
    if (_DAC_Info.cb_event != NULL) {
        _DAC_Info.cb_event(ARM_DAC_EVENT_COMPLETE, ARM_DAC_CHANNEL_ALL);
    }
  #endif
}

static __INLINE void DAC_EventError(void)
{
    SPI_NSS_CTRL( ~(0u) );          // Invalid all SPI NSS
    _DAC_Info.status.busy  = 0;
    _DAC_Info.status.error = 1;

  #if (DAC_MODE_EVENT != 0)
    if (_DAC_Info.cb_event != NULL) {
        _DAC_Info.cb_event(ARM_DAC_EVENT_ERROR, ARM_DAC_CHANNEL_ALL);
    }
  #endif
}


/**
 * 获取驱动程序版本信息.
 *
 * @return  \ref ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION DAC_GetVersion(void)
{
    return( _DAC_DriverVersion );
}


/**
 * 获取驱动程序功能信息.
 *
 * @return  \ref ARM_DAC_CAPABILITIES
 */
static ARM_DAC_CAPABILITIES DAC_GetCapabilities(void)
{
    return( _DAC_DriverCapabilities );
}


/**
 * 初始化
 *
 * @param[in]  cb_event   Pointer to \ref ARM_DAC_SignalEvent
 *
 * @return     \ref execution_status
 */
static int32_t DAC_Initialize(ARM_DAC_SignalEvent_t cb_event)
{
    UNUSED_VAR(cb_event);

    if((_DAC_Info.flags & DAC_FLAG_INIT) != 0) {
        return( ARM_DRIVER_OK );    // Driver is already initialized
    }
    if (SPI_Initialize() != ARM_DRIVER_OK) {
        return( ARM_DRIVER_ERROR );
    }
  #if (DAC_MODE_EVENT != 0)
    _DAC_Info.cb_event = cb_event;
  #endif
    _DAC_Info.flags  = DAC_FLAG_INIT;

    return( ARM_DRIVER_OK );
}


/**
 * 取消初始化.
 *
 * @return  \ref execution_status
 */
static int32_t DAC_Uninitialize(void)
{
    _DAC_Info.flags  = 0;

    if (SPI_Uninitialize() != ARM_DRIVER_OK) {
        return( ARM_DRIVER_ERROR );
    }
    return( ARM_DRIVER_OK );
}


/**
 * 控制 DAC 电源
 *
 * @param[in]  state   电源状态
 *
 * @return     \ref execution_status
 */
static int32_t DAC_PowerControl(ARM_POWER_STATE state)
{
    switch((int32_t)state)
    {
    case ARM_POWER_OFF:
        _DAC_Info.flags &= ~DAC_FLAG_POWER;
        if (SPI_PowerOFF() != ARM_DRIVER_OK) {  
            return( ARM_DRIVER_ERROR );
        }
        return( ARM_DRIVER_OK );

    case ARM_POWER_FULL:
        if((_DAC_Info.flags & DAC_FLAG_INIT) == 0) {
            return( ARM_DRIVER_ERROR );
        }
        if((_DAC_Info.flags & DAC_FLAG_POWER) != 0) {
            return( ARM_DRIVER_OK );
        }
        if (SPI_PowerON() != ARM_DRIVER_OK) {
            return( ARM_DRIVER_ERROR );
        }
        if (Device_Init() != ARM_DRIVER_OK) {   // Initialize DAC device
            return( ARM_DRIVER_ERROR );
        }
        _DAC_Info.status.busy  = 0;
        _DAC_Info.status.error = 0;
        _DAC_Info.flags |= DAC_FLAG_POWER;
        return( ARM_DRIVER_OK );

    case ARM_POWER_LOW:
      return( ARM_DRIVER_ERROR_UNSUPPORTED );

    default:
      return( ARM_DRIVER_ERROR );
    }
}


/**
 * 设置 DAC 输出值(直接输出模式).
 *
 * @param[in]  ch    DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 * @param[in]  val   将要转换的值, 对齐方式:
 *                   @arg 分辨率(01 ~~ 08):  8bit 左对齐
 *                   @arg 分辨率(09 ~~ 16): 16bit 左对齐
 *                   @arg 分辨率(17 ~~ 32): 32bit 左对齐
 *
 * @return     \ref execution_status
 */
static int32_t DAC_SetValue(uint32_t ch, uint32_t val)
{
    if((ch >= (DAC_CHANNELS * DAC_CHIPS)) && (ch != ARM_DAC_CHANNEL_ALL)) {
        return( ARM_DRIVER_ERROR_PARAMETER );
    }
    if ((_DAC_Info.flags & DAC_FLAG_POWER) == 0) {
        return( ARM_DRIVER_ERROR );
    }
    if (_DAC_Info.status.busy) {
        return( ARM_DRIVER_ERROR_BUSY );
    }
    _DAC_Info.status.error = 0;

    if (ch != ARM_DAC_CHANNEL_ALL)
    {   // 指定通道
        _DAC_Info.status.busy  = 1; // Clear in: DAC_EventComplete() or DAC_EventError()
        if (Device_Output(ch / DAC_CHANNELS, ch % DAC_CHANNELS, val) != ARM_DRIVER_OK) {
            return( ARM_DRIVER_ERROR );
        }
        return( ARM_DRIVER_OK );
    }

    for(ch = 0;  ch < (DAC_CHANNELS * DAC_CHIPS);  ch++)
    {   // 所有通道
        while(_DAC_Info.status.busy) ;
        _DAC_Info.status.busy  = 1; // Clear in: DAC_EventComplete() or DAC_EventError()
        if (Device_Output(ch / DAC_CHANNELS, ch % DAC_CHANNELS, val) != ARM_DRIVER_OK) {
            return( ARM_DRIVER_ERROR );
        }
    }
    return( ARM_DRIVER_OK );
}


/**
 * 设置 DAC 转换值(触发输出模式).
 *
 * @param[in]  ch     DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 * @param[in]  data   输出数据条目缓存, NULL表示无输出数据, 类型与对齐方式参考 \ref ARM_DAC_SetValue
 * @param[in]  num    输出数据条目数(非 data 字节数) 
 *
 * @return     \ref execution_status
 */
static int32_t DAC_SetData(uint32_t ch, const void *data, size_t num)
{
    UNUSED_VAR(ch);
    UNUSED_VAR(data);
    UNUSED_VAR(num);
    return( ARM_DRIVER_ERROR_UNSUPPORTED );
}


/**
 * 触发 DAC 转换.
 *
 * @param[in]  ch   DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 *
 * @return     \ref execution_status
 */
static int32_t DAC_Trigger(uint32_t ch)
{
    UNUSED_VAR(ch);
    return( ARM_DRIVER_ERROR_UNSUPPORTED );
}


/**
 * 终止 DAC 转换.
 *
 * @param[in]  ch   DAC 通道, 最小通道为0, ARM_DAC_CHANNEL_ALL 所有通道
 *
 * @return     \ref execution_status
 */
static int32_t DAC_Abort(uint32_t ch)
{
    UNUSED_VAR(ch);
    return( ARM_DRIVER_OK );
}


/**
 * 获取 DAC 已转换的数据条目数.
 *
 * @param[in]  ch   DAC 通道, 最小通道为0
 *
 * @return     已转换的数据条目数量或, 小于(0)为错误状态码 \ref execution_status
 */
static int32_t DAC_GetDataCount(uint32_t ch)
{
    UNUSED_VAR(ch);
    return( ARM_DRIVER_ERROR_UNSUPPORTED );
}


/**
 * 获取 DAC 状态.
 *
 * @return  DAC status \ref ARM_DAC_STATUS
 */
static ARM_DAC_STATUS DAC_GetStatus(void)
{
    return( _DAC_Info.status );
}


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Variables
 * @{
 ******************************************************************************/
/**
 * DAC Driver Control Block
 */
extern
ARM_DRIVER_DAC  ARM_Driver_DAC_(DAC_DRV_NUM);
ARM_DRIVER_DAC  ARM_Driver_DAC_(DAC_DRV_NUM) = {
    DAC_GetVersion,
    DAC_GetCapabilities,
    DAC_Initialize,
    DAC_Uninitialize,
    DAC_PowerControl,
    DAC_SetValue,
    DAC_SetData,
    DAC_Trigger,
    DAC_Abort,
    DAC_GetDataCount,
    DAC_GetStatus
};


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

