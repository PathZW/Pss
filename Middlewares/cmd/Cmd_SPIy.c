/*******************************************************************************
 * @file    Cmd_SPIy.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.4
 * @date    2021/12/28
 * @brief   SPI 命令行接口, 支持 SPI Master 和 SPI Slave 模式. 传输协议为先协商传输
 *          数据长度, 再传输最多 4095 字节数据, 详细协议参考《Cmd_SPI 传输协议》文档. \n
 *          SPI 驱动: 若不使用 CMSIS-Driver, 可定义 SPIy_INIT() SPIy_SEND_START()
 *                    SPIy_RECV_START(), 并在 SPI 完成中断中调用 SPIy_IntrComplete() \n
 *          NSS 信号: 支持 硬件NSS 及 自定义NSS, 自定义需实现3个 SPIy_NSS_ 开头函数. \n
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
 *      2021/8/11 --- Linghu -- 修复 SPI_MasterStart() 函数重入 BUG
 *
 * @par Change Logs:
 *      2021/1/20 --- Linghu -- 添加 SPI DMA 模式支持
 *
 * @par Change Logs:
 *      2021/1/11 --- Linghu -- 添加 NSS(软硬件) 信号支持
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
#define SPIy_BUS_SPEED           4000000        //!< (C) 总线速度(以 Hz 为单位)
#endif
#ifndef SPIy_BUFF_SIZE
#define SPIy_BUFF_SIZE             256          //!< (C) 驱动缓存(以字节为单位)
#endif

#ifndef SPIy_CMD_STR_EN
#define SPIy_CMD_STR_EN              1          //!< (R) 是(1)/否(0) 支持字符串命令
#endif
#ifndef SPIy_CMD_BIN_EN
#define SPIy_CMD_BIN_EN              0          //!< (R) 是(1)/否(0) 支持二进制命令
#endif
#ifndef SPIy_FORWARD_IDX
#define SPIy_FORWARD_IDX            -1          //!< (R) 转发索引, (< 0)非目的接口
#endif
#ifndef SPIy_FORWARD_SRC
#define SPIy_FORWARD_SRC            -1          //!< (R) 转发索引, (< 0)非默认源接口
#endif

/*----------------------------------------------------------------------------*/
#ifndef SPIy_POLL_TICKS
#define SPIy_POLL_TICKS              2          //!< (R) Master 轮询时间, (< 0)不轮询
#endif

#ifndef SPIy_MODE_MASTER
#define SPIy_MODE_MASTER             0          //!< (C) Master模式(1) 或 Slave模式(0)
#endif
#ifndef SPIy_MODE_SIMPLEX
#define SPIy_MODE_SIMPLEX            0          //!< (C) 半双工(1) 或 全双工(0)
#endif
#ifndef SPIy_MODE_NSS_SW
#define SPIy_MODE_NSS_SW             0          //!< (C) NSS信号; 1: CMSIS SW, 0: Master自定义 | Slave CMSIS HW
#endif
#ifndef SPIy_MODE_CFG
#define SPIy_MODE_CFG    (  ARM_SPI_MSB_LSB     /*!< (C) SPI 工作模式     */ \
                          | ARM_SPI_CPOL0_CPHA0                              \
                          | ARM_SPI_DATA_BITS(8)                             \
                         )
#endif

#ifndef SPIy_INIT
#define SPIy_INIT               SPIy_INIT       //!< (C) SPI 初始化 (函数名)
#endif
#ifndef SPIy_SEND_START
#define SPIy_SEND_START      SPIy_SEND_START    //!< (C) SPI 开始发送数据 (函数名)
#endif
#ifndef SPIy_RECV_START
#define SPIy_RECV_START      SPIy_RECV_START    //!< (C) SPI 开始接收数据 (函数名)
#endif

#ifndef SPIy_NSS_INIT
#define SPIy_NSS_INIT()                         //!< (C) SPI NSS 信号初始化 (函数名)
#endif
#ifndef SPIy_NSS_CLR
#define SPIy_NSS_CLR            SPIy_NSS_CLR    //!< (C) SPI NSS 信号置为无效 (函数名)
#endif
#ifndef SPIy_NSS_SET
#define SPIy_NSS_SET            SPIy_NSS_SET    //!< (C) SPI NSS 信号置为有效 (函数名)
#endif

#ifndef SPIy_ACT_LED
#define SPIy_ACT_LED()                          //!< (C) SPI 命令接口激活显示 (函数名)
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
#define CMD_SPI_EVT_SEND    ((uint32_t)0x01)        // 发送完成事件
#define CMD_SPI_EVT_RECV    ((uint32_t)0x02)        // 接收完成事件

#define CMD_SPI_LEN_MASK    ((uint8_t)0x0F)         // 长度 mask
#define CMD_SPI_OPT_MASK    ((uint8_t)0xF0)         // 操作 mask
#define CMD_SPI_OPT_NONE    ((uint8_t)0x00)
#define CMD_SPI_OPT_MOSI    ((uint8_t)0x10)         // 协商 MOSI 长度
#define CMD_SPI_OPT_MISO    ((uint8_t)0x20)         // 协商 MISO 长度

#define CMD_SPI_STAT_FREE           0               // 空闲
#define CMD_SPI_STAT_START          1               // 正在启动
#define CMD_SPI_NEGO_MO_TX          2               // Master 发送 MOSI协商
#define CMD_SPI_NEGO_MI_TX          3               // Master 发送 MISO协商
#define CMD_SPI_NEGO_MO_RX          4               // Master 接收 MOSI协商
#define CMD_SPI_NEGO_MI_RX          5               // Master 接收 MISO协商
#define CMD_SPI_DATA_MO_TX          6               // Master 发送 MOSI数据
#define CMD_SPI_DATA_MI_RX          7               // Master 接收 MISO数据
#define CMD_SPI_NEGO_RX             8               // Slave  接收 协商
#define CMD_SPI_NEGO_SO_TX          9               // Slave  发送 MISO协商
#define CMD_SPI_NEGO_SI_TX         10               // Slave  发送 MOSI协商
#define CMD_SPI_DATA_SO_TX         11               // Slave  发送 MISO数据
#define CMD_SPI_DATA_SI_RX         12               // Slave  接收 MOSI数据


/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

static QUEUE_TYPE(uint8_t, SPIy_BUFF_SIZE)  _tx_Queue;   // 发送队列
static QUEUE_TYPE(uint8_t, SPIy_BUFF_SIZE)  _rx_Queue;   // 接收队列

static osEventFlagsId_t                     _spi_Evt;    // Event(发送/接收)

static uint8_t                              _spi_Stat;   // 通信状态(参考 )
static uint8_t                              _neg_Buf[2]; // 协商缓存


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
#if SPIy_MODE_NSS_SW    // NSS信号为 CMSIS-Driver Software 模式
    if (ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE)");
    }
#endif
}


__WEAK void SPIy_NSS_SET(void)
{
#if SPIy_MODE_NSS_SW    // NSS信号为 CMSIS-Driver Software 模式
    if (ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE) != ARM_DRIVER_OK) {
        LOG_ERROR("ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE)");
    }
#endif
}


void SPIy_IntrMISO(void)
{
#if (SPIy_MODE_MASTER != 0)
    SPI_MasterStart((void *)CMD_SPI_OPT_MISO);      // Master 接收启动
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
        SPIy_NSS_CLR();                                                 // NSS 信号置为无效
    case CMD_SPI_STAT_FREE:  //lint !e825
    case CMD_SPI_STAT_START:
        break;

    case CMD_SPI_NEGO_MO_TX :   // -------- 协商长度(Master 发送) ------------
        _spi_Stat = CMD_SPI_NEGO_MO_RX;
        _neg_Buf[0] = CMD_SPI_OPT_NONE,  _neg_Buf[1] = 0;
        SPIy_RECV_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534   // 开始接收协商
        break;

    case CMD_SPI_NEGO_MI_TX :   // -------- 协商长度(Master 发送) ------------
        _spi_Stat = CMD_SPI_NEGO_MI_RX;
        _neg_Buf[0] = CMD_SPI_OPT_NONE,  _neg_Buf[1] = 0;
        SPIy_RECV_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534   // 开始接收协商
        break;

    case CMD_SPI_NEGO_MO_RX :   // -------- 协商长度(Master 接收) ------------
        if((_neg_Buf[0] & CMD_SPI_OPT_MASK) != CMD_SPI_OPT_MOSI) {
            goto SPI_STAT_NONE;     //lint !e801                        // 无效的操作(slave发送)
        }
        _size = MAKEWORD(_neg_Buf[1], _neg_Buf[0] & CMD_SPI_LEN_MASK);  // 接收到的长度(slave发送)
        len   = QUEUE_O_BUF(_tx_Queue, &_buff);                         // 获取队列缓存(长度, 指针)
        if((_size = MIN(len, _size)) <= 0) { //lint !e775
            goto SPI_STAT_NONE;              //lint !e801               // 最终协商发送长度为0
        }
        _spi_Stat = CMD_SPI_DATA_MO_TX;
        SPIy_SEND_START(_buff, _size); //lint !e534                     // 开始发送数据
        break;

    case CMD_SPI_NEGO_MI_RX :   // -------- 协商长度(Master 接收) ------------
        if((_neg_Buf[0] & CMD_SPI_OPT_MASK) != CMD_SPI_OPT_MISO)  {
            goto SPI_STAT_NONE;        //lint !e801                     // 无效的操作(slave发送)
        }
        _size = MAKEWORD(_neg_Buf[1], _neg_Buf[0] & CMD_SPI_LEN_MASK);  // 接收到的长度(slave发送)
        len   = QUEUE_I_BUF(_rx_Queue, &_buff);                         // 获取队列缓存(长度, 指针)
        if((_size = MIN(len, _size)) <= 0) { //lint !e775
            goto SPI_STAT_NONE;              //lint !e801               // 最终协商接收长度为0
        }
        _spi_Stat = CMD_SPI_DATA_MI_RX;
        SPIy_RECV_START(_buff, _size); //lint !e534                     // 开始接收数据
        break;

    case CMD_SPI_DATA_MO_TX :   // -------- 传输数据(Master 发送) ------------
        QUEUE_O_ADJ(_tx_Queue, _size);                                  // 减少队列中元素个数
        _spi_Stat = CMD_SPI_STAT_FREE;
        SPIy_NSS_CLR();                                                 // NSS 信号置为无效
        if((osEventFlagsGet(_spi_Evt) & CMD_SPI_EVT_SEND) != CMD_SPI_EVT_SEND) {
            osEventFlagsSet(_spi_Evt  , CMD_SPI_EVT_SEND); //lint !e534
        }
      #ifdef RTE_Compiler_EventRecorder
        EventStopDx(3, "__SEND_STOP__", _size);      //lint !e534 !e737 !e835
      #endif
        SPI_MasterStart((void *)CMD_SPI_OPT_MOSI);                      // Master 发送启动
        break;

    case CMD_SPI_DATA_MI_RX :   // -------- 传输数据(Master 接收) ------------
        QUEUE_I_ADJ(_rx_Queue, _size);                                  // 增加队列中元素个数
        _spi_Stat = CMD_SPI_STAT_FREE;
        SPIy_NSS_CLR();                                                 // NSS 信号置为无效
        if((osEventFlagsGet(_spi_Evt) & CMD_SPI_EVT_RECV) != CMD_SPI_EVT_RECV) {
            osEventFlagsSet(_spi_Evt  , CMD_SPI_EVT_RECV); //lint !e534
        }
      #ifdef RTE_Compiler_EventRecorder
        EventStopDx(4, "__RECV_STOP__", _size);      //lint !e534 !e737 !e835
      #endif
        SPI_MasterStart((void *)CMD_SPI_OPT_MISO);                      // Master 接收启动
        break;

#else
    default :
        SPI_NEGO_SI :
     // SPIy_NSS_CLR();     // note                                     // NSS 信号置为无效
        SPI_SlaveStart();                                               // 开始接收协商
        break;

    case CMD_SPI_NEGO_RX :      // -------- 协商长度(Slave 接收) ------------
        _size = MAKEWORD(_neg_Buf[1], _neg_Buf[0] & CMD_SPI_LEN_MASK);  // 接收到的长度(Master发送)
        switch(_neg_Buf[0] & CMD_SPI_OPT_MASK) {
        default               :  goto SPI_NEGO_SI;      //lint !e801    // 无效的操作
        case CMD_SPI_OPT_MOSI :  len = QUEUE_I_BUF(_rx_Queue, &_buff); _spi_Stat = CMD_SPI_NEGO_SI_TX; break;
        case CMD_SPI_OPT_MISO :  len = QUEUE_O_BUF(_tx_Queue, &_buff); _spi_Stat = CMD_SPI_NEGO_SO_TX; break;
        }                            // ↑↑ 获取队列缓存(长度, 指针) ↑↑
        _neg_Buf[0] = (HIBYTE(len) & CMD_SPI_LEN_MASK) | (_neg_Buf[0] & CMD_SPI_OPT_MASK);
        _neg_Buf[1] =  LOBYTE(len);
        SPIy_SEND_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534   // 开始发送协商
        _size = MIN(len, _size);                                        // 最终协商传输长度
        break;

    case CMD_SPI_NEGO_SO_TX :   // -------- 协商长度(Slave 发送) ------------
        if(_size <= 0)  goto SPI_NEGO_SI; //lint !e775 !e801            // 最终协商发送长度为0
        _spi_Stat = CMD_SPI_DATA_SO_TX;
        SPIy_SEND_START(_buff, _size); //lint !e534                     // 开始发送数据
        break;

    case CMD_SPI_NEGO_SI_TX :   // -------- 协商长度(Slave 发送) ------------
        if(_size <= 0)  goto SPI_NEGO_SI; //lint !e775 !e801            // 最终协商接收长度为0
        _spi_Stat = CMD_SPI_DATA_SI_RX;
        SPIy_RECV_START(_buff, _size); //lint !e534                     // 开始接收数据
        break;

    case CMD_SPI_DATA_SO_TX :   // -------- 传输数据(Slave 发送) ------------
        QUEUE_O_ADJ(_tx_Queue, _size);                                  // 减少队列中元素个数
     // SPIy_NSS_CLR();     // note                                     // NSS 信号置为无效
        if((osEventFlagsGet(_spi_Evt) & CMD_SPI_EVT_SEND) != CMD_SPI_EVT_SEND) {
            osEventFlagsSet(_spi_Evt  , CMD_SPI_EVT_SEND); //lint !e534
        }
        goto SPI_NEGO_SI;                            //lint !e801

    case CMD_SPI_DATA_SI_RX :   // -------- 传输数据(Slave 接收) ------------
        QUEUE_I_ADJ(_rx_Queue, _size);                                  // 增加队列中元素个数
     // SPIy_NSS_CLR();     // note                                     // NSS 信号置为无效
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
 * Master 通信启动
 *
 * @param[in]  arg   自动(CMD_SPI_OPT_NONE), 发送(CMD_SPI_OPT_MOSI), 接收(CMD_SPI_OPT_MISO)
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
        return;                             // SPI 非空闲状态
    } else {
        _spi_Stat = CMD_SPI_STAT_START;     // 空闲 --> 正在启动
    }
    CRITICAL_SECTION_EXIT();

    if (opt == CMD_SPI_OPT_NONE) {
        opt =  _NextOpt;                    // 自动切换(发送与接收间)
    }

    switch(opt)
    {
    default :               // ------------ 无效状态 --------------------
    case CMD_SPI_OPT_MOSI : // ------------ 发送优先 --------------------
        if((len = QUEUE_O_BUF(_tx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MOSI;   // 发送队列非空
            _NextOpt  = CMD_SPI_OPT_MISO;
            _spi_Stat = CMD_SPI_NEGO_MO_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(3, buff, len); //lint !e534 !e737 !e835
          #endif
        } else
        if((len = QUEUE_I_BUF(_rx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MISO;   // 接收队列非满
            _NextOpt  = CMD_SPI_OPT_MOSI;
            _spi_Stat = CMD_SPI_NEGO_MI_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(4, "__RECV_START__", len); //lint !e534 !e737 !e835
          #endif
        } else {
            _spi_Stat = CMD_SPI_STAT_FREE;  // 发送队列空 且 接收队列满
            return;
        }
        break;

    case CMD_SPI_OPT_MISO : // ------------ 接收优先 --------------------
        if((len = QUEUE_I_BUF(_rx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MISO;   // 接收队列非满
            _NextOpt  = CMD_SPI_OPT_MOSI;
            _spi_Stat = CMD_SPI_NEGO_MI_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(4, "__RECV_START__", len); //lint !e534 !e737 !e835
          #endif
        } else
        if((len = QUEUE_O_BUF(_tx_Queue, &buff)) > 0) {
            opt       = CMD_SPI_OPT_MOSI;   // 发送队列非空
            _NextOpt  = CMD_SPI_OPT_MISO;
            _spi_Stat = CMD_SPI_NEGO_MO_TX;
          #ifdef RTE_Compiler_EventRecorder
            EventStartDx(3, buff, len);         //lint !e534 !e737 !e835
          #endif
        } else {
            _spi_Stat = CMD_SPI_STAT_FREE;  // 发送队列空 且 接收队列满
            return;
        }
        break;
    }
    UNUSED_VAR(buff);

    SPIy_NSS_SET();                         // NSS 信号置为有效
    _neg_Buf[0] = (HIBYTE(len) & CMD_SPI_LEN_MASK) | opt;
    _neg_Buf[1] =  LOBYTE(len);             // 开始发送协商
    SPIy_SEND_START(_neg_Buf, ARRAY_SIZE(_neg_Buf)); //lint !e534
}
#else
/**
 * Slave 通信启动
 */
__STATIC_FORCEINLINE void SPI_SlaveStart(void)
{
    _spi_Stat = CMD_SPI_NEGO_RX;            // 开始接收协商
    SPIy_NSS_SET();         // note         // NSS 信号置为有效
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
    if (SPIy_INIT() != 0) { // SPI 初始化
        return( -1 );
    }
    SPIy_NSS_INIT();        // SPI NSS 信号初始化
    SPIy_NSS_CLR();         // NSS 信号置为无效

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
            return( -1 );   // 创建一个软定时器
        }
        if (osTimerStart(tmrID, SPIy_POLL_TICKS) != osOK) {
            LOG_ERROR("osTimerStart(tmrID, SPIy_POLL_TICKS)");
            return( -1 );   // Master 通信启动 (周期性)
        }
    }
#endif
#else
    SPI_SlaveStart();       // Slave 通信启动
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
        if((len = QUEUE_I_BUF(_tx_Queue, &buff)) <= 0)  // 获取 发送队列 入队缓存
        {
            if(sum != 0) { break; }                     // 等待发送事件标志
            osEventFlagsWait(_spi_Evt, CMD_SPI_EVT_SEND, osFlagsWaitAll, osWaitForever); //lint !e534
            continue;
        }
        if((len = MIN(len, size)) <= 0) {
            break;                                      // 发送队列满 or 数据全部入队
        }
        memcpy(buff, data, len);     //lint !e732
        QUEUE_I_ADJ(_tx_Queue, len); //lint !e734       // 增加 发送队列中 元素个数

        data  = len + ((uint8_t *)data);
        size -= len;
        sum  += len;
    }
#if (SPIy_MODE_MASTER != 0)
    SPI_MasterStart((void *)CMD_SPI_OPT_MOSI);          // Master 发送启动
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
    SPI_MasterStart((void *)CMD_SPI_OPT_MISO);          // Master 接收启动
#endif

    for(sum = 0;  ;)
    {
        if((len = QUEUE_O_BUF(_rx_Queue, &buff)) <= 0)  // 获取 接收队列 出队缓存
        {
            if(sum != 0) { break; }                     // 等待接收事件标志
            osEventFlagsWait(_spi_Evt, CMD_SPI_EVT_RECV, osFlagsWaitAll, osWaitForever); //lint !e534
            continue;
        }
        if((len = MIN(len, size)) <= 0) {
            break;                                      // 接收队列空 or data已满
        }
        memcpy(data, buff, len);     //lint !e732
        QUEUE_O_ADJ(_rx_Queue, len); //lint !e734       // 减少 接收队列中 元素个数

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

