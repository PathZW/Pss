/*******************************************************************************
 * @file    version.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/8/10
 * @brief   Version information definition.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/8/10 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro((835), VER_BL_HARDWARE)
//lint -emacro((845), VER_BL_HARDWARE)
//lint -emacro((835), _AT_VER_FIRMWARE)
//lint -emacro((845), _AT_VER_FIRMWARE)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Version"
#include "emb_log.h"
#define  ONLY_USED_REGS
#include "board.h"              // Used: ROM_MAIN_ADDR, ROM_RESV_SIZE

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "defines.h"

#include "emb_delay.h"
#include "emb_rtcfg.h"
#include "shell/cli_shell.h"
#include "cmd/Cmd_App.h"
#include "version_def.h"
#include "version.h"


/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          Version
 * @{
 ******************************************************************************/

#ifndef RTCFG_ITEM_SYS_IDN
#define RTCFG_ITEM_SYS_IDN    "Sys:IDN"     //!< (C) 设备 IDN 配置键
#endif

#ifndef IROM_BASE_ADDRESS
#define IROM_BASE_ADDRESS     ROM_MAIN_ADDR         //!< 程序存储空间地址
#endif
#ifndef BL_RESERVE_SIZE
#define BL_RESERVE_SIZE       ROM_RESV_SIZE         //!< Boot Loader 保留空间大小
#endif

#ifndef BL_VER_ADDR_OFFSET
#define BL_VER_ADDR_OFFSET      0x1000              //!< 地址偏移: Boot Loader 版本信息
#endif
#ifndef APP_VER_ADDR_OFFSET
#define APP_VER_ADDR_OFFSET     0x1000              //!< 地址偏移: 固件版本信息
#endif
#ifndef VER_STRING_SIZE_MAX
#define VER_STRING_SIZE_MAX      128                //!< 版本字符串最大长度
#endif

#ifndef COMPANY_NAME
#define COMPANY_NAME             "PSS"              //!< 公司名
#endif
#ifndef DEVICE_NAME
#define DEVICE_NAME             "NAME"              //!< 设备(或模块)名
#endif
#ifndef FIRMWARE_NAME
#define FIRMWARE_NAME            "FW"               //!< 固件名
#endif
#ifndef HARDWARE_NAME
#define HARDWARE_NAME            "HW"               //!< 硬件名
#endif

#ifndef VER_HARDWARE
#define VER_HARDWARE "PSS_NAME_HW_V1.0.0_20190802"  //!< 硬件版本
#endif

//------------------ 默认IDN -------------------------------------------
#ifndef VER_DEVICE_IDN       // "PSS,NAME,00000000,V1.0.0_20190802"
#define VER_DEVICE_IDN          COMPANY_NAME          ","  \
                                DEVICE_NAME           ","  \
                                "00000000"            ","  \
                                TO_VER_STRING(DEVICE) "_"  \
                                VER_DEVICE_DATE
#ifndef VER_DEVICE_MAJOR
#define VER_DEVICE_MAJOR        VER_MAJOR           //!< 主版本号
#endif
#ifndef VER_DEVICE_MINOR
#define VER_DEVICE_MINOR        VER_MINOR           //!< 次版本号
#endif
#ifndef VER_DEVICE_BUILD
#define VER_DEVICE_BUILD        VER_BUILD           //!< 修订号
#endif
#ifndef VER_DEVICE_DATE
#define VER_DEVICE_DATE         VER_CURRENT_DATE    //!< 日期
#endif
#endif  // VER_DEVICE_IDN

//------------------ 固件版本 ------------------------------------------
#ifndef VER_FIRMWARE         // "PSS_NAME_FW_V1.0.0_20190802"
#define VER_FIRMWARE            COMPANY_NAME            "_" \
                                DEVICE_NAME             "_" \
                                FIRMWARE_NAME           "_" \
                                TO_VER_STRING(FIRMWARE) "_" \
                                VER_FIRMWARE_DATE
#ifndef VER_FIRMWARE_MAJOR
#define VER_FIRMWARE_MAJOR      VER_MAJOR           //!< 主版本号
#endif
#ifndef VER_FIRMWARE_MINOR
#define VER_FIRMWARE_MINOR      VER_MINOR           //!< 次版本号
#endif
#ifndef VER_FIRMWARE_BUILD
#define VER_FIRMWARE_BUILD      VER_BUILD           //!< 修订号
#endif
#ifndef VER_FIRMWARE_DATE
#define VER_FIRMWARE_DATE       VER_CURRENT_DATE    //!< 日期
#endif
#endif  // VER_FIRMWARE

#if 0
//------------------ 硬件版本 ------------------------------------------
#ifndef VER_HARDWARE         // "PSS_NAME_HW_V1.0.0_20190802"
#define VER_HARDWARE            COMPANY_NAME            "_" \
                                DEVICE_NAME             "_" \
                                HARDWARE_NAME           "_" \
                                TO_VER_STRING(HARDWARE) "_" \
                                VER_HARDWARE_DATE
#ifndef VER_HARDWARE_MAJOR
#define VER_HARDWARE_MAJOR      VER_MAJOR           //!< 主版本号
#endif
#ifndef VER_HARDWARE_MINOR
#define VER_HARDWARE_MINOR      VER_MINOR           //!< 次版本号
#endif
#ifndef VER_HARDWARE_BUILD
#define VER_HARDWARE_BUILD      VER_BUILD           //!< 修订号
#endif
#ifndef VER_HARDWARE_DATE
#define VER_HARDWARE_DATE       VER_CURRENT_DATE    //!< 日期
#endif
#endif  // VER_HARDWARE
#endif

#ifndef ENDL
#define ENDL                "\r\n"      //!< (C) End of line, start the new line
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Version
 * @{
 * @addtogroup              Pravate_Macros
 * @{
 ******************************************************************************/

#define TO_VER_STRING(name)  "V" TO_STRING(ATPASTE4(VER_, name, _, MAJOR)) \
                             "." TO_STRING(ATPASTE4(VER_, name, _, MINOR)) \
                             "." TO_STRING(ATPASTE4(VER_, name, _, BUILD))

#define APP_VER_BASE_ADDR  (IROM_BASE_ADDRESS + APP_VER_ADDR_OFFSET + BL_RESERVE_SIZE)
#define  BL_VER_BASE_ADDR  (IROM_BASE_ADDRESS +  BL_VER_ADDR_OFFSET                  )

#define VER_BL_HARDWARE    ((const char *)(BL_VER_BASE_ADDR + 0 * VER_STRING_SIZE_MAX))
#define VER_BL_FIRMWARE    ((const char *)(BL_VER_BASE_ADDR + 1 * VER_STRING_SIZE_MAX))


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Variables
 * @{
 ******************************************************************************/

__USED const char  _Ver_Firmware[VER_STRING_SIZE_MAX]   //!< 固件版本
    __MEMORY_AT(APP_VER_BASE_ADDR + 0 * VER_STRING_SIZE_MAX) = { VER_FIRMWARE };

__USED const char  _Ver_Hardware[VER_STRING_SIZE_MAX]   //!< 硬件版本
    __MEMORY_AT(APP_VER_BASE_ADDR + 1 * VER_STRING_SIZE_MAX) = { VER_HARDWARE };

__USED const char  _Ver_BuildTime[VER_STRING_SIZE_MAX]  //!< 编译时间
    __MEMORY_AT(APP_VER_BASE_ADDR + 2 * VER_STRING_SIZE_MAX) = { VER_CURRENT_DATE " " VER_CURRENT_TIME };

__USED const char  _Ver_CommitTime[VER_STRING_SIZE_MAX] //!< 版本库提交时间
    __MEMORY_AT(APP_VER_BASE_ADDR + 3 * VER_STRING_SIZE_MAX) = { VER_COMMIT_DATE " " VER_COMMIT_TIME };

__USED const char  _Ver_Revision[VER_STRING_SIZE_MAX]   //!< 版本库修订号
    __MEMORY_AT(APP_VER_BASE_ADDR + 4 * VER_STRING_SIZE_MAX) = { TO_STRING(VER_REVISION) };

__USED const char  _Ver_Repostory[VER_STRING_SIZE_MAX]  //!< 版本库地址
    __MEMORY_AT(APP_VER_BASE_ADDR + 5 * VER_STRING_SIZE_MAX) = { VER_REPOSITORY_URL };

__USED const char  _Idn_Default[VER_STRING_SIZE_MAX]    //!< 默认IDN
    __MEMORY_AT(APP_VER_BASE_ADDR + 6 * VER_STRING_SIZE_MAX) = { VER_DEVICE_IDN };

/** 
 * 设备 IDN 配置条目 
 */ 
const rtcfg_string_t  _Cmd_SysIdn_Item = { RTCFG_ITEM_SYS_IDN, _Idn_Default };


/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Functions
 * @{
 ******************************************************************************/
/**
 * 检测字符串是否为可打印字符串
 *
 * @param[in]  str  需检查的字符串
 *
 * @retval  (== str) 是可打印字符串
 * @retval  (!= str) 非可打印字符串, 返回字符串 "!!!ERROR!!!"
 */
static const char* IsPrint(const char* str)
{
    static const char error[] = "!!!ERROR!!!";

    for(int i = 0;  i < VER_STRING_SIZE_MAX;  i++)
    {
        char  ch = str[i];
        if (ch == '\0')   { return( str ); }
        if (!isprint(ch)) { break;         }
    }
    return( error );
}


//lint -esym(714, Cmd_Version)
/**
 * 显示详细版本信息 命令处理函数
 */
int Cmd_Version(int cli, const char* args)
{
    char       *ptr;
    const char *idn;

    if (cli_fputs("----------------------------------------------------"ENDL, cli) < 0) {
        return( -1 );
    }

    if ((ptr = malloc(128)) == NULL) {
        return( 1 );
    }
    if((idn = rtcfg_string_read(&_Cmd_SysIdn_Item, ptr, 128)) == NULL) {
        free(ptr);
        return( 1 );
    }
    if (cli_fprintf(cli, "Device IDN:    %.108s"ENDL, idn) < 0) {
        free(ptr);
        return( -1 );
    }
    free(ptr);

    if (cli_fprintf(cli, "Firmware Ver:  %.108s"ENDL, _Ver_Firmware) < 0) {
        return( -1 );
    }
    if (cli_fprintf(cli, "Hardware Ver:  %.108s"ENDL, _Ver_Hardware) < 0) {
        return( -1 );
    }
    if (cli_fprintf(cli, "Build time:    %.108s"ENDL, _Ver_BuildTime) < 0) {
        return( -1 );
    }
    if (cli_fprintf(cli, "Commit Time:   %.108s"ENDL, _Ver_CommitTime) < 0) {
        return( -1 );
    }
    if (cli_fprintf(cli, "Revision:      %.108s"ENDL, _Ver_Revision) < 0) {
        return( -1 );
    }
    if (cli_fprintf(cli, "Repostory:     %.108s"ENDL, _Ver_Repostory) < 0) {
        return( -1 );
    }
    if (cli_fprintf(cli, "BootLoader FW: %.108s"ENDL, IsPrint(VER_BL_FIRMWARE)) < 0) {
        return( -1 );
    }
    if (cli_fprintf(cli, "BootLoader HW: %.108s"ENDL, IsPrint(VER_BL_HARDWARE)) < 0) {
        return( -1 );
    }
    if (cli_fputs(ENDL, cli) < 0) {
        return( -1 );
    }
    UNUSED_VAR(args);
    return( 0 );
}


//lint -esym(714, Cmd_FwVer)
/**
 * 显示软件版本
 */
int Cmd_FwVer(int cli, const char* args)
{
    UNUSED_VAR(args);

    if (cli_fprintf(cli, "%.108s"ENDL, _Ver_Firmware) < 0) {
        return( -1 );
    }
    return( 0 );
}


//lint -esym(714, Cmd_HwVer)
/**
 * 显示硬件版本
 */
int Cmd_HwVer(int cli, const char* args)
{
    UNUSED_VAR(args);

    if (cli_fprintf(cli, "%.108s"ENDL, _Ver_Hardware) < 0) {
        return( -1 );
    }
    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

