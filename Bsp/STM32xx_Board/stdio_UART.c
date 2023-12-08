/*******************************************************************************
 * @file    stdio_UART.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/8/16
 * @brief   Used for Retarget I/O, Port to STM32xxx and CMSIS-RTOS2.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/8/16 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro(506, assert*)    // Constant value Boolean
//lint -emacro(835, ARM_USART_*)
//lint -emacro(835, USART_MODE)
//lint -emacro(845, ARM_DRIVER_OK)
//lint -esym(522, os_tx_sem_init)
//lint -esym(522, os_rx_sem_init)
//lint -esym(522, os_tx_sem_post)
//lint -esym(522, os_rx_sem_post)
//lint -esym(522, os_tx_sem_take)
//lint -esym(522, os_rx_sem_take)

#ifdef   APP_CFG
#include APP_CFG                    // Apps configuration
#endif
#define  LOG_TAG  "stdio_UART"
#include "emb_log.h"

#ifdef   _RTE_                      // RTE_CMSIS_RTOS2
#include "RTE_Components.h"         // Component selection
#endif
#ifdef   RTE_CMSIS_RTOS2
#include "cmsis_os2.h"              // CMSIS:RTOS2
#endif
#include "assert.h" //lint !e451    // Header file repeatedly included
#include "stdio.h"
#include "Driver_USART.h"           // CMSIS USART driver
#include "defines.h"                // Used: ATPASTEx, __INLINE
#include "emb_queue.h"

#define  ONLY_USED_REGS
#include "board.h"                  // Device's defines


//lint -emacro(506, LOG_ASSERT)     // do ... while(1)
#undef  LOG_ASSERT
#define LOG_ASSERT(...)     do{}while(1)    //lint !e683
#undef  assert
#define assert(e)           do{}while(!(e))
#undef  assert_msg
#define assert_msg(e, msg)  do{}while(!(e))


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          STDIO_UART
 * @{
 ******************************************************************************/

#ifndef STDIO_UART_NUM
#define STDIO_UART_NUM        1             //!< (C) UART number
#endif
#ifndef STDIO_UART_BAUD
#define STDIO_UART_BAUD     115200          //!< (C) UART baudrate
#endif
#ifndef STDIO_BUFF_SIZE
#define STDIO_BUFF_SIZE      128            //!< (C) UART buffer size
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup STDIO
 * @{
 * @addtogroup STDIO_UART_Pravate
 * @{
 * @addtogroup          STDIO_UART_Pravate_Macros
 * @{
 ******************************************************************************/

#define USARTx_CONTROLLER   STDIO_UART_NUM
#define USARTx_BAUDRATE     STDIO_UART_BAUD
#define USARTx_BUF_SIZE     STDIO_BUFF_SIZE

/*----------------------------------------------------------------------------*/
extern ARM_DRIVER_USART     ATPASTE2(Driver_USART, USARTx_CONTROLLER);
#define ptrUSART          (&ATPASTE2(Driver_USART, USARTx_CONTROLLER))

#define USART_MODE         (  ARM_USART_MODE_ASYNCHRONOUS  \
                            | ARM_USART_DATA_BITS_8        \
                            | ARM_USART_PARITY_NONE        \
                            | ARM_USART_STOP_BITS_1        \
                            | ARM_USART_FLOW_CONTROL_NONE  \
                           )

#if (USARTx_CONTROLLER == 4) || (USARTx_CONTROLLER == 5) || (USARTx_CONTROLLER == 7) || (USARTx_CONTROLLER == 8)
#define USARTx              ATPASTE2(UART, USARTx_CONTROLLER)
#define USART_NAME          "UART"  TO_STRING(USARTx_CONTROLLER)
#else
#define USARTx              ATPASTE2(USART, USARTx_CONTROLLER)
#define USART_NAME          "USART" TO_STRING(USARTx_CONTROLLER)
#endif


/*******************************************************************************
 * @}
 * @addtogroup          STDIO_UART_Pravate_Variables
 * @{
 ******************************************************************************/

static QUEUE_TYPE(uint8_t, USARTx_BUF_SIZE) _tx_Queue;  // Queue for Send
static QUEUE_TYPE(uint8_t, USARTx_BUF_SIZE) _rx_Queue;  // Queue for Recv

#ifdef RTE_CMSIS_RTOS2
static       osSemaphoreId_t    _tx_Sem;                // Semaphore for Send
static       osSemaphoreId_t    _rx_Sem;                // Semaphore for Recv
static const osSemaphoreAttr_t  _tx_SemAttr = { USART_NAME " _tx_Sem", 0u, NULL, 0u };
static const osSemaphoreAttr_t  _rx_SemAttr = { USART_NAME " _rx_Sem", 0u, NULL, 0u };
#endif


/*******************************************************************************
 * @}
 * @addtogroup          STDIO_UART_Pravate_Functions
 * @{
 ******************************************************************************/
/**
 * Initialize the semaphore of TxD
 */
static __INLINE void os_tx_sem_init(void)
{
#ifdef RTE_CMSIS_RTOS2
    _tx_Sem = NULL;
#endif
}


/**
 * Initialize the semaphore of RxD
 */
static __INLINE void os_rx_sem_init(void)
{
#ifdef RTE_CMSIS_RTOS2
    _rx_Sem = NULL;
#endif
}


/**
 * Post the semaphore of TxD
 */
static __INLINE void os_tx_sem_post(void)
{
#ifdef RTE_CMSIS_RTOS2
    if (_tx_Sem != NULL)
    {
        if (osSemaphoreRelease(_tx_Sem) != osOK) {
            LOG_ASSERT("osSemaphoreRelease(_tx_Sem)");
        }
    }
#endif
}


/**
 * Post the semaphore of RxD
 */
static __INLINE void os_rx_sem_post(void)
{
#ifdef RTE_CMSIS_RTOS2
    if (_rx_Sem != NULL)
    {
        if (osSemaphoreRelease(_rx_Sem) != osOK) {
            LOG_ASSERT("osSemaphoreRelease(_rx_Sem)");
        }
    }
#endif
}


/**
 * Take the semaphore of TxD
 */
static __INLINE void os_tx_sem_take(void)
{
#ifdef RTE_CMSIS_RTOS2
    if((_tx_Sem == NULL) && (osKernelGetState() > osKernelReady))
    {
        if ((_tx_Sem =  osSemaphoreNew(QUEUE_SIZE(_tx_Queue), QUEUE_FREE(_tx_Queue), &_tx_SemAttr)) == NULL) {
            LOG_ASSERT("osSemaphoreNew(QUEUE_SIZE(_tx_Queue), QUEUE_FREE(_tx_Queue), &_tx_SemAttr)");
        }
    }
    if (_tx_Sem != NULL)
    {
        if (osSemaphoreAcquire(_tx_Sem, osWaitForever) != osOK) {
            LOG_ASSERT("osSemaphoreAcquire(_tx_Sem, osWaitForever)");
        }
    }
#endif
}


/**
 * Take the semaphore of RxD
 */
static __INLINE void os_rx_sem_take(void)
{
#ifdef RTE_CMSIS_RTOS2
    if((_rx_Sem == NULL) && (osKernelGetState() > osKernelReady))
    {
        if ((_rx_Sem =  osSemaphoreNew(QUEUE_SIZE(_rx_Queue), QUEUE_ITEM(_rx_Queue), &_rx_SemAttr)) == NULL) {
            LOG_ASSERT("osSemaphoreNew(QUEUE_SIZE(_rx_Queue), QUEUE_ITEM(_rx_Queue), &_rx_SemAttr)");
        }
    }
    if (_rx_Sem != NULL)
    {
        if (osSemaphoreAcquire(_rx_Sem, osWaitForever) != osOK) {
            LOG_ASSERT("osSemaphoreAcquire(_rx_Sem, osWaitForever)");
        }
    }
#endif
}


/*----------------------------------------------------------------------------*/
/**
 * Callback function of Signal USART Events.
 *
 * @param[in]  event   notification mask
 */
static __INLINE void USART_SignalEvent(uint32_t event)
{
    void   *ptr;

    // Send completed
    if (event & ARM_USART_EVENT_SEND_COMPLETE)
    {
        os_tx_sem_post();                       // Releases the semaphore
        QUEUE_O_ADJ(_tx_Queue, 1);              // Dequeue

        if (QUEUE_ITEM((_tx_Queue)) != 0) {     // Queue Non-Empty
            QUEUE_O_BUF(_tx_Queue, &ptr);       // Start sending data
            switch (ptrUSART->Send(ptr, 1)) {
            case ARM_DRIVER_OK:          break; // Success started
            case ARM_DRIVER_ERROR_BUSY:  break; // Send is not completed
            default:  LOG_ASSERT("ptrUSART->Send(ptr, 1)"); // break;
            }
        }
    }

    // Receive completed
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE)
    {
        if (QUEUE_FREE((_rx_Queue)) != 0) {     // Queue Non-Full
            QUEUE_I_ADJ(_rx_Queue, 1);          // Enqueue
            os_rx_sem_post();                   // Releases the semaphore
        }

        QUEUE_I_BUF(_rx_Queue, &ptr);           // Start receiving data
        if (ptrUSART->Receive(ptr, 1) != ARM_DRIVER_OK) {
            LOG_ASSERT("ptrUSART->Receive(ptr, 1)");
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
static __INLINE int USART_PutChar(int ch)
{
    void   *ptr;

    os_tx_sem_take();                       // Waiting for semaphore
    while (QUEUE_FREE(_tx_Queue) == 0) ;    // Waiting for queue Non-Full
    QUEUE_PUSH(_tx_Queue, (uint8_t)ch);     // Enqueue

    if (QUEUE_ITEM((_tx_Queue)) == 1)
    {
        QUEUE_O_BUF(_tx_Queue, &ptr);       // Start sending data
        switch (ptrUSART->Send(ptr, 1)) {
        case ARM_DRIVER_OK:          break; // Success started
        case ARM_DRIVER_ERROR_BUSY:  break; // Send is not completed
        default:  LOG_ASSERT("ptrUSART->Send(ptr, 1)"); // break;
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
static __INLINE int USART_GetChar(void)
{
    uint8_t  ch;

    os_rx_sem_take();                       // Waiting for semaphore
    while (QUEUE_ITEM(_rx_Queue) == 0) ;    // Waiting for queue Non-Empty
    QUEUE_POP(_rx_Queue, &ch);              // Dequeue

    return( ch );
}


/**
 * USART send date of Query Mode.
 *
 * @param[in]  data   Data to send.
 *
 * @retval  (!= -1) Success, the sent character is returned
 * @retval  (== -1) Failed
 */
static __INLINE int USART_QuerySend(int data)
{
    while(QUEUE_ITEM(_tx_Queue) != 0) ;

    while(!READ_BIT(USARTx->SR, USART_SR_TXE)) ;
    WRITE_REG(USARTx->DR, data);  //lint !e732  !e734

    return( data );
}


/**
 * Initialize the USART.
 */
static __INLINE void USART_Initialize(void)
{
    void   *ptr;

    if (ptrUSART->Uninitialize() != ARM_DRIVER_OK) {
        LOG_ASSERT("ptrUSART->Uninitialize()");
    }
    if (ptrUSART->Initialize(USART_SignalEvent) != ARM_DRIVER_OK) {
        LOG_ASSERT("ptrUSART->Initialize(USART_SignalEvent)");
    }
    if (ptrUSART->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
        LOG_ASSERT("ptrUSART->PowerControl(ARM_POWER_FULL)");
    }
    if (ptrUSART->Control(USART_MODE, USARTx_BAUDRATE) != ARM_DRIVER_OK) {
        LOG_ASSERT("ptrUSART->Control(USART_MODE, USARTx_BAUDRATE)");
    }
    if (ptrUSART->Control(ARM_USART_CONTROL_RX, 1) != ARM_DRIVER_OK) {
        LOG_ASSERT("ptrUSART->Control(ARM_USART_CONTROL_RX, 1)");
    }
    if (ptrUSART->Control(ARM_USART_CONTROL_TX, 1) != ARM_DRIVER_OK) {
        LOG_ASSERT("ptrUSART->Control(ARM_USART_CONTROL_TX, 1)");
    }

    QUEUE_INIT(_rx_Queue);
    QUEUE_INIT(_tx_Queue);
    os_rx_sem_init();
    os_tx_sem_init();

    QUEUE_I_BUF(_rx_Queue, &ptr);   // Start receiving data
    if (ptrUSART->Receive(ptr, 1) != ARM_DRIVER_OK) {
        LOG_ASSERT("ptrUSART->Receive(ptr, 1)");
    }
}


/*----------------------------------------------------------------------------*/
void stdio_Retarget(void)
{
    USART_Initialize();
}

int stdin_getchar(void)
{
    return( USART_GetChar() );
}

int stdout_putchar(int ch)
{
    return( USART_PutChar(ch) );
}

int stderr_putchar(int ch)
{
    return( USART_QuerySend(ch) );
}

void ttywrch(int ch)
{
    USART_QuerySend(ch);  //lint !e534  " Ignoring return value "
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

