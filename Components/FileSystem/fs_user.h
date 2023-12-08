/*******************************************************************************
 * @file    fs_user.h
 * @author  Linghu
 * @version v1.0.0
 * @date    2018/12/14
 * @brief   User define from file system.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2018/12/14 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __FS_USER_H__
#define __FS_USER_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup FS_USER
 * @{
 * @addtogroup          FS_USER_Exported_Functions
 * @{
 ******************************************************************************/

void fs_init(void);
int  fs_mkfs(const char* device, const char* opt, void* data, unsigned len);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __FS_USER_H__
/*****************************  END OF FILE  **********************************/

