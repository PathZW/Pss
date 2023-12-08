/*******************************************************************************
 * @file    defines.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.5
 * @date    2019/11/5
 * @brief   Common defines file.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2014/10/29 -- Linghu -- the first version
 *
 * @par Change Logs:
 *      2015/5/11 -- Linghu -- Add defines of TCHAR
 *
 * @par Change Logs:
 *      2016/11/17 -- Linghu -- Add defines of token paste
 *
 * @par Change Logs:
 *      2018/3/14 -- Linghu -- Add defines of compiler related
 *
 * @par Change Logs:
 *      2019/5/20 -- Linghu -- Add define "__CONST" and compiler warning message
 *
 * @par Change Logs:
 *      2019/11/5 -- Linghu -- Add define "ENDIANNESS"
 *
 * @par Change Logs:
 *      2023/4/8 -- Linghu -- Add macro of the float compare
 ******************************************************************************/
#ifndef __DEFINES_H__
#define __DEFINES_H__
/******************************************************************************/

#ifdef   APP_CFG
#include APP_CFG        // Apps configuration
#endif

#if defined(_WIN32)
#define DEFINED_WIN32   1
#else
#define DEFINED_WIN32   0
#endif

#include <stdint.h>     // Used: intX_t, uintX_t, int_fast8_t, uint_fast8_t
#include <stdbool.h>    // Used: bool, true, false
#include <float.h>      // Used: FLT_EPSILON, DBL_EPSILON


/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          _Defines
 * @{
 ******************************************************************************/

#ifndef ENDL
#define ENDL                          "\r\n"            //!< (C) End of line, start the new line
#endif

#ifndef DEFINES_ARG_CHECK_EN
#define DEFINES_ARG_CHECK_EN            1               //!< (C) �Ƿ�����������
#endif

#ifndef DEFINES_WARNING_EN
#define DEFINES_WARNING_EN              1               //!< (C) �Ƿ�������뾯����Ϣ
#endif

#ifndef DEFINES_INTEGER_TYPES
#define DEFINES_INTEGER_TYPES           1               //!< (C) �Ƿ���[INTxx]��������
#endif

#ifndef DEFINES_WORD_TYPES
#define DEFINES_WORD_TYPES              1               //!< (C) �Ƿ���[BYTE/WORD/DWORD]����
#endif

#ifndef DEFINES_BOOLEAN_TYPES
#define DEFINES_BOOLEAN_TYPES   (DEFINED_WIN32 == 0)    //!< (C) �Ƿ���[BOOL]����
#endif

#ifndef DEFINES_TCHAR_TYPES
#define DEFINES_TCHAR_TYPES     (DEFINED_WIN32 == 0)    //!< (C) �Ƿ���[TCHAR]����
#endif

#ifndef DEFINES_TOKEN_PASTE
#define DEFINES_TOKEN_PASTE             1               //!< (C) �Ƿ���[����(token)����]��
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Defines
 * @{
 * @addtogroup          Defines_Base
 * @{
 ******************************************************************************/
/*
 * ������������
 */
typedef uint_fast8_t           uintw_t;             //!< �޷�������(CPUλ��)
typedef  int_fast8_t            intw_t;             //!< �з�������(CPUλ��)
typedef uintw_t                 INTWU;              //!< �޷�������(CPUλ��)
typedef  intw_t                 INTWS;              //!< �з�������(CPUλ��)

#if (DEFINES_INTEGER_TYPES)
typedef uint8_t                 INT8U;              //!< �޷�������(8λ)
typedef  int8_t                 INT8S;              //!< �з�������(8λ)
typedef uint16_t                INT16U;             //!< �޷�������(16λ)
typedef  int16_t                INT16S;             //!< �з�������(16λ)
typedef uint32_t                INT32U;             //!< �޷�������(32λ)
typedef  int32_t                INT32S;             //!< �з�������(32λ)
#endif

#ifndef BASETYPES
#define BASETYPES
typedef unsigned long           ULONG , *PULONG;
typedef unsigned int            UINT  , *PUINT;
typedef unsigned short          USHORT, *PUSHORT;
typedef unsigned char           UCHAR , *PUCHAR;
typedef char                   *PSZ;
#endif

#if (DEFINES_WORD_TYPES)
typedef uint8_t                 BYTE;
typedef uint16_t                WORD;
typedef uint32_t                DWORD;
#endif

#if (DEFINES_BOOLEAN_TYPES)
typedef bool                    BOOL;
#endif  /*
#ifndef BOOLEAN
#define BOOLEAN                 BOOL
#endif  */
#ifndef TRUE
#define TRUE                    true
#endif
#ifndef FALSE
#define FALSE                   false
#endif


/*******************************************************************************
 * @}
 * @addtogroup          Defines_Macros
 * @{
 ******************************************************************************/
//lint -emacro(506, MIN, MAX)           // Constant value Boolean
//lint -emacro(506, IS_POWER_OF_TWO)    // Constant value Boolean
//lint -emacro(774, IS_POWER_OF_TWO)    // Boolean within 'xx' always [True/False]
//lint -emacro(778, IS_POWER_OF_TWO)    // Constant expression evaluates to 0
//lint -emacro( 835 , IS_POWER_OF_TWO)  // A zero has been given to operator 'x'
//lint -emacro((845), IS_POWER_OF_TWO)  // A zero has been given to operator 'x'
//lint -emacro(506, SWAP)               // Constant value Boolean
//lint -emacro(717, SWAP)               // do ... while(0)
//lint -emacro(774, SWAP)               // Boolean within 'xx' always [True/False]
/*
 * ͨ�����ݲ���
 */
#ifndef MIN
#define MIN(a, b)        (((a) < (b)) ? (a) : (b))  //!< ���� a, b ��Сֵ
#endif
#ifndef MAX
#define MAX(a, b)        (((a) > (b)) ? (a) : (b))  //!< ���� a, b ���ֵ
#endif  /*
#ifndef min
#define min(a, b)           MIN((a), (b))
#endif
#ifndef max
#define max(a, b)           MAX((a), (b))
#endif  */

#ifndef IS_POWER_OF_TWO
#define IS_POWER_OF_TWO(n)  (((n) & ((n)-1)) == 0)  //!< (true): n Ϊ 2 ���ݴη�
#endif

#ifndef SWAP                                        // Note: ���� a �� b ��ַ������ͬ
#define SWAP(a, b)  do{if(DEFINES_ARG_CHECK_EN && ((a)!=(b))){(a)^=(b); (b)^=(a); (a)^=(b);}}while(0)
#endif

#ifndef FLT_EPSILON
#define FLT_EPSILON      1e-6f
#endif
#define FLT_AEQU(x, y)  (fabs((x) - (y)) < ( FLT_EPSILON))  // Լ �� �� (x == y)
#define FLT_MORE(x, y)  (    ((x) - (y)) > ( FLT_EPSILON))  // ��    �� (x >  y)
#define FLT_MEQU(x, y)  (    ((x) - (y)) > (-FLT_EPSILON))  // ���ڵ��� (x >= y)
#define FLT_LESS(x, y)  (    ((x) - (y)) < (-FLT_EPSILON))  // С    �� (x <  y)
#define FLT_LEQU(x, y)  (    ((x) - (y)) < ( FLT_EPSILON))  // С�ڵ��� (x <= y)
#ifndef DBL_EPSILON
#define DBL_EPSILON      1e-14
#endif
#define DBL_AEQU(x, y)  (fabs((x) - (y)) < ( DBL_EPSILON))  // Լ �� �� (x == y)
#define DBL_MORE(x, y)  (    ((x) - (y)) > ( DBL_EPSILON))  // ��    �� (x >  y)
#define DBL_MEQU(x, y)  (    ((x) - (y)) > (-DBL_EPSILON))  // ���ڵ��� (x >= y)
#define DBL_LESS(x, y)  (    ((x) - (y)) < (-DBL_EPSILON))  // С    �� (x <  y)
#define DBL_LEQU(x, y)  (    ((x) - (y)) < ( DBL_EPSILON))  // С�ڵ��� (x <= y)

/*----------------------------------------------------------------------------*/
//lint -emacro(835, BIT_GET, BIT_SET, BIT_CLR, BIT_MDF, REG_MDF)
//lint -emacro(845, BIT_GET, BIT_SET, BIT_CLR, BIT_MDF, REG_MDF)
/*
 * λ��Ĵ�������
 */
#ifndef BIT_GET
#define BIT_GET(m, bit)       ((m) &   (bit))               //!< λ��ȡ
#endif
#ifndef BIT_SET
#define BIT_SET(m, bit)       ((m) |=  (bit))               //!< λ����
#endif
#ifndef BIT_CLR
#define BIT_CLR(m, bit)       ((m) &= ~(bit))               //!< λ���
#endif
#ifndef BIT_MDF
#define BIT_MDF(m, clr, set)  ((m) = (((m)& ~(clr))|(set))) //!< λ�޸�
#endif

#ifndef REG_GET
#define REG_GET(reg)           (reg)                        //!< ��ȡ�Ĵ���ֵ
#endif
#ifndef REG_SET
#define REG_SET(reg, val)     ((reg) = (val))               //!< ���üĴ���ֵ
#endif
#ifndef REG_MDF
#define REG_MDF(r, clr, set)  ((r) = (((r)& ~(clr))|(set))) //!< �޸ļĴ���ָ��λ
#endif

/*----------------------------------------------------------------------------*/
//lint -emacro(835, LOBYTE, HIBYTE, LOWORD, HIWORD)
//lint -emacro(845, LOBYTE, HIBYTE, LOWORD, HIWORD)
//lint -emacro( 835 , MAKEWORD, MAKEDWORD, MAKELONG)
//lint -emacro((845), MAKEWORD, MAKEDWORD, MAKELONG)
/*
 * ��������
 */
#ifndef LOBYTE
#define LOBYTE(m)   ((uint8_t)(((uint16_t)(m)) >> 0))   //!< ����16λ���ĵ��ֽ�ֵ
#endif
#ifndef HIBYTE
#define HIBYTE(m)   ((uint8_t)(((uint16_t)(m)) >> 8))   //!< ����16λ���ĸ��ֽ�ֵ
#endif
#ifndef LOWORD
#define LOWORD(m)   ((uint16_t)(((uint32_t)(m)) >>  0)) //!< ����32λ���ĵ�16λֵ
#endif
#ifndef HIWORD
#define HIWORD(m)   ((uint16_t)(((uint32_t)(m)) >> 16)) //!< ����32λ���ĸ�16λֵ
#endif

#ifndef MAKEWORD
#define MAKEWORD(l, h)      ((uint16_t)(    /*!< ���� 8Bit �ϲ���һ�� 16Bit */  \
                                          ((0xFF & ((uint16_t)(l))) << 0)      \
                                        | ((0xFF & ((uint16_t)(h))) << 8)      \
                                       ) )
#endif
#ifndef MAKEDWORD
#define MAKEDWORD(l, h)     ((uint32_t)(    /*!< ���� 16Bit �ϲ���һ�� 32Bit */ \
                                          ((0xFFFF & ((uint32_t)(l))) <<  0)   \
                                        | ((0xFFFF & ((uint32_t)(h))) << 16)   \
                                       ) )
#endif
#ifndef MAKELONG
#define MAKELONG(l, h)      ((int32_t)(     /*!< ���� 16Bit �ϲ���һ�� 32Bit */ \
                                         ((0xFFFF & ((uint32_t)(l))) <<  0)    \
                                       | ((0xFFFF & ((uint32_t)(h))) << 16)    \
                                      ) )
#endif

/*----------------------------------------------------------------------------*/
//lint -emacro((826), ENDIANNESS)   // Suspicious pointer-to-pointer conversion
//lint -emacro((413), offsetof)     // Likely use of null pointer
/*
 * ����
 */
#ifndef ENDIANNESS
#define ENDIANNESS              /*!< �����ֽ�˳��: (== 'l')С��, (== 'b')��� */ \
                                ((char)(*(uint32_t __CONST *)("l\?\?b")))
#endif

#ifndef ARRAY_SIZE              // Note: ��������Ϊ����
#define ARRAY_SIZE(a)           (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef offsetof                // Note: member ����Ϊ type ���ͳ�Ա
#define offsetof(type, member)  ((size_t) &((type *)0) -> member)
#endif

#ifndef container_of            // Note: p ���ͱ���Ϊ m ��ָ��
#define container_of(p, t, m)   ((t*)((char *)p - offsetof(t, m)))
#endif


/*******************************************************************************
 * @}
 * @addtogroup          Defines_Token_Paste
 * @{
 ******************************************************************************/
/*
 * ����(token)����
 */
#if (DEFINES_TOKEN_PASTE)
#define ATPASTE2(a, b)                       TPASTE2( a, b)
#define ATPASTE3(a, b, c)                    TPASTE3( a, b, c)
#define ATPASTE4(a, b, c, d)                 TPASTE4( a, b, c, d)
#define ATPASTE5(a, b, c, d, e)              TPASTE5( a, b, c, d, e)
#define ATPASTE6(a, b, c, d, e, f)           TPASTE6( a, b, c, d, e, f)
#define ATPASTE7(a, b, c, d, e, f, g)        TPASTE7( a, b, c, d, e, f, g)
#define ATPASTE8(a, b, c, d, e, f, g, h)     TPASTE8( a, b, c, d, e, f, g, h)
#define ATPASTE9(a, b, c, d, e, f, g, h, i)  TPASTE9( a, b, c, d, e, f, g, h, i)

#define TPASTE2(a, b)                        a##b
#define TPASTE3(a, b, c)                     a##b##c
#define TPASTE4(a, b, c, d)                  a##b##c##d
#define TPASTE5(a, b, c, d, e)               a##b##c##d##e
#define TPASTE6(a, b, c, d, e, f)            a##b##c##d##e##f
#define TPASTE7(a, b, c, d, e, f, g)         a##b##c##d##e##f##g
#define TPASTE8(a, b, c, d, e, f, g, h)      a##b##c##d##e##f##g##h
#define TPASTE9(a, b, c, d, e, f, g, h, i)   a##b##c##d##e##f##g##h##i
#endif  // DEFINES_TOKEN_PASTE


/*******************************************************************************
 * @}
 * @addtogroup          Defines_Compiler
 * @{
 ******************************************************************************/
/*
 * �洢����
 */
#ifndef __FAR
#define __FAR                                               //!< Զ��ַ�ռ�
#endif
#ifndef __NEAR
#define __NEAR                                              //!< ����ַ�ռ�
#endif
#ifndef __CONST
#define __CONST                 const                       //!< ֻ����ַ�ռ�
#endif
#ifndef __CODE
#define __CODE                                              //!< Code ��ַ�ռ�
#endif
#ifndef __FUN
#define __FUN                                               //!< �����������ιؼ���
#endif

#if defined(__CC_ARM)
/*------------------------------------------------------------------------------
 * Arm Compiler 4/5
 */
#ifndef UNUSED_VAR              // ((void)sizeof((void)(x),0))
#define UNUSED_VAR(x)           ((void)(x))                 //!< ���Ʊ���δʹ�þ���
#endif
#ifndef __MEMORY_AT                                         // after
#define __MEMORY_AT(addr)       __attribute__((at(addr)))   //!< ���Ե�ַ��λ
#endif
#ifndef __SECTION                                           // after
#define __SECTION(x)            __attribute__((section(x))) //!< ����/��������ĳ��
#endif
#ifndef __ALIGN                                             // before
#define __ALIGN(n)              __attribute__((aligned(n))) //!< �ڴ����
#endif
#ifndef __UNUSED                                            // before
#define __UNUSED                __attribute__((unused))     //!< ���ƺ���δʹ�þ���
#endif
#ifndef __USED                  //lint ++d__USED=           // before
#define __USED                  __attribute__((used))       //!< ����δʹ�õı�������
#endif
#ifndef __WEAK                  //lint ++d__WEAK=static     // before
#define __WEAK                  __attribute__((weak))       //!< ��������������
#endif
#ifndef __INLINE                                            // before
#define __INLINE                __inline                    //!< ��������
#endif
#ifndef __NO_RETURN                                         // before
#define __NO_RETURN             __declspec(noreturn)        //!< ����������
#endif

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
/*------------------------------------------------------------------------------
 * Arm Compiler 6 (armclang)
 */
#ifndef UNUSED_VAR              // ((void)sizeof((void)(x),0))
#define UNUSED_VAR(x)           ((void)(x))                 //!< ���Ʊ���δʹ�þ���
#endif
#ifndef __MEMORY_AT                                         // after
#define __MEMORY_AT(addr)       __attribute__((section(".ARM.__AT_"TO_STRING(addr))))
#endif
#ifndef __SECTION                                           // after
#define __SECTION(x)            __attribute__((section(x))) //!< ����/��������ĳ��
#endif
#ifndef __ALIGN                                             // before
#define __ALIGN(n)              __attribute__((aligned(n))) //!< �ڴ����
#endif
#ifndef __UNUSED                                            // before
#define __UNUSED                __attribute__((unused))     //!< ���ƺ���δʹ�þ���
#endif
#ifndef __USED                                              // before
#define __USED                  __attribute__((used))       //!< ����δʹ�õı�������
#endif
#ifndef __WEAK                                              // before
#define __WEAK                  __attribute__((weak))       //!< ��������������
#endif
#ifndef __INLINE                                            // before
#define __INLINE                __inline                    //!< ��������
#endif
#ifndef __NO_RETURN                                         // before
#define __NO_RETURN         __attribute__((__noreturn__))   //!< ����������
#endif

#elif defined(__GNUC__)
/*------------------------------------------------------------------------------
 * GNU GCC Compiler
 */
#if (DEFINES_WARNING_EN)
#warning "__MEMORY_AT() requires place \"__AT_0xXXXXXXXX\" to relevant address in the \".lds\" file!"
#endif
#ifndef UNUSED_VAR              // ((void)sizeof((void)(x),0))
#define UNUSED_VAR(x)           ((void)(x))                 //!< ���Ʊ���δʹ�þ���
#endif
#ifndef __MEMORY_AT                                         // after
#define __MEMORY_AT(addr)       __attribute__((section(".__AT_"TO_STRING(addr))))
#endif
#ifndef __SECTION                                           // after
#define __SECTION(x)            __attribute__((section(x))) //!< ����/��������ĳ��
#endif
#ifndef __ALIGN                                             // before
#define __ALIGN(n)              __attribute__((aligned(n))) //!< �ڴ����
#endif
#ifndef __UNUSED                                            // before
#define __UNUSED                __attribute__((unused))     //!< ���ƺ���δʹ�þ���
#endif
#ifndef __USED                                              // before
#define __USED                  __attribute__((used))       //!< ����δʹ�õı�������
#endif
#ifndef __WEAK                                              // before
#define __WEAK                  __attribute__((weak))       //!< ��������������
#endif
#ifndef __INLINE                                            // before
#define __INLINE                __inline                    //!< ��������
#endif
#ifndef __NO_RETURN                                         // before
#define __NO_RETURN         __attribute__((__noreturn__))   //!< ����������
#endif

#elif defined(__IAR_SYSTEMS_ICC__)
/*------------------------------------------------------------------------------
 * IAR Compiler
 */
#if (DEFINES_WARNING_EN)
#warning "Not supported: __UNUSED !!!"
#endif
#ifndef UNUSED_VAR              // ((void)sizeof((void)(x),0))
#define UNUSED_VAR(x)           ((void)(x))                 //!< ���Ʊ���δʹ�þ���
#endif
#ifndef __MEMORY_AT                                         // after
#define __MEMORY_AT(addr)       __SECTION(TO_STRING(addr))  //!< ���Ե�ַ��λ
#endif
#ifndef __SECTION                                           // after
#define __SECTION(x)            @ x                         //!< ����/��������ĳ��
#endif
#ifndef __ALIGN                                             // before
#define __ALIGN(n)              __PRAGMA(data_alignment=n)  //!< �ڴ����
#endif
#ifndef __UNUSED                                            // before
#define __UNUSED                                            //!< ���ƺ���δʹ�þ���
#endif
#ifndef __USED                                              // before
#define __USED                  __root                      //!< ����δʹ�õı�������
#endif
#ifndef __WEAK                                              // before
#define __WEAK                  __weak                      //!< ��������������
#endif
#ifndef __INLINE                                            // before
#define __INLINE                inline                      //!< ��������
#endif
#ifndef __NO_RETURN                                         // before
#define __NO_RETURN             __noreturn                  //!< ����������
#endif
#ifndef __PRAGMA
#define __PRAGMA(x)             _Pragma(#x)
#endif

#elif defined(_MSC_VER)
/*------------------------------------------------------------------------------
 * Microsoft VC Compiler
 */
#if (DEFINES_WARNING_EN)
#warning "Not supported: __MEMORY_AT(), __SECTION(), __UNUSED, __USED, __WEAK, __NO_RETURN !!!"
#endif
#ifndef UNUSED_VAR              // ((void)sizeof((void)(x),0))
#define UNUSED_VAR(x)           ((void)(x))                 //!< ���Ʊ���δʹ�þ���
#endif
#ifndef __MEMORY_AT                                         // after
#define __MEMORY_AT(addr)                                   //!< ���Ե�ַ��λ
#endif
#ifndef __SECTION                                           // after
#define __SECTION(x)                                        //!< ����/��������ĳ��
#endif
#ifndef __ALIGN                                             // before
#define __ALIGN(n)              __declspec(align(n))        //!< �ڴ����
#endif
#ifndef __UNUSED                                            // before
#define __UNUSED                                            //!< ���ƺ���δʹ�þ���
#endif
#ifndef __USED                                              // before
#define __USED                                              //!< ����δʹ�õı�������
#endif
#ifndef __WEAK                                              // before
#define __WEAK                                              //!< ��������������
#endif
#ifndef __INLINE                                            // before
#define __INLINE                __inline                    //!< ��������
#endif
#ifndef __NO_RETURN                                         // before
#define __NO_RETURN                                         //!< ����������
#endif

#else
/*------------------------------------------------------------------------------
 * Not supported Compiler
 */
#error "Not supported tool chain"
#endif


/*******************************************************************************
 * @}
 * @addtogroup          Defines_TCHAR
 * @{
 ******************************************************************************/

#if (DEFINES_TCHAR_TYPES)
#if defined(UNICODE) || defined(_UNICODE)
#error  "Does not support wide character set!"
#endif

typedef         char              CHAR; /*
typedef         wchar_t          WCHAR; */
typedef         char             TCHAR;

typedef         char         *    PSTR;
typedef         char  __FAR  *   LPSTR;
typedef         char  __NEAR *   NPSTR;
typedef __CONST char         *   PCSTR;
typedef __CONST char  __FAR  *  LPCSTR; /*
typedef __CONST char  __NEAR *  NPCSTR; */

typedef         TCHAR        *   PTSTR;
typedef         TCHAR __FAR  *  LPTSTR; /*
typedef         TCHAR __NEAR *  NPTSTR; */
typedef __CONST TCHAR        *  PCTSTR;
typedef __CONST TCHAR __FAR  * LPCTSTR; /*
typedef __CONST TCHAR __NEAR * NPCTSTR; */
#endif  // DEFINES_TCHAR_TYPES

typedef __CONST char  __NEAR *  NPCSTR;
typedef         TCHAR __NEAR *  NPTSTR;
typedef __CONST TCHAR __NEAR * NPCTSTR;

#ifndef __T
#define __T(x)                    x  // L##x
#endif
#ifndef _T
#define _T(x)                   __T(x)              //!< TCHAR �ַ���
#endif
#ifndef __TEXT
#define __TEXT(x)               __T(x)
#endif
#ifndef _TEXT
#define _TEXT(x)                __T(x)              //!< TCHAR �ַ���
#endif
#ifndef TEXT
#define TEXT(x)                 __T(x)              //!< TCHAR �ַ���
#endif

#ifndef TO_STRING
#define TO_STRING(x)            TO_STRIN1(x)        //!< �ַ�����
#define TO_STRIN1(x)            #x
#endif

#if 0
#ifndef TO_CHAR
#define TO_CHAR(x)              TO_CHA1(x)          //!< ���ַ���
#define TO_CHA1(x)              #@x
#endif
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DEFINES_H__
/*****************************  END OF FILE  **********************************/

