/*******************************************************************************
 * @file    emb_platform.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/5/10
 * @brief   Embedded platform bootstarp and startup
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/5/10 -- Linghu -- the first version
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
#define  LOG_TAG  "Platform"
#include "emb_log.h"
#define  ONLY_USED_REGS
#include "board.h"              // Device's defines

#ifdef   _RTE_                  // RTE_CMSIS_RTOS2
#include "RTE_Components.h"     // Component selection
#endif
#ifdef   RTE_CMSIS_RTOS2
#include "cmsis_os2.h"          // CMSIS:RTOS2
#else
#warning "Unused CMSIS RTOS2, The RTOS RunTime will not load!"
#endif
#include <stdint.h>
#include "defines.h"                // Used: __WEAK

#include "emb_delay.h"
#include "emb_prompt.h"
#include "emb_rtcfg.h"
#include "emb_platform.h"


/******************************************************************************/
/**
 * @addtogroup __Config
 * @{
 * @addtogroup              Emb_Platform
 * @{
 ******************************************************************************/
/**
 * [初始化 / app_main()] 线程配置
 *
 * 注意: 初始化与app_main()为同一个线程, 仅在不同阶段使用各自优先级
 */
#ifndef INIT_THREAD_PRIO
#define INIT_THREAD_PRIO      osPriorityNormal  //!< (C) 系统初始化线程优先级
#endif

#ifndef MAIN_THREAD_PRIO
#define MAIN_THREAD_PRIO      osPriorityNormal  //!< (C) app_main 线性优先级
#endif

#ifndef MAIN_THREAD_STKB
#define MAIN_THREAD_STKB             0          //!< (C) app_main 线程栈大小(以字节为单位)
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Emb_Platform
 * @{
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

static uint32_t  _sys_booting  = 0x55AA5AA5;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/

//lint -esym(401, app_main, SystemEntry_Loader)
//lint -esym(522, app_main, SystemEntry_Loader)
__WEAK void app_main(void *arg){ ((void)arg); }
__WEAK void SystemEntry_Loader(void){}

//lint -esym(401, Board_BareInit, Board_Init, Board_Start)
//lint -esym(522, Board_BareInit, Board_Init, Board_Start)
__WEAK void Board_BareInit(void){}
__WEAK void Board_Init(void){}
__WEAK void Board_Start(void){}

//lint -esym(401, Board_DelayInit, Board_GetMsec, Board_DelayUS)
//lint -esym(522, Board_DelayInit, Board_GetMsec, Board_DelayUS)
__WEAK void     Board_DelayInit(void){}
__WEAK uint32_t Board_GetMsec(void){ return( 0 ); }
__WEAK void     Board_DelayUS(uint16_t uSec){ ((void)uSec); }

//lint -esym(401, stdio_Retarget, cm_backtrace_init)
//lint -esym(522, stdio_Retarget, cm_backtrace_init)
__WEAK void stdio_Retarget(void){}
__WEAK void cm_backtrace_init(void){}

#ifdef RTE_CMSIS_RTOS2
static void thread_main(void *arg);
#endif
static __INLINE void Platform_doStart(void* arg);


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/

void Platform_BareInit(void)
{
    _sys_booting = 0x55AA5AA5;  // Set to the system is booting
    Board_DelayInit();
    SysPromptStateInit();

    SysPrompt_Assert();
    Board_BareInit();           // Todo: Initialize for the bare board
    SysPrompt_Booting();
}


int Platform_SysCheck(void)
{
    return( (_sys_booting == 0) ? 1 : 0 );
}


void Platform_Init(void)
{
    stdio_Retarget();           // Initialize for the stdio retarget
    DelayUS(2);
    cm_backtrace_init();        // Initialize for the CmBacktrace library
    DelayUS(2);

    _sys_booting = 0;           // Set to the system is booted
    LOG_INFO("=============== Runtime is loaded ================");
    LOG_INFO("Main ROM addr: 0x%08lX, size: 0x%08lX", (uint32_t)ROM_MAIN_ADDR,  (uint32_t)(ROM_MAIN_SIZE));
    LOG_INFO("Link ROM addr: 0x%08lX, size: 0x%08lX", (uint32_t)ROM_LINK_START, (uint32_t)(ROM_LINK_LIMIT - ROM_LINK_START));
    LOG_INFO("Main RAM addr: 0x%08lX, size: 0x%08lX", (uint32_t)RAM_MAIN_ADDR,  (uint32_t)(RAM_MAIN_SIZE));
    LOG_INFO("Link RAM addr: 0x%08lX, size: 0x%08lX", (uint32_t)RAM_LINK_START, (uint32_t)(RAM_LINK_LIMIT - RAM_LINK_START));
    LOG_INFO("Platform initing......");
    DelayUS(2);

    Board_Init();               // Todo: Initialize for the board
    DelayUS(2);

    LOG_INFO("Platform initialized -----------------------------");
    DelayUS(2);
}


void Platform_Start(void *arg)
{
    LOG_INFO("Platform starting......");
    DelayMS(2);
    Board_Start();              // Todo: Start for the board
    DelayMS(2);

    #ifdef RTE_CMSIS_RTOS2
    {
        static const osThreadAttr_t  _app_main_attr = {
              "app_main Thread" // name
            , osThreadDetached  // attr_bits: osThreadJoinable
            , NULL              // cb_mem
            , 0                 // cb_size
            , NULL              // stack_mem
            , MAIN_THREAD_STKB  // stack_size
            , INIT_THREAD_PRIO  // priority
            , 0                 // tz_module
            , 0                 // reserved
        };
        if (osOK != osKernelInitialize()) {
            LOG_ASSERT("Assert failed: Initialize the RTOS Kernel");
        }
        if (NULL == osThreadNew(thread_main, arg, &_app_main_attr)) {
            LOG_ASSERT("Assert failed: Create app_main() thread");
        }
        if (osOK != osKernelStart()) {
            LOG_ASSERT("Assert failed: Start the RTOS Kernel");
        }
    }
    #else   // RTE_CMSIS_RTOS2
    {
        Platform_doStart(arg);
    }
    #endif  // RTE_CMSIS_RTOS2
}


static __INLINE void Platform_doStart(void* arg)
{
    SysPrompt_Initing();
    LOG_INFO("Loading SystemEntry_Loader()");
    DelayMS(2);
    SystemEntry_Loader();   // Todo: System entry loader
    DelayMS(2);

#ifdef RTE_CMSIS_RTOS2
    if (osOK != osThreadSetPriority(osThreadGetId(), MAIN_THREAD_PRIO)) {
        LOG_ASSERT("osThreadSetPriority(osThreadGetId(), MAIN_THREAD_PRIO)");
    }
#endif

    SysPrompt_Normal();
    LOG_INFO("Platform started, Loading app_main() -------------");
    DelayMS(2);
    app_main(arg);
}

#ifdef RTE_CMSIS_RTOS2
static void sys_prompt_poll(void *arg)
{
    UNUSED_VAR(arg);
    SysPromptStatePoll();
}

static void thread_main(void *arg)
{
    osTimerId_t  flickTmr;
    uint32_t     tickFreq  = osKernelGetTickFreq();

    LOG_INFO("RTOS Kernel Tick Frequency: %lu Hz", tickFreq);
    LOG_INFO("     Kernel Tick timer CLK: %lu Hz", osKernelGetSysTimerFreq());
    if (tickFreq != 1000) {             // request from DelayMS() and GetSysMsec()
        LOG_ASSERT("Kernel tick frequency must be 1000 Hz, but it's %lu Hz", tickFreq);
    }
    DelayMS(2);

    if (NULL == (flickTmr = osTimerNew(sys_prompt_poll, osTimerPeriodic, NULL, NULL))) {
        LOG_ASSERT("osTimerNew(sys_prompt_poll, osTimerPeriodic, NULL, NULL)");
    }
    if (osOK != osTimerStart(flickTmr, (100 * 1000) / tickFreq)) {
        LOG_ASSERT("osTimerStart(flickTmr, (400 * 1000) / tickFreq)");
    }
    Platform_doStart(arg);
    osThreadExit();
}
#endif  // RTE_CMSIS_RTOS2


/*----------------------------------------------------------------------------*/
//lint -esym(401, rtcfg_init, rtcfg_reset, rtcfg_string_read, rtcfg_string_write)
//lint -esym(401, rtcfg_int32_read, rtcfg_int32_write, rtcfg_double_read, rtcfg_double_write)
//lint -esym(528, rtcfg_init, rtcfg_reset, rtcfg_string_read, rtcfg_string_write)
//lint -esym(528, rtcfg_int32_read, rtcfg_int32_write, rtcfg_double_read, rtcfg_double_write)
#if 0
__WEAK void rtcfg_init(void)
{
}
#endif

__WEAK int rtcfg_reset(void)
{
    return( -1 );
}

__WEAK const char* rtcfg_string_read(const rtcfg_string_t *cfg, char *buf, unsigned len)
{
    UNUSED_VAR(buf);  UNUSED_VAR(len);
    if (cfg == NULL) {
        return( NULL );
    }
    return( cfg->dft );
}

__WEAK int rtcfg_string_write(const rtcfg_string_t *cfg, const char* str)
{
    UNUSED_VAR(cfg);  UNUSED_VAR(str);
    return( -1 );
}

__WEAK int32_t rtcfg_int32_read(const rtcfg_int32_t *cfg)
{
    if (cfg == NULL) {
        return( INT32_MIN );
    }
    return( cfg->dft );
}

__WEAK int rtcfg_int32_write(const rtcfg_int32_t *cfg, int32_t val)
{
    UNUSED_VAR(cfg);  UNUSED_VAR(val);
    return( -1 );
}

__WEAK double rtcfg_double_read(const rtcfg_double_t *cfg)
{
    if (cfg == NULL) {
        return( NAN );
    }
    return( cfg->dft );
}

__WEAK int rtcfg_double_write(const rtcfg_double_t *cfg, double val)
{
    UNUSED_VAR(cfg);  UNUSED_VAR(val);
    return( -1 );
}


/*----------------------------------------------------------------------------*/
//lint -esym(528, GetSysMsec, DelayTO, SleepTO, DelayMS, SleepMS, DelayUS)

__WEAK uint32_t GetSysMsec(void)
{   //lint !e401
#ifdef RTE_CMSIS_RTOS2
    if (Platform_SysCheck() && (osKernelGetState() == osKernelRunning)) {
        return( osKernelGetTickCount() );
    //  return( osKernelGetTickCount() / (1000u / osKernelGetTickFreq()) );
    }
#endif
    return( Board_GetMsec() );
}


__WEAK void DelayTO(uint32_t mCnt)
{   //lint !e401
#ifdef RTE_CMSIS_RTOS2
    if (Platform_SysCheck() && (osKernelGetState() == osKernelRunning)) {
        osDelayUntil(mCnt); //lint !e534
    //  osDelayUntil(mCnt / (1000u / osKernelGetTickFreq()));
        return;
    }
#endif
    while (Board_GetMsec() > mCnt) ;
}


__WEAK void SleepTO(uint32_t mCnt)
{   //lint !e401
     DelayTO(mCnt);
}


__WEAK void DelayMS(uint32_t mSec)
{   //lint !e401
#ifdef RTE_CMSIS_RTOS2
    if (Platform_SysCheck() && (osKernelGetState() == osKernelRunning)) {
        osDelay(mSec);      //lint !e534
    //  osDelay(mSec / (1000u / osKernelGetTickFreq()));
        return;
    }
#endif
    for(;  mSec > 64;  mSec -= 64) {
        Board_DelayUS(64 * 1000);
    }
    if (mSec > 0) {
        Board_DelayUS(mSec * 1000); //lint !e734
    }
}


__WEAK void SleepMS(uint32_t mSec)
{   //lint !e401
    DelayMS(mSec);
}


__WEAK void DelayUS(uint16_t uSec)
{   //lint !e401
    Board_DelayUS(uSec);
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

