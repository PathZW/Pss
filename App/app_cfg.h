/*******************************************************************************
 * @file    app_cfg.h
 * @author  Linghu
 * @version v1.0.0
 * @date    2019/3/26
 * @brief   Apps configuration
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/3/26 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __APP_CFG_H__
#define __APP_CFG_H__
/******************************************************************************/
#ifndef EXTERN_C
#ifdef  __cplusplus
#define EXTERN_C        extern "C"
#else
#define EXTERN_C        extern
#endif
#endif
#ifndef ENABLE
#define ENABLE          1
#endif
#ifndef DISABLE
#define DISABLE         0
#endif
/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          __App_Config
 * @{
 ******************************************************************************/
/**
 * Version information definition, Ref: version.c
 */
#if ENABLE
#ifdef MCU_SENS
#   define COMPANY_NAME               "PSS"                 //!< ��˾��
#   define DEVICE_NAME               "LDBI016SENS"          //!< �豸(��ģ��)��
#   define FIRMWARE_NAME             "SENS"                 //!< �̼���
#   define VER_HARDWARE              "HW_V1.0.0"               //!< Ӳ���汾

#   define VER_MAJOR                    1                   //!< ���汾��
#   define VER_MINOR                    0                   //!< �ΰ汾��
#   define VER_BUILD                    0                   //!< �� �� ��
#   define VER_CURRENT_DATE         "20230317"              //!< ����ʱ��ϵͳ����

#else
#   define COMPANY_NAME               "PSS"                 //!< ��˾��
#   define DEVICE_NAME               "LDBI016SOUR"          //!< �豸(��ģ��)��
#   define FIRMWARE_NAME             "SOUR"                 //!< �̼���
#   define VER_HARDWARE              "HW_V1.0.0"            //!< Ӳ���汾

#   define VER_MAJOR                    1                   //!< ���汾��
#   define VER_MINOR                    0                   //!< �ΰ汾��
#   define VER_BUILD                    0                   //!< �� �� ��
#   define VER_CURRENT_DATE         "20230317"              //!< ����ʱ��ϵͳ����
#endif
#endif


/*******************************************************************************
 * app_main, Ref: board.h, Flash_STM32xx.c, emb_platform.c
 */
#if ENABLE
#   define ROM_MAIN_ADDR           0x08000000       //!< (C) ��ROM�� ��ַ
#   define ROM_MAIN_SIZE           0x00080000       //!< (C) ��ROM�� ��С
#   define ROM_RESV_SIZE           0x00020000       //!< (C) ��ROM�� ������С(BootLoader)
#   define RAM_MAIN_ADDR           0x20000000       //!< (C) ��RAM�� ��ַ
#   define RAM_MAIN_SIZE           0x00020000       //!< (C) ��RAM�� ��С

//# define IAP_FLASH_NUM               0            //!< (C) IAP Flash driver number
#   define EFS_FLASH_NUM               1            //!< (C) EFS Flash driver number, (< 0)Disable
#   define EFS_FLASH_ADDR          0x08008000       //!< (C) EFS Flash addr, (== 0)Auto
#   define EFS_FLASH_SIZE          0x0000C000       //!< (C) EFS Flash size, (== 0)Auto
#   define EFS_ERASE_SIZE          0x00004000       //!< (C) EFS Flash sector size       (ef_cfg.h)
#   define EFS_WRITE_WIDTH             8            //!< (C) EFS Flash write granularity (ef_cfg.h)
//# define LOG_FLASH_SIZE          0x00004000       //!< (C) LOG Flash size              (ef_cfg.h)

//# define INIT_THREAD_PRIO      osPriorityNormal   //!< (C) ϵͳ��ʼ���߳����ȼ�
//# define MAIN_THREAD_PRIO      osPriorityNormal   //!< (C) app_main �������ȼ�
//# define MAIN_THREAD_STKB             0           //!< (C) app_main �߳�ջ��С(���ֽ�Ϊ��λ)

#   define SYS_PROMPT_LED_INIT()   RunLED_Init()    //!< (C) ϵͳ��ʾ LED ��ʼ��
#   define SYS_PROMPT_LED_OFF()    RunLED_OFF()     //!< (C) ϵͳ��ʾ LED �ر�
#   define SYS_PROMPT_LED_RED()    RunLED_ON()      //!< (C) ϵͳ��ʾ LED ����ɫ��
#   define SYS_PROMPT_LED_GREEN()  RunLED_ON()      //!< (C) ϵͳ��ʾ LED ����ɫ��
#   define SYS_PROMPT_LED_YELLOW() RunLED_ON()      //!< (C) ϵͳ��ʾ LED ����ɫ��
    EXTERN_C  void RunLED_Init(void);
    EXTERN_C  void RunLED_ON  (void);
    EXTERN_C  void RunLED_OFF (void);
#endif


/*******************************************************************************
 * stdio �ض���; Ref: stdio_UART.c
 */
#if ENABLE      // ������ CN1
#   define STDIO_UART_NUM        1              //!< (C) USART number
//# define STDIO_UART_BAUD       115200         //!< (C) ������
//# define STDIO_BUFF_SIZE       128            //!< (C) ��������(���ֽ�Ϊ��λ)
#endif


/*******************************************************************************
 * �����нӿ�
 */
#if ENABLE  // ----- Ref: cli_shell_priv.h -------------------------------------
#   define CLI_THREAD_STKB         1024u        //!< �����д����߳�ջ��С(���ֽ�Ϊ��λ)
//# define CLI_THREAD_PRIO    OS_PRIO_NORMAL    //!< �����д����߳����ȼ�

#   define CLI_MAX_COMMAND         128          //!< ��ע��������������
#   define CLI_MAX_INTERFACE         8          //!< �ɴ򿪵���������нӿ�����

#   define CLI_LINE_BUFF_SIZE      1024         //!< ���������뻺���С(���ֽ�Ϊ��λ)
#   define CLI_OUT_BUFF_SIZE       1024         //!< ��������������С(���ֽ�Ϊ��λ)
#endif

#if ENABLE  // ----- Ref: Cmd_App.c --------------------------------------------
#   define CMD_FORWARD_IF_NUM         0         //!< (C) ����ת��Ŀ�Ľӿ�����, (0)��֧������ת��
#   define CMD_FORWARD_HOOK    Cmd_ForwardHook  //!< (C) ����ת�����Ӻ���(����Ӧ����չ����ת������)
//# define CMD_BIN_PARSER      Cmd_BinParser    //!< (C) �������������, Ref: cli_bin_fn

#   define CMD_STDIO_CLI_EN           1         //!< (C) stdio ��(1)/��(0) Ϊ�����нӿ��豸
//# define CMD_STDIO_STR_EN           1         //!< (R) stdio ��(1)/��(0) ֧���ַ�������
//# define CMD_STDIO_BIN_EN           0         //!< (R) stdio ��(1)/��(0) ֧�ֶ���������
//# define CMD_STDIO_FORWARD_IDX     -1         //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define CMD_STDIO_FORWARD_SRC     -1         //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�

#   define CMD_PROMPT_LED_INIT() LinkLED_Init() //!< (C) ��������ʾ LED ��ʼ��
#   define CMD_PROMPT_LED_ON()   LinkLED_ON()   //!< (C) ��������ʾ LED ��ʾ����
#   define CMD_PROMPT_LED_OFF()  LinkLED_OFF()  //!< (C) ��������ʾ LED ��ʾ�ر�
    EXTERN_C void LinkLED_Init(void);
    EXTERN_C void LinkLED_ON  (void);
    EXTERN_C void LinkLED_OFF (void);
#endif

#if ENABLE  // ----- Ref: Cmd_RS232.c (P1) -------------------------------------
#   define RS232_USART_NUM           -1         //!< (C) USART number, ( < 0) Disable
//# define RS232_BAUDRATE          115200       //!< (R) ������
//# define RS232_BUFF_SIZE          128         //!< (C) ��������(���ֽ�Ϊ��λ)

#   define RS232_CMD_STR_EN          1          //!< (R) ��(1)/��(0) ֧���ַ�������
//# define RS232_CMD_BIN_EN          0          //!< (R) ��(1)/��(0) ֧�ֶ���������
//# define RS232_FORWARD_IDX        -1          //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define RS232_FORWARD_SRC        -1          //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�
#endif

#if ENABLE  // ----- Ref: Cmd_RS485.c (P1) -------------------------------------
#   define RS485_USART_NUM           3          //!< (C) USART number, ( < 0) Disable
#   define RS485_BAUDRATE          115200       //!< (R) ������
#   define RS485_BUFF_SIZE          1024        //!< (C) ��������(���ֽ�Ϊ��λ)

#   define RS485_CMD_STR_EN          1          //!< (R) ��(1)/��(0) ֧���ַ�������
//# define RS485_CMD_BIN_EN          0          //!< (R) ��(1)/��(0) ֧�ֶ���������
//# define RS485_FORWARD_IDX        -1          //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define RS485_FORWARD_SRC        -1          //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�

#   define RS485_FLOW_CONTROL        0          //!< (C) Ӳ��������
#   define RS485_FLOW_CTRL_INIT()   RS485_DirInit()
#   define RS485_FLOW_RTS_SET()     RS485_RxEN()
#   define RS485_FLOW_RTS_CLR()     RS485_TxEN()
    EXTERN_C void RS485_DirInit(void);
    EXTERN_C void RS485_RxEN(void);
    EXTERN_C void RS485_TxEN(void);
#endif

#if DISABLE // ----- Ref: Cmd_UART1.c (U13) ------------------------------------
//# define UART1_DEVICE_NUM          -1          //!< (C) USART number, ( < 0) Disable
//# define UART1_BAUDRATE          115200       //!< (R) ������
//# define UART1_BUFF_SIZE          128         //!< (C) ��������(���ֽ�Ϊ��λ)

//# define UART1_CMD_STR_EN          1          //!< (R) ��(1)/��(0) ֧���ַ�������
//# define UART1_CMD_BIN_EN          0          //!< (R) ��(1)/��(0) ֧�ֶ���������
//# define UART1_FORWARD_IDX        -1          //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define UART1_FORWARD_SRC        -1          //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�
#endif

#if ENABLE // ----- Ref: Cmd_UART2.c ------------------------------------------
#   define UART2_DEVICE_NUM         6           //!< (C) USART number, ( < 0) Disable
#   define UART2_BAUDRATE          115200       //!< (R) ������
#   define UART2_BUFF_SIZE          1024        //!< (C) ��������(���ֽ�Ϊ��λ)

#   define UART2_CMD_STR_EN          1          //!< (R) ��(1)/��(0) ֧���ַ�������
//# define UART2_CMD_BIN_EN          0          //!< (R) ��(1)/��(0) ֧�ֶ���������
//# define UART2_FORWARD_IDX        -1          //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define UART2_FORWARD_SRC        -1          //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�

#   define UART2_FLOW_CONTROL        0          //!< (C) Ӳ��������
#   define UART2_FLOW_CTRL_INIT()   RS485_2_DirInit()
#   define UART2_FLOW_RTS_SET()     RS485_2_RxEN()
#   define UART2_FLOW_RTS_CLR()     RS485_2_TxEN()
    EXTERN_C void RS485_2_DirInit(void);
    EXTERN_C void RS485_2_RxEN(void);
    EXTERN_C void RS485_2_TxEN(void);
#endif

#if DISABLE // ----- Ref: Cmd_SPIx.c (default Master mode) ---------------------
#   define SPIx_DEVICE_NUM             1        //!< (C) SPI number, ( < 0) Disable
#   define SPIx_BUS_SPEED       (21000000 / 2)  //!< (R) �����ٶ�(�� Hz Ϊ��λ)
#   define SPIx_BUFF_SIZE            1024       //!< (C) ��������(���ֽ�Ϊ��λ)

//# define SPIx_CMD_STR_EN             1        //!< (R) ��(1)/��(0) ֧���ַ�������
//# define SPIx_CMD_BIN_EN             0        //!< (R) ��(1)/��(0) ֧�ֶ���������
#   define SPIx_FORWARD_IDX            0        //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define SPIx_FORWARD_SRC           -1        //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�
#endif

#if DISABLE // ----- Ref: Cmd_SPIy.c (default Slave mode) ----------------------
#   define SPIy_DEVICE_NUM             2        //!< (C) SPI number, ( < 0) Disable
#   define SPIy_BUS_SPEED       (21000000 / 2)  //!< (R) �����ٶ�(�� Hz Ϊ��λ)
#   define SPIy_BUFF_SIZE            1024       //!< (C) ��������(���ֽ�Ϊ��λ)

//# define SPIy_CMD_STR_EN             1        //!< (R) ��(1)/��(0) ֧���ַ�������
//# define SPIy_CMD_BIN_EN             0        //!< (R) ��(1)/��(0) ֧�ֶ���������
//# define SPIy_FORWARD_IDX           -1        //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define SPIy_FORWARD_SRC           -1        //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�
#endif

#if DISABLE // ----- Ref: Cmd_TCP.c --------------------------------------------
#   define NET_MAX_CONNECT             4            //!< (C) ���������, (<= 0) Disable
//# define NET_MULTI_PORT              0            //!< (C) ���˿ڶ�����(0) or ��˿ڵ�����(1)
//# define NET_SRV_THREAD_STKB       1024           //!< (C) ��������߳�ջ��С(���ֽ�Ϊ��λ)
//# define NET_SRV_THREAD_PRIO  osPriorityNormal1   //!< (C) ��������߳����ȼ�

//# define NET_CMD_STR_EN              1            //!< (R) ��(1)/��(0) ֧���ַ�������
//# define NET_CMD_BIN_EN              0            //!< (R) ��(1)/��(0) ֧�ֶ���������
//# define NET_FORWARD_IDX            -1            //!< (R) ת������, (< 0)��Ŀ�Ľӿ�
//# define NET_FORWARD_SRC            -1            //!< (R) ת������, (< 0)��Ĭ��Դ�ӿ�
#endif


/*******************************************************************************
 * Network Protocol Stack, Ref: lwipopts.h, net_user.c, net_device.h
 */


/*******************************************************************************
 * Config of Component CmBacktrace, ref: cmb_cfg.h
 */
#if ENABLE
#   define cmb_print(...)        printk(__VA_ARGS__)
#   define CMB_OS_PLATFORM_TYPE  CMB_OS_PLATFORM_RTX5

    EXTERN_C int printk(const char* fmt, ...);
#endif


/*******************************************************************************
 * Configurations of Common defines, ref: defines.h
 */
#if ENABLE
#   define ENDL                   "\r\n"    //!< (C) End of line, start the new line

//# define DEFINES_ARG_CHECK_EN      1      //!< (C) �Ƿ�����������
//# define DEFINES_WARNING_EN        1      //!< (C) �Ƿ�������뾯����Ϣ
//# define DEFINES_INTEGER_TYPES     1      //!< (C) �Ƿ���[INTxx]��������
//# define DEFINES_WORD_TYPES        1      //!< (C) �Ƿ���[BYTE/WORD/DWORD]����
//# define DEFINES_BOOLEAN_TYPES     1      //!< (C) �Ƿ���[BOOL]����
//# define DEFINES_TCHAR_TYPES       1      //!< (C) �Ƿ���[TCHAR]����
//# define DEFINES_TOKEN_PASTE       1      //!< (C) �Ƿ���[����(token)����]��
#endif


/*******************************************************************************
 * STM32 StdPeriph or HAL Library configuration.
 * Ref: "stm32xxxx_conf.h" or "stm32fxxxx_hal_conf.h" or "stm32f10x.h"
 */
#if DISABLE
//# define CMSIS_device_header  "stm32f2xx.h"   // stm32f10x.h  stm32f4xx.h

//# define USE_FULL_ASSERT       1U
//# define HSE_VALUE             ((uint32_t)25000000U)
//# define LSE_VALUE             ((uint32_t)32768U)
//# define EXTERNAL_CLOCK_VALUE  (12288000U)
//# define USE_SPI_CRC           1U
#endif

/**
 * Stack and Heap Configuration, Ref: startup_stm32xxxx.s
 */
#if DISABLE
//# define Stack_Size            1024
//# define Heap_Size             16384
#endif

/**
 * RTE Device Configuration, Ref: RTE_Device.h
 */
#if DISABLE
//# define RTE_HSI               8000000
//# define RTE_HSE               8000000
//# define RTE_SYSCLK            72000000
//# define RTE_HCLK              72000000
//# define RTE_PCLK1             36000000
//# define RTE_PCLK2             72000000
//# define RTE_ADCCLK            36000000
//# define RTE_USBCLK            48000000
#endif

/**
 * Parser ini file Configuration, Ref: dictionary.c
 */
#if ENABLE
#   define INI_DICTMINSZ        512
#endif


/*******************************************************************************
 * MDK - Component ::Event Recorder; Ref: EventRecorder.h
 */

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartAx(slot, v1, v2) EventRecord2 (0xEF00U+EventLevelError+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopAx(slot, v1, v2)  EventRecord2 (0xEF20U+EventLevelError+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartBx(slot, v1, v2) EventRecord2 (0xEF40U+EventLevelAPI+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopBx(slot, v1, v2)  EventRecord2 (0xEF60U+EventLevelAPI+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartCx(slot, v1, v2) EventRecord2 (0xEF80U+EventLevelOp+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopCx(slot, v1, v2)  EventRecord2 (0xEFA0U+EventLevelOp+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15)
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStartDx(slot, v1, v2) EventRecord2 (0xEFC0U+EventLevelDetail+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))

/// \param[in]    slot   slot number (up to 16 slots, 0..15) 
/// \param[in]    v1     first data value
/// \param[in]    v2     second data value
#define EventStopDx(slot, v1, v2)  EventRecord2 (0xEFE0U+EventLevelDetail+((slot) & 0xFU), (uint32_t)(v1), (uint32_t)(v2))


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#undef ENABLE
#undef DISABLE
#endif  // __APP_CFG_H__
/*****************************  END OF FILE  **********************************/

