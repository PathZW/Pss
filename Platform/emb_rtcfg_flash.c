/*******************************************************************************
 * @file    emb_rtcfg_flash.c
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
 *      2022/6/14 -- Linghu -- the first version
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
#include "emb_rtcfg.h"

#include <stdint.h>
// #include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "easyflash.h"


/*******************************************************************************
 * @addtogroup Emb_RtCfg
 * @{
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/

void rtcfg_init(void)
{
    if (easyflash_init() != EF_NO_ERR) {
        LOG_WARN("ERROR: easyflash_init()");
    }
}


int rtcfg_reset(void)
{
    if (ef_env_set_default() != EF_NO_ERR) {
        LOG_WARN("ERROR: ef_env_set_default()");
        return( -2 );
    }
    return( 0 );
}


const char* rtcfg_string_read(const rtcfg_string_t *cfg, char *buf, unsigned len)
{
    unsigned    size;

    if((cfg ==  NULL) || (cfg->key == NULL) || (buf == NULL) || (len == 0)) {
        return( NULL );
    }

    if((size = ef_get_env_blob(cfg->key, buf, len, NULL)) == 0) {
        return( cfg->dft );
    }
    buf[size] = '\0';
    return( buf );
}


int rtcfg_string_write(const rtcfg_string_t *cfg, const char* str)
{
    if((cfg == NULL) || (cfg->key == NULL) || (str == NULL)) {
        return( -1 );
    }

    if (ef_set_env_blob(cfg->key, str, strlen(str)) != EF_NO_ERR) {
        LOG_WARN("ERROR: ef_set_env_blob()");
        return( -2 );
    }
    return( 0 );
}


int32_t rtcfg_int32_read(const rtcfg_int32_t *cfg)
{
    char        buf[12];
    unsigned    size;

    if((cfg == NULL) || (cfg->key == NULL)) {
        return( -1 );
    }

    if((size = ef_get_env_blob(cfg->key, buf, sizeof(buf), NULL)) == 0) {
        return( cfg->dft );
    }
    buf[size] = '\0';

    return( strtol(buf, NULL, 0) );
}


int rtcfg_int32_write(const rtcfg_int32_t *cfg, int32_t val)
{
    char    buf[12];
    int     len;

    if((cfg == NULL) || (cfg->key == NULL)) {
        return( -1 );
    }

    if((len = snprintf(buf, sizeof(buf), "%ld", val)) <= 0) {
        return( -3 );
    }
    if (ef_set_env_blob(cfg->key, buf, len) != EF_NO_ERR) {
        LOG_WARN("ERROR: ef_set_env_blob()");
        return( -2 );
    }
    return( 0 );
}


double rtcfg_double_read(const rtcfg_double_t *cfg)
{
    char        buf[24];
    unsigned    size;

    if((cfg == NULL) || (cfg->key == NULL)) {
        return( -1 );
    }

    if((size = ef_get_env_blob(cfg->key, buf, sizeof(buf), NULL)) == 0) {
        return( cfg->dft );
    }
    buf[size] = '\0';

    return( atof(buf) );
}


int rtcfg_double_write(const rtcfg_double_t *cfg, double val)
{
    char    buf[24];
    int     len;

    if((cfg == NULL) || (cfg->key == NULL)) {
        return( -1 );
    }

    if((len = snprintf(buf, sizeof(buf), "%g", val)) <= 0) {
        return( -3 );
    }
    if (ef_set_env_blob(cfg->key, buf, len) != EF_NO_ERR) {
        LOG_WARN("ERROR: ef_set_env_blob()");
        return( -2 );
    }
    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

