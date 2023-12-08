/*******************************************************************************
 * @file    emb_critical.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/4/26
 * @brief   Embedded platform defines for the critical-section
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/8/2 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __EMB_CRITICAL_H__
#define __EMB_CRITICAL_H__
/******************************************************************************/
#include <stdint.h>
#include "cmsis_compiler.h"
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
//lint ++dCRITICAL_SECTION_ALLOC()
//lint ++dCRITICAL_SECTION_ENTER()
//lint ++dCRITICAL_SECTION_EXIT()

#ifndef CRITICAL_SECTION_ALLOC
#define CRITICAL_SECTION_ALLOC()  uint32_t  __cpu_sr
#endif

#ifndef CRITICAL_SECTION_ENTER
#define CRITICAL_SECTION_ENTER()  ((__cpu_sr = __get_PRIMASK()), __disable_irq())
#endif

#ifndef CRITICAL_SECTION_EXIT
#define CRITICAL_SECTION_EXIT()   (__set_PRIMASK(__cpu_sr))
#endif


/*****************************  END OF FILE  **********************************/
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_CRITICAL_H__
/*****************************  END OF FILE  **********************************/

