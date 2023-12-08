/*******************************************************************************
 * @file    Cmd_App.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/6/23
 * @brief   Command application Module.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/6/23 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Cmd_App"
#include "emb_log.h"
#include "Cmd_App.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"

#include "emb_delay.h"
#include "os_abstract.h"
#include "emb_rtcfg.h"
#include "shell/cli_shell.h"

#ifdef  CMD_BIN_PARSER              // 二进制命令处理函数
   int  CMD_BIN_PARSER(int cli, const void *data, int size);
#endif

#ifdef  CMD_FORWARD_HOOK            // 数据转发钩子函数(用于应用扩展数据转发功能)
   int  CMD_FORWARD_HOOK(int cli, const void *data, int size);
#endif

#ifndef STDIO_UART_NUM
#define STDIO_UART_NUM          ?   // stdio used UART number
#endif

__WEAK void Board_Reset(void)  {}   //lint -esym(522, Board_Reset)
__WEAK void net_cmd_init(void) {}   //lint -esym(522, net_cmd_init)


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup              Cmd_App
 * @{
 ******************************************************************************/

#ifndef RTCFG_ITEM_SYS_IDN
#define RTCFG_ITEM_SYS_IDN    "Sys:IDN"     //!< (C) 设备 IDN 配置键
#endif
#ifndef RTCFG_DFT_SYS_IDN                   //!< (C) 设备 IDN 默认值
#define RTCFG_DFT_SYS_IDN  "PSS,NAME,00000000,V1.0.0_20190802"
#endif

#ifndef CMD_BIN_PARSER
#define CMD_BIN_PARSER      Cmd_BinParser   //!< (C) 二进制命令处理函数, Ref: cli_bin_fn
#undef  CMD_BIN_PARSER
#endif

#ifndef CMD_FORWARD_IF_NUM
#define CMD_FORWARD_IF_NUM        0         //!< (C) 数据转发目的接口数量, (0)不支持命令转发
#endif
#ifndef CMD_FORWARD_HOOK
#define CMD_FORWARD_HOOK   Cmd_ForwardHook  //!< (C) 数据转发钩子函数(用于应用扩展数据转发功能)
#undef  CMD_FORWARD_HOOK
#endif

#ifndef CMD_FWD_MODE_GET
#define CMD_FWD_MODE_GET   ":PSS:FWD:MODE?" //!< (C) 数据转发模式查询命令
#undef  CMD_FWD_MODE_GET
#endif
#ifndef CMD_FWD_MODE_STR
#define CMD_FWD_MODE_STR   ":PSS:FWD:STR"   //!< (C) 数据转发模式设置为字符串命令
#undef  CMD_FWD_MODE_STR
#endif
#ifndef CMD_FWD_MODE_BIN
#define CMD_FWD_MODE_BIN   ":PSS:FWD:BIN"   //!< (C) 数据转发模式设置为二进制命令
#undef  CMD_FWD_MODE_BIN
#endif
#ifndef CMD_FWD_BIN1_HEAD
#define CMD_FWD_BIN1_HEAD    0xAA, 0x55     //!< (C) 转发数据包头(二进制1)
#undef  CMD_FWD_BIN1_HEAD
#endif
#ifndef CMD_FWD_BIN1_TAIL
#define CMD_FWD_BIN1_TAIL    0x55, 0xAA     //!< (C) 转发数据包尾(二进制1)
#undef  CMD_FWD_BIN1_TAIL
#endif
#ifndef CMD_FWD_BIN2_HEAD
#define CMD_FWD_BIN2_HEAD    0xAA, 0x55     //!< (C) 转发数据包头(二进制2)
#undef  CMD_FWD_BIN2_HEAD
#endif
#ifndef CMD_FWD_BIN2_TAIL
#define CMD_FWD_BIN2_TAIL    0x55, 0xAA     //!< (C) 转发数据包尾(二进制2)
#undef  CMD_FWD_BIN2_TAIL
#endif

#ifndef CMD_STDIO_CLI_EN
#define CMD_STDIO_CLI_EN           0        //!< (C) stdio 是(1)/否(0) 为命令行接口设备
#endif
#ifndef CMD_STDIO_STR_EN
#define CMD_STDIO_STR_EN           1        //!< (R) stdio 是(1)/否(0) 支持字符串命令
#endif
#ifndef CMD_STDIO_BIN_EN
#define CMD_STDIO_BIN_EN           0        //!< (R) stdio 是(1)/否(0) 支持二进制命令
#endif
#ifndef CMD_STDIO_FORWARD_IDX
#define CMD_STDIO_FORWARD_IDX     -1        //!< (R) 字符串命令转发: (< 0)非转发接口, (>= 0)转发接口索引
#endif
#ifndef CMD_STDIO_FORWARD_SRC
#define CMD_STDIO_FORWARD_SRC     -1        //!< (R) 命令转发源接口: (< 0)非 源 接口, (>= 0)源 接 口索引
#endif

#ifndef CMD_NAME_HELP
#define CMD_NAME_HELP          "*HELP"      //!< (R) 命令名: 显示帮助信息
#endif
#ifndef CMD_NAME_REBOOT
#ifdef MCU_SOUR
#define CMD_NAME_REBOOT        "*RST:SOUR"    //!< (R) 命令名: 重启系统
#else
#define CMD_NAME_REBOOT        "*RST:SENS"    //!< (R) 命令名: 重启系统
#endif
#endif
#ifndef CMD_NAME_WAIT
#define CMD_NAME_WAIT           "*WAI"      //!< (R) 命令名: SCPI99 *WAI
#endif
#ifndef CMD_NAME_ECHO_ON
#define CMD_NAME_ECHO_ON     "**ECHO:ON"    //!< (R) 命令名: 开启回显
#endif
#ifndef CMD_NAME_ECHO_OFF
#define CMD_NAME_ECHO_OFF    "**ECHO:OFF"   //!< (R) 命令名: 关闭回显
#endif
#ifndef CMD_NAME_IDN_GET
#ifdef MCU_SOUR
#define CMD_NAME_IDN_GET        "*IDN:SOUR?"     //!< (R) 命令名: 显示 IDN
#else
#define CMD_NAME_IDN_GET        "*IDN:SENS?"     //!< (R) 命令名: 显示 IDN
#endif
#endif
#ifndef CMD_NAME_IDN_SET
#define CMD_NAME_IDN_SET       "**IDN"      //!< (R) 命令名: 设置 IDN
#endif
#ifndef CMD_NAME_SHOW_VER
#define CMD_NAME_SHOW_VER    "**SHOW:VER?"  //!< (R) 命令名: 显示版本信息
#endif
#ifndef CMD_NAME_FW_VER
#define CMD_NAME_FW_VER    "**SHOW:VER:FW?" //!< (R) 命令名: 显示软件版本
#endif
#ifndef CMD_NAME_HW_VER
#define CMD_NAME_HW_VER    "**SHOW:VER:HW?" //!< (R) 命令名: 显示硬件版本
#endif
#ifndef CMD_NAME_FWD_BIN
#define CMD_NAME_FWD_BIN      "*PSS:BIN"    //!< (C) 命令名: 转发指定长度二进制数据
#undef  CMD_NAME_FWD_BIN
#endif

#ifndef CMD_NAME_MEM_INFO
#define CMD_NAME_MEM_INFO   "**SHOW:MEM?"   //!< (R) 命令名: 显示统内存信息
#endif
#ifndef CMD_MEM_INFO_ISR
#define CMD_MEM_INFO_ISR Cmd_MemInfISR(cli) //!< (R) 输出 ISR栈  内存使用信息函数
#endif
#ifndef CMD_MEM_INFO_OS
#define CMD_MEM_INFO_OS  Cmd_MemInfOS (cli) //!< (R) 输出 OS系统 内存使用信息函数
#endif
#ifndef CMD_MEM_INFO_NET
#define CMD_MEM_INFO_NET Cmd_MemInfNET(cli) //!< (R) 输出 网络栈 内存使用信息函数
#endif
#ifndef CMD_MEM_INFO_STD
#define CMD_MEM_INFO_STD Cmd_MemInfSTD(cli) //!< (R) 输出  Heap  内存使用信息函数
#endif

#ifndef CMD_PROMPT_LED_INIT
#define CMD_PROMPT_LED_INIT()  ((void)0)    //!< (C) 命令行提示 LED 初始化
#endif
#ifndef CMD_PROMPT_LED_ON
#define CMD_PROMPT_LED_ON()    ((void)0)    //!< (C) 命令行提示 LED 显示开启
#endif
#ifndef CMD_PROMPT_LED_OFF
#define CMD_PROMPT_LED_OFF()   ((void)0)    //!< (C) 命令行提示 LED 显示关闭
#endif

#ifndef ENDL
#define ENDL               "\r\n"           //!< (C) End of line, start the new line
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Cmd_App
 * @{
 * @addtogroup              Pravate_Variables
 * @{
 ******************************************************************************/

static osTimerId_t  _Cmd_FlickTmr;                        // RTOS timer for LED flick
static uint8_t      _Cmd_FlickSta;                        // On or Off status of LED

#if (CMD_FORWARD_IF_NUM > 0)
static OS_MUTEX_ID  _Cmd_ForwardMut;                      // Mutex for thread-safe
static   int        _Cmd_ForwardDst[CMD_FORWARD_IF_NUM];  // Interface ID of destination
static   int        _Cmd_ForwardSrc[CMD_FORWARD_IF_NUM];  // Interface ID of source
#endif

/**
 * Config item of IDN
 */
__WEAK const rtcfg_string_t  _Cmd_SysIdn_Item = { RTCFG_ITEM_SYS_IDN, RTCFG_DFT_SYS_IDN };


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Prototypes
 * @{
 ******************************************************************************/

//lint -esym(402, Cmd_MemInfISR, Cmd_MemInfOS, Cmd_MemInfNET, Cmd_MemInfSTD)
#if defined(CMD_NAME_MEM_INFO)
__WEAK void Cmd_MemInfISR(int cli) { UNUSED_VAR(cli); }
__WEAK void Cmd_MemInfOS (int cli) { UNUSED_VAR(cli); }
__WEAK void Cmd_MemInfNET(int cli) { UNUSED_VAR(cli); }
__WEAK void Cmd_MemInfSTD(int cli) { UNUSED_VAR(cli); }
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Functions
 * @{
 ******************************************************************************/
#if (CMD_STDIO_CLI_EN != 0)
/**
 * CLI open the stdio device
 *
 * @param[in]  device   Pointer to device.
 * @param[in]  cli      Command Line Interface.
 *
 * @retval  (>= 0) Success
 * @retval  ( < 0) Failed
 */
static int stdio_DeviceOpen(cli_device_t* device, int cli)
{
    UNUSED_VAR(device);  UNUSED_VAR(cli);

    if (CMD_STDIO_FORWARD_IDX >= 0) {       //lint !e506 !e774
        LOG_INFO("CLI \"stdio\" Opened on the UART" TO_STRING(STDIO_UART_NUM)
                 ", Forward Dst on Idx: %d", (int)CMD_STDIO_FORWARD_IDX
                );
        Cmd_ForwardSetIF((unsigned)CMD_STDIO_FORWARD_IDX, cli);          //lint !e534
    }
    else if (CMD_STDIO_FORWARD_SRC >= 0) {  //lint !e506 !e774
        LOG_INFO("CLI \"stdio\" Opened on the UART" TO_STRING(STDIO_UART_NUM)
                 ", Default Src on Idx: %d", (int)CMD_STDIO_FORWARD_SRC
                );
        Cmd_ForwardToIdx((unsigned)CMD_STDIO_FORWARD_SRC, cli, 0, NULL); //lint !e534
    }
    else {
        LOG_INFO("CLI \"stdio\" Opened on the UART" TO_STRING(STDIO_UART_NUM));
    }
    return( 0 );
}


/**
 * CLI close the stdio device
 *
 * @param[in]  device   Pointer to device.
 * @param[in]  cli      Command Line Interface.
 *
 * @retval  (>= 0) Success
 * @retval  ( < 0) Failed
 */
static int stdio_DeviceClose(cli_device_t* device, int cli)
{
    UNUSED_VAR(device);
    Cmd_ForwardRemIF(cli);    //lint !e534  " Ignoring return value "

    LOG_ERROR("CLI \"stdio\" Closed on the UART" TO_STRING(STDIO_UART_NUM));
    return( 0 );
}


/**
 * CLI read data from stdio device
 *
 * @param[in]  device   Pointer to device.
 * @param[out] buff     Pointer to buffer for read
 * @param[in]  size     Number of bytes to read
 *
 * @retval  (>= 0) Success, number of bytes read is returned
 * @retval  ( < 0) Failed, exception of device operate
 */
static int stdio_DeviceRead(cli_device_t* device, void* buff, int size)
{
    UNUSED_VAR(device);  UNUSED_VAR(size);
    return( fread(buff, 1, 1, stdin) ); //lint !e713
}


/**
 * CLI write data to stdio device
 *
 * @param[in]  device   Pointer to device.
 * @param[in]  data     Pointer to buffer to write
 * @param[in]  size     Number of bytes to write
 *
 * @retval  (>= 0) Success, number of bytes written is returned
 * @retval  ( < 0) Failed, exception of device operate
 */
static int stdio_DeviceWrite(cli_device_t* device, const void* data, int size)
{
    UNUSED_VAR(device);
    return( fwrite(data, 1, size, stdout) ); //lint !e713 !e732
}


/**
 * Initialize the CLI application of stdio
 */
static void stdio_AppInit(void)
{
    static const cli_device_t  _cli_device = {
        stdio_DeviceOpen, stdio_DeviceClose, stdio_DeviceRead, stdio_DeviceWrite
    };
    int  flags = (  (CMD_STDIO_STR_EN ? CLI_EXEC_STR : 0)
                  | (CMD_STDIO_BIN_EN ? CLI_EXEC_BIN : 0)
                 );   //lint !e835

    if (flags == 0) { //lint !e774
        LOG_ASSERT("Invalid defined of CMD_STDIO_STR_EN or CMD_STDIO_BIN_EN");
    }
    if (cli_shell_new(&_cli_device, "stdio", flags) < 0) {
        LOG_ASSERT("cli_shell_new(&_cli_device, \"stdio\", flags)");
    }
}


#endif
/*----------------------------------------------------------------------------*/
/**
 * RTOS timer callback function
 */
static void Cmd_FlickTimerCB(void *arg)
{
    UNUSED_VAR(arg);

    //lint -save -w1
    CMD_PROMPT_LED_OFF();
    //lint -restore

    _Cmd_FlickSta =  0;
}


/**
 * 接收到命令提示回调函数
 *
 * @param[in]  cli    命令行接口
 * @param[in]  data   命令数据指针
 * @param[in]  size   命令数据大小, 以字节为单位
 *
 * @retval  (>= 0) 执行成功
 * @retval  ( < 0) 执行异常, 会导致命令行解析处理结束
 */
static int Cmd_FlickPromptCB(int cli, const void* data, int size)
{
    UNUSED_VAR(cli);  UNUSED_VAR(data);  UNUSED_VAR(size);

    if (_Cmd_FlickSta == 0)
    {
        _Cmd_FlickSta =  1;

        //lint -save -w1
        CMD_PROMPT_LED_ON();
        //lint -restore

        osTimerStart(_Cmd_FlickTmr, (50 * 1000) / osKernelGetTickFreq()); //lint !e534
    }

    return( 1 );
}


/**
 * 接收到命令提示(闪烁LED)初始化
 */
static void Cmd_FlickInit(void)
{
    //lint -save -w1
    CMD_PROMPT_LED_INIT();
    CMD_PROMPT_LED_OFF();
    //lint -restore

    _Cmd_FlickSta = 0;
    if((_Cmd_FlickTmr = osTimerNew(Cmd_FlickTimerCB, osTimerOnce, NULL, NULL)) == NULL) {
        LOG_ASSERT("osTimerNew(Cmd_FlickTimerCB, osTimerOnce, NULL, NULL)");
    }
    cli_prmt_reg(Cmd_FlickPromptCB); //lint !e534  " Ignoring return value "
}


/*----------------------------------------------------------------------------*/
#if (CMD_FORWARD_IF_NUM > 0)
static int Cmd_ForwardFind(unsigned idx, int cli);
#ifdef CMD_FWD_MODE_GET
/**
 * 数据转发模式查询 命令处理函数
 *
 * @retval  (>= 0) 命令执行成功, 返回命令字符串长度
 * @retval  ( < 0) 设备操作异常
 */
static int Cmd_ForwardModeG__(int cli, const void *data, int size)
{
    const char *ptr = data;
    int         len = strlen(CMD_FWD_MODE_GET); //lint !e713

    if (size != 0)
    {
        if((ptr  = cli_memmem(data, size, CMD_FWD_MODE_GET, len)) != NULL) //lint !e732
        {   ptr += len;
            if((ptr  = memchr(ptr, '\n', (int)data + size - (int)ptr)) != NULL) //lint !e732
            {   ptr += 1;
                goto EXEC_GET; //lint !e801
            }
        }
        return( 0 );
    }
    EXEC_GET :

    switch( cli_shell_flag(cli, 0, 0) & (CLI_EXEC_STR | CLI_EXEC_BIN) ) {
    case CLI_EXEC_STR | CLI_EXEC_BIN : len = cli_fputs("STR and BIN" ENDL, cli); break;
    case CLI_EXEC_STR                : len = cli_fputs("STR"         ENDL, cli); break;
    case CLI_EXEC_BIN                : len = cli_fputs(        "BIN" ENDL, cli); break;
    default                          : len = cli_fputs(   "No Any"   ENDL, cli); break;
    }
    if (len < 0) {
        return( len );
    }
    return( (int)ptr - (int)data );
}

static int Cmd_ForwardModeGet(int cli, const char* args)
{
    return( Cmd_ForwardModeG__(cli, args, 0) );
}
#endif // CMD_FWD_MODE_GET


#ifdef CMD_FWD_MODE_STR
/**
 * 数据转发模式设置为字符串 命令处理函数
 *
 * @retval  (>= 0) 命令执行成功, 返回命令字符串长度
 * @retval  ( < 0) 设备操作异常
 */
static int Cmd_ForwardModeS__(int cli, const void *data, int size)
{
    int         dst;
    const char *ptr = data;
    int         len = strlen(CMD_FWD_MODE_STR); //lint !e713

    if (size != 0)
    {
        if((ptr  = cli_memmem(data, size, CMD_FWD_MODE_STR, len)) != NULL) //lint !e732
        {   ptr += len;
            if((ptr  = memchr(ptr, '\n', (int)data + size - (int)ptr)) != NULL) //lint !e732
            {   ptr += 1;
                goto EXEC_STR; //lint !e801
            }
        }
        return( 0 );
    }
    EXEC_STR:

    cli_shell_flag(cli, CLI_EXEC_STR, CLI_EXEC_BIN); //lint !e534
    if((dst = Cmd_ForwardFind(0xFFFF, cli)) == CLI_INVALID) {
        return( 0 );    // 查找转发目的接口失败
    }
    cli_shell_flag(dst, CLI_EXEC_STR, CLI_EXEC_BIN); //lint !e534

    len = (int)ptr - (int)data;
    if (cli_fwrite(data, len, 1, dst) != 1) { //lint !e732
        return( 0 );    // 设备操作异常
    }
    if (cli_fflush(dst) < 0) {
        return( -1 );   // 设备操作异常
    }
    LOG_VERB("%s Forward to %s: %d", cli_shell_name(cli), cli_shell_name(dst), len);
    return( len );
}

static int Cmd_ForwardModeStr(int cli, const char *args)
{
    return( Cmd_ForwardModeS__(cli, args, 0) );
}
#endif // CMD_FWD_MODE_STR


#ifdef CMD_FWD_MODE_BIN
/**
 * 数据转发模式设置为二进制 命令处理函数
 *
 * @retval  (>= 0) 命令执行成功, 返回命令字符串长度
 * @retval  ( < 0) 设备操作异常
 */
static int Cmd_ForwardModeB__(int cli, const void *data, int size)
{
    int         dst;
    const char *ptr = data;
    int         len = strlen(CMD_FWD_MODE_BIN); //lint !e713

    if (size != 0)
    {
        if((ptr  = cli_memmem(data, size, CMD_FWD_MODE_BIN, len)) != NULL) //lint !e732
        {   ptr += len;
            if((ptr  = memchr(ptr, '\n', (int)data + size - (int)ptr)) != NULL) //lint !e732
            {   ptr += 1;
                goto EXEC_BIN; //lint !e801
            }
        }
        return( 0 );
    }
    EXEC_BIN:

    cli_shell_flag(cli, CLI_EXEC_BIN, CLI_EXEC_STR); //lint !e534
    if((dst = Cmd_ForwardFind(0xFFFF, cli)) == CLI_INVALID) {
        return( 0 );    // 查找转发目的接口失败
    }
    cli_shell_flag(dst, CLI_EXEC_BIN, CLI_EXEC_STR); //lint !e534

    len = (int)ptr - (int)data;
    if (cli_fwrite(data, len, 1, dst) != 1) { //lint !e732
        return( 0 );    // 设备操作异常
    }
    if (cli_fflush(dst) < 0) {
        return( -1 );   // 设备操作异常
    }
    LOG_VERB("%s Forward to %s: %d", cli_shell_name(cli), cli_shell_name(dst), len);
    return( len );
}

static int Cmd_ForwardModeBin(int cli, const char *args)
{
    return( Cmd_ForwardModeB__(cli, args, 0) );
}
#endif // CMD_FWD_MODE_BIN


/**
 * 二进制数据 转发到默认接口 或 转发返回消息
 *
 * @param[in]  cli   命令行接口(转发源接口)
 * @param[in]  data  数据指针
 * @param[in]  size  数据长度
 *
 * @retval  ( > 0) 成功, 返回已转发的数据长度
 * @retval  (== 0) 失败, 无法转发
 * @retval  ( < 0) 设备操作异常
 */
static int Cmd_ForwardBin(int cli, const void *data, int size)
{
    int             tmp;
    int             sum = 0;
    int             len = size;
    const uint8_t  *ptr = data;
    int             dst = CLI_INVALID;

#ifdef  CMD_BIN_PARSER
    if((tmp = CMD_BIN_PARSER(cli, ptr, len)) < 0) {
        return( tmp );          // 设备操作异常
    }
    if((sum += tmp, ptr += tmp, len -= tmp) <= 0) {
        return( sum );          // 已处理所有数据
    }
#endif

#ifdef  CMD_FORWARD_HOOK
    if((tmp = CMD_FORWARD_HOOK(cli, ptr, len)) < 0) {
        return( tmp );          // 设备操作异常
    }
    if((sum += tmp, ptr += tmp, len -= tmp) <= 0) {
        return( sum );          // 已处理所有数据
    }
#endif

#if defined(CMD_FWD_BIN1_HEAD) && defined(CMD_FWD_BIN1_TAIL) \
 || defined(CMD_FWD_BIN2_HEAD) && defined(CMD_FWD_BIN2_TAIL)
    while(1)                    //lint !e716
    {
        const uint8_t  *cur;

    #if defined(CMD_FWD_BIN1_HEAD) && defined(CMD_FWD_BIN1_TAIL)
        do {
            static const uint8_t  head[] = { CMD_FWD_BIN1_HEAD };
            static const uint8_t  tail[] = { CMD_FWD_BIN1_TAIL };

            if (len < ((int)sizeof(head) + (int)sizeof(tail))) {
                break;          // 长度不够
            }
            if (NULL == (cur = cli_memmem(ptr, len            , head, sizeof(head)))) { //lint !e732
                break;          // 未找到头
            }
            cur += sizeof(head);
            if (NULL == (cur = cli_memmem(cur, ptr + len - cur, tail, sizeof(tail)))) { //lint !e732
                break;          // 未找到尾
            }
            cur += sizeof(tail);
            goto FORWARD_BIN;   //lint !e801
        } while(0);             //lint !e717
    #endif

    #if defined(CMD_FWD_BIN2_HEAD) && defined(CMD_FWD_BIN2_TAIL)
    #endif

        break;
        FORWARD_BIN:

        if (dst == CLI_INVALID) {
            if((dst = Cmd_ForwardFind(0xFFFF, cli)) == CLI_INVALID) {
                break;          // 查找转发目的接口失败
            }
        }
        tmp  = cur - ptr;

        LOG_VERB("%s Forward BIN to %s: %d", cli_shell_name(cli), cli_shell_name(dst), tmp);
        if (cli_fwrite(ptr, tmp, 1, dst) != 1) {   //lint !e732
            return( 0 );        // 设备操作异常
        }
        if (cli_fflush(dst) < 0) {
            return( -1 );       // 设备操作异常
        }
        if((sum += tmp, ptr += tmp, len -= tmp) <= 0) {
            return( sum );      // 已处理所有数据
        }
    } // while (1)
#endif

#if defined(CMD_FWD_MODE_STR) || defined(CMD_FWD_MODE_BIN) || defined(CMD_FWD_MODE_GET)
    if(!(cli_shell_flag(cli, 0, 0) & CLI_NO_SEARCH))
    {   // 非不执行任何命令(非不搜索命令执行函数)
    #ifdef CMD_FWD_MODE_STR
        if((tmp = Cmd_ForwardModeS__(cli, ptr, len)) < 0) {
            return( tmp );      // 设备操作异常
        }
        if((sum += tmp, ptr += tmp, len -= tmp) <= 0) {
            return( sum );      // 已处理所有数据
        }
    #endif

    #ifdef CMD_FWD_MODE_BIN
        if((tmp = Cmd_ForwardModeB__(cli, ptr, len)) < 0) {
            return( tmp );      // 设备操作异常
        }
        if((sum += tmp, ptr += tmp, len -= tmp) <= 0) {
            return( sum );      // 已处理所有数据
        }
    #endif

    #ifdef CMD_FWD_MODE_GET
        if((tmp = Cmd_ForwardModeG__(cli, ptr, len)) < 0) {
            return( tmp );      // 设备操作异常
        }
        if((sum += tmp, ptr += tmp, len -= tmp) <= 0) {
            return( sum );      // 已处理所有数据
        }
    #endif
    }
#endif

    UNUSED_VAR(ptr);
    return( sum );
}


/**
 * 命令字符串 转发到默认接口 或 转发返回消息
 *
 * @retval  ( > 0) 成功, 返回转发的命令字符数
 * @retval  (== 0) 失败, 无法转发
 * @retval  ( < 0) 设备操作异常
 */
static int Cmd_ForwardCmd(int cli, const char* args)
{
    int         dst;
    int         tmp = 0;
    int         len = strlen(args); //lint !e713
    const char *ptr =        args ;

#ifdef  CMD_FORWARD_HOOK
    if((tmp = CMD_FORWARD_HOOK(cli, ptr, len)) < 0) { //lint !e838
        return( tmp );  // 设备操作异常
    }
//  if (tmp >= len) {
//      return( tmp );  // 已处理所有数据
//  }
    if((ptr += tmp, len -= tmp) <= 0) {
        return( tmp );  // 已处理所有数据
    }
#endif

    if((dst = Cmd_ForwardFind(0xFFFF, cli)) == CLI_INVALID) {
        return( tmp );  // 查找转发目的接口失败
    }
    LOG_VERB("%s Forward STR to %s: %d", cli_shell_name(cli), cli_shell_name(dst), len);
    if (cli_fwrite(ptr, len, 1, dst) != 1) { //lint !e732
        return( 0 );    // 设备操作异常
    }
    if (cli_fflush(dst) < 0) {
        return( -1 );   // 设备操作异常
    }
    return( len + tmp ); //lint !e845
}


/**
 * 查找转发目的接口
 *
 * @param[in]  idx  转发到的接口索引, (0xFFFF)为自动转发
 * @param[in]  cli  转发的源接口
 *
 * @retval  (!= CLI_INVALID) 查找到的 转发目的接口
 * @retval  (== CLI_INVALID) 查找失败
 */
static int Cmd_ForwardFind(unsigned idx, int cli)
{
    int   dst = CLI_INVALID;

    OS_MUTEX_ENTER(_Cmd_ForwardMut, OS_WAIT_FOREVER)
    {
        if (idx >= ARRAY_SIZE(_Cmd_ForwardDst))
        {   //------------------------------------ 自动转发 ---------------------
            for(size_t i = 0;  i < ARRAY_SIZE(_Cmd_ForwardDst);  i++)
            {
                if (cli == _Cmd_ForwardDst[i]) {
                    dst  = _Cmd_ForwardSrc[i];  // 返回源接口 ------- 是转发返回消息
                    goto FIND_END;  //lint !e801
                }
            }
            idx = 0;                            // 返回(0索引)接口 -- 非转发返回消息
        }
        {  //------------------------------------- 转发到指定接口 ---------------
            if (cli != _Cmd_ForwardDst[idx]) {
                _Cmd_ForwardSrc[idx]  = cli;    // 保存源接口, 用于转发返回消息
                dst  = _Cmd_ForwardDst[idx];    // 返回(指定索引)接口
            }
        }
        FIND_END:   UNUSED_VAR(dst);
    }
    OS_MUTEX_LEAVE(_Cmd_ForwardMut, OS_MUTEX_TIMEOUT_HANDLER)

    return( dst );
}
#endif // CMD_FORWARD_IF_NUM


/**
 * 命令手动转发(注意:未注册命令会自动转发到默认转发接口)
 */
int Cmd_ForwardToIdx(unsigned idx, int cli, int argc, const char * const argv[])
{
#if (CMD_FORWARD_IF_NUM <= 0)
    UNUSED_VAR(idx);  UNUSED_VAR(cli);  UNUSED_VAR(argc);  UNUSED_VAR(argv);
    return( 0 );
#else
    int       num;
    unsigned  max = idx + 1;

    if((argc > 0) && (argv == NULL)) {
        return( 0 );                        // 参数错误
    }
    if (idx >= ARRAY_SIZE(_Cmd_ForwardDst)) {
        max  = ARRAY_SIZE(_Cmd_ForwardDst); // 转发到所有
        idx  = 0;
    }
    for(num = 0;  idx < max;  idx++) //lint !e443
    {
        int dst, len, tmp, i;

        if((dst = Cmd_ForwardFind(idx, cli)) == CLI_INVALID) {
            continue;                       // 目的接口无效
        }
        for(i = len = 0;  i < argc;  len += tmp, i++)
        {   // 输出命令字符串
            LOG_VERB("%s Forward STR to %s: %d", cli_shell_name(cli), cli_shell_name(dst), strlen(argv[i]));
            if((tmp = cli_fputs(argv[i], dst)) < 0) {
                return( tmp );              // 设备操作异常
            }
        }
        num += len;
    }
    return( num );
#endif
}


//lint -esym(522, Cmd_ForwardInit)
/**
 * 命令转发初始化
 */
static void Cmd_ForwardInit(void)
{
#if (CMD_FORWARD_IF_NUM > 0)
    for(size_t i = 0;  i < ARRAY_SIZE(_Cmd_ForwardDst);  i++)
    {
        _Cmd_ForwardDst[i] = CLI_INVALID;
        _Cmd_ForwardSrc[i] = CLI_INVALID;
    }
    OS_MUTEX_CREATE(_Cmd_ForwardMut, "Cmd_Forward mutex");
#endif

#ifdef CMD_BIN_PARSER
    cli_bin_reg(CMD_BIN_PARSER);    //lint !e534
    LOG_INFO("Load the Module of Binary Command Parser");
#endif

#if (CMD_FORWARD_IF_NUM > 0)
    cli_ufd_reg(Cmd_ForwardCmd);    //lint !e534
    LOG_INFO("Load the Module of String Data Forward");

#ifdef CMD_FWD_MODE_GET
    cli_cmd_reg(CMD_FWD_MODE_GET, Cmd_ForwardModeGet, "Get Forward Mode (STR/BIN), Case-Sensitive" ); //lint !e534
#endif

#ifdef CMD_FWD_MODE_STR
    cli_cmd_reg(CMD_FWD_MODE_STR, Cmd_ForwardModeStr, "Set Forward Mode to STR, Case-Sensitive"); //lint !e534
#endif

#ifdef CMD_FWD_MODE_BIN
    cli_cmd_reg(CMD_FWD_MODE_BIN, Cmd_ForwardModeBin, "Set Forward Mode to BIN, Case-Sensitive"); //lint !e534
    cli_bin_reg(Cmd_ForwardBin);        //lint !e534
    LOG_INFO("Load the Module of Binary Data Forward");
#endif
#endif // CMD_FORWARD_IF_NUM
}


/**
 * 移去命令转发接口(所有索引)
 */
int Cmd_ForwardRemIF(int cli)
{
    int  retval = -1;    UNUSED_VAR(cli);

#if (CMD_FORWARD_IF_NUM > 0)
    OS_MUTEX_ENTER(_Cmd_ForwardMut, OS_WAIT_FOREVER)
    {
        for(size_t i = 0;  i < ARRAY_SIZE(_Cmd_ForwardDst);  i++)
        {
            if (_Cmd_ForwardDst[i] == cli) {
                _Cmd_ForwardDst[i] =  CLI_INVALID;
                retval = 0;
            }
            if (_Cmd_ForwardSrc[i] == cli) {
                _Cmd_ForwardSrc[i] =  CLI_INVALID;
                retval = 0;
            }
        }
    }
    OS_MUTEX_LEAVE(_Cmd_ForwardMut, OS_MUTEX_TIMEOUT_HANDLER)
#endif

    return( retval );
}


/**
 * 设置命令转发接口(指定索引)
 */
int Cmd_ForwardSetIF(unsigned idx, int cli)
{
    int  retval = -1;    UNUSED_VAR(idx);  UNUSED_VAR(cli);

#if (CMD_FORWARD_IF_NUM > 0)
    if (idx < ARRAY_SIZE(_Cmd_ForwardDst))
    {
        OS_MUTEX_ENTER(_Cmd_ForwardMut, OS_WAIT_FOREVER)
        {
            _Cmd_ForwardDst[idx] = cli;
            retval = 0;
        }
        OS_MUTEX_LEAVE(_Cmd_ForwardMut, OS_MUTEX_TIMEOUT_HANDLER)
    }
#endif

    return( retval );
}


/**
 * 获取命令转发接口(指定索引)
 */
int Cmd_ForwardGetIF(unsigned idx)
{
    int   cli = CLI_INVALID;    UNUSED_VAR(idx);

#if (CMD_FORWARD_IF_NUM > 0)
    if (idx < ARRAY_SIZE(_Cmd_ForwardDst))
    {
        OS_MUTEX_ENTER(_Cmd_ForwardMut, OS_WAIT_FOREVER)
        {
            cli = _Cmd_ForwardDst[idx];
        }
        OS_MUTEX_LEAVE(_Cmd_ForwardMut, OS_MUTEX_TIMEOUT_HANDLER)
    }
#endif

    return( cli );
}


/*----------------------------------------------------------------------------*/
/**
 * 重启 命令处理函数
 */
__WEAK int Cmd_Reboot(int cli, const char* args)
{
    Cmd_ForwardToIdx(0xFFFF, cli, 1, &args); //lint !e534
    DelayMS(200);
    Board_Reset();  // System Reset
    return( 0 );
}


/**
 * SCPI *WAI 命令处理函数
 */
__WEAK int Cmd_Wait(int cli,  const char* args)
{
    float       sec;
    uint32_t    msec;   UNUSED_VAR(cli);

    if (sscanf(args, "%*s %f", &sec) != 1) {
        sec = 0;        // 无等待时间参数
    }
    if (sec < 0) {
        return( 0 );    // 无效的参数
    }
    if((msec = sec * 1000 + 0.5f) == 0) { //lint !e524 !e732
        return( 0 );
    }
    msec += GetSysMsec();
    LOG_VERB("Wait command start");

    //

    DelayTO(msec);
    LOG_VERB("Wait command end");
    return( 0 );
}


/**
 * SCPI *IDN? 命令处理函数
 */
__WEAK int Cmd_IdnGet(int cli, const char* args)
{
    const char *idn;
    char       *ptr;
    int         retval;   UNUSED_VAR(args);

    if ((ptr = malloc(128)) == NULL) {
        return( 1 );
    }
    if((idn = rtcfg_string_read(&_Cmd_SysIdn_Item, ptr, 128)) != NULL) {
        retval = cli_fprintf(cli, "%s"ENDL, idn);
    } else {
        retval = 0; // 命令执行成功
    }
    free(ptr);
    return( (retval < 0) ? -1 : 0 );
}


/**
 * 设置 IDN 命令处理函数
 */
__WEAK int Cmd_IdnSet(int cli, const char* args)
{
    char       *ptr;
    const char *idn;
    char        buff[8];
    int         retval = 0;

    if ((ptr = malloc(128)) == NULL) {
        return( 1 );
    }
    for(;;)
    {
        if (sscanf(args, "%*s %127s %7s", ptr, buff) != 2) {
            retval = 1;   break;    // 命令格式错误
        }
        if (cli_strtrimcmp(buff, "pss") != 0) {
            retval = 1;   break;    // 命令格式错误
        }
        if (rtcfg_string_write(&_Cmd_SysIdn_Item, ptr) != 0) {
            retval = 1;   break;    // 保存配置异常
        }
        if((idn = rtcfg_string_read(&_Cmd_SysIdn_Item, ptr, 128)) == NULL) {
            retval = 1;   break;    // 保存配置异常
        }
        if (cli_fechof(cli, "Set IDN: %s  OK!"ENDL, idn) < 0) {
            retval = -1;  break;    // 设备操作异常
        }
        break;
    }
    free(ptr);
    return( retval );
}


/**
 * 显示详细版本信息 命令处理函数
 */
__WEAK int Cmd_Version(int cli, const char* args)
{
    UNUSED_VAR(cli);  UNUSED_VAR(args);
    return( 0 );
}


/**
 * 显示软件版本 命令处理函数
 */
__WEAK int Cmd_FwVer(int cli, const char* args)
{
    UNUSED_VAR(cli);  UNUSED_VAR(args);
    return( 0 );
}


/**
 * 显示硬件版本 命令处理函数
 */
__WEAK int Cmd_HwVer(int cli, const char* args)
{
    UNUSED_VAR(cli);  UNUSED_VAR(args);
    return( 0 );
}


/**
 * 开启回显 命令处理函数
 */
__WEAK int Cmd_EchoOn(int cli, const char* args)
{
    char    buff[8];

    if (sscanf(args, "%*s %7s", buff) != 1) {
        return( 1 );
    }
    if (cli_strtrimcmp(buff, "pss") != 0) {
        return( 1 );
    }
    cli_shell_flag(cli, CLI_ECHO_ON, 0); //lint !e534
    return( 0 );
}


/**
 * 关闭回显 命令处理函数
 */
__WEAK int Cmd_EchoOff(int cli, const char* args)
{
    char    buff[8];

    if (sscanf(args, "%*s %7s", buff) != 1) {
        return( 1 );
    }
    if (cli_strtrimcmp(buff, "pss") != 0) {
        return( 1 );
    }
    cli_shell_flag(cli, 0, CLI_ECHO_ON); //lint !e534
    return( 0 );
}


#ifdef CMD_NAME_MEM_INFO
/**
 * 显示系统内存信息 命令处理函数
 */
__WEAK int Cmd_MemInfo(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);    UNUSED_VAR(args);

#if defined(__CC_ARM) || defined(__ARMCC_VERSION) //--- ARM Compiler -----------
//  cli_fprintf(cli, "--------------- std heap stats ---------------"ENDL);
//  __heapstats((int (*)(void *, char const *, ...))cli_fprintf, (void *)cli);
//  __heapvalid((int (*)(void *, char const *, ...))cli_fprintf, (void *)cli, 1);

    //lint -save -w1
    CMD_MEM_INFO_ISR;
    CMD_MEM_INFO_OS ;
    CMD_MEM_INFO_NET;
    CMD_MEM_INFO_STD;
    //lint -restore
    return( 0 );

#elif defined(__ICCARM__)   //------------------------- IAR Compiler -----------
#error "not supported compiler"
#elif defined(__GNUC__)     //------------------------- GNU Compiler -----------
#error "not supported compiler"
#else                       //------------------------- Unknow Compiler --------
#error "not supported compiler"
#endif
}
#endif


/**
 * 显示所有命令帮助信息; 密码错误掩藏 "**" 开头命令.
 *
 * @param[in]  cli    命令行接口
 * @param[in]  args   命令字符串, 包括命令名, 包括命令结束符('\n')
 *
 * @retval  (>= 0) 命令执行成功
 * @retval  ( < 0) 设备操作异常
 */
__WEAK int Cmd_Help(int cli, const char* args)
{
    char        buff[8];

    if (sscanf(args, "%*s %7s", buff) != 1) {
        buff[0] = '\0';                     // 命令格式错误
    }
    if (cli_strtrimcmp(buff, "pss") == 0) { // 显示(特殊命令帮助信息)
        return( cli_cmd_help(cli, CMD_NAME_HELP " 0\n") );
    }
    else {                                  // 掩藏(特殊命令帮助信息)
        return( cli_cmd_help(cli, CMD_NAME_HELP " 1\n") );
    }
}


/**
 * 内存读; **PSS:MEM:ACC? <Addr>
 */
static int Cmd_MemGet(int cli, const char* args)
{
    uint32_t   *ptr;
    UNUSED_VAR(cli);

    if (sscanf(args, "%*s %p", &ptr) != 1) {
        return( 1 );
    }
    cli_fprintf(cli, "0x%08X"ENDL, *ptr); //lint !e534
    return( 0 );
}


/**
 * 内存写; **PSS:MEM:ACC <Addr>,<Val>
 */
static int Cmd_MemSet(int cli, const char* args)
{
    uint32_t   *ptr;
    uint32_t    val;
    UNUSED_VAR(cli);

    if (sscanf(args, "%*s %p,%u", &ptr, &val) != 2) {
        return( 1 );
    }
    *ptr = val;
    return( 0 );
}


/*----------------------------------------------------------------------------*/
/**
 * 初始化命令接口应用
 */
int Cmd_AppInit(void)
{   //lint --e{534}  " Ignoring return value "
    LOG_INFO("Loading Command Line Interface Module");
    cli_shell_init();                       // Initialize Command line interface
    Cmd_FlickInit();                        // Initialize Command status LED

    {
        cli_cmd_reg(CMD_NAME_HELP     , Cmd_Help    , "Lists all commands"      );
        cli_cmd_reg(CMD_NAME_REBOOT   , Cmd_Reboot  , "Reboot system"           );
        cli_cmd_reg(CMD_NAME_WAIT     , Cmd_Wait    , "Wait-to-Continue"        );
        cli_cmd_reg(CMD_NAME_ECHO_ON  , Cmd_EchoOn  , "Turn command echoing on" );
        cli_cmd_reg(CMD_NAME_ECHO_OFF , Cmd_EchoOff , "Turn command echoing off");
        cli_cmd_reg(CMD_NAME_IDN_GET  , Cmd_IdnGet  , "The serial number Query" );
        cli_cmd_reg(CMD_NAME_IDN_SET  , Cmd_IdnSet  , "The serial number Config");
    #ifdef CMD_NAME_MEM_INFO
        cli_cmd_reg(CMD_NAME_MEM_INFO , Cmd_MemInfo , "Show system memory info" );
    #endif
        cli_cmd_reg("**PSS:MEM:ACC?"  , Cmd_MemGet  , "Get memory Addr Val"     );
        cli_cmd_reg("**PSS:MEM:ACC"   , Cmd_MemSet  , "Set memory Addr Val"     );
    }
    if (strcmp(_Cmd_SysIdn_Item.dft, RTCFG_DFT_SYS_IDN))
    {
        cli_cmd_reg(CMD_NAME_SHOW_VER , Cmd_Version , "Show version information");
        cli_cmd_reg(CMD_NAME_FW_VER   , Cmd_FwVer   , "Show firmware version"   );
        cli_cmd_reg(CMD_NAME_HW_VER   , Cmd_HwVer   , "Show hardware version"   );
    }
    net_cmd_init();                         // Initialize shell command of network

    Cmd_ForwardInit();                      // Initialize Command forward
#if (CMD_STDIO_CLI_EN != 0)
    stdio_AppInit();                        // Initialize the CLI application of stdio
#endif

    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

