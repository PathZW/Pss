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

#define ARM_DAC_CHANNEL_ALL     (0xFFFFFFFFul)      //!< DAC ����ͨ��

/*-------------------------- DAC_Event ---------------------------------------*/
#define ARM_DAC_EVENT_COMPLETE      (1ul)           //!< ת�����
#define ARM_DAC_EVENT_ERROR         (3ul)           //!< ��⵽����

/*----------------------------------------------------------------------------*/
/**
 * @brief DAC Status
 */
typedef volatile struct _ARM_DAC_STATUS {
    uint32_t    busy        : 1;    //!< ��æ��־
    uint32_t    error       : 1;    //!< �����־(�´β�����ʼʱ���)
    uint32_t    reserved    : 30;
} ARM_DAC_STATUS;

/**
 * @brief DAC Driver Capabilities.
 */
typedef struct _ARM_DAC_CAPABILITIES {
    uint32_t    channels    : 10;   //!< DAC ͨ����, 1 ~ 512
    uint32_t    resolution  : 6;    //!< DAC �ֱ���, 1 ~ 32
    uint32_t    event       : 1;    //!< �Ƿ�֧���¼�ģʽ
    uint32_t    trigger     : 1;    //!< �Ƿ�֧�ִ���ģʽ
    uint32_t    reserved    : 14;   //!< Reserved (must be zero)
} ARM_DAC_CAPABILITIES;

/**
 * Pointer to \ref ARM_DAC_SignalEvent : �źŴ���ص�����.
 */
typedef void (*ARM_DAC_SignalEvent_t)(uint32_t event, uint32_t arg);

/**
 * @brief Access structure of the DAC Driver
 */
typedef struct _ARM_DRIVER_DAC {
    ARM_DRIVER_VERSION   (*GetVersion)     (void);                                      //!< Pointer to \ref ARM_DAC_GetVersion      : ��ȡ��������汾��Ϣ.
    ARM_DAC_CAPABILITIES (*GetCapabilities)(void);                                      //!< Pointer to \ref ARM_DAC_GetCapabilities : ��ȡ������������Ϣ(��ʵ�����).
    int32_t              (*Initialize)     (ARM_DAC_SignalEvent_t cb_event);            //!< Pointer to \ref ARM_DAC_Initialize      : ��ʼ��
    int32_t              (*Uninitialize)   (void);                                      //!< Pointer to \ref ARM_DAC_Uninitialize    : ȡ����ʼ��.
    int32_t              (*PowerControl)   (ARM_POWER_STATE state);                     //!< Pointer to \ref ARM_DAC_PowerControl    : ���� DAC ��Դ.
    int32_t              (*SetValue)       (uint32_t ch, uint32_t    val);              //!< Pointer to \ref ARM_DAC_SetValue        : ���� DAC ���ֵ(ֱ�����ģʽ).
    int32_t              (*SetData)        (uint32_t ch, const void *data, size_t num); //!< Pointer to \ref ARM_DAC_SetData         : ���� DAC ת��ֵ(�������ģʽ).
    int32_t              (*Trigger)        (uint32_t ch);                               //!< Pointer to \ref ARM_DAC_Trigger         : ���� DAC ת��.
    int32_t              (*Abort)          (uint32_t ch);                               //!< Pointer to \ref ARM_DAC_Abort           : ��ֹ DAC ת��.
    int32_t              (*GetDataCount)   (uint32_t ch);                               //!< Pointer to \ref ARM_DAC_GetDataCount    : ��ȡ DAC ��ת����������Ŀ��
    ARM_DAC_STATUS       (*GetStatus)      (void);                                      //!< Pointer to \ref ARM_DAC_GetStatus       : ��ȡ DAC ״̬.
} const ARM_DRIVER_DAC;


/*------------------ Function documentation ----------------------------------*/
/**
 * @fn      ARM_DRIVER_VERSION ARM_DAC_GetVersion(void)
 * @brief   ��ȡ��������汾��Ϣ(��ʵ�����).
 *
 * @return  \ref ARM_DRIVER_VERSION
 */

/**
 * @fn      ARM_DAC_CAPABILITIES ARM_DAC_GetCapabilities(void)
 * @brief   ��ȡ������������Ϣ(��ʵ�����).
 *
 * @return  \ref ARM_DAC_CAPABILITIES
 */

/**
 * @fn      int32_t ARM_DAC_Initialize(ARM_DAC_SignalEvent_t cb_event)
 * @brief   ��ʼ��
 *
 * @param[in]  cb_event   Pointer to \ref ARM_DAC_SignalEvent
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_Uninitialize(void)
 * @brief   ȡ����ʼ��.
 *
 * @return  \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_PowerControl(ARM_POWER_STATE state)
 * @brief   ���� DAC ��Դ
 *
 * @param[in]  state   ��Դ״̬
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_SetValue(uint32_t ch, uint32_t val)
 * @brief   ���� DAC ���ֵ(ֱ�����ģʽ).
 *
 * @param[in]  ch    DAC ͨ��, ��Сͨ��Ϊ0, ARM_DAC_CHANNEL_ALL ����ͨ��
 * @param[in]  val   ��Ҫת����ֵ, ���뷽ʽ:
 *                   @arg �ֱ���(01 ~~ 08):  8bit �����
 *                   @arg �ֱ���(09 ~~ 16): 16bit �����
 *                   @arg �ֱ���(17 ~~ 32): 32bit �����
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_SetData(uint32_t ch, const void *data, size_t num)
 * @brief   ���� DAC ת��ֵ(�������ģʽ).
 *
 * @param[in]  ch     DAC ͨ��, ��Сͨ��Ϊ0, ARM_DAC_CHANNEL_ALL ����ͨ��
 * @param[in]  data   ���������Ŀ����, NULL��ʾ���������, ��������뷽ʽ�ο� \ref ARM_DAC_SetValue
 * @param[in]  num    ���������Ŀ��(�� data �ֽ���)
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_Trigger(uint32_t ch)
 * @brief   ���� DAC ת��.
 *
 * @param[in]  ch   DAC ͨ��, ��Сͨ��Ϊ0, ARM_DAC_CHANNEL_ALL ����ͨ��
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_Abort(uint32_t ch)
 * @brief   ��ֹ DAC ת��.
 *
 * @param[in]  ch   DAC ͨ��, ��Сͨ��Ϊ0, ARM_DAC_CHANNEL_ALL ����ͨ��
 *
 * @return     \ref execution_status
 */

/**
 * @fn      int32_t ARM_DAC_GetDataCount(uint32_t ch)
 * @brief   ��ȡ DAC ��ת����������Ŀ��.
 *
 * @param[in]  ch   DAC ͨ��, ��Сͨ��Ϊ0
 *
 * @return     ��ת����������Ŀ������, С��(0)Ϊ����״̬�� \ref execution_status
 */

/**
 * @fn      ARM_DAC_STATUS ARM_DAC_GetStatus(void)
 * @brief   ��ȡ DAC ״̬.
 *
 * @return  DAC status \ref ARM_DAC_STATUS
 */

/**
 * @fn      void ARM_DAC_SignalEvent(uint32_t event, uint32_t arg)
 * @brief   �źŴ���ص�����, ����������������ڼ䷢����һ�������ź�.
 *
 * @param[in]  event   �¼�֪ͨ��, \ref DAC_Event
 * @param[in]  arg     DAC ͨ��, ��Сͨ��Ϊ0, ARM_DAC_CHANNEL_ALL ����ͨ��
 */


/*****************************  END OF FILE  **********************************/
#ifdef  __cplusplus
}
#endif
#endif  // __DRIVER_DAC_H__
/*****************************  END OF FILE  **********************************/

