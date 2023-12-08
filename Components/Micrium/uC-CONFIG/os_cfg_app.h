/*
*********************************************************************************************************
*                                              uC/OS-III
*                                        The Real-Time Kernel
*
*                    Copyright 2009-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                               OS CONFIGURATION (APPLICATION SPECIFICS)
*
* Filename : os_cfg_app.h
* Version  : V3.08.00
*********************************************************************************************************
*/

#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#ifdef   _RTE_                  // RTE_CMSIS_RTOS2
#include "RTE_Components.h"     // Component selection
#endif
#ifdef   RTE_CMSIS_RTOS2
#include "cmsis_os2.h"
#endif

/**
 * 请注意: OS_CFG_ISR_STK_SIZE, Stack_Size, Stack_Mem, __initial_sp
 */

/*
**************************************************************************************************************************
*                                                      CONSTANTS
**************************************************************************************************************************
*/

#ifndef OS_CFG_TICK_RATE_HZ                                         /* ------------------ MISCELLANEOUS ------------------- */
#define OS_CFG_TICK_RATE_HZ               1000u          /* 1000u */// 系统节拍(Tick)频率 (10 to 1000 Hz)
#endif
#ifndef OS_CFG_ISR_STK_SIZE
#define OS_CFG_ISR_STK_SIZE                256u           /* 128u */// 中断(ISR)栈大小 (number of CPU_STK elements)
#endif
#ifndef OS_CFG_MSG_POOL_SIZE
#define OS_CFG_MSG_POOL_SIZE                64u            /* 32u */// 系统最大同时发送(POST)消息数量
#endif
#define OS_CFG_TASK_STK_LIMIT_PCT_EMPTY      0u            // 10u   // Stack limit position in percentage to empty


#ifndef OS_CFG_IDLE_TASK_PRIO                                       /* -------------------- IDLE TASK --------------------- */
#define OS_CFG_IDLE_TASK_PRIO  ((OS_PRIO)(OS_CFG_PRIO_MAX - 1u))    // 空闲(IDLE)任务优先级
#endif
#ifndef OS_CFG_IDLE_TASK_STK_SIZE
#define OS_CFG_IDLE_TASK_STK_SIZE  OS_STKB_TO_STKW(512u)    /* 64u */// 空闲(IDLE)任务栈大小 (number of CPU_STK elements)
#endif


#ifndef OS_CFG_STAT_TASK_PRIO                                       /* ------------------ STATISTIC TASK ------------------ */
#define OS_CFG_STAT_TASK_PRIO  ((OS_PRIO)(OS_CFG_PRIO_MAX - 2u))    // 统计(STAT)任务优先级
#endif
#ifndef OS_CFG_STAT_TASK_STK_SIZE
#define OS_CFG_STAT_TASK_STK_SIZE  OS_STKB_TO_STKW(512u)  /* 100u */// 统计(STAT)任务栈大小 (number of CPU_STK elements)
#endif
#define OS_CFG_STAT_TASK_RATE_HZ            10u                     // 统计任务执行频率 (1 to 10 Hz)


#ifndef OS_CFG_TMR_TASK_PRIO                                        /* --------------------- TIMERS ----------------------- */
#define OS_CFG_TMR_TASK_PRIO           OS_PRIO_HIGH                 // 定时器(Timer)任务优先级
#endif
#ifndef OS_CFG_TMR_TASK_STK_SIZE
#define OS_CFG_TMR_TASK_STK_SIZE   OS_STKB_TO_STKW(512u)  /* 128u */// 定时器(Timer)任务栈大小 (number of CPU_STK elements)
#endif

#define OS_CFG_TMR_TASK_RATE_HZ     OS_CFG_TICK_RATE_HZ             /* DEPRECATED - Rate for timers (10 Hz Typ.)            */
                                                                    /* The timer task now calculates its timeouts based     */
                                                                    /* on the timers in the list. It no longer runs at a    */
                                                                    /* static frequency.                                    */
                                                                    /* This define is included for compatibility reasons.   */
                                                                    /* It will determine the period of a timer tick.        */
                                                                    /* We recommend setting it to OS_CFG_TICK_RATE_HZ       */
                                                                    /* for new projects.                                    */


/* ------------------------------------------------------------------------------------------------------------------------ */
#ifndef OS_STKB_TO_STKW
#define OS_STKB_TO_STKW(bytes)  ((CPU_STK_SIZE)((bytes) / CPU_CFG_ADDR_SIZE))       // 字节数量 转换为 任务栈长度
#define OS_STKW_TO_STKB(words)  ((CPU_STK_SIZE)((words) * CPU_CFG_ADDR_SIZE))       // 任务栈长度 转换为 字节数量

#define OS_PRIO_TO_CMSIS(prio) ((osPriority_t)(OS_CFG_PRIO_MAX - (unsigned)(prio))) // uCOS3优先级 转换为 CMSIS-RTOS2优先级
#define OS_PRIO_TO_UCOS3(prio) ((OS_PRIO)     (OS_CFG_PRIO_MAX - (unsigned)(prio))) // CMSIS-RTOS2优先级 转换为 uCOS优先级3
// -------+----+----+-----+----+----+----+----+----+-----+----+----+--------------
//        |IDLE|    | ... | LOW| ...|    |... |REAL| ... |    | ISR|
// -------+----+----+-----+----+----+----+----+----+-----+----+----+--------------
//  CMSIS |  1 |  2 | ... |  8 | ...| 32 |... | 48 | ... | 55 | 56 |
// -------+----+----+-----+----+----+----+----+----+-----+----+----+--------------
//  uCOS3 | 55 | 54 | ... | 48 | ...| 24 |... |  8 | ... |  1 |  0 | (56 - prio)
// -------+----+----+ --- +----+----+----+----+----+-----+----+----+--------------
//  uCOS3 | 63 | 62 | ... | 56 | ...| 32 |... | 16 | ... |  9 |  8 | (64 - prio)
// -------+----+----+ --- +----+----+----+----+----+-----+----+----+--------------

#ifndef RTE_CMSIS_RTOS2                     // -------------------- 8 16 32 64 128 256 -- OS_CFG_PRIO_MAX
#define osPriorityIdle          OS_PRIO_TO_CMSIS(OS_PRIO_IDLE)   // 1  1  1  1   1   1 -- IDLE      
#define osPriorityLow           OS_PRIO_TO_CMSIS(OS_PRIO_LOW)    // 2  6  8 15  26  54 -- 低        
#define osPriorityBelowNormal   OS_PRIO_TO_CMSIS(OS_PRIO_BELOW)  // 3  7 11 21  39  79 -- 低于常用   
#define osPriorityNormal        OS_PRIO_TO_CMSIS(OS_PRIO_NORMAL) // 4  8 14 27  52 104 -- 常用      
#define osPriorityAboveNormal   OS_PRIO_TO_CMSIS(OS_PRIO_ABOVE)  // 5  9 17 33  65 129 -- 高于常用   
#define osPriorityHigh          OS_PRIO_TO_CMSIS(OS_PRIO_HIGH)   // 6 10 20 39  78 154 -- 高        
#define osPriorityRealtime      OS_PRIO_TO_CMSIS(OS_PRIO_REAL)   // 7 11 21 45  91 179 -- 实时(驱动) 
#define osPriorityISR           OS_PRIO_TO_CMSIS(OS_PRIO_ISR)    // 8 12 26 51 104 204 -- ISR       
#define osPriority_t            OS_PRIO
#endif
#endif//OS_STKB_TO_STKW

#ifndef RTE_CMSIS_RTOS2                     // --------------------------------------- 8 16 32 64 128 256 -- OS_CFG_PRIO_MAX
#define OS_PRIO_IDLE   ((OS_PRIO)(OS_CFG_PRIO_MAX - 1))                             // 7 15 31 63 127 255 -- IDLE
#define OS_PRIO_LOW    ((OS_PRIO)(OS_PRIO_ABOVE + (((OS_PRIO_IDLE + 3) / 10) * 3))) // 6 10 24 49 102 202 -- 低
#define OS_PRIO_BELOW  ((OS_PRIO)(OS_PRIO_ABOVE + (((OS_PRIO_IDLE + 3) / 10) * 2))) // 5  9 21 43  89 177 -- 低于常用
#define OS_PRIO_NORMAL ((OS_PRIO)(OS_PRIO_ABOVE + (((OS_PRIO_IDLE + 3) / 10) * 1))) // 4  8 18 37  76 152 -- 常用
#define OS_PRIO_ABOVE  ((OS_PRIO)(OS_PRIO_IDLE  - (( OS_PRIO_IDLE * 5  + 5) / 10))) // 3  7 15 31  63 127 -- 高于常用
#define OS_PRIO_HIGH   ((OS_PRIO)(OS_PRIO_ABOVE - (((OS_PRIO_IDLE + 3) / 10) * 1))) // 2  6 12 25  50 102 -- 高
#define OS_PRIO_REAL   ((OS_PRIO)(OS_PRIO_ABOVE - (((OS_PRIO_IDLE + 3) / 10) * 2))) // 1  5  9 19  37  77 -- 实时(驱动)
#define OS_PRIO_ISR    ((OS_PRIO)(OS_PRIO_ABOVE - (((OS_PRIO_IDLE + 3) / 10) * 3))) // 0  4  6 13  24  52 -- ISR
#if defined(OS_CFG_PRIO_MAX) && (OS_CFG_PRIO_MAX < 8) && defined(OS_GLOBALS)
#error  "配置项 OS_CFG_PRIO_MAX 必须不小于 8"
#endif
#else                                       // ----------------------------------------- 56 60 64 128 256 -- OS_CFG_PRIO_MAX
#define OS_PRIO_IDLE   OS_PRIO_TO_UCOS3(osPriorityIdle)                             //   55 59 63 127 255 -- IDLE
#define OS_PRIO_LOW    OS_PRIO_TO_UCOS3(osPriorityLow)                              //   48 52 56 120 248 -- 低
#define OS_PRIO_BELOW  OS_PRIO_TO_UCOS3(osPriorityBelowNormal)                      //   40 44 48 112 240 -- 低于常用
#define OS_PRIO_NORMAL OS_PRIO_TO_UCOS3(osPriorityNormal)                           //   32 36 40 104 232 -- 常用
#define OS_PRIO_ABOVE  OS_PRIO_TO_UCOS3(osPriorityAboveNormal)                      //   24 28 32  96 224 -- 高于常用
#define OS_PRIO_HIGH   OS_PRIO_TO_UCOS3(osPriorityHigh)                             //   16 20 24  88 216 -- 高
#define OS_PRIO_REAL   OS_PRIO_TO_UCOS3(osPriorityRealtime)                         //    8 12 16  80 208 -- 实时(驱动)
#define OS_PRIO_ISR    OS_PRIO_TO_UCOS3(osPriorityISR)                              //    0  4  8  72 200 -- ISR
#if defined(OS_CFG_PRIO_MAX) && (OS_CFG_PRIO_MAX < 56) && defined(OS_GLOBALS)
#error  "配置项 OS_CFG_PRIO_MAX 必须不小于 56 (CMSIS-RTOS2 API)"
#endif
#endif//RTE_CMSIS_RTOS2


#endif

