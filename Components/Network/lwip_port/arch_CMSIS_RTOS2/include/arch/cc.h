/*******************************************************************************
 * @file    cc.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/13
 * @brief   CPU and Compiler
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/11/13 -- Linghu -- the first version
 ******************************************************************************/
#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H
/******************************************************************************/

#ifndef  LOG_TAG
#define  LOG_TAG  "lwIP"
#endif
#include "emb_log.h"

#include "cmsis_compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
//lint -emacro(506, LWIP_PLATFORM_DIAG, LWIP_PLATFORM_ASSERT, LWIP_PLATFORM_ERROR, LWIP_ERROR, LWIP_ASSERT)
//lint -emacro(534, LWIP_PLATFORM_DIAG, LWIP_PLATFORM_ASSERT, LWIP_PLATFORM_ERROR, LWIP_ERROR, LWIP_ASSERT)
//lint -emacro(717, LWIP_PLATFORM_DIAG, LWIP_PLATFORM_ASSERT, LWIP_PLATFORM_ERROR, LWIP_ERROR, LWIP_ASSERT)
//lint -emacro(774, LWIP_PLATFORM_DIAG, LWIP_PLATFORM_ASSERT, LWIP_PLATFORM_ERROR, LWIP_ERROR, LWIP_ASSERT)

// Platform specific assertion handling
#define LWIP_PLATFORM_ASSERT(x)     LOG_ASSERT(x)

// if "exp" isn't true, then print "msg" and execute "hand" expression
#ifdef  LWIP_DEBUG
#define LWIP_PLATFORM_ERROR(msg)    LOG_WARN(msg)
#else
#define LWIP_PLATFORM_ERROR(msg)
#endif
#define LWIP_ERROR(msg, exp, hand)  do{if(!(exp)){LWIP_PLATFORM_ERROR(msg); hand;}}while(0)

// Plaform specific diagnostic output
#ifdef  EMB_LOG_PRINT
#define LWIP_PLATFORM_DIAG(x)       do{ EMB_LOG_PRINT x; }while(0)
#else
#define LWIP_PLATFORM_DIAG(x)       do{ printf x; }while(0)
#endif

/*----------------------------------------------------------------------------*/
// Define platform ERROR type
#define LWIP_ERR_T                  int

// Define platform endianness (might already be defined)
#define BYTE_ORDER                  LITTLE_ENDIAN

// Define random number generator function of your system
#define LWIP_RAND()                 ((u32_t)rand())

// Compiler provide the stdint.h header ?
#define LWIP_NO_STDDEF_H            0

// Compiler provide the inttypes.h header ?
#define LWIP_NO_INTTYPES_H          0

// Define (sn)printf formatters for these lwIP types
#if 0
#define X8_F                        "02x"
#define U16_F                       "u"
#define S16_F                       "d"
#define X16_F                       "x"
#define U32_F                       "u"
#define S32_F                       "d"
#define X32_F                       "x"
#define SZT_F                       "u"
#endif

// Compiler provide the limits.h header ?
#define LWIP_NO_LIMITS_H            0

// Do we need to define ssize_t and SSIZE_MAX ?
#if 0
#define SSIZE_MAX                INT_MAX
#endif

// Compiler provide the ctype.h header ?
#define LWIP_NO_CTYPE_H             0

// Convert host to network byte order
#if defined(BYTE_ORDER) && (BYTE_ORDER == LITTLE_ENDIAN)
#define lwip_htonl(n32)             __REV(n32)
#define lwip_htons(n16)             __ROR(__REV((n16) & 0xFFFF), 16)
  #ifdef _lint
  #undef  __REV
  #define __REV(n32)    (n32)
  #undef  __ROR
  #define __ROR(n32, m) (n32)
  #endif
#endif  // defined(BYTE_ORDER) && (BYTE_ORDER == LITTLE_ENDIAN)


/******************************************************************************/
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) //--- ARM Compiler -----------

// Compiler hints for packing structures
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#define PACK_STRUCT_STRUCT          __attribute__((packed))
#define PACK_STRUCT_FIELD(x)        x

// if you want to include <errno.h> ?
#if 0
#define LWIP_ERRNO_STDINCLUDE
#endif

// Define an equivalent of <errno.h> to include ?
#if 0
#define LWIP_ERRNO_INCLUDE          "cc_errno.h"
#endif

// Let lwIP provide ERRNO values and the 'errno' variable
#define LWIP_PROVIDE_ERRNO
//lint +fem         // Allow Early Modifiers Flag
#include <errno.h>
#undef  EDOM        // todo: 对于 ARM 编译器, 这几个 定义与std库 不一致
#undef  ERANGE
#undef  EILSEQ
#undef  ESIGNUM
#undef  EINVAL
#undef  ENOMEM

// This can be #defined to itoa() or snprintf((buff), (size), "%d", (num))
#if 0
#define lwip_itoa(buff, size, n)    (void)snprintf((buff), (size), "%d", (n))
#endif

// This can be #defined to strnicmp() or strncasecmp()
#define lwip_strnicmp(s1, s2, n)    strncasecmp((s1), (s2), (n))

// This can be #defined to stricmp() or strcasecmp()
#define lwip_stricmp(s1, s2)        strcasecmp((s1), (s2))

// This can be #defined to strnstr()
#if 0
#define lwip_strnstr(s1, s2, n)     strnstr((s1), (s2), (n))
#endif

#elif defined(__ICCARM__)   //------------------------- IAR Compiler -----------
#error "not supported compiler"

#elif defined(__GNUC__)     //------------------------- GNU Compiler -----------
#error "not supported compiler"

#else                       //------------------------- Unknow Compiler --------
#error "not supported compiler"
#endif  // defined(__CC_ARM) || defined(__ARMCC_VERSION)


/*****************************  END OF FILE  **********************************/
#ifdef  __cplusplus
}
#endif
#endif  // LWIP_ARCH_CC_H
/*****************************  END OF FILE  **********************************/

