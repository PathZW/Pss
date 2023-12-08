/*******************************************************************************
 * @file    Cmd_SPIy.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.4
 * @date    2021/12/28
 * @brief   SPI �����нӿ�, ֧�� SPI Master �� SPI Slave ģʽ. ����Э��Ϊ��Э�̴���
 *          ���ݳ���, �ٴ������ 4095 �ֽ�����, ��ϸЭ��ο���Cmd_SPI ����Э�顷�ĵ�. \n
 *          SPI ����: ����ʹ�� CMSIS-Driver, �ɶ��� SPIy_INIT() SPIy_SEND_START()
 *                    SPIy_RECV_START(), ���� SPI ����ж��е��� SPIy_IntrComplete() \n
 *          NSS �ź�: ֧�� Ӳ��NSS �� �Զ���NSS, �Զ�����ʵ��3�� SPIy_NSS_ ��ͷ����. \n
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
 *      2021/8/17 --- Linghu -- Add Event Recorder Execution Statistics D(3, 4, 5, 6)
 *
 * @par Change Logs:
 *      2021/8/11 --- Linghu -- �޸� SPI_MasterStart() �������� BUG
 *
 * @par Change Logs:
 *      2021/1/20 --- Linghu -- ��� SPI DMA ģʽ֧��
 *
 * @par Change Logs:
 *      2021/1/11 --- Linghu -- ��� NSS(��Ӳ��) �ź�֧��
 *
 * @par Change Logs:
 *      2021/12/28 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro(835, ARM_SPI_*_*)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Cmd_SPIy"
#include "emb_log.h"
#include "Cmd_SPIy.h"

#ifdef   _RTE_                  // CMSIS_device_header
#include "RTE_Components.h"     // Component selection
#endif// _RTE_
#ifdef    RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

#include <stdint.h>
#include <string.h>
#include <defines.h>            // Used: ATPASTE2(), UNUSED_VAR(), __WEAK
#include "cmsis_compiler.h"     // Used: __STATIC_FORCEINLINE
#include "cmsis_os2.h"
#include "Driver_SPI.h"

#include "emb_critical.h"
#include "emb_queue.h"
#include "cmd/Cmd_App.h"
#include "shell/cli_shell.h"

// #undef  __STATIC_FORCEINLINE
// #define __STATIC_FORCEINLINE static

#if defined SPIy_NSS_INIT
__WEAK void SPIy_NSS_INIT(void) {}  //lint -esym(522, SPIy_NSS_INIT)
#endif
#if defined SPIy_ACT_LED
__WEAK void SPIy_ACT_LED(void) {}   //lint -esym(522, SPIy_ACT_LED)
#endif


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          Cmd_SPIy
 * @{
 ******************************************************************************/

#ifndef SPIy_DEVICE_NUM
#define SPIy_DEVICE_NUM             -1          //!< (C) SPI number, ( < 0) Disable
#endif
#ifndef SPIy_BUS_SPEED
#define SPIy_BUS_SPEED           4000000        //!< (C) �����ٶ�(�� Hz Ϊ��λ)
#endif
#ifndef SPIy_BUFF_SIZE
#define SPIy_BUFF_SIZE             256          //!< (C) ��������(���ֽ�Ϊ��λ)
#endif

#ifndef SPIy_CMD_STR_EN
#define SPIy_CMD_STR_EN              1          //!< (R) ��(1)/��(0) ֧���ַ�������
#endif
#ifndef SPIy_CMD_BIN_EN
#define SPIy_CMD_BIN_EN              0          //!< (R) ��(1)/��(0) ֧�ֶ���������
#endif
#ifndef SPIy_FORWARD_IDX
#define SPIy_FORWARD_IDX            -1          //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
#endif
#ifndef SPIy_FORWARD_SRC
#define SPIy_FORWARD_SRC            -1          //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�
#endif

/*----------------------------------------------------------------------------*/
#ifndef SPIy_POLL_TICKS
#define SPIy_POLL_TICKS              2          //!< (R) Master ��ѯʱ��, (< 0)����ѯ
#endif

#ifndef SPIy_MODE_MASTER
#define SPIy_MODE_MASTER             0          //!< (C) Masterģʽ(1) �� Slaveģʽ(0)
#endif
#ifndef SPIy_MODE_SIMPLEX
#define SPIy_MODE_SIMPLEX            0          //!< (C) ��˫��(1) �� ȫ˫��(0)
#endif
#ifndef SPIy_MODE_NSS_SW
#define SPIy_MODE_NSS_SW             0          //!< (C) NSS�ź�; 1: CMSIS SW, 0: Master�Զ��� | Slave CMSIS HW
#endif
#ifndef SPIy_MODE_CFG
#define SPIy_MODE_CFG    (  ARM_SPI_MSB_LSB     /*!< (C) SPI ����ģʽ     */ \
                          | ARM_SPI_CPOL0_CPHA0                              \
                          | ARM_SPI_DATA_BITS(8)                             \
                         )
#endif

#ifndef SPIy_INIT
#define SPIy_INIT               SPIy_INIT       //!< (C) SPI ��ʼ�� (������)
#endif
#ifndef SPIy_SEND_START
#define SPIy_SEND_START      SPIy_SEND_START    //!< (C) SPI ��ʼ�������� (������)
#endif
#ifndef SPIy_RECV_START
#define SPIy_RECV_START      SPIy_RECV_START    //!< (C) SPI ��ʼ�������� (������)
#endif

#ifndef SPIy_NSS_INIT
#define SPIy_NSS_INIT()                         //!< (C) SPI NSS �źų�ʼ�� (������)
#endif
#ifndef SPIy_NSS_CLR
#define SPIy_NSS_CLR            SPIy_NSS_CLR    //!< (C) SPI NSS �ź���Ϊ��Ч (������)
#endif
#ifndef SPIy_NSS_SET
#define SPIy_NSS_SET            SPIy_NSS_SET    //!< (C) SPI NSS �ź���Ϊ��Ч (������)
#endif

#ifndef SPIy_ACT_LED
#define SPIy_ACT_LED()                          //!< (C) SPI ����ӿڼ�����ʾ (������)
#endif


/*******************************************************************************
 * @}
 * @}
 */
#if (SPIy_DEVICE_NUM >= 0)
/**
 * @addtogroup Cmd_SPIy
 * @{
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/
//lint -esym(526, Driver_SPI*)
//lint -esym(767, ptrSPI, CMD_CLI_*, CMD_SPI_CFG_*)

extern ARM_DRIVER_SPI       ATPASTE2(Driver_SPI, SPIy_DEVICE_NUM);
#define ptrSPI            (&ATPASTE2(Driver_SPI, SPIy_DEVICE_NUM))

#define CMD_CLI_NAME        "SPIy"

#define CMD_CLI_FLAGS(bSTR, bBIN)   /*lint -save -w1 */            \
                                    (  ((bSTR) ? CLI_EXEC_STR : 0) \
                                     | ((bBIN) ? CLI_EXEC_BIN : 0) \
                                    )                              \
                                    /*lint -restore */

/*----------------------------------------------------------------------------*/
#define CMD_SPI_EVT_SEND    ((uint32_t)0x01)        // ��������¼�
#define CMD_SPI_EVT_RECV    ((uint32_t)0x02)        // ��������¼�

#define CMD_SPI_LEN_MASK    ((uint8_t)0x0F)         // ���� mask
#define CMD_SPI_OPT_MASK    ((uint8_t)0xF0)         // ���� mask
#define CMD_SPI_OPT_NONE    ((uint8_t)0x00)
#define CMD_SPI_OPT_MOSI    ((uint8_t)0x10)         // Э�� MOSI ����
#define CMD_SPI_OPT_MISO    ((uint8_t)0x20)         // Э�� MISO ����

#define CMD_SPI_STAT_FREE           0               // ����
#define CMD_SPI_STAT_START          1               // ��������
#define CMD_SPI_NEGO_MO_TX          2               // Master ���� MOSIЭ��
#define CMD_SPI_NEGO_MI_TX          3               // Master ���� MISOЭ��
#define CMD_SPI_NEGO_MO_RX          4               // Master ���� MOSIЭ��
#define CMD_SPI_NEGO_MI_RX          5               // Master ���� MISOЭ��
#define CMD_SPI_DATA_MO_TX          6               // Master ���� MOSI����
#define CMD_SPI_DATA_MI_RX          7               // Master ���� MISO����
#define CMD_SPI_NEGO_RX             8               // Slave  ���� Э��
#define CMD_SPI_NEGO_SO_TX          9               // Slave  ���� MISOЭ��
#define CMD_SPI_NEGO_SI_TX         10               // Slave  ���� MOSIЭ��
#define CMD_SPI_DATA_SO_TX         11               // Slave  ���� MISO����
#define CMD_SPI_DATA_SI_RX         12               // Slave  ���� MOSI����


/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

static QUEUE_TYPE(uint8_t, SPIy_BUFF_SIZE)  _tx_Queue;   // ���Ͷ���
static QUEUE_TYPE(uint8_t, SPIy_BUFF_SIZE)  _rx_Queue;   // ���ն���

static osEventFlagsId_t                     _spi_Evt;    // Event(����/����)

static uint8_t                              _spi_Stat;   // ͨ��״̬(�ο� )
static uint8_t                              _neg_Buf[2]; // Э�̻���


/*******************************************************************************
 * @}
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/

#if (SPIy_MODE_MASTER != 0)
static               void SPI_MasterStart(void *arg);
#else
__STATIC_FORCEINLINE void SPI_SlaveStart (void);
#endif

static void SPI_SignalEvent(uint32_t event);


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
//lint -esym(522, SPIy_NSS*)
//lint -esym(528, SPIy_NSS*)

#ifdef SPIy_INIT
__WEAK int SPIy_INIT(void)
{
    uint32_t    control;

#if SPIy_MODE_MASTER            /*lint -save -w1 */
  #if SPIy_MODE_SIMPLEX
    control  = ARM_SPI_MODE_MASTER_SIMPLEX;
  #else
    control  = ARM_SPI_MODE_MASTER;
  #endif
  #if SPIy_MODE_NSS_SW
    control |= ARM_SPI_SS_MASTER_SW;
  #else
    control |= ARM_SPI_SS_MASTER_UNUSED;
  #endif
#else  // SPIy_MODE_MASTER
  #if SPIy_MODE_SIMPLEX
    control  = ARM_SPI_MODE_SLAVE_SIMPLEX;
  #else
    control  = ARM_SPI_MODE_SLAVE;
  #endif
  #if SPIy_MODE_NSS_SW
    control |= ARM_SPI_SS_SLAVE_SW;
  #else
    control |= ARM_SPI_SS_SLAVE_HW;
  #endif
#endif // SPIy_MODE_MASTER
    control |= SPIy_MODE_CFG;   /*lint -restore */


    if (ptrSPI->GetVersion().api < 0x200) {
        LOG_ERROR("requires at CMSIS-Driver API version 2.00 or higher");
        return( -1 );
    }
    if (ptrSPI->Initialize(SPI_SignalEvent) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->Initialize(SPI_SignalEvent)");
        return( -1 );
    }
    if (ptrSPI->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->PowerControl(ARM_POWER_FULL)");
        return( -1 );
    }
    if (ptrSPI->Control(control, SPIy_BUS_SPEED) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->Control(SPIy_MODE_CFG, SPIy_BUS_SPEED)");
        return( -1 );
    }
    if (ptrSPI->Control(ARM_SPI_SET_DEFAULT_TX_VALUE, 0x0000) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->Control(ARM_SPI_SET_DEFAULT_TX_VALUE, 0x0000)");
        return( -1 );
    }
    return( 0 );
}


__WEAK int SPIy_SEND_START(const void *data, unsigned num)
{
    return( ptrSPI->Send(data, num) );
}


__WEAK int SPIy_RECV_START(void *data, unsigned num)
{
    return( ptrSPI->Receive(data, num) );
}
#endif


__WEAK void SPIy_NSS_CLR(void)
{
#if SPIy_MODE_NSS_SW    // NSS�ź�Ϊ CMSIS-Driver Software ģʽ
    if (ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE)");
    }
#endif
}


__WEAK void SPIy_NSS_SET(void)
{
#if SPIy_MODE_NSS_SW    // NSS�ź�Ϊ CMSIS-Driver Software ģʽ
    if (ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE)");
    }
#endif
}


void SPIy_IntrMISO(void)
{
#if (SPIy_MODE_MASTER != 0)
    SPI_MasterStart((void *)CMD_SPI_OPT_MISO);      // Master ��������
#endif
}


void SPIy_IntrComplete(void)
{
    static uint16_t  _size;
    static void     *_buff;
    uint16_t          len;

    switch(_spi_Stat)
    {
#if (SPIy_MODE_MASTER != 0)
    default :
        SPI_STAT_NONE :
        _spi_Stat = CMD_SPI_STAT_FREE;
        SPIy_NSS_CLR();                                                 // NSS �ź���Ϊ��Ч
    case CMD_SPI_STAT_FREE:  //lint !e825
    case CMD_SPI_STAT_START:
        break;

    case CMD_SPI_NEGO_MO_TX :   // -------- Э�̳���(Master ����) ------------
        _spi_Stat = CMD_SPI_NEGO_MO_RX;
        _neg_Buf[0] = CMD_SPI_OPT_NONE,  _neg_Buf[1] = 0;
        SPIy_RECV_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534   // ��ʼ����Э��
        break;

    case CMD_SPI_NEGO_MI_TX :   // -------- Э�̳���(Master ����) ------------
        _spi_Stat = CMD_SPI_NEGO_MI_RX;
        _neg_Buf[0] = CMD_SPI_OPT_NONE,  _neg_Buf[1] = 0;
        SPIy_RECV_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534   // ��ʼ����Э��
        break;

    case CMD_SPI_NEGO_MO_RX :   // -------- Э�̳���(Master ����) ------------
        if((_neg_Buf[0] & CMD_SPI_OPT_MASK) != CMD_SPI_OPT_MOSI) {
            goto SPI_STAT_NONE;     //lint !e801                        // ��Ч�Ĳ���(slave����)
        }
        _size = MAKEWORD(_neg_Buf[1], _neg_Buf[0] & CMD_SPI_LEN_MASK);  // ���յ��ĳ���(slave����)
        len   = QUEUE_O_BUF(_tx_Queue, &_buff);                         // ��ȡ���л���(����, ָ��)
        if((_size = MIN(len, _size)) <= 0) { //lint !e775
            goto SPI_STAT_NONE;              //lint !e801               // ����Э�̷��ͳ���Ϊ0
        }
        _spi_Stat = CMD_SPI_DATA_MO_TX;
        SPIy_SEND_START(_buff, _size); //lint !e534                     // ��ʼ��������
        break;

    case CMD_SPI_NEGO_MI_RX :   // -------- Э�̳���(Master ����) ------------
        if((_neg_Buf[0] & CMD_SPI_OPT_MASK) != CMD_SPI_OPT_MISO)  {
            goto SPI_STAT_NONE;        //lint !e801                     // ��Ч�Ĳ���(slave����)
        }
        _size = MAKEWORD(_neg_Buf[1], _neg_Buf[0] & CMD_SPI_LEN_MASK);  // ���յ��ĳ���(slave����)
        len   = QUEUE_I_BUF(_rx_Queue, &_buff);                         // ��ȡ���л���(����, ָ��)
        if((_size = MIN(len, _size)) <= 0) { //lint !e775
            goto SPI_STAT_NONE;              //lint !e801               // ����Э�̽��ճ���Ϊ0
        }
        _spi_Stat = CMD_SPI_DATA_MI_RX;
        SPIy_RECV_START(_buff, _size); //lint !e534                     // ��ʼ��������
        break;

    case CMD_SPI_DATA_MO_TX :   // -------- ��������(Master ����) ------------
        QUEUE_O_ADJ(_tx_Queue, _size);                                  // ���ٶ�����Ԫ�ظ���
        _spi_Stat = CMD_SPI_STAT_FREE;
        SPIy_NSS_CLR();                                                 // NSS �ź���Ϊ��Ч
        if((osEventFlagsGet(_spi_Evt) & CMD_SPI_EVT_SEND) != CMD_SPI_EVT_SEND) {
            osEventFlagsSet(_spi_Evt  , CMD_SPI_EVT_SEND); //lint !e534
        }
      #ifdef RTE_Compiler_EventRecorder
        EventStopDx(3, "__SEND_STOP__", _size);      //lint !e534 !e737 !e835
      #endif
        SPI_MasterStart((void *)CMD_SPI_OPT_MOSI);                      // Master ��������
        break;

    case CMD_SPI_DATA_MI_RX :   // -------- ��������(Master ����) ------------
        QUEUE_I_ADJ(_rx_Queue, _size);                                  // ���Ӷ�����Ԫ�ظ���
        _spi_Stat = CMD_SPI_STAT_FREE;
        SPIy_NSS_CLR();                                                 // NSS �ź���Ϊ��Ч
        if((osEventFlagsGet(_spi_Evt) & CMD_SPI_EVT_RECV) != CMD_SPI_EVT_RECV) {
            osEventFlagsSet(_spi_Evt  , CMD_SPI_EVT_RECV); //lint !e534
        }
      #ifdef RTE_Compiler_EventRecorder
        EventStopDx(4, "__RECV_STOP__", _size);      //lint !e534 !e737 !e835
      #endif
        SPI_MasterStart((void *)CMD_SPI_OPT_MISO);                      // Master ��������
        break;

#else
    default :
        SPI_NEGO_SI :
     // SPIy_NSS_CLR();     // note                                     // NSS �ź���Ϊ��Ч
        SPI_SlaveStart();                                               // ��ʼ����Э��
        break;

    case CMD_SPI_NEGO_RX :      // -------- Э�̳���(Slave ����) ------------
        _size = MAKEWORD(_neg_Buf[1], _neg_Buf[0] & CMD_SPI_LEN_MASK);  // ���յ��ĳ���(Master����)
        switch(_neg_Buf[0] & CMD_SPI_OPT_MASK) {
        default               :  goto SPI_NEGO_SI;      //lint !e801    // ��Ч�Ĳ���
        case CMD_SPI_OPT_MOSI :  len = QUEUE_I_BUF(_rx_Queue, &_buff); _spi_Stat = CMD_SPI_NEGO_SI_TX; break;
        case CMD_SPI_OPT_MISO :  len = QUEUE_O_BUF(_tx_Queue, &_buff); _spi_Stat = CMD_SPI_NEGO_SO_TX; break;
        }                            // ���� ��ȡ���л���(����, ָ��) ����
        _neg_Buf[0] = (HIBYTE(len) & CMD_SPI_LEN_MASK) | (_neg_Buf[0] & CMD_SPI_OPT_MASK);
        _neg_Buf[1] =  LOBYTE(len);
        SPIy_SEND_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534   // ��ʼ����Э��
        _size = MIN(len, _size);                                        // ����Э�̴��䳤��
        break;

    case CMD_SPI_NEGO_SO_TX :   // -------- Э�̳���(Slave ����) ------------
        if(_size <= 0)  goto SPI_NEGO_SI; //lint !e775 !e801            // ����Э�̷��ͳ���Ϊ0
        _spi_Stat = CMD_SPI_DATA_SO_TX;
        SPIy_SEND_START(_buff, _size); //lint !e534                     // ��ʼ��������
        break;

    case CMD_SPI_NEGO_SI_TX :   // -------- Э�̳���(Slave ����) ------------
        if(_size <= 0)  goto SPI_NEGO_SI; //lint !e775 !e801            // ����Э�̽��ճ���Ϊ0
        _spi_Stat = CMD_SPI_DATA_SI_RX;
        SPIy_RECV_START(_buff, _size); //lint !e534                     // ��ʼ��������
        break;

    case CMD_SPI_DATA_SO_TX :   // -------- ��������(Slave ����) ------------
        QUEUE_O_ADJ(_tx_Queue, _size);                                  // ���ٶ�����Ԫ�ظ���
     // SPIy_NSS_CLR();     // note                                     // NSS �ź���Ϊ��Ч
        if((osEventFlagsGet(_spi_Evt) & CMD_SPI_EVT_SEND) != CMD_SPI_EVT_SEND) {
            osEventFlagsSet(_spi_Evt  , CMD_SPI_EVT_SEND); //lint !e534
        }
        goto SPI_NEGO_SI;                            //lint !e801

    case CMD_SPI_DATA_SI_RX :   // -------- ��������(Slave ����) ------------
        QUEUE_I_ADJ(_rx_Queue, _size);                                  // ���Ӷ�����Ԫ�ظ���
     // SPIy_NSS_CLR();     // note                                     // NSS �ź���Ϊ��Ч
        if((osEventFlagsGet(_spi_Evt) & CMD_SPI_EVT_RECV) != CMD_SPI_EVT_RECV) {
            osEventFlagsSet(_spi_Evt  , CMD_SPI_EVT_RECV); //lint !e534
        }
        goto SPI_NEGO_SI;                            //lint !e801
#endif // (SPIy_MODE_MASTER != 0)
    }
}


/*----------------------------------------------------------------------------*/
#if (SPIy_MODE_MASTER != 0)
/**
 * Master ͨ������
 *
 * @param[in]  arg   �Զ�(CMD_SPI_OPT_NONE), ����(CMD_SPI_OPT_MOSI), ����(CMD_SPI_OPT_MISO)
 */
static void SPI_MasterStart(void *arg)
{
    static uint8_t  _NextOpt =  0;
    void            *buff;
    uint16_t         len;
    uint8_t          opt     = ((unsigned)arg); //lint !e507 !e734
    CRITICAL_SECTION_ALLOC();

    CRITICAL_SECTION_ENTER();
    if (_spi_Stat != CMD_SPI_STAT_FREE) {
        CRITICAL_SECTION_EXIT();
        return;                             // SPI �ǿ���״̬
    } else {
        _spi_Stat = CMD_SPI_STAT_START;     // ���� --> ��������
    }
    CRITICAL_SECTION_EXIT();

    if (opt == CMD_SPI_OPT_NONE) {
        opt =  _NextOpt;                    // �Զ��л�(��������ռ�)
    }

    switch(opt)
    {
    default :               // ------------ ��Ч״̬ --------------------
    case CMD_SPI_OPT_MOSI : // ------------ �������� --------------------
        if((len = QUEUE_O_BUF(_tx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MOSI;   // ���Ͷ��зǿ�
            _NextOpt  = CMD_SPI_OPT_MISO;
            _spi_Stat = CMD_SPI_NEGO_MO_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(3, buff, len); //lint !e534 !e737 !e835
          #endif
        } else
        if((len = QUEUE_I_BUF(_rx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MISO;   // ���ն��з���
            _NextOpt  = CMD_SPI_OPT_MOSI;
            _spi_Stat = CMD_SPI_NEGO_MI_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(4, "__RECV_START__", len); //lint !e534 !e737 !e835
          #endif
        } else {
            _spi_Stat = CMD_SPI_STAT_FREE;  // ���Ͷ��п� �� ���ն�����
            return;
        }
        break;

    case CMD_SPI_OPT_MISO : // ------------ �������� --------------------
        if((len = QUEUE_I_BUF(_rx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MISO;   // ���ն��з���
            _NextOpt  = CMD_SPI_OPT_MOSI;
            _spi_Stat = CMD_SPI_NEGO_MI_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(4, "__RECV_START__", len); //lint !e534 !e737 !e835
          #endif
        } else
        if((len = QUEUE_O_BUF(_tx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MOSI;   // ���Ͷ��зǿ�
            _NextOpt  = CMD_SPI_OPT_MISO;
            _spi_Stat = CMD_SPI_NEGO_MO_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(3, buff, len);         //lint !e534 !e737 !e835
          #endif
        } else {
            _spi_Stat = CMD_SPI_STAT_FREE;  // ���Ͷ��п� �� ���ն�����
            return;
        }
        break;
    }
    UNUSED_VAR(buff);

    SPIy_NSS_SET();                         // NSS �ź���Ϊ��Ч
    _neg_Buf[0] = (HIBYTE(len) & CMD_SPI_LEN_MASK) | opt;
    _neg_Buf[1] =  LOBYTE(len);             // ��ʼ����Э��
    SPIy_SEND_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534
}
#else
/**
 * Slave ͨ������
 */
__STATIC_FORCEINLINE void SPI_SlaveStart(void)
{
    _spi_Stat = CMD_SPI_NEGO_RX;            // ��ʼ����Э��
    SPIy_NSS_SET();         // note         // NSS �ź���Ϊ��Ч
    _neg_Buf[0] = CMD_SPI_OPT_NONE,  _neg_Buf[1] = 0;
    SPIy_RECV_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534
}
#endif  // (SPIy_MODE_MASTER != 0)


/**
 * Callback function of Signal SPI Events(CMSIS-Driver).
 */
static void SPI_SignalEvent(uint32_t event)
{
    if (event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
        SPIy_IntrComplete();
    }
    if (event & ARM_SPI_EVENT_DATA_LOST) {
        LOG_WARN("SPI" TO_STRING(SPIy_DEVICE_NUM) " data lost: %u", (unsigned)_spi_Stat);
    }
    if (event & ARM_SPI_EVENT_MODE_FAULT) {
        LOG_WARN("SPI" TO_STRING(SPIy_DEVICE_NUM) " Master mode NSS fault");
    }
}


__STATIC_FORCEINLINE int SPI_Init(void)
{
    if (SPIy_INIT() != 0) { // SPI ��ʼ��
        return( -1 );
    }
    SPIy_NSS_INIT();        // SPI NSS �źų�ʼ��
    SPIy_NSS_CLR();         // NSS �ź���Ϊ��Ч

    /*------------------------------------------------------------------------*/
    if((_spi_Evt = osEventFlagsNew(NULL)) == NULL) {
        LOG_ERROR("osEventFlagsNew(NULL)");
        return( -1 );
    }
    osEventFlagsSet(_spi_Evt, CMD_SPI_EVT_SEND); //lint !e534
    QUEUE_INIT(_tx_Queue);
    QUEUE_INIT(_rx_Queue);
    _spi_Stat = CMD_SPI_STAT_FREE;

#if (SPIy_MODE_MASTER != 0)
#if (SPIy_POLL_TICKS > 0)
    {   osTimerId_t     tmrID;

        if((tmrID = osTimerNew(SPI_MasterStart, osTimerPeriodic, ((void *)CMD_SPI_OPT_NONE), NULL)) == NULL) {
            LOG_ERROR("osTimerNew(SPI_MasterStart, osTimerPeriodic, ((void*)0), NULL)");
            return( -1 );   // ����һ����ʱ��
        }
        if (osTimerStart(tmrID, SPIy_POLL_TICKS) != osOK) {
            LOG_ERROR("osTimerStart(tmrID, SPIy_POLL_TICKS)");
            return( -1 );   // Master ͨ������ (������)
        }
    }
#endif
#else
    SPI_SlaveStart();       // Slave ͨ������
#endif  // (SPIy_MODE_MASTER != 0)

    return( 0 );
}


/**
 * sending data to the SPI
 *
 * @param[in]  data     Pointer to buffer for send
 * @param[in]  size     Number of bytes for send
 *
 * @return     number of bytes send is returned
 */
__STATIC_FORCEINLINE int SPI_Send(const void *data, int size)
{
    void   *buff;
    int     len, sum;

#ifdef RTE_Compiler_EventRecorder
    EventStartDx(6, data, size); //lint !e534 !e737 !e835
#endif

    for(sum = 0;  ;)
    {
        if((len = QUEUE_I_BUF(_tx_Queue, &buff)) <= 0)  // ��ȡ ���Ͷ��� ��ӻ���
        {
            if(sum != 0) { break; }                     // �ȴ������¼���־
            osEventFlagsWait(_spi_Evt, CMD_SPI_EVT_SEND, osFlagsWaitAll, osWaitForever); //lint !e534
            continue;
        }
        if((len = MIN(len, size)) <= 0) {
            break;                                      // ���Ͷ����� or ����ȫ�����
        }
        memcpy(buff, data, len);     //lint !e732
        QUEUE_I_ADJ(_tx_Queue, len); //lint !e734       // ���� ���Ͷ����� Ԫ�ظ���

        data  = len + ((uint8_t *)data);
        size -= len;
        sum  += len;
    }
#if (SPIy_MODE_MASTER != 0)
    SPI_MasterStart((void *)CMD_SPI_OPT_MOSI);          // Master ��������
#endif
    LOG_VERB("SPIy Send: %d", sum);
    return( sum );
}


/**
 * Receive data from SPI
 *
 * @param[out] data     Pointer to buffer for receive
 * @param[in]  size     Number of bytes for receive
 *
 * @return     number of bytes received is returned
 */
__STATIC_FORCEINLINE int SPI_Recv(void *data, int size)
{
    void   *buff;
    int     len, sum;

#if (SPIy_MODE_MASTER != 0)
    SPI_MasterStart((void *)CMD_SPI_OPT_MISO);          // Master ��������
#endif

    for(sum = 0;  ;)
    {
        if((len = QUEUE_O_BUF(_rx_Queue, &buff)) <= 0)  // ��ȡ ���ն��� ���ӻ���
        {
            if(sum != 0) { break; }                     // �ȴ������¼���־
            osEventFlagsWait(_spi_Evt, CMD_SPI_EVT_RECV, osFlagsWaitAll, osWaitForever); //lint !e534
            continue;
        }
        if((len = MIN(len, size)) <= 0) {
            break;                                      // ���ն��п� or data����
        }
        memcpy(data, buff, len);     //lint !e732
        QUEUE_O_ADJ(_rx_Queue, len); //lint !e734       // ���� ���ն����� Ԫ�ظ���

        data  = len + ((uint8_t *)data);
        size -= len;
        sum  += len;
    }
#ifdef RTE_Compiler_EventRecorder
    EventStopDx(5, data, size);  //lint !e534 !e737 !e835
#endif
    LOG_VERB("SPIy Recv: %d", sum);
    return( sum );
}


/*----------------------------------------------------------------------------*/
/**
 * CLI open the device
 *
 * @param[in]  device   Pointer to device.
 * @param[in]  cli      Command Line Interface.
 *
 * @retval  (>= 0) Success
 * @retval  ( < 0) Failed
 */
static int CLI_DeviceOpen(cli_device_t* device, int cli)
{
    UNUSED_VAR(device);

    if (SPI_Init() < 0) {
        LOG_ERROR("CLI \"" CMD_CLI_NAME "\" fault on Init the SPI" TO_STRING(SPIy_DEVICE_NUM));
        return( -1 );
    }

    if (SPIy_FORWARD_IDX >= 0) {      //lint !e506 !e774
        LOG_INFO("CLI \"" CMD_CLI_NAME "\" Opened on the SPI" TO_STRING(SPIy_DEVICE_NUM)
                 ", Forward Dst on Idx: %d", (int)SPIy_FORWARD_IDX
                );
        Cmd_ForwardSetIF((unsigned)SPIy_FORWARD_IDX, cli);          //lint !e534
    }
    else if (SPIy_FORWARD_SRC >= 0) { //lint !e506 !e774
        LOG_INFO("CLI \"" CMD_CLI_NAME "\" Opened on the SPI" TO_STRING(SPIy_DEVICE_NUM)
                 ", Default Src on Idx: %d", (int)SPIy_FORWARD_SRC
                );
        Cmd_ForwardToIdx((unsigned)SPIy_FORWARD_SRC, cli, 0, NULL); //lint !e534
    }
    else {
        LOG_INFO("CLI \"" CMD_CLI_NAME "\" Opened on the SPI" TO_STRING(SPIy_DEVICE_NUM));
    }

    return( 0 );
}


/**
 * CLI close the device
 *
 * @param[in]  device   Pointer to device.
 * @param[in]  cli      Command Line Interface.
 *
 * @retval  (>= 0) Success
 * @retval  ( < 0) Failed
 */
static int CLI_DeviceClose(cli_device_t* device, int cli)
{
    UNUSED_VAR(device);

    Cmd_ForwardRemIF(cli);    //lint !e534  " Ignoring return value "
    LOG_ERROR("CLI \"" CMD_CLI_NAME "\" Closed on the SPI" TO_STRING(SPIy_DEVICE_NUM));
    return( 0 );
}


/**
 * CLI read data from device
 *
 * @param[in]  device   Pointer to device.
 * @param[out] buff     Pointer to buffer for read
 * @param[in]  size     Number of bytes for read
 *
 * @retval  (>= 0) Success, number of bytes read is returned
 * @retval  ( < 0) Failed, exception of device operate
 */
static int CLI_DeviceRead(cli_device_t* device, void* buff, int size)
{
    int     len;
    UNUSED_VAR(device);

    if((buff == NULL) || (size <= 0)) {
        LOG_ERROR("CLI \"" CMD_CLI_NAME "\" Read fault on the SPI" TO_STRING(SPIy_DEVICE_NUM)
                  "; buff: %p, size: %d", buff, size
                 );
        return( -1 );
    }
    while( (len = SPI_Recv(buff, size)) == 0 ) ;
    return( len );
}


/**
 * CLI write data to device
 *
 * @param[in]  device   Pointer to device.
 * @param[in]  data     Pointer to buffer for write
 * @param[in]  size     Number of bytes for write
 *
 * @retval  (>= 0) Success, number of bytes written is returned
 * @retval  ( < 0) Failed, exception of device operate
 */
static int CLI_DeviceWrite(cli_device_t* device, const void* data, int size)
{
    UNUSED_VAR(device);

    if((data == NULL) || (size <= 0)) {
        LOG_ERROR("CLI \"" CMD_CLI_NAME "\" Write fault on the SPI" TO_STRING(SPIy_DEVICE_NUM)
                  "; data: %p, size: %d", data, size
                 );
        return( -1 );
    }
    return( SPI_Send(data, size) );
}


int Cmd_SPIyInit(void)
{
    static const cli_device_t  _cli_device = {
        CLI_DeviceOpen, CLI_DeviceClose, CLI_DeviceRead, CLI_DeviceWrite
    };
    int  flags = CMD_CLI_FLAGS(SPIy_CMD_STR_EN, SPIy_CMD_BIN_EN);
    assert((SPIy_CMD_STR_EN + SPIy_CMD_BIN_EN) > 0);

    if (cli_shell_new(&_cli_device, CMD_CLI_NAME, flags | CLI_NO_SEARCH) < 0) {
        LOG_ERROR("ERROR: cli_shell_new(&_cli_device, CMD_CLI_NAME, flags)");
        return( -1 );
    }
    SPIy_ACT_LED();
    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#endif
/*****************************  END OF FILE  **********************************/

