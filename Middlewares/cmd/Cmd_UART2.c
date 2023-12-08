/*******************************************************************************
 * @file    Cmd_UART2.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/6/29
 * @brief   Command line interface on UART2.
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
 *      2019/6/29 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro(835, ARM_USART_EVENT_SEND_COMPLETE)
//lint -emacro(835, ARM_USART_CONTROL_RX)
//lint -emacro(835, ARM_USART_CONTROL_TX)
//lint -emacro(845, ARM_DRIVER_OK)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Cmd_UART2"
#include "emb_log.h"

#include <stdint.h>
#include "assert.h"//lint !e451 // Header file repeatedly included
#include "cmsis_os2.h"
#include "Driver_USART.h"
#include "defines.h"

#include "shell/cli_shell.h"
#include "emb_queue.h"
#include "Cmd_App.h"
#include "Cmd_UART2.h"


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          Cmd_UART2
 * @{
 ******************************************************************************/

#ifndef UART2_DEVICE_NUM
#define UART2_DEVICE_NUM          -1        //!< (C) USART number, ( < 0) Disable
#endif
#ifndef UART2_BAUDRATE
#define UART2_BAUDRATE           115200     //!< (R) USART baudrate
#endif
#ifndef UART2_BUFF_SIZE
#define UART2_BUFF_SIZE           128       //!< (C) USART buffer size
#endif

#ifndef UART2_FLOW_CONTROL
#define UART2_FLOW_CONTROL        -1        //!< (R) USART Flow Control: (-1)None, (0)Custom, (1)Custom/CTS, (2)RTS, (3)RTS/CTS
#endif
#ifndef UART2_FLOW_CTRL_INIT
#define UART2_FLOW_CTRL_INIT()  ((void)0)   //!< (C) USART Custom Flow Control Initialize, default Soft-Mode RTS
#endif
#ifndef UART2_FLOW_RTS_SET
#define UART2_FLOW_RTS_SET()  USARTx_SetRTS //!< (C) USART Custom Flow Control Activate RTS(low), default Soft-Mode RTS
#endif
#ifndef UART2_FLOW_RTS_CLR
#define UART2_FLOW_RTS_CLR()  USARTx_ClrRTS //!< (C) USART Custom Flow Control Deactivate RTS(high), default Soft-Mode RTS
#endif

#ifndef UART2_CMD_STR_EN
#define UART2_CMD_STR_EN           1        //!< (R) 是(1)/否(0) 支持字符串命令
#endif
#ifndef UART2_CMD_BIN_EN
#define UART2_CMD_BIN_EN           0        //!< (R) 是(1)/否(0) 支持二进制命令
#endif
#ifndef UART2_FORWARD_IDX
#define UART2_FORWARD_IDX         -1        //!< (R) 字符串命令转发: (< 0)非转发接口, (>= 0)转发接口索引
#endif
#ifndef UART2_FORWARD_SRC
#define UART2_FORWARD_SRC         -1        //!< (R) 命令转发源接口: (< 0)非源接口, (>= 0)源接口索引
#endif


/*******************************************************************************
 * @}
 * @}
 */
#if (UART2_DEVICE_NUM >= 0)
/**
 * @addtogroup Cmd_UART2
 * @{
 * @addtogroup              Pravate_Macros
 * @{
 ******************************************************************************/

#define USARTx_CONTROLLER                           UART2_DEVICE_NUM
#define USARTx_BAUDRATE         /*lint -save -w1 */ UART2_BAUDRATE         /*lint -restore */
#define USARTx_BUF_SIZE         /*lint -save -w1 */ UART2_BUFF_SIZE        /*lint -restore */

#define USARTx_FLOW_CONTROL     /*lint -save -w1 */ UART2_FLOW_CONTROL     /*lint -restore */
#define USARTx_FLOW_CTRL_INIT() /*lint -save -w1 */ UART2_FLOW_CTRL_INIT() /*lint -restore */
#define USARTx_FLOW_RTS_SET()   /*lint -save -w1 */ UART2_FLOW_RTS_SET()   /*lint -restore */
#define USARTx_FLOW_RTS_CLR()   /*lint -save -w1 */ UART2_FLOW_RTS_CLR()   /*lint -restore */

#define CMD_CLI_STR_EN          /*lint -save -w1 */ UART2_CMD_STR_EN       /*lint -restore */
#define CMD_CLI_BIN_EN          /*lint -save -w1 */ UART2_CMD_BIN_EN       /*lint -restore */
#define CMD_FORWARD_IDX         /*lint -save -w1 */ UART2_FORWARD_IDX      /*lint -restore */
#define CMD_FORWARD_SRC         /*lint -save -w1 */ UART2_FORWARD_SRC      /*lint -restore */

#define CMD_CLI_FLAGS(bSTR, bBIN)                  /*lint -save -w1 */            \
                                                   (  ((bSTR) ? CLI_EXEC_STR : 0) \
                                                    | ((bBIN) ? CLI_EXEC_BIN : 0) \
                                                   )                              \
                                                   /*lint -restore */

/*----------------------------------------------------------------------------*/
extern ARM_DRIVER_USART     ATPASTE2(Driver_USART, USARTx_CONTROLLER);
#define ptrUSART          (&ATPASTE2(Driver_USART, USARTx_CONTROLLER))

#define USARTx_MODE        /*lint -save -w1 */             \
                           (  ARM_USART_MODE_ASYNCHRONOUS  \
                            | ARM_USART_DATA_BITS_8        \
                            | ARM_USART_PARITY_NONE        \
                            | ARM_USART_STOP_BITS_1        \
                            | ARM_USART_FLOW_CONTROL_NONE  \
                           )                               \
                           /*lint -restore */

#define USARTx_FLOW        /*lint -save -w1 */                                            \
                           (  (USARTx_FLOW_CONTROL == 1) ? ARM_USART_FLOW_CONTROL_CTS     \
                            : (USARTx_FLOW_CONTROL == 2) ? ARM_USART_FLOW_CONTROL_RTS     \
                            : (USARTx_FLOW_CONTROL == 3) ? ARM_USART_FLOW_CONTROL_RTS_CTS \
                            : 0                                                           \
                           )                                                              \
                           /*lint -restore */

#define USARTx_SetRTS       ptrUSART->SetModemControl(ARM_USART_RTS_SET)
#define USARTx_ClrRTS       ptrUSART->SetModemControl(ARM_USART_RTS_CLEAR)

/*----------------------------------------------------------------------------*/
#ifndef OS_SEMAPHORE_TAKE
#define OS_SEMAPHORE_TAKE(sem)  /*lint -save -w1 */                                     \
                                if (osSemaphoreAcquire((sem), osWaitForever) != osOK) { \
                                    assert_msg(0, "ERROR: osSemaphoreAcquire()");       \
                                }   ((void)0)                                           \
                                /*lint -restore */
#endif
#ifndef OS_SEMAPHORE_POST
#define OS_SEMAPHORE_POST(sem)  /*lint -save -w1 */                                     \
                                switch(osSemaphoreRelease(sem)) {                       \
                                default:  assert_msg(0, "ERROR: osSemaphoreRelease()"); \
                                case osOK:                                              \
                                case osErrorResource:  break;                           \
                                }   ((void)0)                                           \
                                /*lint -restore */
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Variables
 * @{
 ******************************************************************************/

static QUEUE_TYPE(uint8_t, USARTx_BUF_SIZE) _tx_Queue;  // Queue for Send
static QUEUE_TYPE(uint8_t, USARTx_BUF_SIZE) _rx_Queue;  // Queue for Recv

static       osSemaphoreId_t    _tx_Sem;                // Semaphore for Send
static       osSemaphoreId_t    _rx_Sem;                // Semaphore for Recv
static const osSemaphoreAttr_t  _tx_SemAttr = { "USART" TO_STRING(USARTx_CONTROLLER) " tx_Sem", 0u, NULL, 0u };
static const osSemaphoreAttr_t  _rx_SemAttr = { "USART" TO_STRING(USARTx_CONTROLLER) " rx_Sem", 0u, NULL, 0u };


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Functions
 * @{
 ******************************************************************************/
/**
 * Callback function of Signal USART Events.
 *
 * @param[in]  event   notification mask
 */
static void USART_SignalEvent(uint32_t event)
{
    void   *ptr;

    // The data has been transferred
    if (event &  ARM_USART_EVENT_TX_COMPLETE)
    {
        if (((unsigned)USARTx_FLOW_CONTROL & ~1) == 0) //lint !e506 !e774 !e778 !e835
        {
            if (QUEUE_ITEM(_tx_Queue) == 0) {   // Queue Empty
                USARTx_FLOW_RTS_SET();          // USART Activate RTS(low)
            }
        }
    }

    // Send completed
    if (event & ARM_USART_EVENT_SEND_COMPLETE)
    {
        OS_SEMAPHORE_POST(_tx_Sem);
        QUEUE_O_ADJ(_tx_Queue, 1);              // Dequeue

        if (QUEUE_ITEM(_tx_Queue) != 0)         // Queue Non-Empty
        {
            if (((unsigned)USARTx_FLOW_CONTROL & ~1) == 0) { //lint !e506 !e774 !e778 !e835
                USARTx_FLOW_RTS_CLR();          // USART Deactivate RTS(high)
            }
            QUEUE_O_BUF(_tx_Queue, &ptr);       // Start sending data
            switch (ptrUSART->Send(ptr, 1)) {
            case ARM_DRIVER_OK:          break; // Success started
            case ARM_DRIVER_ERROR_BUSY:  break; // Send is not completed
            default:  assert_msg(0, "ERROR: ptrUSART->Send()");  break;
            }
        }
    }

    // Receive completed
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE)
    {
        if (QUEUE_FREE((_rx_Queue)) != 0) {     // Queue Non-Full
            QUEUE_I_ADJ(_rx_Queue, 1);          // Enqueue
            OS_SEMAPHORE_POST(_rx_Sem);
        }

        QUEUE_I_BUF(_rx_Queue, &ptr);           // Start receiving data
        if (ptrUSART->Receive(ptr, 1) != ARM_DRIVER_OK) {
            assert_msg(0, "ERROR: ptrUSART->Receive()");
        }
    }
}


/**
 * USART send a character.
 *
 * @param[in]  ch   Integer value specifying the character to send.
 *
 * @retval  (!= -1) Success, the sent character is returned
 * @retval  (== -1) Failed
 */
static int USART_PutChar(int ch)
{
    void   *ptr;

    OS_SEMAPHORE_TAKE(_tx_Sem);             // Waiting for semaphore
    while (QUEUE_FREE(_tx_Queue) == 0) ;    // Waiting for queue Non-Full
    QUEUE_PUSH(_tx_Queue, (uint8_t)ch);     // Enqueue

    if (QUEUE_ITEM(_tx_Queue) == 1)
    {
        if (((unsigned)USARTx_FLOW_CONTROL & ~1) == 0) { //lint !e506 !e774 !e778 !e835
            USARTx_FLOW_RTS_CLR();          // USART Deactivate RTS(high)
        }
        QUEUE_O_BUF(_tx_Queue, &ptr);       // Start sending data
        switch (ptrUSART->Send(ptr, 1)) {
        case ARM_DRIVER_OK:          break; // Success started
        case ARM_DRIVER_ERROR_BUSY:  break; // Send is not completed
        default:  assert_msg(0, "ERROR: ptrUSART->Send()");  break;
        }
    }

    return( ch );
}


/**
 * USART receive a character.
 *
 * @retval  (!= -1) Success, the received character is returned
 * @retval  (== -1) Failed
 */
static int USART_GetChar(void)
{
    uint8_t  ch;

    OS_SEMAPHORE_TAKE(_rx_Sem);             // Waiting for semaphore
    while (QUEUE_ITEM(_rx_Queue) == 0) ;    // Waiting for queue Non-Empty
    QUEUE_POP(_rx_Queue, &ch);              // Dequeue

    return( ch );
}


/**
 * Initialize the USART.
 */
static void USART_Init(void)
{
    void   *ptr;

    if (ptrUSART->GetVersion().api < 0x200) {
        assert_msg(0, "ERROR: USART driver minimum API version 2.00 or higher");
    }
    if (ptrUSART->Initialize(USART_SignalEvent) != ARM_DRIVER_OK) {
        assert_msg(0, "ERROR: ptrUSART->Initialize()");
    }
    if (ptrUSART->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
        assert_msg(0, "ERROR: ptrUSART->PowerControl()");
    }
    if (ptrUSART->Control(USARTx_MODE | USARTx_FLOW, USARTx_BAUDRATE) != ARM_DRIVER_OK) {
        assert_msg(0, "ERROR: ptrUSART->Control()");
    }
    if (ptrUSART->Control(ARM_USART_CONTROL_RX, 1) != ARM_DRIVER_OK) {
        assert_msg(0, "ERROR: ptrUSART->Control(ARM_USART_CONTROL_RX, 1)");
    }
    if (ptrUSART->Control(ARM_USART_CONTROL_TX, 1) != ARM_DRIVER_OK) {
        assert_msg(0, "ERROR: ptrUSART->Control(ARM_USART_CONTROL_TX, 1)");
    }
    if (((unsigned)USARTx_FLOW_CONTROL & ~1) == 0) //lint !e506 !e774 !e778 !e835
    {
        if (ptrUSART->GetCapabilities().event_tx_complete == 0) {
            assert_msg(0, "ERROR: USART must support transmit completed event");
        }
        USARTx_FLOW_CTRL_INIT();    // USART Custom Flow Control Initialize
        USARTx_FLOW_RTS_SET();      // USART Activate RTS(low)
    }

    /*------------------------------------------------------------------------*/
    if ((_rx_Sem = osSemaphoreNew(QUEUE_SIZE(_rx_Queue), 0, &_rx_SemAttr)) == NULL) {
        assert_msg(0, "ERROR: osSemaphoreNew()");
    }
    if ((_tx_Sem = osSemaphoreNew(QUEUE_SIZE(_tx_Queue), QUEUE_SIZE(_tx_Queue), &_tx_SemAttr)) == NULL) {
        assert_msg(0, "ERROR: osSemaphoreNew()");
    }
    QUEUE_INIT(_rx_Queue);
    QUEUE_INIT(_tx_Queue);

    QUEUE_I_BUF(_rx_Queue, &ptr);   // Start receiving data
    if (ptrUSART->Receive(ptr, 1) != ARM_DRIVER_OK) {
        assert_msg(0, "ERROR: ptrUSART->Receive()");
    }
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
    UNUSED_VAR(device);  UNUSED_VAR(cli);

    USART_Init();

    if (CMD_FORWARD_IDX >= 0) { //lint !e506 !e774
        LOG_INFO("Command Line Interface open the UART" TO_STRING(USARTx_CONTROLLER)
                 ", Forward Dst on Idx: %d", (int)CMD_FORWARD_IDX
                );
        Cmd_ForwardSetIF((unsigned)CMD_FORWARD_IDX, cli);           //lint !e534
    }
    else if (CMD_FORWARD_SRC >= 0) { //lint !e506 !e774
        LOG_INFO("Command Line Interface open the UART" TO_STRING(USARTx_CONTROLLER)
                 ", Default Src on Idx: %d", (int)CMD_FORWARD_SRC
                );
        Cmd_ForwardToIdx((unsigned)CMD_FORWARD_SRC, cli, 0, NULL);  //lint !e534
    }
    else {
        LOG_INFO("Command Line Interface open the UART" TO_STRING(USARTx_CONTROLLER));
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
    assert_msg(0, "ERROR: CLI Device Closed");
    return( 0 );
}


/**
 * CLI read data from device
 *
 * @param[in]  device   Pointer to device.
 * @param[out] buff     Pointer to buffer for read
 * @param[in]  size     Number of bytes to read
 *
 * @retval  (>= 0) Success, number of bytes read is returned
 * @retval  ( < 0) Failed, exception of device operate
 */
static int CLI_DeviceRead(cli_device_t* device, void* buff, int size)
{
    UNUSED_VAR(device);  UNUSED_VAR(size);

    ((uint8_t *)buff)[0] = USART_GetChar();  //lint !e734  " Loss of precision "
    return( 1 );
}


/**
 * CLI write data to device
 *
 * @param[in]  device   Pointer to device.
 * @param[in]  data     Pointer to buffer to write
 * @param[in]  size     Number of bytes to write
 *
 * @retval  (>= 0) Success, number of bytes written is returned
 * @retval  ( < 0) Failed, exception of device operate
 */
static int CLI_DeviceWrite(cli_device_t* device, const void* data, int size)
{
    UNUSED_VAR(device);

    for(int i = 0;  i < size;  i++)
    {
        USART_PutChar(((uint8_t *)data)[i]);  //lint !e534  " Ignoring return value "
    }
    return( size );
}


/*----------------------------------------------------------------------------*/
/**
 * Initialize the UART2 application
 */
void Cmd_UART2Init(void)
{
    static const cli_device_t  _cli_device = {
        CLI_DeviceOpen, CLI_DeviceClose, CLI_DeviceRead, CLI_DeviceWrite
    };
    int  flags = CMD_CLI_FLAGS(CMD_CLI_STR_EN, CMD_CLI_BIN_EN);
    assert((CMD_CLI_STR_EN + CMD_CLI_BIN_EN) > 0);

    if (cli_shell_new(&_cli_device, "UART2", flags) < 0) {
        assert_msg(0, "ERROR: cli_shell_new(&_cli_device, \"UART2\", flags)");
    }
}

void UART2_Send(char* buf, int len)
{
    for(int i=0; i<len; i++)
    USART_PutChar(buf[i]);
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#endif
/*****************************  END OF FILE  **********************************/

