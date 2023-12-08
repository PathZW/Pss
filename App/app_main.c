/*******************************************************************************
 * @file    app_main.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/7/12
 * @brief
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/7/12 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -esym(714, app_main)
//lint -esym(765, app_main)

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "App_Main"
#include "emb_log.h"
#include "defines.h"
#include "emb_critical.h"
#include "emb_delay.h"


/*******************************************************************************
 * @addtogroup App_Main
 * @{
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/

__WEAK void fs_init   (void) {}             //lint -esym(522, fs_init)
__WEAK void rtcfg_init(void) {}             //lint -esym(522, rtcfg_init)
__WEAK void net_init  (void) {}             //lint -esym(522, net_init)

__WEAK void Cmd_AppInit  (void) {}          //lint -esym(522, Cmd_AppInit)
__WEAK void Cmd_UART1Init(void) {}          //lint -esym(522, Cmd_UART1Init)
__WEAK void Cmd_UART2Init(void) {}          //lint -esym(522, Cmd_UART2Init)
__WEAK void Cmd_RS232Init(void) {}          //lint -esym(522, Cmd_RS232Init)
__WEAK void Cmd_RS485Init(void) {}          //lint -esym(522, Cmd_RS485Init)
__WEAK int  Cmd_SPIxInit (void) {return 0;} //lint -esym(522, Cmd_SPIxInit)
__WEAK int  Cmd_SPIyInit (void) {return 0;} //lint -esym(522, Cmd_SPIyInit)
__WEAK void Cmd_GPIBInit (void) {}          //lint -esym(522, Cmd_GPIBInit)
__WEAK void Cmd_TcpInit  (void) {}          //lint -esym(522, Cmd_TcpInit)
__WEAK void Cmd_UdpInit  (void) {}          //lint -esym(522, Cmd_UdpInit)

__WEAK int  SOUR_Init    (void) {return 0;} //lint -esym(522, SOUR_Init)
__WEAK int  LDBI016_init (void) {return 0;} //lint -esym(522, LDBI016_init)

/*******************************************************************************
 * @}
 * @addtogroup              Pravate_Functions
 * @{
 ******************************************************************************/
//lint -esym(714, SystemEntry_Loader)
//lint -esym(765, SystemEntry_Loader)
/**
 * System entry loader, todo init the drivers and components
 */
void SystemEntry_Loader(void)
{
    fs_init();       DelayMS(2);
    rtcfg_init();    DelayMS(2);
    net_init();      DelayMS(2);

    Cmd_AppInit();   DelayMS(2);
    Cmd_UART1Init(); DelayMS(2);
    Cmd_UART2Init(); DelayMS(2);
    Cmd_RS232Init(); DelayMS(2);
    Cmd_RS485Init(); DelayMS(2);
    Cmd_SPIyInit();  DelayMS(2);    //lint !e534
    Cmd_SPIxInit();  DelayMS(2);    //lint !e534
    Cmd_GPIBInit();  DelayMS(9);
    Cmd_TcpInit();   DelayMS(2);
    Cmd_UdpInit();   DelayMS(2);

    SOUR_Init();     DelayMS(2);    //lint !e534
    LDBI016_init();  DelayMS(2);    //lint !e534
}


/*----------------------------------------------------------------------------*/
/**
 * The function for application entry point
 */
void app_main(void *arg)
{
    UNUSED_VAR(arg);
}


#if 0
#include "shell/cli_shell.h"
#include <string.h>
//lint -esym(714, Cmd_ForwardBinHook, Cmd_ForwardHook)
//lint -esym(765, Cmd_ForwardBinHook, Cmd_ForwardHook)

/**
 * 数据转发钩子函数(用于应用扩展数据转发功能)
 */
int Cmd_ForwardHook(int cli, const void *data, int size)
{
    const char* name = cli_shell_name(cli);

    if (strcmp(name, "SPIy") != 0) {
        return( 0 );
    }
    if(cli_fwrite(data, size, 1, cli) != 1) { //lint !e732
        return( 0 );                        // 设备操作异常
    }
    if (cli_fflush(cli) < 0) {
        return( 0 );                        // 设备操作异常
    }
    return( size );
}
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

