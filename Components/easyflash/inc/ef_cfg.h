/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
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
 * Created on: 2015-07-14
 */

#ifndef EF_CFG_H_
#define EF_CFG_H_

/*----------------------------------------------------------------------------*/
#ifdef   APP_CFG
#include APP_CFG                        // Apps configuration
#endif
// #define  LOG_TAG  "eFlash"
// #include "emb_log.h"


#ifndef EFS_FLASH_NUM
#define EFS_FLASH_NUM          -1       // EFS Flash driver number, (< 0)Disable
#endif

#define ef_log_debug(file, line, format, ...)   (            \
            (ef_print("[eFlash](%s:%ld) ", (file), (line))), \
            (ef_print((format), ##__VA_ARGS__))              \
        )
#define ef_log_info(format, ...)   (                         \
            (ef_print("[eFlash] ")),                         \
            (ef_print((format), ##__VA_ARGS__))              \
        )
#define ef_print            printf      // The function for print, only used ef_print_env()
/*----------------------------------------------------------------------------*/

/* using ENV function, default is NG (Next Generation) mode start from V4.0 */
#define EF_USING_ENV

#ifdef EF_USING_ENV
/* Auto update ENV to latest default when current ENV version number is changed. */
#define EF_ENV_AUTO_UPDATE      1
#undef  EF_ENV_AUTO_UPDATE

/**
 * ENV version number defined by user.
 * Please change it when your firmware add a new ENV to default_env_set.
 */
#define EF_ENV_VER_NUM          104     /* @note you must define it for a value, such as 0 */
#undef  EF_ENV_VER_NUM

/* MCU Endian Configuration, default is Little Endian Order. */
#define EF_BIG_ENDIAN
#undef  EF_BIG_ENDIAN
#endif /* EF_USING_ENV */

/* using IAP function */
#define EF_USING_IAP
#undef  EF_USING_IAP

/* using save log function */
#ifdef  LOG_FLASH_SIZE
#define EF_USING_LOG
//#undef  EF_USING_LOG
#endif

/* The minimum size of flash erasure. May be a flash sector size. */
#ifdef  EFS_ERASE_SIZE
#define EF_ERASE_MIN_SIZE   EFS_ERASE_SIZE
#else                                                       /*
#define EF_ERASE_MIN_SIZE	(1024 * 128)      // STM32F4    */
#endif

/* the flash write granularity, unit: bit
 * only support 1(nor flash)/ 8(stm32f4)/ 32(stm32f1) */
#ifdef  EFS_WRITE_WIDTH
#define EF_WRITE_GRAN       EFS_WRITE_WIDTH
#else                                           /*
#define EF_WRITE_GRAN		8                   */
#endif

/*
 *
 * This all Backup Area Flash storage index. All used flash area configure is under here.
 * |----------------------------|   Storage Size
 * | Environment variables area |   ENV area size @see ENV_AREA_SIZE
 * |----------------------------|
 * |      Saved log area        |   Log area size @see LOG_AREA_SIZE
 * |----------------------------|
 * |(IAP)Downloaded application |   IAP already downloaded application, unfixed size
 * |----------------------------|
 *
 * @note all area sizes must be aligned with EF_ERASE_MIN_SIZE
 *
 * The EasyFlash add the NG (Next Generation) mode start from V4.0. All old mode before V4.0, called LEGACY mode.
 *
 * - NG (Next Generation) mode is default mode from V4.0. It's easy to settings, only defined the ENV_AREA_SIZE.
 * - The LEGACY mode has been DEPRECATED. It is NOT RECOMMENDED to continue using.
 *   Beacuse it will use ram to buffer the ENV and spend more flash erase times.
 *   If you want use it please using the V3.X version.
 */

/* backup area start address */
#ifdef  EFS_FLASH_ADDR
#define EF_START_ADDR       EFS_FLASH_ADDR
#else                                                   /*
#define EF_START_ADDR       (FLASH_BASE + 256 * 1024)   */
#endif

/* ENV area size. It's at least one empty sector for GC. So it's definition must more then or equal 2 flash sector size. */
#ifdef  EFS_FLASH_SIZE
#ifdef  LOG_FLASH_SIZE
#define ENV_AREA_SIZE      (EFS_FLASH_SIZE - LOG_FLASH_SIZE)
#else
#define ENV_AREA_SIZE       EFS_FLASH_SIZE
#endif
#else                                               /*
#define ENV_AREA_SIZE      (2 * EF_ERASE_MIN_SIZE)  */
#endif//EFS_FLASH_SIZE

/* saved log area size */
#ifdef  LOG_FLASH_SIZE
#define LOG_AREA_SIZE       LOG_FLASH_SIZE
#endif

/* print debug information of flash */
#define PRINT_DEBUG         1
#undef  PRINT_DEBUG

#endif /* EF_CFG_H_ */
