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
#define VER_MAJOR           $WCMAJOR$       //!< 主版本号, eg: 1
#endif
#ifndef VER_MINOR
#define VER_MINOR           $WCMINOR$       //!< 次版本号, eg: 0
#endif
#ifndef VER_BUILD
#define VER_BUILD           $WCBUILD$       //!< 修 订 号, eg: 0
#endif

/*----------------------------------------------------------------------------*/
#ifndef VER_REVISION
#define VER_REVISION        $WCREV$         //!< 工作副本中的最高版本号, eg: 200
#endif

#ifndef VER_COMMIT_DATE
#define VER_COMMIT_DATE "$WCDATE=%Y%m%d$"   //!< 最高版本的提交日期, eg: "20190801"
#endif
#ifndef VER_COMMIT_TIME
#define VER_COMMIT_TIME "$WCDATE=%H:%M:%S$" //!< 最高版本的提交时间, eg: "15:08:06"
#endif

#ifndef VER_CURRENT_DATE
#define VER_CURRENT_DATE "$WCNOW=%Y%m%d$"   //!< 编译时的系统日期, eg: "20190802"
#endif
#ifndef VER_CURRENT_TIME
#define VER_CURRENT_TIME "$WCNOW=%H:%M:%S$" //!< 编译时的系统时间, eg: "08:10:32"
#endif

#ifndef VER_LOCAL_MODS
#define VER_LOCAL_MODS      $WCMODS?1:0$    //!< 本地工作副本是否修改, eg: 0
#endif
#ifndef VER_UNVER_ITEM
#define VER_UNVER_ITEM      $WCUNVER?1:0$   //!< 存在未版本管理的文件, eg: 1
#endif

#ifndef VER_REPOSITORY_URL
#define VER_REPOSITORY_URL  "$WCURL$"       //!< 库地址, eg: "svn/BI_COCBunrIn/"
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

