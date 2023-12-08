/*******************************************************************************
 * @file    version_def.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/8/12
 * @brief   Version information definition.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/8/12 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __VERSION_DEF_H__
#define __VERSION_DEF_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
//lint -save
//lint -e27     // Illegal character
//lint -e750    // local macro 'xxxx' not referenced
//lint -e773    // Expression-like macro 'xxx' not parenthesized
/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          Version
 * @{
 ******************************************************************************/

#ifndef VER_MAJOR
#define VER_MAJOR           $WCMAJOR$       //!< ���汾��, eg: 1
#endif
#ifndef VER_MINOR
#define VER_MINOR           $WCMINOR$       //!< �ΰ汾��, eg: 0
#endif
#ifndef VER_BUILD
#define VER_BUILD           $WCBUILD$       //!< �� �� ��, eg: 0
#endif

/*----------------------------------------------------------------------------*/
#ifndef VER_REVISION
#define VER_REVISION        $WCREV$         //!< ���������е���߰汾��, eg: 200
#endif

#ifndef VER_COMMIT_DATE
#define VER_COMMIT_DATE "$WCDATE=%Y%m%d$"   //!< ��߰汾���ύ����, eg: "20190801"
#endif
#ifndef VER_COMMIT_TIME
#define VER_COMMIT_TIME "$WCDATE=%H:%M:%S$" //!< ��߰汾���ύʱ��, eg: "15:08:06"
#endif

#ifndef VER_CURRENT_DATE
#define VER_CURRENT_DATE "$WCNOW=%Y%m%d$"   //!< ����ʱ��ϵͳ����, eg: "20190802"
#endif
#ifndef VER_CURRENT_TIME
#define VER_CURRENT_TIME "$WCNOW=%H:%M:%S$" //!< ����ʱ��ϵͳʱ��, eg: "08:10:32"
#endif

#ifndef VER_LOCAL_MODS
#define VER_LOCAL_MODS      $WCMODS?1:0$    //!< ���ع��������Ƿ��޸�, eg: 0
#endif
#ifndef VER_UNVER_ITEM
#define VER_UNVER_ITEM      $WCUNVER?1:0$   //!< ����δ�汾������ļ�, eg: 1
#endif

#ifndef VER_REPOSITORY_URL
#define VER_REPOSITORY_URL  "$WCURL$"       //!< ���ַ, eg: "svn/BI_COCBunrIn/"
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
//lint -restore
#ifdef  __cplusplus
}
#endif
#endif  // __VERSION_DEF_H__
/*****************************  END OF FILE  **********************************/

