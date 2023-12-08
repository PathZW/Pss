/*******************************************************************************
 * @file    devDriver_Common.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/25
 * @brief   ͨ���豸�������
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/3/25 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DEV_DRIVER_COMMON_H__
#define __DEV_DRIVER_COMMON_H__
/******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
//lint -emacro(774, DEV_CMD_*, DEV_DRIVER_*, DEV_EVT_*)
//lint -emacro(845, DEV_CMD_*, DEV_DRIVER_*, DEV_EVT_*)
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Device_Driver
 * @{
 ******************************************************************************/
/**
 * �豸����
 */
typedef struct {
    int32_t   (*Send)(const void *data, size_t size, uint32_t opt); //!< ��������
    int32_t   (*Recv)(      void *data, size_t size, uint32_t opt); //!< ��������
    int32_t   (*Ctrl)(uint32_t cmd, uint32_t opt, ...);             //!< �豸����
    int32_t   (*Init)(void);                                        //!< ��ʼ��
    int32_t (*Uninit)(void);                                        //!< ����ʼ��
    const char *Name;                                               //!< �豸��
} const devDriver_t;

/**
 * ����һ���豸������, �� devDriver(ADC, 1) ������ _devDriver_ADC_1
 * @param[in]  name     �豸��; �� SPI ADC FLASH ��
 * @param[in]  n        �豸��; �� 1 2 3
 */
#define  devDriver(name, n)     ATPASTE4(_devDriver_, name, _, n)

/**
 * ��������
 * @param[in]  data     ���ͻ���ָ��
 * @param[in]  size     ���ͻ����С(���ֽ�Ϊ��λ)
 * @param[in]  opt      ��ѡ������
 * @retval    (>=0)     �ɹ����������ֽ���
 * @retval    (< 0)     ������
 * @note    1. �����ó�ʱ, ���¼�����ص������޹�, �ò���Ϊͬ��IO, �������ж��е���
 * @note    2. δ���ó�ʱ, ���¼�����ص�����Ϊ��, �ò���Ϊͬ��IO, �������ж��е���
 * @note    3. δ���ó�ʱ, ���¼�����ص������ǿ�, �ò���Ϊ�첽IO, �ܹ����ж��е���
 */
typedef int32_t (*devDriver_SEND)(const void *data, size_t size, uint32_t opt);

/**
 * ��������
 * @param[in]  data     ���ջ���ָ��
 * @param[in]  size     ���ջ����С(���ֽ�Ϊ��λ)
 * @param[in]  opt      ��ѡ������
 * @retval    (>=0)     �ɹ����������ֽ���
 * @retval    (< 0)     ������
 * @note    1. �����ó�ʱ, ���¼�����ص������޹�, �ò���Ϊͬ��IO, �������ж��е���
 * @note    2. δ���ó�ʱ, ���¼�����ص�����Ϊ��, �ò���Ϊͬ��IO, �������ж��е���
 * @note    3. δ���ó�ʱ, ���¼�����ص������ǿ�, �ò���Ϊ�첽IO, �ܹ����ж��е���
 */
typedef int32_t (*devDriver_RECV)(void *data, size_t size, uint32_t opt);

/**
 * �豸����, �ܹ����ж��е���
 * @param[in]  cmd      ������
 * @param[in]  opt      ��ѡ������
 * @retval    (>=0)     �����ֵ
 * @retval    (< 0)     ������
 */
typedef int32_t (*devDriver_CTRL)(uint32_t cmd, uint32_t opt, ...);

/**
 * ��ʼ��, �������ж��е���
 * @return     ������
 */
typedef int32_t (*devDriver_INIT)(void);

/**
 * ����ʼ��, �������ж��е���
 * @return     ������
 */
typedef int32_t (*devDriver_UNINIT)(void);

/**
 * flags ��������, �������ж��е���
 * @param[in]  opt      ��ѡ������
 * @retval     (>=0)    �ɹ�
 * @retval     (< 0)    ʧ��
 */
typedef int32_t (*devDriver_POST)(uint32_t opt);

/**
 * flags �ȴ�����, Send() �� Recv() �е���
 * @param[in]  opt      ��ѡ������( Send(, , opt) or Recv(, , opt) )
 * @param[in]  timeout  ��ʱ, (0)���ȴ�, (UINT32_MAX)���޵ȴ�
 * @retval     (>=0)    �ɹ�
 * @retval     (< 0)    ʧ��
 */
typedef int32_t (*devDriver_WAIT)(uint32_t timeout, uint32_t opt);

/**
 * �¼�����ص�����, �������ж��е���
 * @param[in]  device   �豸����ָ��
 * @param[in]  optsend  ��ѡ������(����)
 * @param[in]  optrecv  ��ѡ������(����)
 * @param[in]  evt      �¼���
 */
typedef void (*devDriver_CB)(devDriver_t* device, uint32_t optsend, uint32_t optrecv, uint32_t evt, ...);


/*******************************************************************************
 *
 * @addtogroup              Event_Codes
 * @{
 ******************************************************************************/
// devDriver_CB()

#define DEV_EVT_ARG_MSK     (0x000000FFul)
#define DEV_EVT_POWER_CTRL     (1ul << 12)  //!< �¼�: ��Դ����; ����ARG:(0/1/2)�ر�/��/��λ
#define DEV_EVT_ERROE_SEND     (1ul << 13)  //!< �¼�: ���ʹ���;     �޲���
#define DEV_EVT_ERROE_RECV     (1ul << 14)  //!< �¼�: ���մ���;     �޲���
#define DEV_EVT_COMPLETE_SEND  (1ul << 15)  //!< �¼�: �������;     �޲���
#define DEV_EVT_COMPLETE_RECV  (1ul << 16)  //!< �¼�: �������;     �޲���
#define DEV_EVT_TIMEOUT_SEND   (1ul << 17)  //!< �¼�: ���ͳ�ʱ;     �޲���
#define DEV_EVT_TIMEOUT_RECV   (1ul << 18)  //!< �¼�: ���ճ�ʱ;     �޲���
#define DEV_EVT_ABORT_SEND     (1ul << 19)  //!< �¼�: ��ֹ����;     �޲���
#define DEV_EVT_ABORT_RECV     (1ul << 20)  //!< �¼�: ��ֹ����;     �޲���
#define DEV_EVT_FLUSH_SEND     (1ul << 21)  //!< �¼�: ��ϴ���ͻ���; �޲���
#define DEV_EVT_FLUSH_RECV     (1ul << 22)  //!< �¼�: ��ϴ���ջ���; �޲���
#define DEV_EVT_BIT_DEF(n)     (1ul <<(23 + (n)))
#define DEV_EVT_BIT_CHK(n)     (31u >=(23 + (n)))


// devDriver_CB()
/*******************************************************************************
 * @}
 * @addtogroup              Command_Codes
 * @{
 ******************************************************************************/
//lint -emacro(835, DEV_CMD_*)
//lint -emacro(845, DEV_CMD_*)
// devDriver_CTRL()

#define DEV_CMD_ARG_DEF(n)   (0x0000000Ful & (n))
#define DEV_CMD_ARG_MSK      (0x0000000Ful)
#define DEV_CMD_GRP_MSK      (0x0000FF00ul)
#define DEV_CMD_BIT_MSK      (0xFFFF0000ul)
#define DEV_CMD_GROUP_ECMD   (0x00000100ul) //!< ��������չ    ; ����3:��չ������
#define DEV_CMD_GROUP_FUNC   (0x00000200ul) //!< ����������չ  ; ����3:����ָ��
#define DEV_CMD_COUNT_SEND   (0x00000300ul) //!< ��ȡ�ѷ����ֽ�; �޲���; ����ֵ:����
#define DEV_CMD_COUNT_RECV   (0x00000400ul) //!< ��ȡ�ѽ����ֽ�; �޲���; ����ֵ:����
#define DEV_CMD_POWER_ON     (0x00000500ul) //!< �豸��Դ��  ; �޲���; �޷���ֵ
#define DEV_CMD_POWER_OFF    (0x00000600ul) //!< �豸��Դ�ر�  ; �޲���; �޷���ֵ
#define DEV_CMD_POWER_RST    (0x00000700ul) //!< �豸��λ      ; �޲���; �޷���ֵ
#define DEV_CMD_GRP______
#define DEV_CMD_GRP_DEF(n)   (0x00000800ul + ((n) << 8))
#define DEV_CMD_GRP_CHK(n)  ((0x00000800ul + ((n) << 8)) <= 0x0000FF00ul)

#define DEV_CMD_CALLBACK_FUN    (1ul << 16) //!< �¼�����; ����2(opt): δʹ��; ����3:�¼�����ص�����        ; �޷���ֵ
#define DEV_CMD_TIMEOUT_SEND    (1ul << 17) //!< ��ʱ����; ����2(opt):32b��ʱ; ����3:��������; ����4:�ȴ�����; �޷���ֵ
#define DEV_CMD_TIMEOUT_RECV    (1ul << 18) //!< ��ʱ����; ����2(opt):32b��ʱ; ����3:��������; ����4:�ȴ�����; �޷���ֵ
#define DEV_CMD_ABORT_SEND      (1ul << 19) //!< ��ֹ����; �޲���; �޷���ֵ
#define DEV_CMD_ABORT_RECV      (1ul << 20) //!< ��ֹ����; �޲���; �޷���ֵ
#define DEV_CMD_FLUSH_SEND      (1ul << 21) //!< ��ϴ����; �޲���; �޷���ֵ
#define DEV_CMD_FLUSH_RECV      (1ul << 22) //!< ��ϴ����; �޲���; �޷���ֵ
#define DEV_CMD_BIT______
#define DEV_CMD_BIT_DEF(n)      (1ul <<(23 + (n)))
#define DEV_CMD_BIT_CHK(n)      (31u >=(23 + (n)))


// devDriver_CTRL()
/*******************************************************************************
 * @}
 * @addtogroup              Error_Codes
 * @{
 ******************************************************************************/

#define DEV_DRIVER_OK                0      //!< �����ɹ�
#define DEV_DRIVER_ERR_PARAMETER    -1      //!< ��������
#define DEV_DRIVER_ERR_UNSUPPORTED  -2      //!< ������֧��
#define DEV_DRIVER_ERR_BUSY         -3      //!< �豸æ
#define DEV_DRIVER_ERR_TIMEOUT      -4      //!< ��ʱ
#define DEV_DRIVER_ERR_ABORT        -5      //!< ��ֹ����
#define DEV_DRIVER_ERR_FLUSH        -6      //!< ��ϴ����
#define DEV_DRIVER_ERR_EMPTY        -7      //!< �����
#define DEV_DRIVER_ERR_FULL         -8      //!< ������
#define DEV_DRIVER_ERR(n)     (-(n) -9)     //!< δ֪����


/*******************************************************************************
 * @}
 * @addtogroup              Macros
 * @{
 ******************************************************************************/
/**
 * @brief DEV_CMD_GPR_BLOCK_BGN() : Group ����鴦��ʼ��(����ʹ��";"��������)
 * @brief DEV_CMD_GPR_BLOCK_END() : Group ����鴦�������(����ʹ��";"��������)
 * @brief DEV_CMD_GPR_CODE_BGN()  : Group �����봦��ʼ��(����ʹ��";"��������)
 * @brief DEV_CMD_GPR_CODE_END()  : Group �����봦�������(����ʹ��";"��������)
 */
#define DEV_CMD_GPR_BLOCK_BGN(cmd)      switch (cmd) { default: break;
#define DEV_CMD_GPR_CODE_BGN(code,cmd)  case(code): { (cmd) &= ~DEV_CMD_GRP_MSK;
#define DEV_CMD_GPR_CODE_END(code,cmd)  } break;
#define DEV_CMD_GPR_BLOCK_END(cmd)      }

/**
 * @brief DEV_CMD_BIT_BLOCK_BGN() : Bit ����鴦��ʼ��(����ʹ��";"��������)
 * @brief DEV_CMD_BIT_BLOCK_END() : Bit ����鴦�������(����ʹ��";"��������)
 * @brief DEV_CMD_BIT_CODE_BGN()  : Bit �����봦��ʼ��(����ʹ��";"��������)
 * @brief DEV_CMD_BIT_CODE_END()  : Bit �����봦�������(����ʹ��";"��������)
 */

#define DEV_CMD_BIT_BLOCK_BGN(cmd)      {
#define DEV_CMD_BIT_CODE_BGN(code,cmd)  if((cmd) & (code)) { (cmd) &= ~(code);
#define DEV_CMD_BIT_CODE_END(code,cmd)  }
#define DEV_CMD_BIT_BLOCK_END(cmd)      }


/**
 * DEV_EVT_VAR_ALLOC  : �����¼�����
 * DEV_EVT_CODE_SET() : �����¼���
 * DEV_EVT_FUN_CALL() : �����¼�������
 */
#ifdef  DEV_DRIVER_INFO_USED
#define DEV_EVT_FUN_CALL(dev, ...)  if(((dev)->drvInfo->callback != NULL) && (evt_var != 0)) {             \
                                        (dev)->drvInfo->callback((dev)->device, (dev)->drvInfo->tx_opt     \
                                                                              , (dev)->drvInfo->rx_opt     \
                                                                              , evt_var                    \
                                                                              , ##__VA_ARGS__              \
                                                                );                                         \
                                    }
#define DEV_EVT_CODE_SET(code,arg)  BIT_MDF(evt_var, ((code) & ~DEV_EVT_ARG_MSK) |         DEV_EVT_ARG_MSK \
                                                   , ((code) & ~DEV_EVT_ARG_MSK) | (arg) & DEV_EVT_ARG_MSK \
                                           );
#define DEV_EVT_VAR_ALLOC           uint32_t  evt_var = 0;
#else
#define DEV_EVT_FUN_CALL(dev, ...)
#define DEV_EVT_CODE_SET(code,arg)
#define DEV_EVT_VAR_ALLOC
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif
/*****************************  END OF FILE  **********************************/

