/*******************************************************************************
 * @file    stdio_Retarget.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/8/2
 * @brief   Device's defines for stdio re-target
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/8/2 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __STDIO_RETARGET_H__
#define __STDIO_RETARGET_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup STDIO
 * @{
 * @addtogroup          STDIO_Exported_Functions
 * @{
 ******************************************************************************/
/**
 * Initialize the stdio retarget (C RunTime Uninitialized)
 */
void stdio_Retarget(void);

/**
 * Get a character from stdin.
 *
 * @return  The character from the input, or -1 on read error.
 */
int stdin_getchar(void);

/**
 * Put a character to the stdout.
 *
 * @param[in]  ch   Character to output. 
 *
 * @return  The character written, or -1 on write error.
 */
int stdout_putchar(int ch);

/**
 * Put a character to the stderr.
 *
 * @param[in]  ch   Character to output. 
 *
 * @return  The character written, or -1 on write error.
 */
int stderr_putchar(int ch);

/**
 * Put a character to the teletypewritter.
 *
 * @param[in]  ch   Character to output.
 */
void ttywrch(int ch);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __STDIO_RETARGET_H__
/*****************************  END OF FILE  **********************************/

