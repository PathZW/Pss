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
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#ifdef   _RTE_                  // CMSIS_device_header
#include "RTE_Components.h"     // Component selection
#endif// _RTE_
#ifdef   RTE_CMSIS_RTOS2
#include "cmsis_os2.h"          // CMSIS:RTOS2
#endif
#include "Driver_Flash.h"
#include "defines.h"

extern ARM_DRIVER_FLASH     ARM_Driver_Flash_(EFS_FLASH_NUM);
#define flashDev          (&ARM_Driver_Flash_(EFS_FLASH_NUM))

static uint8_t              _EF_DataWidth;  // Flash R/W data width for Bytes

#ifdef RTE_CMSIS_RTOS2
static osMutexId_t         _EF_envMutex;    // Mutex for the ENV
static const osMutexAttr_t _EF_envMutexAttr = {
    "EasyFlash Mutex", osMutexRecursive | osMutexPrioInherit | osMutexRobust, NULL, NULL
};
#endif



/* default environment variables set for user */
const ef_env default_env_set[] = {
	{ "ssid",   "test",     0 },    //字符串大小设置为0，会自动检测
};


/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size)
{
    UNUSED_VAR(default_env);   UNUSED_VAR(default_env_size);

#ifdef RTE_CMSIS_RTOS2
    if((_EF_envMutex = osMutexNew(&_EF_envMutexAttr)) == NULL) {
        goto ERROR;
    }
#endif

#if(EFS_FLASH_NUM >= 0)
    switch((flashDev->GetCapabilities()).data_width) {
    case 0 :  _EF_DataWidth = 1;  break;
    case 1 :  _EF_DataWidth = 2;  break;
    case 2 :  _EF_DataWidth = 4;  break;
    default:                      break;
    }
    EF_ASSERT( _EF_DataWidth == ((EF_WRITE_GRAN + 7) / 8) );

    if((flashDev->Initialize(NULL)) != ARM_DRIVER_OK) {
        goto ERROR;
    }
    if((flashDev->PowerControl(ARM_POWER_FULL)) != ARM_DRIVER_OK) {
        goto ERROR;
    }
    *default_env      =        default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    return( EF_NO_ERR );
    ERROR:
#endif

    return( EF_ENV_INIT_FAILED );
}


/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size)
{
    UNUSED_VAR(addr);  UNUSED_VAR(buf);  UNUSED_VAR(size);

#if(EFS_FLASH_NUM >= 0)
    EF_ASSERT((size % _EF_DataWidth) == 0);

    while ((flashDev->GetStatus()).busy) ;      // Waiting for not busy
    if (flashDev->ReadData(addr, buf, size / _EF_DataWidth) <= 0) {
        goto ERROR;                             // Read data from Flash
    }
    while ((flashDev->GetStatus()).busy) ;      //  Waiting for not busy

    return( EF_NO_ERR );
    ERROR:
#endif

    return( EF_READ_ERR );
}


/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size)
{
    UNUSED_VAR(addr);  UNUSED_VAR(size);

#if(EFS_FLASH_NUM >= 0)
    uint32_t        sec;
    size_t          erased;
    ARM_FLASH_INFO *info = flashDev->GetInfo();

    if (info->sector_info != NULL)
    {
        for(erased = 0;  erased < size;  )
        {
            for(sec = 0;  sec < info->sector_count;  sec++) {   // Get the sector size
                if((info->sector_info[sec].start <= (addr + erased)) && ((addr + erased) <= info->sector_info[sec].end)) {
                    break;
                }
            }
            if (sec >= info->sector_count) {
                goto ERROR;
            }

            while ((flashDev->GetStatus()).busy) ;              // Waiting for not busy
            if (flashDev->EraseSector(addr + erased) != ARM_DRIVER_OK) {
                goto ERROR;                                     // Erase Flash Sector
            }
         // while ((flashDev->GetStatus()).busy) ;              // Waiting for not busy

            erased += info->sector_info[sec].end - info->sector_info[sec].start + 1;
        }
        return( EF_NO_ERR );
    }

    if (info->sector_size != 0)
    {
        for(erased = 0;  erased < size;  )
        {
            while ((flashDev->GetStatus()).busy) ;              // Waiting for not busy
            if (flashDev->EraseSector(addr + erased) != ARM_DRIVER_OK) {
                goto ERROR;                                     // Erase Flash Sector
            }
         // while ((flashDev->GetStatus()).busy) ;              // Waiting for not busy

            erased += info->sector_size;
        }
        return( EF_NO_ERR );
    }

    ERROR:
#endif

    return( EF_ERASE_ERR );
}


/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size)
{
    UNUSED_VAR(addr);  UNUSED_VAR(buf);  UNUSED_VAR(size);

#if(EFS_FLASH_NUM >= 0)
    EF_ASSERT((size % _EF_DataWidth) == 0);

    while ((flashDev->GetStatus()).busy) ;      // Waiting for not busy
    if (flashDev->ProgramData(addr, buf, size / _EF_DataWidth) <= 0) {
        goto ERROR;                             // Program data to Flash
    }
 // while ((flashDev->GetStatus()).busy) ;      // Waiting for not busy

    return( EF_NO_ERR );
    ERROR:
#endif

    return( EF_READ_ERR );
}


/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void)
{
#ifdef RTE_CMSIS_RTOS2
    osStatus_t  result  = osMutexAcquire(_EF_envMutex, osWaitForever);
    EF_ASSERT(result == osOK);  UNUSED_VAR(result);
#endif
}


/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void)
{
#ifdef RTE_CMSIS_RTOS2
    osStatus_t  result  = osMutexRelease(_EF_envMutex);
    EF_ASSERT(result == osOK);  UNUSED_VAR(result);
#endif
}


#if 0
//easyflash打印数据缓冲区
void ef_print(const char *format, ...)
{
    va_list  args;
    va_start(args, format);

    va_end(args);
}


/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...)
{
#ifdef PRINT_DEBUG
    va_list  args;
    va_start(args, format);

    ef_print("[eFlash](%s:%ld) ", file, line);
    ef_vprint(format, args);

    va_end(args);
#endif
}


/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...)
{
    va_list  args;
    va_start(args, format);

    ef_print("[eFlash] ");
    ef_vprint(format, args);

    va_end(args);
}
#endif

