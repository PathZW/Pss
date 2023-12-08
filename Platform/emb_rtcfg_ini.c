/*******************************************************************************
 * @file    emb_rtcfg_ini.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/4/24
 * @brief   Runtime config item read and write.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/4/24 -- Linghu -- the first version
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
#define  LOG_TAG  "RtCfg"
#include "emb_log.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "assert.h"//lint !e451 // Header file repeatedly included
#include "iniparser.h"
#include "cmsis_os2.h"
#include "emb_rtcfg.h"


/*******************************************************************************
 * @addtogroup Emb_RtCfg
 * @{
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/

#ifndef RTCFG_INI_FILE_NAME
#define RTCFG_INI_FILE_NAME  "config.sys"   //!< (C) 默认配置文件名
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

static char*        _rtcfg_ini_name = NULL; // Config file name
static dictionary*  _rtcfg_ini_dic  = NULL; // iniparser dictionary

static osMutexId_t  _rtcfg_mutex_id;        // Mutex for thread-safe


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/

void rtcfg_init(void)
{
    osMutexAttr_t  attr = { "rtcfg_mutex", osMutexRecursive | osMutexPrioInherit, NULL, 0 };

    if((_rtcfg_mutex_id = osMutexNew(&attr)) == NULL) {
        LOG_ASSERT("osMutexNew(attr)");
    }
    if (osMutexAcquire(_rtcfg_mutex_id, osWaitForever) != osOK) {
        LOG_ASSERT("osMutexAcquire(_rtcfg_mutex_id, osWaitForever)");
    }
    {
        _rtcfg_ini_name = RTCFG_INI_FILE_NAME;
        LOG_INFO("Loading config file: %s.", _rtcfg_ini_name);

        if ((_rtcfg_ini_dic = iniparser_load(_rtcfg_ini_name)) == NULL)
        {
            if (rtcfg_reset() != 0)  LOG_ASSERT("ERROR: reset config"); 
            LOG_INFO("Error of load, restore all config to default!");
        }
    }
    if (osMutexRelease(_rtcfg_mutex_id) != osOK) {
        LOG_ASSERT("osMutexRelease(_rtcfg_mutex_id)");
    }
}


int rtcfg_reset(void)
{
    FILE*   fini;
    int     retval;

    if (osMutexAcquire(_rtcfg_mutex_id, osWaitForever) != osOK) {
        LOG_ERROR("osMutexAcquire(_rtcfg_mutex_id, osWaitForever)");
        return( -5 );
    }
    for(;;)
    {
        if (_rtcfg_ini_name == NULL) {
            retval = -2;    break;
        }
        if (_rtcfg_ini_dic != NULL) {
            iniparser_freedict(_rtcfg_ini_dic);
            _rtcfg_ini_dic = NULL;
        }
        if ((fini = fopen(_rtcfg_ini_name, "w")) == NULL) {
            retval = -4;    break;
        }
        if (fputs("; Default setting is empty!\n", fini) == EOF) {
            retval = -4;    break;
        }
        if (fclose(fini) != 0) {
            retval = -4;    break;
        }
        if ((_rtcfg_ini_dic = iniparser_load(_rtcfg_ini_name)) == NULL) {
            retval = -3;    break;
        } else {
            retval =  0;    break;
        }
    }
    if (osMutexRelease(_rtcfg_mutex_id) != osOK) {
        LOG_ERROR("osMutexRelease(_rtcfg_mutex_id)");
        return( -5 );
    }
    return( retval );
}


const char* rtcfg_string_read(const rtcfg_string_t *cfg, char *buf, unsigned len)
{
    const char* retval;

    if((cfg ==  NULL) || (buf == NULL) || (len == 0)) {
        return( NULL );
    }
    if (osMutexAcquire(_rtcfg_mutex_id, osWaitForever) != osOK) {
        LOG_ERROR("osMutexAcquire(_rtcfg_mutex_id, osWaitForever)");
        return( NULL );
    }
    for(;;)
    {
        if (_rtcfg_ini_name == NULL) {
            retval = cfg->dft;  break;
        }
        if (_rtcfg_ini_dic == NULL) {
            _rtcfg_ini_dic = iniparser_load(_rtcfg_ini_name);
        }
        if (_rtcfg_ini_dic == NULL) {
            retval = cfg->dft;  break;
        }
        if((retval = iniparser_getstring(_rtcfg_ini_dic, cfg->key, cfg->dft)) != cfg->dft) {
            retval = strncpy(buf, retval, len);  buf[len - 1] = '\0';
        }
        break;
    }
    if (osMutexRelease(_rtcfg_mutex_id) != osOK) {
        LOG_ERROR("osMutexRelease(_rtcfg_mutex_id)");
        return( NULL );
    }
    return( retval );
}


int rtcfg_string_write(const rtcfg_string_t *cfg, const char* str)
{
    FILE*   fini;
    int     retval;

    if((cfg == NULL) || (cfg->key == NULL)) {
        return( -1 );
    }
    if (osMutexAcquire(_rtcfg_mutex_id, osWaitForever) != osOK) {
        LOG_ERROR("osMutexAcquire(_rtcfg_mutex_id, osWaitForever)");
        return( -5 );
    }
    for(;;)
    {
        if (_rtcfg_ini_name == NULL) {
            retval = -2;    break;
        }
        if (_rtcfg_ini_dic == NULL) {
            _rtcfg_ini_dic =  iniparser_load(_rtcfg_ini_name);
        }
        if (_rtcfg_ini_dic == NULL) {
            retval = -3;    break;
        }
        if (iniparser_set(_rtcfg_ini_dic, cfg->key, str) != 0) {
            retval = -3;    break;
        }
        if ((fini = fopen(_rtcfg_ini_name, "w")) == NULL) {
            retval = -4;    break;
        }
        iniparser_dump_ini(_rtcfg_ini_dic, fini);
        if(fclose(fini) != 0) {
            retval = -4;    break;
        } else {
            retval =  0;    break;
        }
    }
    if (osMutexRelease(_rtcfg_mutex_id) != osOK) {
        LOG_ERROR("osMutexRelease(_rtcfg_mutex_id)");
        return( -5 );
    }
    return( retval );
}


int32_t rtcfg_int32_read(const rtcfg_int32_t *cfg)
{
    int32_t  retval;

    if (cfg == NULL) {
        return( INT32_MIN );
    }
    if (osMutexAcquire(_rtcfg_mutex_id, osWaitForever) != osOK) {
        LOG_ERROR("osMutexAcquire(_rtcfg_mutex_id, osWaitForever)");
        return( INT32_MIN );
    }
    for(;;)
    {
        if (_rtcfg_ini_name == NULL) {
            retval = cfg->dft;  break;
        }
        if (_rtcfg_ini_dic == NULL) {
            _rtcfg_ini_dic = iniparser_load(_rtcfg_ini_name);
        }
        if (_rtcfg_ini_dic == NULL) {
            retval = cfg->dft;  break;
        }
        retval = iniparser_getlongint(_rtcfg_ini_dic, cfg->key, cfg->dft);
        break;
    }
    if (osMutexRelease(_rtcfg_mutex_id) != osOK) {
        LOG_ERROR("osMutexRelease(_rtcfg_mutex_id)");
        return( INT32_MIN );
    }
    return( retval );
}


int rtcfg_int32_write(const rtcfg_int32_t *cfg, int32_t val)
{
    char    buf[12];

    snprintf(buf, sizeof(buf), "%ld", val); //lint !e534 -e(740)
    return( rtcfg_string_write((const rtcfg_string_t *)cfg, buf) );
}


double rtcfg_double_read(const rtcfg_double_t *cfg)
{
    double  retval;

    if (cfg == NULL) {
        return( NAN );
    }
    if (osMutexAcquire(_rtcfg_mutex_id, osWaitForever) != osOK) {
        LOG_ERROR("osMutexAcquire(_rtcfg_mutex_id, osWaitForever)");
        return( NAN );
    }
    for(;;)
    {
        if (_rtcfg_ini_name == NULL) {
            retval = cfg->dft;  break;
        }
        if (_rtcfg_ini_dic == NULL) {
            _rtcfg_ini_dic = iniparser_load(_rtcfg_ini_name);
        }
        if (_rtcfg_ini_dic == NULL) {
            retval = cfg->dft;  break;
        }
        retval = iniparser_getdouble(_rtcfg_ini_dic, cfg->key, cfg->dft);
        break;
    }
    if (osMutexRelease(_rtcfg_mutex_id) != osOK) {
        LOG_ERROR("osMutexRelease(_rtcfg_mutex_id)");
        return( NAN );
    }
    return( retval );
}


int rtcfg_double_write(const rtcfg_double_t *cfg, double val)
{
    char    buf[24];

    snprintf(buf, sizeof(buf), "%g", val); //lint !e534 -e(740)
    return( rtcfg_string_write((const rtcfg_string_t *)cfg, buf) );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

