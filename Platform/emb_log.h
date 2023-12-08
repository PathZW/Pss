/*******************************************************************************
 * @file    emb_log.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/5/10
 * @brief   Embedded platform defines for the log
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/5/10 -- Linghu(Linghu0060@qq.com) -- the first version
 ******************************************************************************/
#ifndef __EMB_LOG_H__
#define __EMB_LOG_H__
/******************************************************************************/
#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#include "assert.h"//lint !e451 // Header file repeatedly included
#include <stdint.h>             // uint32_t
#include <stdlib.h>             // abort()
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup __Config
 * @{
 * @addtogroup                  Emb_LOG
 * @{
 ******************************************************************************/

#ifndef EMB_LOG_LEVEL
#define EMB_LOG_LEVEL           LOG_LEVEL_INFO      //!< (C) 日志级别
#endif

#ifndef EMB_LOG_COLOR
#define EMB_LOG_COLOR                   0           //!< (C) 是否开启日志颜色
#endif

//lint -esym(767, LOG_TAG)
#ifndef LOG_TAG
#define LOG_TAG                    "NO_TAG"         //!< (C) 日志标签
#endif

#ifndef EMB_LOG_PRINT
#define EMB_LOG_PRINT(...)      printk(__VA_ARGS__) //!< (C) 日志打印函数
int printk(const char*fmt,...);
#endif

#ifndef EMB_LOG_GET_MSEC
#define EMB_LOG_GET_MSEC()        GetSysMsec()      //!< (C) 当前系统时间(毫秒)
uint32_t GetSysMsec(void);
#endif

#ifndef ENDL
#define ENDL                       "\r\n"           //!< (C) End of line, start the new line
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup LOG
 * @{
 * @addtogroup                  Exported_Constants
 * @{
 ******************************************************************************/

#define LOG_LEVEL_ASSERT            0
#define LOG_LEVEL_ERROR             1
#define LOG_LEVEL_WARN              2
#define LOG_LEVEL_INFO              3
#define LOG_LEVEL_DEBUG             4
#define LOG_LEVEL_VERB              5


/*******************************************************************************
 * @}
 * @addtogroup                  Exported_Macros
 * @{
 ******************************************************************************/
//lint -printf(1, LOG_ASSERT, LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG, LOG_VERB)
//lint -emacro(506, LOG_ASSERT)     // do ... while(1)
//lint -emacro(717, LOG_ASSERT)     // do ... while(0)
//lint -emacro(716, LOG_ASSERT)     // while(1) ...

#  if !defined(LOG_ASSERT)&& (EMB_LOG_LEVEL >= LOG_LEVEL_ASSERT)
#      define  LOG_ASSERT(...)  do{_EMB_LOG_O_(LOG_LEVEL_ASSERT, __VA_ARGS__); assert(0);} while(0)
#elif !defined(LOG_ASSERT)
#      define  LOG_ASSERT(...)      abort()         //!< 断言, 严重错误, 系统 abort
#endif
#  if !defined(LOG_ERROR) && (EMB_LOG_LEVEL >= LOG_LEVEL_ERROR)
#      define  LOG_ERROR(...)      _EMB_LOG_O_(LOG_LEVEL_ERROR, __VA_ARGS__)
#elif !defined(LOG_ERROR)
#      define  LOG_ERROR(...)                       //!< 错误, 会导致 错误的行为或输出
#endif
#  if !defined(LOG_WARN)  && (EMB_LOG_LEVEL >= LOG_LEVEL_WARN)
#      define  LOG_WARN(...)       _EMB_LOG_O_(LOG_LEVEL_WARN, __VA_ARGS__)
#elif !defined(LOG_WARN)
#      define  LOG_WARN(...)                        //!< 警告, 会导致 部分功能不正常
#endif
#  if !defined(LOG_INFO)  && (EMB_LOG_LEVEL >= LOG_LEVEL_INFO)
#      define  LOG_INFO(...)       _EMB_LOG_O_(LOG_LEVEL_INFO, __VA_ARGS__)
#elif !defined(LOG_INFO)
#      define  LOG_INFO(...)                        //!< 信息, 记录 重要的信息
#endif
#  if !defined(LOG_DEBUG) && (EMB_LOG_LEVEL >= LOG_LEVEL_DEBUG) && !defined(NDEBUG)
#      define  LOG_DEBUG(...)      _EMB_LOG_O_(LOG_LEVEL_DEBUG, __VA_ARGS__)
#elif !defined(LOG_DEBUG)
#      define  LOG_DEBUG(...)                       //!< 调试, 仅用于调试阶段
#endif
#  if !defined(LOG_VERB)  && (EMB_LOG_LEVEL >= LOG_LEVEL_VERB)
#      define  LOG_VERB(...)       _EMB_LOG_O_(LOG_LEVEL_VERB, __VA_ARGS__)
#elif !defined(LOG_VERB)
#      define  LOG_VERB(...)                        //!< 详细, 记录 详细的信息
#endif


/*******************************************************************************
 * @}
 * @addtogroup                  Pravate_Macros
 * @{
 ******************************************************************************/

#ifndef EMB_LOG_OUT                                 //!< (C) 日志格式化输出宏
#define EMB_LOG_OUT(level, tag, file, func, line, ...)  /*lint -save -w1 */             \
do{ EMB_LOG_PRINT(  (  (EMB_LOG_COLOR == 0        ) ?              " %c [%010lu][%9s]"  \
                     : ((level) == LOG_LEVEL_ERROR) ? "\033[40;31m"" %c [%010lu][%9s]"  \
                     : ((level) == LOG_LEVEL_WARN ) ? "\033[40;33m"" %c [%010lu][%9s]"  \
                     : ((level) == LOG_LEVEL_INFO ) ? "\033[40;36m"" %c [%010lu][%9s]"  \
                     : ((level) == LOG_LEVEL_DEBUG) ? "\033[47;30m"" %c [%010lu][%9s]"  \
                     : ((level) == LOG_LEVEL_VERB ) ? "\033[40;32m"" %c [%010lu][%9s]"  \
                     :                                "\033[41;37m"" %c [%010lu][%9s]"  \
                    )                                                                   \
                  , (char)(  ((level) == LOG_LEVEL_ERROR) ? 'E'                         \
                           : ((level) == LOG_LEVEL_WARN ) ? 'W'                         \
                           : ((level) == LOG_LEVEL_INFO ) ? 'I'                         \
                           : ((level) == LOG_LEVEL_DEBUG) ? 'D'                         \
                           : ((level) == LOG_LEVEL_VERB ) ? 'V' : 'A'                   \
                          )                                                             \
                  , (unsigned long)EMB_LOG_GET_MSEC(), (const char *)(tag)              \
                 );                                                                     \
    if (((level) == LOG_LEVEL_VERB) || ((level) == LOG_LEVEL_INFO)) {                   \
        EMB_LOG_PRINT(       " : ");                                                    \
    } else {                                                                            \
        EMB_LOG_PRINT("[%s:%u] : ", (const char *)(file), (unsigned)(line));            \
    }                                                                                   \
    EMB_LOG_PRINT(__VA_ARGS__);                                                         \
    EMB_LOG_PRINT((EMB_LOG_COLOR == 0) ? ENDL : "\033[0m"ENDL);                         \
} while(0)                                              /*lint -restore */
#endif

#define _EMB_LOG_O_(level, ...) EMB_LOG_OUT(  level, LOG_TAG                   \
                                            , __FILE__, __FUNCTION__, __LINE__ \
                                            , __VA_ARGS__                      \
                                           )


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_LOG_H__
/*****************************  END OF FILE  **********************************/

