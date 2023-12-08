/*******************************************************************************
 * @file    main_defs.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/5/10
 * @brief   Defines for main
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/5/10 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __MAIN_DEFS_H__
#define __MAIN_DEFS_H__
/******************************************************************************/
/*
 *                              œµÕ≥∆Ù∂ØÀ≥–Ú
 *
 * --+--> Platform_BareInit() --> Board_BareInit() --+--> CMSIS SystemInit()  
 *   |                                               |                        
 *   |                                               +--> SystemClock_Config()
 *   +--> [Runtime LIB Init]                         |                        
 *   |                                               +--> SystemExMem_Config()
 *   +----> main() ---->
 *                     |
 *   <-----------------+
 *   |
 *   +----> eg: HAL_Init()
 *   |
 *   +--> Platform_Init() ------> Board_Init() ---------> SystemBoard_Config()
 *   |
 *   +----> eg: MX_GPIO_Init()
 *   |
 *   +--> Platform_Start() --+--> Board_Start() --------> SystemStart_Config()
 *                           |
 *                           |
 *                           |
 *                           |                     +----> SystemEntry_Loader()
 *                           +--> [Start RTOS] --->|
 *                           |                     |
 *                           +-------------------->|
 *                                                 +--------> app_main()
 *
 ******************************************************************************/

#ifndef _lint
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) //--- ARM Compiler ---------
    #warning ±ÿ–Î π”√ª„±‡±‡“Î√¸¡Ó: --cpreproc --cpreproc_opts=-DSystemInit="Platform_BareInit"
#elif defined(__ICCARM__)   //------------------------- IAR Compiler ---------
    #error "not supported compiler"
#elif defined(__GNUC__)     //------------------------- GNU Compiler ---------
    #error "not supported compiler"
#else                       //------------------------- Unknow Compiler ------
    #error "not supported compiler"
#endif
#endif  // _lint


#ifdef   _RTE_                      // RTE_CMSIS_RTOS2
#include "RTE_Components.h"         // Component selection
#endif
#ifdef   RTE_CMSIS_RTOS2
#include "cmsis_os2.h"              // CMSIS:RTOS2
#endif

#define  LOG_TAG  "main"
#include "emb_log.h"
#include "emb_platform.h"


/*****************************  END OF FILE  **********************************/
#endif  // __MAIN_DEFS_H__
/*****************************  END OF FILE  **********************************/

