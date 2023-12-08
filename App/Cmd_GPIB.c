/*******************************************************************************
 * @file    Cmd_GPIB.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/10/20
 * @brief   Command line interface on GPIB.
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
 *      2020/9/21 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -esym(534, cli_fprintf, cli_fechof, cli_cmd_reg)

#include "Cmd_GPIB.h"
#ifdef    APP_CFG
#include  APP_CFG               // Apps configuration
#endif
#define   LOG_TAG   "Cmd_GPIB"
#include "emb_log.h"
#include "emb_delay.h"
#include "emb_rtcfg.h"
#include "shell/cli_shell.h"

#include "Drv_TNT4882.h"

#include "cmsis_os2.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "defines.h"                // Used: UNUSED_VAR(), ARRAY_SIZE()


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup              Cmd_GPIB
 * @{
 ******************************************************************************/

#ifndef CLI_LINE_BUFF_SIZE
#define CLI_LINE_BUFF_SIZE            128           //!< (C) 命令行输入缓存大小(以字节为单位)
#endif
#ifndef CLI_OUT_BUFF_SIZE
#define CLI_OUT_BUFF_SIZE             128           //!< (C) 命令行输出缓存大小(以字节为单位)
#endif

#ifndef GPIB_DRV_THREAD_STKB
#define GPIB_DRV_THREAD_STKB          1024          //!< (C) GPIB 驱动线程栈大小(以字节为单位)
#endif
#ifndef GPIB_DRV_THREAD_PRIO
#define GPIB_DRV_THREAD_PRIO  osPriorityAboveNormal //!< (C) GPIB 驱动线程优先级
#endif

#ifndef GPIB_DRV_TIMEOUT
#define GPIB_DRV_TIMEOUT           (2 * 1000)       //!< (C) GPBI 驱动等待超时(以毫秒为单位)
#endif

#ifndef RTCFG_ITME_GPIB_ADDR
#define RTCFG_ITME_GPIB_ADDR     {"GPIB:Addr", 8}   //!< (C) GPIB地址 配置项
#endif

#ifndef ENDL
#define ENDL                         "\r\n"         //!< (C) End of line, start the new line
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Cmd_GPIB
 * @{
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

static osSemaphoreId_t      _GPIB_rxSem;      // Event of Listen End
static osSemaphoreId_t      _GPIB_txSem;      // Event of Talk Enable
static osMutexId_t          _GPIB_txMut;
static volatile uint16_t    _GPIB_txLen = 0;
static uint8_t              _GPIB_txBuf[CLI_OUT_BUFF_SIZE];

static const rtcfg_int32_t  _GPIB_AddrCfg = RTCFG_ITME_GPIB_ADDR;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * Read the GPIB address from DIP switch and config item.
 */
static uint8_t GPIB_AddrRead(void)
{
    uint8_t     addr;

    if((addr = GPIB_AddrGet()) > 30) {
        addr = rtcfg_int32_read(&_GPIB_AddrCfg);  //lint !e734
    }
    return( addr );
}


/**
 * Callback Function for Event of Listen End
 */
static void GPIB_ListenEnd(void)
{
//  if (osSemaphoreGetCount(_GPIB_rxSem) != 0) {
//      return;
//  }
    osSemaphoreRelease(_GPIB_rxSem); //lint !e534
}


/**
 * Callback Function for Event of Talk Enable
 */
static void GPIB_TalkEnable(void)
{
//  if (osSemaphoreGetCount(_GPIB_txSem) != 0) {
//      return;
//  }
    osSemaphoreRelease(_GPIB_txSem); //lint !e534
}


/**
 * Thread of GPIB driver
 */
static void GPIB_DrvThread(void *arg)
{
    unsigned    timeout;
    UNUSED_VAR(arg);

    for(;;)
    {
        if (osSemaphoreAcquire(_GPIB_txSem, osWaitForever) != osOK) {
            continue;                   // Waitting for Event of Talk Enable
        }
        while (osSemaphoreAcquire(_GPIB_txSem, 0) == osOK) ;
        LOG_DEBUG("GPIB driver enable to Send");

        if (_GPIB_txLen != 0) {
            DelayMS(3);                // 防止: 返回前一条指令的返回值
        }
        for(timeout = GPIB_DRV_TIMEOUT;  timeout > 0;  timeout--) {
            if(_GPIB_txLen != 0) break; // Waitting for transmit data
            DelayMS(2);
        }

        osMutexAcquire(_GPIB_txMut, osWaitForever); //lint !e534
        {
            LOG_DEBUG("GPIB driver begin to Send length: %d", (int)_GPIB_txLen);
            if (TNT4882_SendBegin(_GPIB_txBuf, _GPIB_txLen) >= 0) { // Begin Send
                TNT4882_SendWait();  //lint !e534   // Waitted for Send completed
                LOG_DEBUG("GPIB driver end of Send length: %d", (int)_GPIB_txLen);
            } else {
                LOG_DEBUG("GPIB driver Failed of Begin Send Data");
            }
            _GPIB_txLen = 0;                        // Clear _GPIB_txBuf
        }
        osMutexRelease(_GPIB_txMut); //lint !e534
    }
}


/*----------------------------------------------------------------------------*/
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
    LOG_DEBUG("CLI begin to write: 0x%08p - %d", data, size);

    if((size > (int)ARRAY_SIZE(_GPIB_txBuf)) || (size <= 0)) {
        LOG_INFO("_GPIB_txBuf too small");
        return( size );
    }

    if (osMutexAcquire(_GPIB_txMut, GPIB_DRV_TIMEOUT) == osOK) {
        memcpy(_GPIB_txBuf, data, size); //lint !e732   // Copy data to txBuf
        _GPIB_txLen = size;              //lint !e734
        osMutexRelease(_GPIB_txMut);     //lint !e534
        LOG_DEBUG("CLI end of write: 0x%08p - %d", data, size);
    } else {
        LOG_INFO("GPIB Write timeout");
    }
    return( size );
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
    int   num;  UNUSED_VAR(device);
    LOG_DEBUG("CLI begin to read: 0x%08p - %d", buff, size);

    if (size < (CLI_LINE_BUFF_SIZE - 2)) {
        LOG_INFO("Packet cannot be received at one time");
        return( 0 );
    }
    while (osSemaphoreAcquire(_GPIB_rxSem, 0) == osOK) ;
    if (TNT4882_RecvBegin(buff, size) < 0){ // Begin Receive
        LOG_INFO("Failed to TNT4882 begin receive");
        return( 0 );
    }
    if (osSemaphoreAcquire(_GPIB_rxSem, osWaitForever) != osOK) {
        return( 0 );                        // Waitting for Event of Listen End
    }
    if((num = TNT4882_RecvWait()) < 0) {    // Waitting for Receive completed
        LOG_INFO("Failed to TNT4882 receive completed");
        return( 0 );
    }
    if (num > 0) {
        osMutexAcquire(_GPIB_txMut, osWaitForever); //lint !e534
        _GPIB_txLen = 0;                    // Clear _GPIB_txBuf
        osMutexRelease(_GPIB_txMut);                //lint !e534
    }
    LOG_DEBUG("CLI end of read: 0x%08p - %d", buff, num);
    return( num );
}


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
    LOG_INFO("Command Line Interface open the GPIB, Addr: %u", (unsigned)GPIB_AddrRead());
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
    UNUSED_VAR(device);  UNUSED_VAR(cli);
    LOG_ASSERT("ERROR: CLI Device Closed");
    return( 0 );
}


/*----------------------------------------------------------------------------*/
/**
 * The device GPIB address configuration
 */
static int Cmd_GPIBAddrCfg(int cli, const char* args)
{
    int32_t  addr;

    if (sscanf(args, "%*s %li", &addr) != 1) {  //lint !e706
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    else {
        rtcfg_int32_write(&_GPIB_AddrCfg, addr); //lint !e534
        cli_fechof(cli, "GPIB address config to %li"ENDL, addr);
        return( 0 );
    }
}


/**
 * The device GPIB address display
 */
static int Cmd_GPIBAddrGet(int cli, const char* args)
{
    UNUSED_VAR(args);

    cli_fprintf(cli, "%li"ENDL, rtcfg_int32_read(&_GPIB_AddrCfg));
    return( 0 );
}


/**
 * Initialize Command Line Interface on GPIB
 */
void Cmd_GPIBInit(void)
{
    static const osMutexAttr_t _GPIB_MutAttr = {
        "GPIB_txMut", osMutexRecursive | osMutexPrioInherit, NULL, NULL
    };
    static const osThreadAttr_t  _GPIB_DrvThreadAttr = {
        "GPIB_DrvThread", 0,  NULL, 0, NULL, GPIB_DRV_THREAD_STKB, GPIB_DRV_THREAD_PRIO, 0, 0
    };
    static const cli_device_t  _GPIB_device = {
        CLI_DeviceOpen, CLI_DeviceClose, CLI_DeviceRead, CLI_DeviceWrite
    };
    GPIB_AddrInit();

    if((_GPIB_rxSem = osSemaphoreNew(1, 0, NULL)) == NULL) {
        LOG_ASSERT("ERROR: osSemaphoreNew(1, 0, NULL)");
    }
    if((_GPIB_txSem = osSemaphoreNew(1, 0, NULL)) == NULL) {
        LOG_ASSERT("ERROR: osSemaphoreNew(1, 0, NULL)");
    }
    if((_GPIB_txMut = osMutexNew(&_GPIB_MutAttr)) == NULL) {
        LOG_ASSERT("ERROR: osMutexNew(&_GPIB_MutAttr)");
    }
    if (osThreadNew(GPIB_DrvThread, NULL, &_GPIB_DrvThreadAttr) == NULL) {
        LOG_ASSERT("osThreadNew(GPIB_DrvThread, NULL, &_GPIB_DrvThreadAttr)");
    }
    cli_cmd_reg(":SYST:GPIB:ADDR?", Cmd_GPIBAddrGet, "The GPIB address Query" );
    cli_cmd_reg(":SYST:GPIB:ADDR" , Cmd_GPIBAddrCfg, "The GPIB address Config");

    if (TNT4882_Init(GPIB_AddrRead()) == 0)
    {
        TNT4882_EventFunReg(GPIB_ListenEnd, GPIB_TalkEnable);

        if (cli_shell_new(&_GPIB_device, "GPIB", CLI_EXEC_STR) < 0) {
            LOG_ASSERT("ERROR: cli_shell_new(&_GPIB_device, \"GPIB\", CMD_CLI_STR_EN)");
        }
    }
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

