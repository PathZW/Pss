/*******************************************************************************
 * @file    emb_rtcfg.h
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
#ifndef __EMB_RTCFG_H__
#define __EMB_RTCFG_H__
/******************************************************************************/

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#include <stdint.h>             // Used: int32_t
#include <math.h>               // Used: NAN
#include "defines.h"            // Used: UNUSED_VAR(), ATPASTEx()

#ifdef _lint
#undef  NAN
#define NAN 0
#endif

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/**
 * @addtogroup Emb_RtCfg
 * @{
 * @addtogroup              Exported_Types
 * @{
 ******************************************************************************/

#ifndef rtcfg_string_t
/**
 * config item for string.
 */
typedef struct rtcfg_string_s {
    const char *key;            //!< value of key
    const char *dft;            //!< value default
} rtcfg_string_t;
#endif//rtcfg_string_t


#ifndef rtcfg_int32_t
/**
 * config item for int32.
 */
typedef struct rtcfg_int32_s {
    const char *key;            //!< value of key
    int32_t     dft;            //!< value default
} rtcfg_int32_t;
#endif//rtcfg_int32_t

#ifndef rtcfg_double_t
/**
 * config item for double.
 */
typedef struct rtcfg_double_s {
    const char *key;            //!< value of key
    double      dft;            //!< value default
} rtcfg_double_t;
#endif//rtcfg_double_t

//rtcfg_array_t           //!< (C) array  ÅäÖÃÌõÄ¿


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
//lint -emacro(506, rtcfg_?*_?*)
/**
 * Initialize the runtime config module.
 */
void rtcfg_init(void);

/**
 * Reset all runtime config item to default value.
 *
 * @return  (0)Succeed, (Other)Failed.
 */
int rtcfg_reset(void);

/**
 * Read string from runtime config item
 *
 * @param[in]  cfg  config item
 * @param[out] buf  buffer for read
 * @param[in]  len  buffer length in bytes
 *
 * @retval  (!= NULL) string
 * @retval  (== NULL) failed
 */
const char* rtcfg_string_read(const rtcfg_string_t *cfg, char *buf, unsigned len);

/**
 * Write string to runtime config item
 *
 * @param[in]  cfg  config item
 * @param[in]  str  string to write
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int rtcfg_string_write(const rtcfg_string_t *cfg, const char* str);

/**
 * Read 32Bit integer from runtime config item
 *
 * @param[in]  cfg   config item
 *
 * @retval  (!= INT32_MIN) succeed
 * @retval  (== INT32_MIN) failed (may also succeed)
 */
int32_t rtcfg_int32_read(const rtcfg_int32_t *cfg);

/**
 * Write 32Bit integer to runtime config item
 *
 * @param[in]  cfg   config item
 * @param[in]  val   value to write
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int rtcfg_int32_write(const rtcfg_int32_t *cfg, int32_t val);

/**
 * Read double from runtime config item
 *
 * @param[in]  cfg   config item
 *
 * @retval  (!= NAN) succeed
 * @retval  (== NAN) failed
 */
double rtcfg_double_read(const rtcfg_double_t *cfg);

/**
 * Write double to runtime config item
 *
 * @param[in]  cfg   config item
 * @param[in]  val   value to write
 *
 * @retval  (== 0) succeed
 * @retval  (!= 0) failed
 */
int rtcfg_double_write(const rtcfg_double_t *cfg, double val);


/****************************  END OF HEADER  **********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_RTCFG_H__
/*****************************  END OF FILE  **********************************/

