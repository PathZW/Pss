/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: It is the configure head file for this library.
 * Created on: 2016-12-15
 */

#ifndef _CMB_CFG_H_
#define _CMB_CFG_H_

#  ifdef   APP_CFG
#  include APP_CFG                      // Apps configuration
#  endif

/* print string, must config by user */
#  ifndef cmb_print
#  define cmb_print(...)                printf(__VA_ARGS__)
#  endif

/* handle system abort */
#  ifndef cmb_abort
//#define cmb_abort()                   ((void)0)                   // default
#  endif

/* OS platform type */
#  ifndef CMB_OS_PLATFORM_TYPE
//#define CMB_OS_PLATFORM_TYPE          CMB_OS_PLATFORM_NONE        // default
//#define CMB_OS_PLATFORM_TYPE          CMB_OS_PLATFORM_RTT
//#define CMB_OS_PLATFORM_TYPE          CMB_OS_PLATFORM_UCOSII
//#define CMB_OS_PLATFORM_TYPE          CMB_OS_PLATFORM_UCOSIII
//#define CMB_OS_PLATFORM_TYPE          CMB_OS_PLATFORM_FREERTOS
//#define CMB_OS_PLATFORM_TYPE          CMB_OS_PLATFORM_RTX5
#  endif

/* cpu platform type */
#  ifndef CMB_CPU_PLATFORM_TYPE
//#define CMB_CPU_PLATFORM_TYPE         CMB_CPU_ARM_CORTEX_M0
//#define CMB_CPU_PLATFORM_TYPE         CMB_CPU_ARM_CORTEX_M3
//#define CMB_CPU_PLATFORM_TYPE         CMB_CPU_ARM_CORTEX_M4
//#define CMB_CPU_PLATFORM_TYPE         CMB_CPU_ARM_CORTEX_M7
#  endif

/* enable dump stack information */
#  ifndef CMB_USING_DUMP_STACK_INFO
//#define CMB_USING_DUMP_STACK_INFO     CMB_ENABLE                  // default
//#define CMB_USING_DUMP_STACK_INFO     CMB_DISABLE
#  endif

/* language of print information */
#  ifndef CMB_PRINT_LANGUAGE
//#define CMB_PRINT_LANGUAGE            CMB_PRINT_LANGUAGE_ENGLISH  // default
//#define CMB_PRINT_LANGUAGE            CMB_PRINT_LANGUAGE_CHINESE
#  endif

#endif /* _CMB_CFG_H_ */

