/*******************************************************************************
 * @file    cli_shell.c
 * @author  Linghu
 * @version v1.0.4
 * @date    2019/5/23
 * @brief   Command Line Interface of Shell application Module.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2022/8/17 -- Linghu -- Add Event Recorder Execution Statistics A() B()
 *
 * @par Change Logs:
 *      2022/8/12 -- Linghu -- Optimizing memmove() and memcpy() Usage
 *
 * @par Change Logs:
 *      2022/5/17 -- Linghu -- Add cli_strdelim() and cli_strsplitnum()
 *
 * @par Change Logs:
 *      2022/2/17 -- Linghu -- Fix Bug on (CLI_LINE_BUFF_SIZE > 512)
 *
 * @par Change Logs:
 *      2019/5/23 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -emacro((740), va_start, va_arg)
//lint ++d"va_start(ap, parmN) = ( *((char**)&(ap)) = ((char*)&(parmN) + sizeof(parmN)) )"
//lint ++d"va_arg(ap, type)    = ( *((*((type**)&(ap)))++) )"
//lint ++d"va_end(ap)          = ( (void)(ap) )"

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "CLI"
#include "emb_log.h"
#include "cli_shell.h"

#ifdef   _RTE_                  // CMSIS_device_header
#include "RTE_Components.h"     // Component selection
#endif// _RTE_
#ifdef    RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "assert.h"//lint !e451 // Header file repeatedly included
#include "defines.h"            // Used: MIN(), ARRAY_SIZE()

#include "emb_repeat.h"
#include "emb_delay.h"
#include "os_abstract.h"

#define CLI_EXTERN
#include "cli_shell_priv.h"


/*******************************************************************************
 * @addtogroup CLI_Shell
 * @{
 * @addtogroup          Pravate_Constants
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup          Pravate_Macros
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup          Pravate_Types
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup          Pravate_Variables
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup          Private_Prototypes
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup          Pravate_Functions
 * @{
 ******************************************************************************/
#if (CLI_THREAD_STKB > 0)
/**
 * 命令行处理线程函数
 *
 * @param[in]  arg  命令行接口ID(需强制类型转换)
 */
static void cli_shell_thread(void *arg)
{
    while (cli_shell_poll((int)arg) >= 0)
    {
        DelayMS(1);
    }
    cli_shell_free((int)arg);  //lint !e534  " Ignoring return value "

    OS_THREAD_EXIT();
}
#endif


/*----------------------------------------------------------------------------*/
/**
 * 命令行接口初始化
 */
void cli_shell_init(void)
{
    for(size_t cli = 1;  cli <= ARRAY_SIZE(_cli_if_tbl);  cli++)
    {
        CLI_IF_DATA_INIT(NULL, NULL, 0);    // 命令行接口数据初始化
        OS_MUTEX_CREATE(CLI_IF_I_LOCK, CLI_LOCK_NAME[0]);
        OS_MUTEX_CREATE(CLI_IF_O_LOCK, CLI_LOCK_NAME[1]);
        OS_MUTEX_CREATE(CLI_IF_LOCK,   CLI_LOCK_NAME[2]);
        CLI_IF_REFS =  0;
        CLI_IF_ID   = -1;                   // 命令行接口数据设置为未分配
    }
    OS_MUTEX_CREATE(_cli_if_mutex, "cli_if Mutex");

    cli_command_init();
}


/**
 * 创建命令行接口
 */
int cli_shell_new(const cli_device_t *device, const char *name, int flag)
{
    static int  _new_ops = 0;
    int         cli, retval;

    if (device == NULL) {
        return( -1 );                       // 无效的参数
    }
    if((device->open == NULL) || (device->close == NULL) || (device->read == NULL) || (device->write == NULL)) {
        return( -1 );                       // 无效的参数
    }

    for(size_t i = 0;  i < ARRAY_SIZE(_cli_if_tbl);  i++)
    {
        retval = -1;
        OS_MUTEX_ENTER(_cli_if_mutex, OS_WAIT_FOREVER)
        {
            cli = _new_ops + 1;
            OS_MUTEX_ENTER(CLI_IF_LOCK, OS_WAIT_FOREVER)
            {
                if (CLI_IF_ID > 0) {
                    retval = -1;   break;   // 已分配
                }
                CLI_IF_DATA_INIT(device, name, flag);

                if (CLI_DEVICE_OPEN() < 0) {
                    retval = -1;   break;   // 打开设备失败
                }
                if ((flag & CLI_NO_THREAD) == 0)
                {
                #if (CLI_THREAD_STKB > 0)
                    OS_THREAD_CREATE(  CLI_IF_THREAD
                                     , CLI_THREAD_NAME
                                     , cli_shell_thread
                                     , (void*)cli
                                     , CLI_THREAD_STACK
                                     , sizeof(CLI_THREAD_STACK)
                                     , CLI_THREAD_PRIO
                                    );      //lint !e866
                #endif
                }
                CLI_IF_REFS = 1;
                CLI_IF_ID   = cli;  //lint !e734 " Loss of precision "
                retval      = cli;          // 成功分配
            }
            OS_MUTEX_LEAVE(CLI_IF_LOCK, OS_MUTEX_TIMEOUT_HANDLER)
            _new_ops  = ((_new_ops < (int)(ARRAY_SIZE(_cli_if_tbl) - 1)) ? (_new_ops + 1) : 0);
        }
        OS_MUTEX_LEAVE(_cli_if_mutex, OS_MUTEX_TIMEOUT_HANDLER)

        if (retval > 0) {
            return( retval );               // 成功分配
        }
    }   // for()

    return( -1 );
}


/**
 * 释放命令行接口
 */
int cli_shell_free(int cli)
{
    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );   // 无效的参数
    }
    return( cli_refs_dec(cli) );
}


/**
 * 设置命令行接口控制标志
 */
int cli_shell_flag(int cli, int set, int clr)
{
    int  flag = -1;

    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );       // 无效的参数
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    OS_MUTEX_ENTER(CLI_IF_LOCK, OS_WAIT_FOREVER)
    {
        flag = (unsigned)CLI_IF_STAT;
        CLI_IF_STAT = (flag | set) & ~clr; //lint !e734 !e502 !e502
    }
    OS_MUTEX_LEAVE(CLI_IF_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( flag );
}


/**
 * 获取命令行接口名
 */
const char *cli_shell_name(int cli)
{
    const char *str;

    if(!CLI_ID_IS_VALID(cli)) {
        return( NULL );     // 无效的参数
    }
    if (cli_refs_inc(cli) < 0) {
        return( NULL );
    }
    str = CLI_IF_NAME;

    if (cli_refs_dec(cli) < 0) {
        return( NULL );
    }
    return( str );
}


/**
 * 命令执行轮询, 从设备读数据, 解析为命令与参数, 然后执行
 */
int cli_shell_poll(int cli)
{
    char    ch;
    char   *line;
    void   *func;
    int     ret = -1;

    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );       // 无效的参数
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }

    OS_MUTEX_ENTER(CLI_IF_I_LOCK, OS_WAIT_FOREVER)
    {
        CLI_IF_DEV_ECHO(ret = -1, break, ENDL         , strlen(ENDL)
                                       , CLI_IF_PROMPT, strlen(CLI_IF_PROMPT)
                                       , NULL         , 0
                       );
        for(CLI_IF_L_BUFF[0] = CLI_IF_L_CUR = CLI_IF_L_POS = 0;  ;)
        {
            if (cli_buff_get(cli, &ch, sizeof(ch)) > 0)
            { //---- 输入缓存数据 转移到 行缓存 ----
                if((ret = cli_line_char(cli, ch, CLI_IF_L_BUFF, sizeof(CLI_IF_L_BUFF))) < 0) {
                    break;  // (ret < 0)        // 设备操作异常
                }
                if (ret < (int)(sizeof(CLI_IF_L_BUFF) - 1)) {
                    continue;                   // 行缓存未满, 且无行结束符
                }
                if (CLI_IF_L_BUFF[0] != '\0') { // 行缓存已满, 或有行结束符
                    break;  // (ret >= 0)       // 行输入完成
                }
             // if (CLI_IF_I_CUR == 1) {        // 输入 "\r\n" 残留 '\n'
             //     cli_buff_free(cli, 1);      // 释放输入缓存
             // }
                continue;
            } //---- 输入缓存数据 转移到 行缓存 ----

            if((ret = cli_buff_read(cli)) <= 0)
            {   // (< 0): 设备操作异常; (== 0): 已读取到一条完整命令
                break;  // (ret <= 0)
            }

            if((CLI_IF_STAT & CLI_EXEC_BIN) && (CLI_IF_I_POS != 0))
            { //---- 二进制命令 处理 ----
                if((ret = CLI_FUNC_BIN(cli, CLI_IF_I_BUFF, CLI_IF_I_POS)) == 0) {
                    continue;                   // 无法识别 二进制命令
                }
                if (ret < 0) {
                    break;  // (ret < 0)        // 设备操作异常
                }
                if (CLI_FUNC_PRMT(cli, CLI_IF_I_BUFF, CLI_IF_I_POS) < 0) {
                    ret = -1;
                    break;                      // 设备操作异常
                }
                CLI_IF_DEV_ECHO(ret = -1, break, ENDL          , strlen(ENDL)
                                               , CLI_BIN_PROMPT, strlen(CLI_BIN_PROMPT)
                                               , CLI_IF_PROMPT , strlen(CLI_IF_PROMPT)
                               );
             // LOG_VERB("%s exec BIN CMD, Len: %u", cli_shell_name(cli), ret);
              #ifdef RTE_Compiler_EventRecorder
                EventStopAx(cli, CLI_IF_I_BUFF, CLI_IF_I_POS);  //lint !e534 !e737 !e835
              #endif
                cli_buff_free(cli, ret);        // 释放输入缓存
                CLI_IF_L_CUR = CLI_IF_L_POS = 0;
                continue;                       // 成功执行 二进制命令
            } //---- 二进制命令 处理 ----
        } // for()

        if((ret < 0) || ((ret = CLI_FUNC_PRMT(cli, CLI_IF_I_BUFF, CLI_IF_I_POS)) < 0)) {
            break;  // (ret < 0)                // 设备操作异常
        }

        if (CLI_IF_STAT & CLI_EXEC_STR)
        { //---- 执行 字符串命令 处理函数 ----    // ↓↓↓↓ 删除行首空白 ↓↓↓↓
            for(line = CLI_IF_L_BUFF;  (line[0] != '\0') && isspace(line[0]);  line++) ;

            if((line[0] != '\0') && !(CLI_IF_STAT & CLI_NO_SEARCH) && ((func = cli_command_find(line)) != NULL))
            {   // 非空字符串        // 搜索字符串命令处理函数          // 搜索到有效命令处理函数
              #ifdef RTE_Compiler_EventRecorder
                EventStopAx (cli, __FILE__, __LINE__); //lint !e534 !e737 !e835
                EventStartBx(cli, line, strlen(line)); //lint !e534 !e737 !e835
              #endif
                LOG_VERB("%s exec STR CMD: %s", cli_shell_name(cli), line);

                if ((ret = ((cli_cmd_fn)func)(cli, line)) < 0) { //lint !e611
                    break; // (ret < 0)         // 设备操作异常
                }
                cli_buff_free(cli, 0);          // 成功执行字符串命令 释放输入缓存

              #ifdef RTE_Compiler_EventRecorder
                EventStopBx(cli, __FILE__, __LINE__);  //lint !e534 !e737 !e835
              #endif
            }
            else
            {
                line = CLI_IF_L_BUFF;

              #ifdef RTE_Compiler_EventRecorder
                EventStopAx (cli, __FILE__, __LINE__); //lint !e534 !e737 !e835
                EventStartBx(cli, line, strlen(line)); //lint !e534 !e737 !e835
              #endif

                if((ret = CLI_FUNC_UFD(cli, line)) > 0) {
                 // LOG_VERB("%s exec STR UFD: %s", cli_shell_name(cli), line);
                    cli_buff_free(cli, 0);      // 成功处理未注册命令 释放输入缓存

              #ifdef RTE_Compiler_EventRecorder
                EventStopBx(cli, __FILE__, __LINE__);  //lint !e534 !e737 !e835
              #endif
                }
                else if (ret == 0) {            // 无法处理未注册命令
                    CLI_IF_DEV_ECHO(ret = -1, break, CLI_UFD_PROMPT, strlen(CLI_UFD_PROMPT), NULL, 0, NULL, 0);
                } else {
                    break; // (ret < 0)         // 设备操作异常
                }
            }
        } //---- 执行 字符串命令 处理函数 ----
    }
    OS_MUTEX_LEAVE(CLI_IF_I_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( ret );
}


/******************************************************************************/
/*----------------------------------------------------------------------------*/
#if 0
/**
 * 读数据, 类似 stdio.h fread(), 但没有 feof() 相关特性
 */
size_t cli_fread(void *ptr, size_t size, size_t nmemb, int cli)
{
    size_t  len;
    size_t  num = 0;

    if(!CLI_ID_IS_VALID(cli) || (ptr == NULL) || (size == 0) || (size > sizeof(CLI_IF_I_BUFF)) || (nmemb == 0)) {
        return( 0 );                            // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( 0 );
    }

    OS_MUTEX_ENTER(CLI_IF_I_LOCK, OS_WAIT_FOREVER)
    {
        cli_buff_free(cli, 0);                  // 释放输入缓存

        for(len = CLI_IF_I_POS - CLI_IF_I_CUR;  len < size;) {
            if(cli_buff_read(cli) < 0)  break;  // 从设备读入数据, 直到缓存至少一个元素
        }
        if (len >= size) {
            num  = len / size;
            num  = MIN(num, nmemb);
            memcpy(ptr, &(CLI_IF_I_BUFF[CLI_IF_I_CUR]), size * num);
            CLI_IF_I_CUR += size * num;         //lint !e734
            cli_buff_free(cli, 0);              // 释放输入缓存
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_I_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( 0 );
    }
    return( num );
}
#endif


/**
 * 写数据(全缓冲模式), 类似 stdio.h fwrite(), 但没有 feof() 相关特性
 */
size_t cli_fwrite(const void *ptr, size_t size, size_t nmemb, int cli)
{
    int     ret, tmp;
    size_t  num = 0;

    if(!CLI_ID_IS_VALID(cli) || (ptr == NULL) || (size == 0) || (nmemb == 0)) {
        return( 0 );                        // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( 0 );
    }

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {
        // 元素大小 > 输出缓存, 直接输出
        if (size > sizeof(CLI_IF_O_BUFF))
        {
            if (cli_buff_flush(cli, 1) < 0) {
                break;                      // 设备操作异常
            }
            for(num = 0;  num < nmemb;  num++)
            {
                CLI_IF_DEV_WRITE(ptr, size, tmp, ret);
                if (ret != size) {          //lint !e737  " Loss of sign "
                    break;                  // 设备操作异常
                }
            }
            break;
        }

        // 元素大小 <= 输出缓存, 保存到缓存
        for(num = 0;  num < nmemb;  num++)
        {
            if ((sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS) < size)
            {   // 输出缓存不够, 刷新输出缓存(全缓冲模式)
                if (cli_buff_flush(cli, 1) < 0) {
                    break;                  // 设备操作异常
                }
            }
            assert((sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS) >= size);
         // memmove(&(CLI_IF_O_BUFF[CLI_IF_O_POS]), &(((uint8_t *)ptr)[num * size]), size);
            memcpy (&(CLI_IF_O_BUFF[CLI_IF_O_POS]), &(((uint8_t *)ptr)[num * size]), size);

            if ((CLI_IF_O_POS += size) == sizeof(CLI_IF_O_BUFF))  //lint !e734  " Loss of precision "
            {   // 输出缓存已满, 刷新输出缓存(全缓冲模式)
                if (cli_buff_flush(cli, 1) < 0) {
                    break;                  // 设备操作异常
                }
            }
        }   // for()
    }
    OS_MUTEX_LEAVE(CLI_IF_O_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( 0 );
    }
    return( num );
}


/**
 * 字符串输出, 类似 stdio.h fputs(), 但没有 feof() 相关特性
 */
int cli_fputs(const char *str, int cli)
{
    int  num, size, tmp;

    if(!CLI_ID_IS_VALID(cli) || (str == NULL)) {
        return( -1 );                       // 参数错误
    }
    if ((size = strlen(str)) <= 0) {        //lint !e713  " Loss of precision "
        return( 0 );                        // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    num = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {
        // 输出缓存不够, 刷新输出缓存(全缓冲模式)
        if (size > (sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS))  //lint !e574 !e737
        {
            if (cli_buff_flush(cli, 1) < 0) {
                num = -1;                   // 设备操作异常
                break;
            }
        }

        // 字符串长度 > 输出缓存, 直接输出
        if (size > sizeof(CLI_IF_O_BUFF))   //lint !e574 !e737
        {
            CLI_IF_DEV_WRITE(str, size, tmp, num);
            if (num != size) {
                num = -1;                   // 设备操作异常
                break;
            }
            break;
        }
        assert(size <= (sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS));

        // 字符串 复制到输出缓存
     // memmove(&(CLI_IF_O_BUFF[CLI_IF_O_POS]), str, size);  //lint !e732
        memcpy (&(CLI_IF_O_BUFF[CLI_IF_O_POS]), str, size);  //lint !e732
        CLI_IF_O_POS += size;   //lint !e734
        num           = size;

        if (cli_buff_flush(cli, 0) < 0) {
            num = -1;                       // 设备操作异常
            break;
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_O_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( num );
}


/**
 * 格式化输出(行缓存模式), 类似 stdio.h fprintf(), 但没有 feof() 相关特性
 */
int cli_fprintf(int cli, const char *format, ...)
{
    va_list  args;
    int      size;
    int      retval;

    if(!CLI_ID_IS_VALID(cli) || (format == NULL)) {
        return( -1 );                   // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    retval = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {   //lint --e{850}  " for loop index variable 'xxx' is modified in body "
        for(size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;  ;  size = 0)
        {
            if (size == 0)
            {   // 输出缓存满, 刷新输出缓存(全缓冲模式)
                if (cli_buff_flush(cli, 1) < 0) {
                    retval = -1;        // 设备操作异常
                    break;
                }
                size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;
            }
            assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

            va_start(args, format); //lint -e{732}  " Loss of sign "
            retval = vsnprintf((char*)&(CLI_IF_O_BUFF[CLI_IF_O_POS]), size, format, args);
            va_end(args);

            if (retval < 0) {
                retval = -1;            // 编码错误
                break;
            }
            if (retval < size) { //lint -e{734}  " Loss of precision "
                CLI_IF_O_POS += retval; // 成功完全输出
                break;
            }
            if (size == sizeof(CLI_IF_O_BUFF)) { //lint !e737  " Loss of sign "
                CLI_IF_O_POS  = size;            //lint !e734  " Loss of precision "
                CLI_IF_O_BUFF[sizeof(CLI_IF_O_BUFF) - 1] = '\n';
                break;                  // 不能完全输出, 使用整个输出缓存则退出
            }
        }   // for()

        if (retval > 0)                 // 有输出, 刷新输出缓存
        {   //lint -e{730}  " Boolean argument to function "
            if (cli_buff_flush(cli, CLI_IF_O_POS == sizeof(CLI_IF_O_BUFF)) < 0) {
                retval = -1;            // 设备操作异常
                break;
            }
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_O_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( retval );
}


/**
 * 格式化输出(行缓存模式), 类似 stdio.h vfprintf(), 但没有 feof() 相关特性
 */
int cli_vfprintf(int cli, const char *format, va_list arg)
{
    int      size;
    int      retval;

    if(!CLI_ID_IS_VALID(cli) || (format == NULL)) {
        return( -1 );                   // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    retval = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {   //lint --e{850}  " for loop index variable 'xxx' is modified in body "
        for(size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;  ;  size = 0)
        {
            if (size == 0)
            {   // 输出缓存满, 刷新输出缓存(全缓冲模式)
                if (cli_buff_flush(cli, 1) < 0) {
                    retval = -1;        // 设备操作异常
                    break;
                }
                size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;
            }
            assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

            //lint -e{732}  " Loss of sign "
            retval = vsnprintf((char*)&(CLI_IF_O_BUFF[CLI_IF_O_POS]), size, format, arg);
            if (retval < 0) {
                retval = -1;            // 编码错误
                break;
            }
            if (retval < size) { //lint -e{734}  " Loss of precision "
                CLI_IF_O_POS += retval; // 成功完全输出
                break;
            }
            if (size == sizeof(CLI_IF_O_BUFF)) { //lint !e737  " Loss of sign "
                CLI_IF_O_POS  = size;            //lint !e734  " Loss of precision "
                CLI_IF_O_BUFF[sizeof(CLI_IF_O_BUFF) - 1] = '\n';
                break;                  // 不能完全输出, 使用整个输出缓存则退出
            }
        }   // for()

        if (retval > 0)                 // 有输出, 刷新输出缓存
        {   //lint -e{730}  " Boolean argument to function "
            if (cli_buff_flush(cli, CLI_IF_O_POS == sizeof(CLI_IF_O_BUFF)) < 0) {
                retval = -1;            // 设备操作异常
                break;
            }
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_O_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( retval );
}


/**
 * 回显开启才格式化输出(行缓存模式), 类似 stdio.h fprintf(), 但没有 feof() 相关特性
 */
int cli_fechof(int cli, const char *format, ...)
{
    va_list  args;
    int      size;
    int      retval;

    if(!CLI_ID_IS_VALID(cli) || (format == NULL)) {
        return( -1 );                   // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    if((CLI_IF_STAT & CLI_ECHO_ON) == 0) {
        return( 0 );                    // 回显关闭
    }
    retval = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {   //lint --e{850}  " for loop index variable 'xxx' is modified in body "
        for(size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;  ;  size = 0)
        {
            if (size == 0)
            {   // 输出缓存满, 刷新输出缓存(全缓冲模式)
                if (cli_buff_flush(cli, 1) < 0) {
                    retval = -1;        // 设备操作异常
                    break;
                }
                size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;
            }
            assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

            va_start(args, format); //lint -e{732}  " Loss of sign "
            retval = vsnprintf((char*)&(CLI_IF_O_BUFF[CLI_IF_O_POS]), size, format, args);
            va_end(args);

            if (retval < 0) {
                retval = -1;            // 编码错误
                break;
            }
            if (retval < size) { //lint -e{734}  " Loss of precision "
                CLI_IF_O_POS += retval; // 成功完全输出
                break;
            }
            if (size == sizeof(CLI_IF_O_BUFF)) { //lint !e737  " Loss of sign "
                CLI_IF_O_POS  = size;            //lint !e734  " Loss of precision "
                CLI_IF_O_BUFF[sizeof(CLI_IF_O_BUFF) - 1] = '\n';
                break;                  // 不能完全输出, 使用整个输出缓存则退出
            }
        }   // for()

        if (retval > 0)                 // 有输出, 刷新输出缓存
        {   //lint -e{730}  " Boolean argument to function "
            if (cli_buff_flush(cli, CLI_IF_O_POS == sizeof(CLI_IF_O_BUFF)) < 0) {
                retval = -1;            // 设备操作异常
                break;
            }
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_O_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( retval );
}


/**
 * 刷新输出缓存, 类似 stdio.h fflush(), 但没有 feof() 相关特性
 */
int cli_fflush(int cli)
{
    int      retval;

    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );       // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    retval = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {
        if (cli_buff_flush(cli, 1) >= 0) {
            retval = 0;     // 成功
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_O_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( retval );
}


/******************************************************************************/
/*----------------------------------------------------------------------------*/
/**
 * 行字符输入
 *
 * @param[in]  cli   命令行接口
 * @param[in]  ch    输入字符
 * @param[in]  line  行缓存
 * @param[in]  size  行缓存大小
 *
 * @return  小于0: 设备操作异常, 其它: 行缓存字符数
 *          @arg 小于(size - 1): 行缓存未满, 也未输入换行符, 可再次输入
 *          @arg 等于(size - 1): 行缓存已满, 再次输入的字符将被丢弃
 *          @arg 大于(size - 1): 输入字符为行结束符, 行缓存是否满需其他方法判断
 *
 * @note    外部变量 (RW)CLI_IF_L_STAT, (RW)CLI_IF_L_POS, (RW)CLI_IF_L_CUR
 * @note    外部函数 CLI_DEVICE_ECHO()
 */
static int cli_line_char(int cli, char ch, char *line, int size)
{
    if (CLI_IF_L_POS >= (size - 1)) {
        return( size - 1 );                             // 行缓存满
    }
    assert(CLI_IF_L_CUR <= CLI_IF_L_POS);

    if (CLI_IF_STAT & CLI_ECHO_ON)
    {   /**
         * 序列键码
         * Up arrow     ESC,0x1B  [,0x5B  A,0x41
         * Down arrow   ESC,0x1B  [,0x5B  B,0x42
         * Right arrow  ESC,0x1B  [,0x5B  C,0x43
         * Left arrow   ESC,0x1B  [,0x5B  D,0x44
         * Home         ESC,0x1B  [,0x5B  1,0x31  ~,0x7E
         * Insert       ESC,0x1B  [,0x5B  2,0x32  ~,0x7E
         * Delete       ESC,0x1B  [,0x5B  3,0x33  ~,0x7E
         * End          ESC,0x1B  [,0x5B  4,0x34  ~,0x7E
         * Page Up      ESC,0x1B  [,0x5B  5,0x35  ~,0x7E
         * Page Down    ESC,0x1B  [,0x5B  6,0x36  ~,0x7E
         */
        switch (ch) {
        case '\x1B' :       // ESC  换码符
            CLI_IF_L_STAT  = CLI_LINE_STAT_SEQUC;       // ESC,0x1B
            return( CLI_IF_L_POS );
        case '[' :          //
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SEQUC) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC1;   // ESC,0x1B  [,0x5B
                return( CLI_IF_L_POS );
            }
            break;
        case 'A' :          // 'A'  ↑
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) { // ESC,0x1B  [,0x5B  A,0x41
                ch  =  CLI_KEY_UP;
            }
            break;
        case 'B' :          // 'B'  ↓
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) { // ESC,0x1B  [,0x5B  B,0x42
                ch  =  CLI_KEY_DOWN;
            }
            break;
        case 'C' :          // 'C'  →
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) { // ESC,0x1B  [,0x5B  C,0x43
                ch  =  CLI_KEY_RIGHT;
            }
            break;
        case 'D' :          // 'D'  ←
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) { // ESC,0x1B  [,0x5B  D,0x44
                ch  =  CLI_KEY_LEFT;
            }
            break;
        case '1' :          // Home
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC2_1; // ESC,0x1B  [,0x5B  1,0x31
                return( CLI_IF_L_POS );
            }
            break;
        case '2' :          // Insert
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC2_2; // ESC,0x1B  [,0x5B  2,0x32
                return( CLI_IF_L_POS );
            }
            break;
        case '3' :          // Delete
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC2_3; // ESC,0x1B  [,0x5B  3,0x33
                return( CLI_IF_L_POS );
            }
            break;
        case '4' :          // End
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC2_4; // ESC,0x1B  [,0x5B  4,0x34
                return( CLI_IF_L_POS );
            }
            break;
        case '5' :          // Page UP
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC2_5; // ESC,0x1B  [,0x5B  5,0x35
                return( CLI_IF_L_POS );
            }
            break;
        case '6' :          // Page Down
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC2_6; // ESC,0x1B  [,0x5B  6,0x36
                return( CLI_IF_L_POS );
            }
            break;
        case '~' :
            switch (CLI_IF_L_STAT) {
            case CLI_LINE_STAT_SPEC2_1 :  ch  =  CLI_KEY_HOME;    break;
            case CLI_LINE_STAT_SPEC2_2 :  ch  =  CLI_KEY_INSERT;  break;
            case CLI_LINE_STAT_SPEC2_3 :  ch  =  CLI_KEY_DEL;     break;
            case CLI_LINE_STAT_SPEC2_4 :  ch  =  CLI_KEY_END;     break;
            case CLI_LINE_STAT_SPEC2_5 :  ch  =  CLI_KEY_PU;      break;
            case CLI_LINE_STAT_SPEC2_6 :  ch  =  CLI_KEY_PD;      break;
            default :  break;
            }
            break;
    
      #if 0
        case CLI_KEY_CR :   // '\r' 0x0D 回车(Enter)
            if (CLI_IF_L_STAT != CLI_LINE_STAT_FEED2) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_FEED1;
                line[CLI_IF_L_POS++] = '\n';            // 插入行结束符
                line[CLI_IF_L_POS  ] = '\0';
            } else {
                CLI_IF_L_STAT  = CLI_LINE_STAT_NORMAL;
                line[CLI_IF_L_POS  ] = '\0';
            }
            return( size );
    
        case CLI_KEY_LF :   // '\n' 0x0A 换行(Enter)
            if (CLI_IF_L_STAT != CLI_LINE_STAT_FEED1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_FEED2;
                line[CLI_IF_L_POS++] = '\n';            // 插入行结束符
                line[CLI_IF_L_POS  ] = '\0';
            } else {
                CLI_IF_L_STAT  = CLI_LINE_STAT_NORMAL;
                line[CLI_IF_L_POS  ] = '\0';
            }
            return( size );
      #endif
    
        default :   break;
        }
        CLI_IF_L_STAT  = CLI_LINE_STAT_NORMAL;

        //----------------- 控制键 ---------------------
        switch (ch) {
        case CLI_KEY_INSERT :   // 0x13 Insert
        case CLI_KEY_BEL :      // 0x07 响铃
        case CLI_KEY_FF :       // 0x0C 换页
        case CLI_KEY_VT :       // 0x0B 垂直制表
            return( CLI_IF_L_POS );
        
        case CLI_KEY_HT :       // 0x09 水平制表(Tab)
            return( CLI_IF_L_POS );
        
        case CLI_KEY_PU :       // 0x15 Page Up
        case CLI_KEY_UP :       // 0x0E Up    Arrow
            return( CLI_IF_L_POS );
        
        case CLI_KEY_PD :       // 0x16 Page Down
        case CLI_KEY_DOWN :     // 0x0F Down  Arrow
            return( CLI_IF_L_POS );
        
        case CLI_KEY_HOME :     // 0x12 Home
            if (CLI_IF_L_CUR > 0) {
                CLI_IF_DEV_ECHO(, return -1, &_cli_str_bs[1], CLI_IF_L_CUR, NULL, 0, NULL, 0);
                CLI_IF_L_CUR = 0;                       // 光标移到行首
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_END :      // 0x14 End
            if (CLI_IF_L_SUB > 0) {
                CLI_IF_DEV_ECHO(, return -1, &line[CLI_IF_L_CUR], CLI_IF_L_SUB, NULL, 0, NULL, 0);
                CLI_IF_L_CUR = CLI_IF_L_POS;            // 光标移到行尾
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_RIGHT :    // 0x10 Right Arrow
            if (CLI_IF_L_SUB > 0) {
                CLI_IF_DEV_ECHO(, return -1, &line[CLI_IF_L_CUR], 1, NULL, 0, NULL, 0);
                CLI_IF_L_CUR++;                         // 光标右移
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_LEFT :     // 0x11 Left  Arrow
            if (CLI_IF_L_CUR > 0) {
                CLI_IF_DEV_ECHO(, return -1, &_cli_str_bs[1], 1, NULL, 0, NULL, 0);
                CLI_IF_L_CUR--;                         // 光标左移
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_BS :       // 0x08 退格(Backspace)
            if (CLI_IF_L_CUR == 0) {
                return( CLI_IF_L_POS );                 // 光标位于行首
            }
            if (CLI_IF_L_SUB > 0) {
             // memmove(&line[CLI_IF_L_CUR - 1], &line[CLI_IF_L_CUR], CLI_IF_L_SUB);
                memcpy (&line[CLI_IF_L_CUR - 1], &line[CLI_IF_L_CUR], CLI_IF_L_SUB);
            }
            CLI_IF_DEV_ECHO(, return -1, &_cli_str_bs[1]        , 1
                                       , &line[CLI_IF_L_CUR - 1], CLI_IF_L_SUB
                                       , &_cli_str_bs[0]        , CLI_IF_L_SUB + 2
                           );
            CLI_IF_L_CUR--;
            CLI_IF_L_POS--;
            line[CLI_IF_L_POS]  =  '\0';
            return( CLI_IF_L_POS );
        
        case CLI_KEY_DEL :      // 0x7F Delete
            if(CLI_IF_L_SUB == 0) {
                return( CLI_IF_L_POS );                 // 光标位于行尾
            }
            if (CLI_IF_L_SUB > 1) {
             // memmove(&line[CLI_IF_L_CUR], &line[CLI_IF_L_CUR + 1], CLI_IF_L_SUB - 1);
                memcpy (&line[CLI_IF_L_CUR], &line[CLI_IF_L_CUR + 1], CLI_IF_L_SUB - 1);
            }
            CLI_IF_DEV_ECHO(, return -1, &line[CLI_IF_L_CUR], CLI_IF_L_SUB - 1
                                       , &_cli_str_bs[0]    ,(CLI_IF_L_SUB - 1) + 2
                                       ,  NULL              , 0
                           );
            CLI_IF_L_POS--;
            line[CLI_IF_L_POS]  =  '\0';
            return( CLI_IF_L_POS );
        default :  break;
        }

        //----------------- 普通键 ---------------------
        if((ch != '\r') && (ch != '\n') && !isprint(ch)) {
            return( CLI_IF_L_POS );
        }
    }

    if (CLI_IF_L_SUB > 0) {                             // 行中间插入字符
        memmove(&line[CLI_IF_L_CUR + 1], &line[CLI_IF_L_CUR], CLI_IF_L_SUB);
    }
    line[CLI_IF_L_CUR]  =  ch;
    CLI_IF_DEV_ECHO(, return -1, &line[CLI_IF_L_CUR], CLI_IF_L_SUB + 1
                               , &_cli_str_bs[1]    , CLI_IF_L_SUB
                               ,  NULL              , 0
                   );
    CLI_IF_L_CUR++;
    CLI_IF_L_POS++;
    line[CLI_IF_L_POS]  =  '\0';
    return( (ch == '\n') ? size : CLI_IF_L_POS );
}


/*----------------------------------------------------------------------------*/
//lint -esym(714, cli_data_get)
/**
 * 从设备通信接口获取数据(阻塞函数, 仅可用于命令行接口回调函数)
 *
 * @param[in]  cli   命令行接口
 * @param[in]  size  需获取的数据长度
 *
 * @return  获取到的指定长度数据的指针(注意不可修改数据)
 */
const void *cli_data_get(int cli, size_t size)
{
    int         len;
    const void *ptr;

    if(!CLI_ID_IS_VALID(cli) || (size == 0) || (size > sizeof(CLI_IF_I_BUFF))) {
        return( NULL );                             // 参数错误
    }

    if((sizeof(CLI_IF_I_BUFF) - CLI_IF_I_CUR) < size) {
        cli_buff_free(cli, 0);                      // 释放输入缓存
    }
    LOG_VERB("Get BIN: %u(%u)\t on %s", size, CLI_IF_I_POS - CLI_IF_I_CUR, cli_shell_name(cli));

    while((CLI_IF_I_POS - CLI_IF_I_CUR) < size)     //lint !e574
    {
        if((len = CLI_DEVICE_READ(&(CLI_IF_I_BUFF[CLI_IF_I_POS]), sizeof(CLI_IF_I_BUFF) - CLI_IF_I_POS)) < 0) {
            return( NULL );                         // 从设备读入数据
        }
        CLI_IF_I_POS += len;  //lint !e734          // 设备读成功
        LOG_VERB("Rev BIN: %u(%u)\t on %s", size, CLI_IF_I_POS - CLI_IF_I_CUR, cli_shell_name(cli));
    }
    ptr  =  &(CLI_IF_I_BUFF[CLI_IF_I_CUR]);
    CLI_IF_I_CUR  +=  size;   //lint !e734          // 释放输入缓存

    return(ptr);
}


/**
 * 从设备读数据到输入缓存
 *
 * @param[in]  cli  命令行接口
 *
 * @retval  ( > 0) 读入的数据长度(以字节为单位)
 * @retval  (== 0) 已读取到一条完整命令, 用于以数据包(非'\n')分隔的命令
 * @retval  ( < 0) 设备操作异常
 */
static int cli_buff_read(int cli)
{
    int  len;
    int  size  = sizeof(CLI_IF_I_BUFF) - CLI_IF_I_POS;
    assert((CLI_IF_I_CUR <= CLI_IF_I_POS) && (CLI_IF_I_POS <= sizeof(CLI_IF_I_BUFF)));

    if (size == 0)
    {  // 输入缓存满, 丢弃输入缓存最旧一字节数据
        size =  1;
        CLI_IF_I_CUR = (CLI_IF_I_CUR > 0) ? (CLI_IF_I_CUR - 1) : 0 ;
        CLI_IF_I_POS--;
     // memmove(&(CLI_IF_I_BUFF[0]), &(CLI_IF_I_BUFF[1]), sizeof(CLI_IF_I_BUFF) - 1);
        memcpy (&(CLI_IF_I_BUFF[0]), &(CLI_IF_I_BUFF[1]), sizeof(CLI_IF_I_BUFF) - 1);
    }

    if((len = CLI_DEVICE_READ(&(CLI_IF_I_BUFF[CLI_IF_I_POS]), size)) > 0)
    {   //lint --e{732}  " Loss of sign "
        assert(len <= size);
    #ifdef RTE_Compiler_EventRecorder
        EventStartAx(cli, &(CLI_IF_I_BUFF[CLI_IF_I_POS]), len); //lint !e534 !e737 !e835
    #endif
        CLI_IF_I_POS += (int16_t)len;   // 设备读成功
    }
    return( len );
}


/**
 * 从输入缓存获取数据
 *
 * @param[in]  cli   命令行接口
 * @param[out] buff  输出缓冲区
 * @param[in]  size  缓冲区大小, 以字节为单位
 *
 * @retval  ( > 0) 成功获取的数据长度
 * @retval  (<= 0) 输入缓存为空
 */
static int cli_buff_get(int cli, void *buff, int size)
{
    int len;

    len = CLI_IF_I_POS - CLI_IF_I_CUR;
    len = MIN(size, len);
    assert((CLI_IF_I_CUR <= CLI_IF_I_POS) && (CLI_IF_I_POS <= sizeof(CLI_IF_I_BUFF)));

    if (len > 0)
    {   //lint --e{732, 734}  " Loss of sign / Loss of precision "
        memmove(buff, &(CLI_IF_I_BUFF[CLI_IF_I_CUR]), len);
        CLI_IF_I_CUR += len;
    }

    return( len );
}


/**
 * 释放输入缓存(以字节为单位)
 *
 * @param[in]  cli   命令行接口
 * @param[in]  size  ( > 0)释放指定字节, (<= 0)释放 cli_buff_get() 中已输出的.
 */
static void cli_buff_free(int cli, int size)
{
    if (size <= 0)
    {   // 释放所有在 cli_buff_get() 中已输出的
        size =  CLI_IF_I_CUR;
    }

    if ((size < 0) || (size >= CLI_IF_I_POS))
    {   // 清空输入缓存
        CLI_IF_I_CUR = 0;
        CLI_IF_I_POS = 0;
        return;
    }
    assert((CLI_IF_I_CUR <= CLI_IF_I_POS) && (CLI_IF_I_POS <= sizeof(CLI_IF_I_BUFF)));

    //lint --e{732, 734}  " Loss of sign / Loss of precision "
 // memmove(&(CLI_IF_I_BUFF[0]), &(CLI_IF_I_BUFF[size]), CLI_IF_I_POS - size);
    memcpy (&(CLI_IF_I_BUFF[0]), &(CLI_IF_I_BUFF[size]), CLI_IF_I_POS - size);
    CLI_IF_I_CUR = (CLI_IF_I_CUR > size) ? (CLI_IF_I_CUR - size) : 0 ;
    CLI_IF_I_POS =                          CLI_IF_I_POS - size      ;
}


/**
 * 刷新输出缓存
 *
 * @param[in]  cli   命令行接口
 * @param[in]  all   (1)全缓冲模式, (0)行缓冲模式
 *
 * @retval  (>= 0) 写入设备的数据长度(以字节为单位)
 * @retval  ( < 0) 设备操作异常
 */
static int cli_buff_flush(int cli, int all)
{
    int     num, size, tmp;
    assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

    // size <-- 写到设备数据长度
    if(((size = CLI_IF_O_POS), all) == 0)
    {
        for(;  size > 0;  size--)
        {
            switch (CLI_IF_O_BUFF[size - 1]) {
            default :          continue;
         // case CLI_KEY_CR :  break;
            case CLI_KEY_LF :  break;   // 查找换行符
            }
            break;
        }
    }

    // size 长度数据写到设备, num <-- 实际写长度
    if((num = 0, size) > 0) {
        CLI_IF_DEV_WRITE(CLI_IF_O_BUFF, size, tmp, num);
    }

    // 移动输出缓存未写到设备的数据
    if (num > 0)
    {
        if ((CLI_IF_O_POS - num) > 0) { //lint -e{732}  " Loss of sign "
         // memmove(&(CLI_IF_O_BUFF[0]), &(CLI_IF_O_BUFF[num]), CLI_IF_O_POS - num);
            memcpy (&(CLI_IF_O_BUFF[0]), &(CLI_IF_O_BUFF[num]), CLI_IF_O_POS - num);
            CLI_IF_O_POS -= num;        //lint !e734    " Loss of precision "
        }
        else {
            CLI_IF_O_POS  = 0;
        }
    }

    if (num != size) {
        return( -1 );
    }
    return( num );
}

/*----------------------------------------------------------------------------*/
/**
 * 增加命令行接口引用计数
 *
 * @param[in]  cli  命令行接口
 *
 * @retval  (>= 0) 成功, 返回修改后的命令行接口引用计数
 * @retval  ( < 0) 失败
 */
static int cli_refs_inc(int cli)
{
    int   retval = -1;

    OS_MUTEX_ENTER(CLI_IF_LOCK, OS_WAIT_FOREVER)
    {
        if ((CLI_IF_ID == cli) && (CLI_IF_REFS < UINT16_MAX))
        {
            retval = ++CLI_IF_REFS;
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    return( retval );
}


/**
 * 减少命令行接口引用计数
 *
 * @param[in]  cli  命令行接口
 *
 * @retval  (>= 0) 成功, 返回修改后的命令行接口引用计数
 * @retval  ( < 0) 失败
 *
 * @note    当返回值为(0)时, 该函数会释放命令行接口
 */
static int cli_refs_dec(int cli)
{
    int   retval = -1;

    OS_MUTEX_ENTER(CLI_IF_LOCK, OS_WAIT_FOREVER)
    {
        if ((CLI_IF_ID == cli) && (CLI_IF_REFS > 0))
        {
            retval = --CLI_IF_REFS;
        }
        if (retval == 0)
        {
            CLI_IF_ID = -1;
            CLI_DEVICE_CLOSE(); //lint !e534  " Ignoring return value "
        }
    }
    OS_MUTEX_LEAVE(CLI_IF_LOCK, OS_MUTEX_TIMEOUT_HANDLER)

    return( retval );
}


/******************************************************************************/
/*----------------------------------------------------------------------------*/
/**
 * 删除字符串首尾空白
 */
char *cli_strdelspace(char *str)
{
    char  *tmp = str + strlen(str) - 1;

    for(;  (tmp != str) && isspace(*tmp);  tmp--)
    {   // 删除行尾空白
        *tmp = '\0';
    }
    for(;  (str[0] != '\0') && isspace(str[0]);  str++)
    {   // 删除行首空白
    }
    return( str );
}


/**
 * 返回字符串 n 次定界后的子串
 */
const char *cli_strdelim(const char *s, unsigned n, const char *delim)
{
    if (delim == NULL)
    {
        while (1)   //lint !e716
        {
            for(;  isspace(*s);  s++) {     // 跳过定界符
                if(*s == '\0')  return( NULL );
            }
            if(n-- == 0)  break;            // 到达指定定界次数

            for(;  !isspace(*s);  s++) {    // 跳过非定界符
                if(*s == '\0')  return( NULL );
            }
        }
    }
    else
    {
        while (1)   //lint !e716
        {
            for(;  strchr(delim, *s) != NULL;  s++) {   // 跳过定界符
                if(*s == '\0')  return( NULL );
            }
            if(n-- == 0)  break;                        // 到达指定定界次数

            for(;  strchr(delim, *s) == NULL;  s++) {   // 跳过非定界符
                if(*s == '\0')  return( NULL );
            }
        }
    }
    return( s );
}


/**
 * 字符串比较, 忽略行首空白和大小写, 遇到非行首空白或终止符时结束比较.
 */
int cli_strtrimcmp(const char *s1, const char *s2)
{
    int  c1, c2;

    // 忽略行首空白, Todo: 非空白分隔符处理
    for(;  (*s1 != '\0') && isspace(*s1);  s1++) ;
    for(;  (*s2 != '\0') && isspace(*s2);  s2++) ;

    /*----*/  c1 = tolower(*s1++), c2 = tolower(*s2++);
    for(;  ;  c1 = tolower(*s1++), c2 = tolower(*s2++))
    {
        if (isspace(c1)) {
            c1  = '\0';         // 空白处理 与 终止符相同
        }
        if (isspace(c2)) {
            c2  = '\0';         // 空白处理 与 终止符相同
        }

        if (c1 != c2) {
            break;              // 不同字符
        }
        if (c1 == '\0') {
            return( 0 );        // 相同字符, 且为终止符
        }
    }
    return( c1 - c2 );
}


/**
 * 字符串比较, 忽略行首空白和大小写, 遇到非行首空白或终止符时结束比较
 */
int cli_strntrimcmp(const char *s1, const char *s2, size_t n)
{
    int  c1, c2;

    // 忽略行首空白, Todo: 非空白分隔符处理
    for(;  (*s1 != '\0') && isspace(*s1);  s1++) ;
    for(;  (*s2 != '\0') && isspace(*s2);  s2++) ;

    /*--------------*/  c1 = tolower(*s1++), c2 = tolower(*s2++);
    for(;  n > 0;  n--, c1 = tolower(*s1++), c2 = tolower(*s2++))
    {
        if (isspace(c1)) {
            c1  = '\0';         // 空白处理 与 终止符相同
        }
        if (isspace(c2)) {
            c2  = '\0';         // 空白处理 与 终止符相同
        }

        if (c1 != c2) {
            break;              // 不同字符
        }
        if (c1 == '\0') {
            return( 0 );        // 相同字符, 且为终止符
        }
    }
    return( c1 - c2 );
}


//lint -esym(714, cli_strcmdcmp)
/**
 * 命令名比较, 遇到空白或终止符时结束比较, 比较时忽略 s1 中合法数字子串, 并返回该数值. \n
 * eg: s1 = ":SOUR:VOLT:RANG  1.2e-3\n", s2 = ":SOUR:RANG", 则返回 0. \n
 * eg: s1 = ":SOUR1:VOLT:RANG 1.2e-3\n", s2 = ":SOUR:RANG", 则返回 1. \n
 * eg: s1 = ":SOUR2:VOLT:RANG 1.2e-3\n", s2 = ":SOUR:RANG", 则返回 2.
 *
 * @param[in]  s1   输入的字符串.
 * @param[in]  s2   命令名字符串.
 *
 * @retval  (== 0) 命令名匹配成功, 无数字子串.
 * @retval  ( > 0) 命令名匹配成功, 有数字子串并返回数字子串值.
 * @retval  ( < 0) 命令名匹配失败, s1 与 s2 不等, 或数字子串非法.
 *
 * @note    合法数字子串: 不能位于行首, 必须以冒号或问号或空白结束, 值不能为0
 */
int cli_strcmdcmp(const char *s1, const char *s2)
{
    int  num;
    int  c1, c2;

//  // 忽略行首空白, Todo: 非空白分隔符处理
//  for(;  (*s1 != '\0') && isspace(*s1);  s1++) ;
//  for(;  (*s2 != '\0') && isspace(*s2);  s2++) ;

    /*-----------*/  c1 = tolower(*s1++), c2 = tolower(*s2++);
    if (isdigit(c1)) {
        return( -1 );           // 首字符为数字
    }
    for(num = 0;  ;  c1 = tolower(*s1++), c2 = tolower(*s2++))
    {
        if((num == 0) && isdigit(c1))
        {                       // 未出现数字子串时, 出现数字字符则将其转化为数值
            do {
                num  = (num << 3) + (num << 1)/*(num * 10)*/ + c1 - '0'; //lint !e701
            } while( isdigit(c1 = tolower(*s1++)) );

            if((num == 0) || (c1 != ':') && (c1 != '?') && !isspace(c1)) {
                return( -1 );   // 数字子串值为0, 非以冒号或问号或空白结束
            }
        }

        if (isspace(c1)) {
            c1  = '\0';         // 空白处理 与 终止符相同
        }
        if (isspace(c2)) {
            c2  = '\0';         // 空白处理 与 终止符相同
        }

        if (c1 != c2) {
            break;              // 不同字符
        }
        if (c1 == '\0') {
            return( num );      // 相同字符, 且为终止符
        }
    }
    return( -1 );               // 字符串不等
}


/**
 * 在字符串 n 次定界后的子串中, 识别定关键字后的正整数
 */
int cli_strsplitnum(const char *s, const char *key, unsigned n, const char *delim)
{
    unsigned  num;
    size_t    len = strlen(key);

    if((s = cli_strdelim(s, n, delim)) == NULL) {
        return( 0 );    // 无子串
    }
    if (cli_strntrimcmp(s, key, len) != 0) {
        return( 0 );    // 关键字错误
    }
    if((num = strtoul(&(s[len]), NULL, 10)) == 0) {
        return( -1 );   // 格式错误(关键字后的正整数)
    }
    return( num );      //lint !e713
}


/**
 * 在一个内存块中搜索匹配另一个内存块的第一个位置.
 */
const void *cli_memmem(const void *haystack, size_t hs_len, const void *needle, size_t ne_len)
{
    const unsigned char *end, *cur;
    const unsigned char *hs = haystack;
    const unsigned char *ne = needle;
    unsigned char        ch = ne[0];

    if (hs_len < ne_len) {
        return( NULL );
    }
    if (ne_len == 0) {
        return( (const void *)hs );
    }

    if((NULL == (cur = memchr(hs, ch, hs_len))) || (ne_len == 1)) {
        return( (const void *)cur );
    }

    for(end = hs + hs_len - ne_len;  cur <= end; )
    {
    #if 0
        if (memcmp(cur, ne, ne_len) == 0) {
            return( (const void *)cur );
        }
        if (NULL == (cur = memchr(cur + 1, ch, end - cur))) { //lint !e732
            break;
        }
    #else
        if((cur[0] == ch) && (memcmp(cur, ne, ne_len) == 0)) {
            return( (const void *)cur );
        }
        cur += 1;
    #endif
    }

    return( NULL );
}


/******************************************************************************/
/*----------------------------------------------------------------------------*/
/**
 * 修改命令地址, 对所有命令有效.
 */
int32_t cli_cmd_addr(size_t type, uint16_t addr)
{
    int32_t     result = -1;

    if (type < ARRAY_SIZE(_cli_cmd_addr))
    {
        OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
        {
            result  = _cli_cmd_addr[type];
            _cli_cmd_addr[type]  = addr;
        }
        OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)
    }
    return( result );
}


/**
 * 修改命令选项, 仅对指定命令有效.
 */
int32_t cli_cmd_flag(const char *name, uint16_t clr, uint16_t set)
{
    const char *str;
    int32_t     result = -1;

    if (name != NULL)
    {
        OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
        {
            for(size_t i = 0;  i < ARRAY_SIZE(_cli_cmd_tbl);  i++)
            {
                if((str = _cli_cmd_tbl[i].name) == NULL) {
                    continue;
                }
                if(cli_strtrimcmp(name, str) != 0) {
                    continue;
                }
                result  = _cli_cmd_tbl[i].flag;
                _cli_cmd_tbl[i].flag  &= ~clr;
                _cli_cmd_tbl[i].flag  |=  set;
            }
        }
        OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)
    }
    return( result );
}


/**
 * 命令注册
 */
int cli_cmd_reg(const char *name, cli_cmd_fn func, const char *desc)
{
    int  i = -1;

    if ((name == NULL) || (func == NULL)) {
        return( -1 );           // 无效的参数
    }
    // 忽略行首空白,   Todo: 非空白分隔符处理
    for(;  (*name != '\0') &&  isspace(*name);  name++) ;
    if (*name == '\0') {
        return( -1 );           // 命令名为空白
    }
    if (desc != NULL) {
        for(;  (*desc != '\0') &&  isspace(*desc);  desc++) ;
    }
    if (cli_command_find(name) != NULL) {
        return( -1 );           // 命令已存在
    }

    OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
    {   //lint --e{574, 737}    " Signed-unsigned mix / Loss of sign "
        for(i = 0;  i < ARRAY_SIZE(_cli_cmd_tbl);  i++)
        {
            if (_cli_cmd_tbl[i].name != NULL) {
                continue;
            }
            _cli_cmd_tbl[i].name = name;
            _cli_cmd_tbl[i].func = func;
            _cli_cmd_tbl[i].desc = desc;
            break;
        }
        if (i == ARRAY_SIZE(_cli_cmd_tbl)) {
            i =  -1;            // 命令内存池不够
        }
    }
    OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

    return( i );
}


/**
 * 注册二进制命令处理函数
 */
cli_bin_fn cli_bin_reg(cli_bin_fn func)
{
    cli_bin_fn  retval = NULL;

    if (func == NULL) {
        func  = cli_cmd_bin;
//      return( NULL );         // 无效的参数
    }
    OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
    {
        retval  = (cli_bin_fn)CLI_CMD_BIN->func; //lint !e611
        CLI_CMD_BIN->func  = func;
    }
    OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

    return( retval );
}


/**
 * 注册未知命令的处理函数
 */
cli_ufd_fn cli_ufd_reg(cli_ufd_fn func)
{
    cli_ufd_fn  retval = NULL;

    if (func == NULL) {
        func  = cli_cmd_unfound;
//      return( NULL );         // 无效的参数
    }
    OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
    {
        retval  = (cli_ufd_fn)CLI_CMD_UFD->func; //lint !e611
        CLI_CMD_UFD->func  = func;
    }
    OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

    return( retval );
}


/**
 * 注册识别到有效命令(字符串与二进制)提示处理函数
 */
cli_prmt_fn cli_prmt_reg(cli_prmt_fn func)
{
    cli_prmt_fn  retval = NULL;

    if (func == NULL) {
        func  = cli_cmd_idc;
//      return( NULL );         // 无效的参数
    }
    OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
    {
        retval  = (cli_prmt_fn)CLI_CMD_PRMT->func; //lint !e611
        CLI_CMD_PRMT->func  = func;
    }
    OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

    return( retval );
}


/**
 * 显示所有命令帮助信息; "*Help [1]" 掩藏 "**"开头命令, "*Help 0" 显示所有命令
 */
int cli_cmd_help(int cli, const char *args)
{
    const char *name;
    const char *desc;
    size_t      i;
    int         hide;

    if (sscanf(args, "%*s %d", &hide) != 1) {
        hide = 1;       // 命令格式错误
    }

    if (cli_fputs(  ENDL
                    "|   Command name      \t   Command description"ENDL
                    "+-------------------  \t---------------------------------------"ENDL , cli ) < 0) {
        return( -1 );
    }
    for(name = desc = NULL, i = 0;  i < CLI_MAX_COMMAND;  i++)
    {
        OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
        {
            name = _cli_cmd_tbl[i].name;
            desc = _cli_cmd_tbl[i].desc;
        }
        OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

        if (name == NULL)  continue;
        if (desc == NULL)  desc = "";

        if((hide != 0) && (strncmp(name, "**", strlen("**")) == 0)) {
            continue;   // 掩藏, 头为 "**" 的命令信息
        }
        if (cli_fprintf(cli, "| %-20s\t%s"ENDL, name, desc) < 0) {
            return( -1 );
        }
    }
    if (cli_fputs(  "+-------------------  \t---------------------------------------"ENDL ENDL, cli ) < 0) {
        return( -1 );
    }
    return( 0 );
}


/*----------------------------------------------------------------------------*/
/**
 * 默认未注册命令的处理函数
 */
static int cli_cmd_unfound(int cli, const char *args)
{   //lint !e401
    (void)cli;  (void)args;

    return( 0 );
}


/**
 * 默认二进制命令处理函数
 */
static int cli_cmd_bin(int cli, const void *data, int size)
{   //lint !e401
    (void)cli;  (void)data;  (void)size;

    return( 0 );
}


/**
 * 默认接收到命令提示处理函数
 */
static int cli_cmd_idc(int cli, const void *data, int size)
{   //lint !e401
    (void)cli;  (void)data;  (void)size;

    return( 0 );
}


/*----------------------------------------------------------------------------*/
/**
 * 命令函数搜索
 *
 * @param[in]  str   命令名, 字符串长度不能为(0)
 *
 * @return  命令函数指针, (NULL)失败
 */
static void *cli_command_find(const char *str)
{
    const void *name = str;
    void       *func = NULL;

    OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
    {
        if (name == CLI_CMD_UFD) {
            func =  CLI_CMD_UFD->func;  // 未注册命令
            break;
        }
        if (name == CLI_CMD_BIN) {
            func =  CLI_CMD_BIN->func;  // 二进制命令
            break;
        }
        if (name == CLI_CMD_PRMT) {
            func =  CLI_CMD_PRMT->func; // 命令输入提示
            break;
        }
        for(size_t i = 0;  i < ARRAY_SIZE(_cli_cmd_tbl);  i++)
        {
            int         addr;
            uint16_t    flag = _cli_cmd_tbl[i].flag;

            if((name = _cli_cmd_tbl[i].name) == NULL) {
                continue;
            }
            if((addr = cli_strcmdcmp(str, name)) >= 0)
            {
                if (addr == 0) {
                    addr = _cli_cmd_addr[CLI_ADDR_T_DEFAULT];
                }
                if((flag & CLI_ADDR_M_DEFAULT  ) && (addr == _cli_cmd_addr[CLI_ADDR_T_DEFAULT])) {
                    func = _cli_cmd_tbl[i].func; // 默认地址
                    break;
                }
                if((flag & CLI_ADDR_M_BROADCAST) && (addr == _cli_cmd_addr[CLI_ADDR_T_BROADCAST])) {
                    func = _cli_cmd_tbl[i].func; // 默认地址
                    break;
                }
                if((flag & CLI_ADDR_M_MULITCAST) && (addr == _cli_cmd_addr[CLI_ADDR_T_MULITCAST])) {
                    func = _cli_cmd_tbl[i].func; // 默认地址
                    break;
                }
                if((flag & CLI_ADDR_M_UNICAST  ) && (addr == _cli_cmd_addr[CLI_ADDR_T_UNICAST])) {
                    func = _cli_cmd_tbl[i].func; // 默认地址
                    break;
                }
            }
        }
    }
    OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

    return( func );
}


/**
 * 命令数据初始化
 */
static void cli_command_init(void)
{
    size_t  i;

    for(i = 0;  i < ARRAY_SIZE(_cli_cmd_tbl);  i++)
    {
        _cli_cmd_tbl[i].name =   NULL;
        _cli_cmd_tbl[i].desc =   NULL;
        _cli_cmd_tbl[i].func =   NULL;
        _cli_cmd_tbl[i].flag =   CLI_ADDR_M_DEFAULT
                               | CLI_ADDR_M_BROADCAST
                               | CLI_ADDR_M_MULITCAST
                               | CLI_ADDR_M_UNICAST;
    }
    {
        CLI_CMD_UFD->name  = "";
        CLI_CMD_UFD->desc  = "Executive function of unfound command";
        CLI_CMD_UFD->func  = cli_cmd_unfound;

        CLI_CMD_BIN->name  = "";
        CLI_CMD_BIN->desc  = "Executive function of binary command";
        CLI_CMD_BIN->func  = cli_cmd_bin;

        CLI_CMD_PRMT->name = "";
        CLI_CMD_PRMT->desc = "To indicate to receive command";
        CLI_CMD_PRMT->func = cli_cmd_idc;
    }
    {
        _cli_cmd_addr[CLI_ADDR_T_DEFAULT  ] =  CLI_CMD_ADDR_DFT;
        _cli_cmd_addr[CLI_ADDR_T_BROADCAST] =  CLI_CMD_ADDR_INV;
        _cli_cmd_addr[CLI_ADDR_T_MULITCAST] =  CLI_CMD_ADDR_INV;
        _cli_cmd_addr[CLI_ADDR_T_UNICAST  ] =  CLI_CMD_ADDR_INV;
    }
    OS_MUTEX_CREATE(_cli_cmd_mutex, "cli_cmd Mutex");
}


#if 0
// #ifndef _lint
/*----------------------------------------------------------------------------*/
/**
 * 执行字符串命令
 */
int cli_cmd_exec(int cli, const char *str)
{
    int  retval;

    if(!CLI_ID_IS_VALID(cli) || (str == NULL)) {
        return( -1 );               // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }

    {
        if (str[0] == 'C')
        {
            if (str[strlen(str) - 1] == '\n') {
                cli_fprintf(cli, "StrExe:%s", str);
            }
            else {
                cli_fprintf(cli, "StrExe:%s"ENDL, str);
            }
            CLI_IF_STAT |= CLI_EXEC_BIN;

            retval = strlen(str);
        }
        else
        {
            retval = 0;
        }
    }

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( retval );
}


/**
 * 执行二进制命令
 */
int cli_bin_exec(int cli, const void* data, int size)
{
    int  retval;

    if(!CLI_ID_IS_VALID(cli) || (data == NULL) || (size <= 0)) {
        return( -1 );               // 参数错误
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }

    {
        fprintf(stderr, "BinCmd:");
        for(int i = 0;  i < size;  i++)
        {
            fprintf(stderr, " 0x%02X", ((uint8_t *)data)[i]);
        }
        fprintf(stderr, ENDL);

        retval = ((uint8_t *)data)[0] - '0';
        if ((5 <= retval) && (retval <= 9) && (size >= retval))
        {
            cli_fprintf(cli, ENDL"BinExe:");
            for(int i = 0;  i < retval;  i++)
            {
                cli_fprintf(cli, " 0x%02X", ((uint8_t *)data)[i]);
            }
            cli_fflush(cli);
        }
        else
        {
            retval = 0;
        }
    }

    if (cli_refs_dec(cli) < 0) {
        return( -1 );
    }
    return( retval );
}
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

