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
 * �����д����̺߳���
 *
 * @param[in]  arg  �����нӿ�ID(��ǿ������ת��)
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
 * �����нӿڳ�ʼ��
 */
void cli_shell_init(void)
{
    for(size_t cli = 1;  cli <= ARRAY_SIZE(_cli_if_tbl);  cli++)
    {
        CLI_IF_DATA_INIT(NULL, NULL, 0);    // �����нӿ����ݳ�ʼ��
        OS_MUTEX_CREATE(CLI_IF_I_LOCK, CLI_LOCK_NAME[0]);
        OS_MUTEX_CREATE(CLI_IF_O_LOCK, CLI_LOCK_NAME[1]);
        OS_MUTEX_CREATE(CLI_IF_LOCK,   CLI_LOCK_NAME[2]);
        CLI_IF_REFS =  0;
        CLI_IF_ID   = -1;                   // �����нӿ���������Ϊδ����
    }
    OS_MUTEX_CREATE(_cli_if_mutex, "cli_if Mutex");

    cli_command_init();
}


/**
 * ���������нӿ�
 */
int cli_shell_new(const cli_device_t *device, const char *name, int flag)
{
    static int  _new_ops = 0;
    int         cli, retval;

    if (device == NULL) {
        return( -1 );                       // ��Ч�Ĳ���
    }
    if((device->open == NULL) || (device->close == NULL) || (device->read == NULL) || (device->write == NULL)) {
        return( -1 );                       // ��Ч�Ĳ���
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
                    retval = -1;   break;   // �ѷ���
                }
                CLI_IF_DATA_INIT(device, name, flag);

                if (CLI_DEVICE_OPEN() < 0) {
                    retval = -1;   break;   // ���豸ʧ��
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
                retval      = cli;          // �ɹ�����
            }
            OS_MUTEX_LEAVE(CLI_IF_LOCK, OS_MUTEX_TIMEOUT_HANDLER)
            _new_ops  = ((_new_ops < (int)(ARRAY_SIZE(_cli_if_tbl) - 1)) ? (_new_ops + 1) : 0);
        }
        OS_MUTEX_LEAVE(_cli_if_mutex, OS_MUTEX_TIMEOUT_HANDLER)

        if (retval > 0) {
            return( retval );               // �ɹ�����
        }
    }   // for()

    return( -1 );
}


/**
 * �ͷ������нӿ�
 */
int cli_shell_free(int cli)
{
    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );   // ��Ч�Ĳ���
    }
    return( cli_refs_dec(cli) );
}


/**
 * ���������нӿڿ��Ʊ�־
 */
int cli_shell_flag(int cli, int set, int clr)
{
    int  flag = -1;

    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );       // ��Ч�Ĳ���
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
 * ��ȡ�����нӿ���
 */
const char *cli_shell_name(int cli)
{
    const char *str;

    if(!CLI_ID_IS_VALID(cli)) {
        return( NULL );     // ��Ч�Ĳ���
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
 * ����ִ����ѯ, ���豸������, ����Ϊ���������, Ȼ��ִ��
 */
int cli_shell_poll(int cli)
{
    char    ch;
    char   *line;
    void   *func;
    int     ret = -1;

    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );       // ��Ч�Ĳ���
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
            { //---- ���뻺������ ת�Ƶ� �л��� ----
                if((ret = cli_line_char(cli, ch, CLI_IF_L_BUFF, sizeof(CLI_IF_L_BUFF))) < 0) {
                    break;  // (ret < 0)        // �豸�����쳣
                }
                if (ret < (int)(sizeof(CLI_IF_L_BUFF) - 1)) {
                    continue;                   // �л���δ��, �����н�����
                }
                if (CLI_IF_L_BUFF[0] != '\0') { // �л�������, �����н�����
                    break;  // (ret >= 0)       // ���������
                }
             // if (CLI_IF_I_CUR == 1) {        // ���� "\r\n" ���� '\n'
             //     cli_buff_free(cli, 1);      // �ͷ����뻺��
             // }
                continue;
            } //---- ���뻺������ ת�Ƶ� �л��� ----

            if((ret = cli_buff_read(cli)) <= 0)
            {   // (< 0): �豸�����쳣; (== 0): �Ѷ�ȡ��һ����������
                break;  // (ret <= 0)
            }

            if((CLI_IF_STAT & CLI_EXEC_BIN) && (CLI_IF_I_POS != 0))
            { //---- ���������� ���� ----
                if((ret = CLI_FUNC_BIN(cli, CLI_IF_I_BUFF, CLI_IF_I_POS)) == 0) {
                    continue;                   // �޷�ʶ�� ����������
                }
                if (ret < 0) {
                    break;  // (ret < 0)        // �豸�����쳣
                }
                if (CLI_FUNC_PRMT(cli, CLI_IF_I_BUFF, CLI_IF_I_POS) < 0) {
                    ret = -1;
                    break;                      // �豸�����쳣
                }
                CLI_IF_DEV_ECHO(ret = -1, break, ENDL          , strlen(ENDL)
                                               , CLI_BIN_PROMPT, strlen(CLI_BIN_PROMPT)
                                               , CLI_IF_PROMPT , strlen(CLI_IF_PROMPT)
                               );
             // LOG_VERB("%s exec BIN CMD, Len: %u", cli_shell_name(cli), ret);
              #ifdef RTE_Compiler_EventRecorder
                EventStopAx(cli, CLI_IF_I_BUFF, CLI_IF_I_POS);  //lint !e534 !e737 !e835
              #endif
                cli_buff_free(cli, ret);        // �ͷ����뻺��
                CLI_IF_L_CUR = CLI_IF_L_POS = 0;
                continue;                       // �ɹ�ִ�� ����������
            } //---- ���������� ���� ----
        } // for()

        if((ret < 0) || ((ret = CLI_FUNC_PRMT(cli, CLI_IF_I_BUFF, CLI_IF_I_POS)) < 0)) {
            break;  // (ret < 0)                // �豸�����쳣
        }

        if (CLI_IF_STAT & CLI_EXEC_STR)
        { //---- ִ�� �ַ������� ������ ----    // �������� ɾ�����׿հ� ��������
            for(line = CLI_IF_L_BUFF;  (line[0] != '\0') && isspace(line[0]);  line++) ;

            if((line[0] != '\0') && !(CLI_IF_STAT & CLI_NO_SEARCH) && ((func = cli_command_find(line)) != NULL))
            {   // �ǿ��ַ���        // �����ַ����������          // ��������Ч�������
              #ifdef RTE_Compiler_EventRecorder
                EventStopAx (cli, __FILE__, __LINE__); //lint !e534 !e737 !e835
                EventStartBx(cli, line, strlen(line)); //lint !e534 !e737 !e835
              #endif
                LOG_VERB("%s exec STR CMD: %s", cli_shell_name(cli), line);

                if ((ret = ((cli_cmd_fn)func)(cli, line)) < 0) { //lint !e611
                    break; // (ret < 0)         // �豸�����쳣
                }
                cli_buff_free(cli, 0);          // �ɹ�ִ���ַ������� �ͷ����뻺��

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
                    cli_buff_free(cli, 0);      // �ɹ�����δע������ �ͷ����뻺��

              #ifdef RTE_Compiler_EventRecorder
                EventStopBx(cli, __FILE__, __LINE__);  //lint !e534 !e737 !e835
              #endif
                }
                else if (ret == 0) {            // �޷�����δע������
                    CLI_IF_DEV_ECHO(ret = -1, break, CLI_UFD_PROMPT, strlen(CLI_UFD_PROMPT), NULL, 0, NULL, 0);
                } else {
                    break; // (ret < 0)         // �豸�����쳣
                }
            }
        } //---- ִ�� �ַ������� ������ ----
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
 * ������, ���� stdio.h fread(), ��û�� feof() �������
 */
size_t cli_fread(void *ptr, size_t size, size_t nmemb, int cli)
{
    size_t  len;
    size_t  num = 0;

    if(!CLI_ID_IS_VALID(cli) || (ptr == NULL) || (size == 0) || (size > sizeof(CLI_IF_I_BUFF)) || (nmemb == 0)) {
        return( 0 );                            // ��������
    }
    if (cli_refs_inc(cli) < 0) {
        return( 0 );
    }

    OS_MUTEX_ENTER(CLI_IF_I_LOCK, OS_WAIT_FOREVER)
    {
        cli_buff_free(cli, 0);                  // �ͷ����뻺��

        for(len = CLI_IF_I_POS - CLI_IF_I_CUR;  len < size;) {
            if(cli_buff_read(cli) < 0)  break;  // ���豸��������, ֱ����������һ��Ԫ��
        }
        if (len >= size) {
            num  = len / size;
            num  = MIN(num, nmemb);
            memcpy(ptr, &(CLI_IF_I_BUFF[CLI_IF_I_CUR]), size * num);
            CLI_IF_I_CUR += size * num;         //lint !e734
            cli_buff_free(cli, 0);              // �ͷ����뻺��
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
 * д����(ȫ����ģʽ), ���� stdio.h fwrite(), ��û�� feof() �������
 */
size_t cli_fwrite(const void *ptr, size_t size, size_t nmemb, int cli)
{
    int     ret, tmp;
    size_t  num = 0;

    if(!CLI_ID_IS_VALID(cli) || (ptr == NULL) || (size == 0) || (nmemb == 0)) {
        return( 0 );                        // ��������
    }
    if (cli_refs_inc(cli) < 0) {
        return( 0 );
    }

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {
        // Ԫ�ش�С > �������, ֱ�����
        if (size > sizeof(CLI_IF_O_BUFF))
        {
            if (cli_buff_flush(cli, 1) < 0) {
                break;                      // �豸�����쳣
            }
            for(num = 0;  num < nmemb;  num++)
            {
                CLI_IF_DEV_WRITE(ptr, size, tmp, ret);
                if (ret != size) {          //lint !e737  " Loss of sign "
                    break;                  // �豸�����쳣
                }
            }
            break;
        }

        // Ԫ�ش�С <= �������, ���浽����
        for(num = 0;  num < nmemb;  num++)
        {
            if ((sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS) < size)
            {   // ������治��, ˢ���������(ȫ����ģʽ)
                if (cli_buff_flush(cli, 1) < 0) {
                    break;                  // �豸�����쳣
                }
            }
            assert((sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS) >= size);
         // memmove(&(CLI_IF_O_BUFF[CLI_IF_O_POS]), &(((uint8_t *)ptr)[num * size]), size);
            memcpy (&(CLI_IF_O_BUFF[CLI_IF_O_POS]), &(((uint8_t *)ptr)[num * size]), size);

            if ((CLI_IF_O_POS += size) == sizeof(CLI_IF_O_BUFF))  //lint !e734  " Loss of precision "
            {   // �����������, ˢ���������(ȫ����ģʽ)
                if (cli_buff_flush(cli, 1) < 0) {
                    break;                  // �豸�����쳣
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
 * �ַ������, ���� stdio.h fputs(), ��û�� feof() �������
 */
int cli_fputs(const char *str, int cli)
{
    int  num, size, tmp;

    if(!CLI_ID_IS_VALID(cli) || (str == NULL)) {
        return( -1 );                       // ��������
    }
    if ((size = strlen(str)) <= 0) {        //lint !e713  " Loss of precision "
        return( 0 );                        // ��������
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    num = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {
        // ������治��, ˢ���������(ȫ����ģʽ)
        if (size > (sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS))  //lint !e574 !e737
        {
            if (cli_buff_flush(cli, 1) < 0) {
                num = -1;                   // �豸�����쳣
                break;
            }
        }

        // �ַ������� > �������, ֱ�����
        if (size > sizeof(CLI_IF_O_BUFF))   //lint !e574 !e737
        {
            CLI_IF_DEV_WRITE(str, size, tmp, num);
            if (num != size) {
                num = -1;                   // �豸�����쳣
                break;
            }
            break;
        }
        assert(size <= (sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS));

        // �ַ��� ���Ƶ��������
     // memmove(&(CLI_IF_O_BUFF[CLI_IF_O_POS]), str, size);  //lint !e732
        memcpy (&(CLI_IF_O_BUFF[CLI_IF_O_POS]), str, size);  //lint !e732
        CLI_IF_O_POS += size;   //lint !e734
        num           = size;

        if (cli_buff_flush(cli, 0) < 0) {
            num = -1;                       // �豸�����쳣
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
 * ��ʽ�����(�л���ģʽ), ���� stdio.h fprintf(), ��û�� feof() �������
 */
int cli_fprintf(int cli, const char *format, ...)
{
    va_list  args;
    int      size;
    int      retval;

    if(!CLI_ID_IS_VALID(cli) || (format == NULL)) {
        return( -1 );                   // ��������
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
            {   // ���������, ˢ���������(ȫ����ģʽ)
                if (cli_buff_flush(cli, 1) < 0) {
                    retval = -1;        // �豸�����쳣
                    break;
                }
                size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;
            }
            assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

            va_start(args, format); //lint -e{732}  " Loss of sign "
            retval = vsnprintf((char*)&(CLI_IF_O_BUFF[CLI_IF_O_POS]), size, format, args);
            va_end(args);

            if (retval < 0) {
                retval = -1;            // �������
                break;
            }
            if (retval < size) { //lint -e{734}  " Loss of precision "
                CLI_IF_O_POS += retval; // �ɹ���ȫ���
                break;
            }
            if (size == sizeof(CLI_IF_O_BUFF)) { //lint !e737  " Loss of sign "
                CLI_IF_O_POS  = size;            //lint !e734  " Loss of precision "
                CLI_IF_O_BUFF[sizeof(CLI_IF_O_BUFF) - 1] = '\n';
                break;                  // ������ȫ���, ʹ����������������˳�
            }
        }   // for()

        if (retval > 0)                 // �����, ˢ���������
        {   //lint -e{730}  " Boolean argument to function "
            if (cli_buff_flush(cli, CLI_IF_O_POS == sizeof(CLI_IF_O_BUFF)) < 0) {
                retval = -1;            // �豸�����쳣
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
 * ��ʽ�����(�л���ģʽ), ���� stdio.h vfprintf(), ��û�� feof() �������
 */
int cli_vfprintf(int cli, const char *format, va_list arg)
{
    int      size;
    int      retval;

    if(!CLI_ID_IS_VALID(cli) || (format == NULL)) {
        return( -1 );                   // ��������
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
            {   // ���������, ˢ���������(ȫ����ģʽ)
                if (cli_buff_flush(cli, 1) < 0) {
                    retval = -1;        // �豸�����쳣
                    break;
                }
                size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;
            }
            assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

            //lint -e{732}  " Loss of sign "
            retval = vsnprintf((char*)&(CLI_IF_O_BUFF[CLI_IF_O_POS]), size, format, arg);
            if (retval < 0) {
                retval = -1;            // �������
                break;
            }
            if (retval < size) { //lint -e{734}  " Loss of precision "
                CLI_IF_O_POS += retval; // �ɹ���ȫ���
                break;
            }
            if (size == sizeof(CLI_IF_O_BUFF)) { //lint !e737  " Loss of sign "
                CLI_IF_O_POS  = size;            //lint !e734  " Loss of precision "
                CLI_IF_O_BUFF[sizeof(CLI_IF_O_BUFF) - 1] = '\n';
                break;                  // ������ȫ���, ʹ����������������˳�
            }
        }   // for()

        if (retval > 0)                 // �����, ˢ���������
        {   //lint -e{730}  " Boolean argument to function "
            if (cli_buff_flush(cli, CLI_IF_O_POS == sizeof(CLI_IF_O_BUFF)) < 0) {
                retval = -1;            // �豸�����쳣
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
 * ���Կ����Ÿ�ʽ�����(�л���ģʽ), ���� stdio.h fprintf(), ��û�� feof() �������
 */
int cli_fechof(int cli, const char *format, ...)
{
    va_list  args;
    int      size;
    int      retval;

    if(!CLI_ID_IS_VALID(cli) || (format == NULL)) {
        return( -1 );                   // ��������
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    if((CLI_IF_STAT & CLI_ECHO_ON) == 0) {
        return( 0 );                    // ���Թر�
    }
    retval = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {   //lint --e{850}  " for loop index variable 'xxx' is modified in body "
        for(size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;  ;  size = 0)
        {
            if (size == 0)
            {   // ���������, ˢ���������(ȫ����ģʽ)
                if (cli_buff_flush(cli, 1) < 0) {
                    retval = -1;        // �豸�����쳣
                    break;
                }
                size = sizeof(CLI_IF_O_BUFF) - CLI_IF_O_POS;
            }
            assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

            va_start(args, format); //lint -e{732}  " Loss of sign "
            retval = vsnprintf((char*)&(CLI_IF_O_BUFF[CLI_IF_O_POS]), size, format, args);
            va_end(args);

            if (retval < 0) {
                retval = -1;            // �������
                break;
            }
            if (retval < size) { //lint -e{734}  " Loss of precision "
                CLI_IF_O_POS += retval; // �ɹ���ȫ���
                break;
            }
            if (size == sizeof(CLI_IF_O_BUFF)) { //lint !e737  " Loss of sign "
                CLI_IF_O_POS  = size;            //lint !e734  " Loss of precision "
                CLI_IF_O_BUFF[sizeof(CLI_IF_O_BUFF) - 1] = '\n';
                break;                  // ������ȫ���, ʹ����������������˳�
            }
        }   // for()

        if (retval > 0)                 // �����, ˢ���������
        {   //lint -e{730}  " Boolean argument to function "
            if (cli_buff_flush(cli, CLI_IF_O_POS == sizeof(CLI_IF_O_BUFF)) < 0) {
                retval = -1;            // �豸�����쳣
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
 * ˢ���������, ���� stdio.h fflush(), ��û�� feof() �������
 */
int cli_fflush(int cli)
{
    int      retval;

    if(!CLI_ID_IS_VALID(cli)) {
        return( -1 );       // ��������
    }
    if (cli_refs_inc(cli) < 0) {
        return( -1 );
    }
    retval = -1;

    OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)
    {
        if (cli_buff_flush(cli, 1) >= 0) {
            retval = 0;     // �ɹ�
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
 * ���ַ�����
 *
 * @param[in]  cli   �����нӿ�
 * @param[in]  ch    �����ַ�
 * @param[in]  line  �л���
 * @param[in]  size  �л����С
 *
 * @return  С��0: �豸�����쳣, ����: �л����ַ���
 *          @arg С��(size - 1): �л���δ��, Ҳδ���뻻�з�, ���ٴ�����
 *          @arg ����(size - 1): �л�������, �ٴ�������ַ���������
 *          @arg ����(size - 1): �����ַ�Ϊ�н�����, �л����Ƿ��������������ж�
 *
 * @note    �ⲿ���� (RW)CLI_IF_L_STAT, (RW)CLI_IF_L_POS, (RW)CLI_IF_L_CUR
 * @note    �ⲿ���� CLI_DEVICE_ECHO()
 */
static int cli_line_char(int cli, char ch, char *line, int size)
{
    if (CLI_IF_L_POS >= (size - 1)) {
        return( size - 1 );                             // �л�����
    }
    assert(CLI_IF_L_CUR <= CLI_IF_L_POS);

    if (CLI_IF_STAT & CLI_ECHO_ON)
    {   /**
         * ���м���
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
        case '\x1B' :       // ESC  �����
            CLI_IF_L_STAT  = CLI_LINE_STAT_SEQUC;       // ESC,0x1B
            return( CLI_IF_L_POS );
        case '[' :          //
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SEQUC) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_SPEC1;   // ESC,0x1B  [,0x5B
                return( CLI_IF_L_POS );
            }
            break;
        case 'A' :          // 'A'  ��
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) { // ESC,0x1B  [,0x5B  A,0x41
                ch  =  CLI_KEY_UP;
            }
            break;
        case 'B' :          // 'B'  ��
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) { // ESC,0x1B  [,0x5B  B,0x42
                ch  =  CLI_KEY_DOWN;
            }
            break;
        case 'C' :          // 'C'  ��
            if (CLI_IF_L_STAT == CLI_LINE_STAT_SPEC1) { // ESC,0x1B  [,0x5B  C,0x43
                ch  =  CLI_KEY_RIGHT;
            }
            break;
        case 'D' :          // 'D'  ��
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
        case CLI_KEY_CR :   // '\r' 0x0D �س�(Enter)
            if (CLI_IF_L_STAT != CLI_LINE_STAT_FEED2) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_FEED1;
                line[CLI_IF_L_POS++] = '\n';            // �����н�����
                line[CLI_IF_L_POS  ] = '\0';
            } else {
                CLI_IF_L_STAT  = CLI_LINE_STAT_NORMAL;
                line[CLI_IF_L_POS  ] = '\0';
            }
            return( size );
    
        case CLI_KEY_LF :   // '\n' 0x0A ����(Enter)
            if (CLI_IF_L_STAT != CLI_LINE_STAT_FEED1) {
                CLI_IF_L_STAT  = CLI_LINE_STAT_FEED2;
                line[CLI_IF_L_POS++] = '\n';            // �����н�����
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

        //----------------- ���Ƽ� ---------------------
        switch (ch) {
        case CLI_KEY_INSERT :   // 0x13 Insert
        case CLI_KEY_BEL :      // 0x07 ����
        case CLI_KEY_FF :       // 0x0C ��ҳ
        case CLI_KEY_VT :       // 0x0B ��ֱ�Ʊ�
            return( CLI_IF_L_POS );
        
        case CLI_KEY_HT :       // 0x09 ˮƽ�Ʊ�(Tab)
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
                CLI_IF_L_CUR = 0;                       // ����Ƶ�����
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_END :      // 0x14 End
            if (CLI_IF_L_SUB > 0) {
                CLI_IF_DEV_ECHO(, return -1, &line[CLI_IF_L_CUR], CLI_IF_L_SUB, NULL, 0, NULL, 0);
                CLI_IF_L_CUR = CLI_IF_L_POS;            // ����Ƶ���β
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_RIGHT :    // 0x10 Right Arrow
            if (CLI_IF_L_SUB > 0) {
                CLI_IF_DEV_ECHO(, return -1, &line[CLI_IF_L_CUR], 1, NULL, 0, NULL, 0);
                CLI_IF_L_CUR++;                         // �������
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_LEFT :     // 0x11 Left  Arrow
            if (CLI_IF_L_CUR > 0) {
                CLI_IF_DEV_ECHO(, return -1, &_cli_str_bs[1], 1, NULL, 0, NULL, 0);
                CLI_IF_L_CUR--;                         // �������
            }
            return( CLI_IF_L_POS );
        
        case CLI_KEY_BS :       // 0x08 �˸�(Backspace)
            if (CLI_IF_L_CUR == 0) {
                return( CLI_IF_L_POS );                 // ���λ������
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
                return( CLI_IF_L_POS );                 // ���λ����β
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

        //----------------- ��ͨ�� ---------------------
        if((ch != '\r') && (ch != '\n') && !isprint(ch)) {
            return( CLI_IF_L_POS );
        }
    }

    if (CLI_IF_L_SUB > 0) {                             // ���м�����ַ�
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
 * ���豸ͨ�Žӿڻ�ȡ����(��������, �������������нӿڻص�����)
 *
 * @param[in]  cli   �����нӿ�
 * @param[in]  size  ���ȡ�����ݳ���
 *
 * @return  ��ȡ����ָ���������ݵ�ָ��(ע�ⲻ���޸�����)
 */
const void *cli_data_get(int cli, size_t size)
{
    int         len;
    const void *ptr;

    if(!CLI_ID_IS_VALID(cli) || (size == 0) || (size > sizeof(CLI_IF_I_BUFF))) {
        return( NULL );                             // ��������
    }

    if((sizeof(CLI_IF_I_BUFF) - CLI_IF_I_CUR) < size) {
        cli_buff_free(cli, 0);                      // �ͷ����뻺��
    }
    LOG_VERB("Get BIN: %u(%u)\t on %s", size, CLI_IF_I_POS - CLI_IF_I_CUR, cli_shell_name(cli));

    while((CLI_IF_I_POS - CLI_IF_I_CUR) < size)     //lint !e574
    {
        if((len = CLI_DEVICE_READ(&(CLI_IF_I_BUFF[CLI_IF_I_POS]), sizeof(CLI_IF_I_BUFF) - CLI_IF_I_POS)) < 0) {
            return( NULL );                         // ���豸��������
        }
        CLI_IF_I_POS += len;  //lint !e734          // �豸���ɹ�
        LOG_VERB("Rev BIN: %u(%u)\t on %s", size, CLI_IF_I_POS - CLI_IF_I_CUR, cli_shell_name(cli));
    }
    ptr  =  &(CLI_IF_I_BUFF[CLI_IF_I_CUR]);
    CLI_IF_I_CUR  +=  size;   //lint !e734          // �ͷ����뻺��

    return(ptr);
}


/**
 * ���豸�����ݵ����뻺��
 *
 * @param[in]  cli  �����нӿ�
 *
 * @retval  ( > 0) ��������ݳ���(���ֽ�Ϊ��λ)
 * @retval  (== 0) �Ѷ�ȡ��һ����������, ���������ݰ�(��'\n')�ָ�������
 * @retval  ( < 0) �豸�����쳣
 */
static int cli_buff_read(int cli)
{
    int  len;
    int  size  = sizeof(CLI_IF_I_BUFF) - CLI_IF_I_POS;
    assert((CLI_IF_I_CUR <= CLI_IF_I_POS) && (CLI_IF_I_POS <= sizeof(CLI_IF_I_BUFF)));

    if (size == 0)
    {  // ���뻺����, �������뻺�����һ�ֽ�����
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
        CLI_IF_I_POS += (int16_t)len;   // �豸���ɹ�
    }
    return( len );
}


/**
 * �����뻺���ȡ����
 *
 * @param[in]  cli   �����нӿ�
 * @param[out] buff  ���������
 * @param[in]  size  ��������С, ���ֽ�Ϊ��λ
 *
 * @retval  ( > 0) �ɹ���ȡ�����ݳ���
 * @retval  (<= 0) ���뻺��Ϊ��
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
 * �ͷ����뻺��(���ֽ�Ϊ��λ)
 *
 * @param[in]  cli   �����нӿ�
 * @param[in]  size  ( > 0)�ͷ�ָ���ֽ�, (<= 0)�ͷ� cli_buff_get() ���������.
 */
static void cli_buff_free(int cli, int size)
{
    if (size <= 0)
    {   // �ͷ������� cli_buff_get() ���������
        size =  CLI_IF_I_CUR;
    }

    if ((size < 0) || (size >= CLI_IF_I_POS))
    {   // ������뻺��
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
 * ˢ���������
 *
 * @param[in]  cli   �����нӿ�
 * @param[in]  all   (1)ȫ����ģʽ, (0)�л���ģʽ
 *
 * @retval  (>= 0) д���豸�����ݳ���(���ֽ�Ϊ��λ)
 * @retval  ( < 0) �豸�����쳣
 */
static int cli_buff_flush(int cli, int all)
{
    int     num, size, tmp;
    assert(sizeof(CLI_IF_O_BUFF) >= CLI_IF_O_POS);

    // size <-- д���豸���ݳ���
    if(((size = CLI_IF_O_POS), all) == 0)
    {
        for(;  size > 0;  size--)
        {
            switch (CLI_IF_O_BUFF[size - 1]) {
            default :          continue;
         // case CLI_KEY_CR :  break;
            case CLI_KEY_LF :  break;   // ���һ��з�
            }
            break;
        }
    }

    // size ��������д���豸, num <-- ʵ��д����
    if((num = 0, size) > 0) {
        CLI_IF_DEV_WRITE(CLI_IF_O_BUFF, size, tmp, num);
    }

    // �ƶ��������δд���豸������
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
 * ���������нӿ����ü���
 *
 * @param[in]  cli  �����нӿ�
 *
 * @retval  (>= 0) �ɹ�, �����޸ĺ�������нӿ����ü���
 * @retval  ( < 0) ʧ��
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
 * ���������нӿ����ü���
 *
 * @param[in]  cli  �����нӿ�
 *
 * @retval  (>= 0) �ɹ�, �����޸ĺ�������нӿ����ü���
 * @retval  ( < 0) ʧ��
 *
 * @note    ������ֵΪ(0)ʱ, �ú������ͷ������нӿ�
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
 * ɾ���ַ�����β�հ�
 */
char *cli_strdelspace(char *str)
{
    char  *tmp = str + strlen(str) - 1;

    for(;  (tmp != str) && isspace(*tmp);  tmp--)
    {   // ɾ����β�հ�
        *tmp = '\0';
    }
    for(;  (str[0] != '\0') && isspace(str[0]);  str++)
    {   // ɾ�����׿հ�
    }
    return( str );
}


/**
 * �����ַ��� n �ζ������Ӵ�
 */
const char *cli_strdelim(const char *s, unsigned n, const char *delim)
{
    if (delim == NULL)
    {
        while (1)   //lint !e716
        {
            for(;  isspace(*s);  s++) {     // ���������
                if(*s == '\0')  return( NULL );
            }
            if(n-- == 0)  break;            // ����ָ���������

            for(;  !isspace(*s);  s++) {    // �����Ƕ����
                if(*s == '\0')  return( NULL );
            }
        }
    }
    else
    {
        while (1)   //lint !e716
        {
            for(;  strchr(delim, *s) != NULL;  s++) {   // ���������
                if(*s == '\0')  return( NULL );
            }
            if(n-- == 0)  break;                        // ����ָ���������

            for(;  strchr(delim, *s) == NULL;  s++) {   // �����Ƕ����
                if(*s == '\0')  return( NULL );
            }
        }
    }
    return( s );
}


/**
 * �ַ����Ƚ�, �������׿հ׺ʹ�Сд, ���������׿հ׻���ֹ��ʱ�����Ƚ�.
 */
int cli_strtrimcmp(const char *s1, const char *s2)
{
    int  c1, c2;

    // �������׿հ�, Todo: �ǿհ׷ָ�������
    for(;  (*s1 != '\0') && isspace(*s1);  s1++) ;
    for(;  (*s2 != '\0') && isspace(*s2);  s2++) ;

    /*----*/  c1 = tolower(*s1++), c2 = tolower(*s2++);
    for(;  ;  c1 = tolower(*s1++), c2 = tolower(*s2++))
    {
        if (isspace(c1)) {
            c1  = '\0';         // �հ״��� �� ��ֹ����ͬ
        }
        if (isspace(c2)) {
            c2  = '\0';         // �հ״��� �� ��ֹ����ͬ
        }

        if (c1 != c2) {
            break;              // ��ͬ�ַ�
        }
        if (c1 == '\0') {
            return( 0 );        // ��ͬ�ַ�, ��Ϊ��ֹ��
        }
    }
    return( c1 - c2 );
}


/**
 * �ַ����Ƚ�, �������׿հ׺ʹ�Сд, ���������׿հ׻���ֹ��ʱ�����Ƚ�
 */
int cli_strntrimcmp(const char *s1, const char *s2, size_t n)
{
    int  c1, c2;

    // �������׿հ�, Todo: �ǿհ׷ָ�������
    for(;  (*s1 != '\0') && isspace(*s1);  s1++) ;
    for(;  (*s2 != '\0') && isspace(*s2);  s2++) ;

    /*--------------*/  c1 = tolower(*s1++), c2 = tolower(*s2++);
    for(;  n > 0;  n--, c1 = tolower(*s1++), c2 = tolower(*s2++))
    {
        if (isspace(c1)) {
            c1  = '\0';         // �հ״��� �� ��ֹ����ͬ
        }
        if (isspace(c2)) {
            c2  = '\0';         // �հ״��� �� ��ֹ����ͬ
        }

        if (c1 != c2) {
            break;              // ��ͬ�ַ�
        }
        if (c1 == '\0') {
            return( 0 );        // ��ͬ�ַ�, ��Ϊ��ֹ��
        }
    }
    return( c1 - c2 );
}


//lint -esym(714, cli_strcmdcmp)
/**
 * �������Ƚ�, �����հ׻���ֹ��ʱ�����Ƚ�, �Ƚ�ʱ���� s1 �кϷ������Ӵ�, �����ظ���ֵ. \n
 * eg: s1 = ":SOUR:VOLT:RANG  1.2e-3\n", s2 = ":SOUR:RANG", �򷵻� 0. \n
 * eg: s1 = ":SOUR1:VOLT:RANG 1.2e-3\n", s2 = ":SOUR:RANG", �򷵻� 1. \n
 * eg: s1 = ":SOUR2:VOLT:RANG 1.2e-3\n", s2 = ":SOUR:RANG", �򷵻� 2.
 *
 * @param[in]  s1   ������ַ���.
 * @param[in]  s2   �������ַ���.
 *
 * @retval  (== 0) ������ƥ��ɹ�, �������Ӵ�.
 * @retval  ( > 0) ������ƥ��ɹ�, �������Ӵ������������Ӵ�ֵ.
 * @retval  ( < 0) ������ƥ��ʧ��, s1 �� s2 ����, �������Ӵ��Ƿ�.
 *
 * @note    �Ϸ������Ӵ�: ����λ������, ������ð�Ż��ʺŻ�հ׽���, ֵ����Ϊ0
 */
int cli_strcmdcmp(const char *s1, const char *s2)
{
    int  num;
    int  c1, c2;

//  // �������׿հ�, Todo: �ǿհ׷ָ�������
//  for(;  (*s1 != '\0') && isspace(*s1);  s1++) ;
//  for(;  (*s2 != '\0') && isspace(*s2);  s2++) ;

    /*-----------*/  c1 = tolower(*s1++), c2 = tolower(*s2++);
    if (isdigit(c1)) {
        return( -1 );           // ���ַ�Ϊ����
    }
    for(num = 0;  ;  c1 = tolower(*s1++), c2 = tolower(*s2++))
    {
        if((num == 0) && isdigit(c1))
        {                       // δ���������Ӵ�ʱ, ���������ַ�����ת��Ϊ��ֵ
            do {
                num  = (num << 3) + (num << 1)/*(num * 10)*/ + c1 - '0'; //lint !e701
            } while( isdigit(c1 = tolower(*s1++)) );

            if((num == 0) || (c1 != ':') && (c1 != '?') && !isspace(c1)) {
                return( -1 );   // �����Ӵ�ֵΪ0, ����ð�Ż��ʺŻ�հ׽���
            }
        }

        if (isspace(c1)) {
            c1  = '\0';         // �հ״��� �� ��ֹ����ͬ
        }
        if (isspace(c2)) {
            c2  = '\0';         // �հ״��� �� ��ֹ����ͬ
        }

        if (c1 != c2) {
            break;              // ��ͬ�ַ�
        }
        if (c1 == '\0') {
            return( num );      // ��ͬ�ַ�, ��Ϊ��ֹ��
        }
    }
    return( -1 );               // �ַ�������
}


/**
 * ���ַ��� n �ζ������Ӵ���, ʶ�𶨹ؼ��ֺ��������
 */
int cli_strsplitnum(const char *s, const char *key, unsigned n, const char *delim)
{
    unsigned  num;
    size_t    len = strlen(key);

    if((s = cli_strdelim(s, n, delim)) == NULL) {
        return( 0 );    // ���Ӵ�
    }
    if (cli_strntrimcmp(s, key, len) != 0) {
        return( 0 );    // �ؼ��ִ���
    }
    if((num = strtoul(&(s[len]), NULL, 10)) == 0) {
        return( -1 );   // ��ʽ����(�ؼ��ֺ��������)
    }
    return( num );      //lint !e713
}


/**
 * ��һ���ڴ��������ƥ����һ���ڴ��ĵ�һ��λ��.
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
 * �޸������ַ, ������������Ч.
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
 * �޸�����ѡ��, ����ָ��������Ч.
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
 * ����ע��
 */
int cli_cmd_reg(const char *name, cli_cmd_fn func, const char *desc)
{
    int  i = -1;

    if ((name == NULL) || (func == NULL)) {
        return( -1 );           // ��Ч�Ĳ���
    }
    // �������׿հ�,   Todo: �ǿհ׷ָ�������
    for(;  (*name != '\0') &&  isspace(*name);  name++) ;
    if (*name == '\0') {
        return( -1 );           // ������Ϊ�հ�
    }
    if (desc != NULL) {
        for(;  (*desc != '\0') &&  isspace(*desc);  desc++) ;
    }
    if (cli_command_find(name) != NULL) {
        return( -1 );           // �����Ѵ���
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
            i =  -1;            // �����ڴ�ز���
        }
    }
    OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

    return( i );
}


/**
 * ע��������������
 */
cli_bin_fn cli_bin_reg(cli_bin_fn func)
{
    cli_bin_fn  retval = NULL;

    if (func == NULL) {
        func  = cli_cmd_bin;
//      return( NULL );         // ��Ч�Ĳ���
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
 * ע��δ֪����Ĵ�����
 */
cli_ufd_fn cli_ufd_reg(cli_ufd_fn func)
{
    cli_ufd_fn  retval = NULL;

    if (func == NULL) {
        func  = cli_cmd_unfound;
//      return( NULL );         // ��Ч�Ĳ���
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
 * ע��ʶ����Ч����(�ַ����������)��ʾ������
 */
cli_prmt_fn cli_prmt_reg(cli_prmt_fn func)
{
    cli_prmt_fn  retval = NULL;

    if (func == NULL) {
        func  = cli_cmd_idc;
//      return( NULL );         // ��Ч�Ĳ���
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
 * ��ʾ�������������Ϣ; "*Help [1]" �ڲ� "**"��ͷ����, "*Help 0" ��ʾ��������
 */
int cli_cmd_help(int cli, const char *args)
{
    const char *name;
    const char *desc;
    size_t      i;
    int         hide;

    if (sscanf(args, "%*s %d", &hide) != 1) {
        hide = 1;       // �����ʽ����
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
            continue;   // �ڲ�, ͷΪ "**" ��������Ϣ
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
 * Ĭ��δע������Ĵ�����
 */
static int cli_cmd_unfound(int cli, const char *args)
{   //lint !e401
    (void)cli;  (void)args;

    return( 0 );
}


/**
 * Ĭ�϶������������
 */
static int cli_cmd_bin(int cli, const void *data, int size)
{   //lint !e401
    (void)cli;  (void)data;  (void)size;

    return( 0 );
}


/**
 * Ĭ�Ͻ��յ�������ʾ������
 */
static int cli_cmd_idc(int cli, const void *data, int size)
{   //lint !e401
    (void)cli;  (void)data;  (void)size;

    return( 0 );
}


/*----------------------------------------------------------------------------*/
/**
 * ���������
 *
 * @param[in]  str   ������, �ַ������Ȳ���Ϊ(0)
 *
 * @return  �����ָ��, (NULL)ʧ��
 */
static void *cli_command_find(const char *str)
{
    const void *name = str;
    void       *func = NULL;

    OS_MUTEX_ENTER(_cli_cmd_mutex, OS_WAIT_FOREVER)
    {
        if (name == CLI_CMD_UFD) {
            func =  CLI_CMD_UFD->func;  // δע������
            break;
        }
        if (name == CLI_CMD_BIN) {
            func =  CLI_CMD_BIN->func;  // ����������
            break;
        }
        if (name == CLI_CMD_PRMT) {
            func =  CLI_CMD_PRMT->func; // ����������ʾ
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
                    func = _cli_cmd_tbl[i].func; // Ĭ�ϵ�ַ
                    break;
                }
                if((flag & CLI_ADDR_M_BROADCAST) && (addr == _cli_cmd_addr[CLI_ADDR_T_BROADCAST])) {
                    func = _cli_cmd_tbl[i].func; // Ĭ�ϵ�ַ
                    break;
                }
                if((flag & CLI_ADDR_M_MULITCAST) && (addr == _cli_cmd_addr[CLI_ADDR_T_MULITCAST])) {
                    func = _cli_cmd_tbl[i].func; // Ĭ�ϵ�ַ
                    break;
                }
                if((flag & CLI_ADDR_M_UNICAST  ) && (addr == _cli_cmd_addr[CLI_ADDR_T_UNICAST])) {
                    func = _cli_cmd_tbl[i].func; // Ĭ�ϵ�ַ
                    break;
                }
            }
        }
    }
    OS_MUTEX_LEAVE(_cli_cmd_mutex, OS_MUTEX_TIMEOUT_HANDLER)

    return( func );
}


/**
 * �������ݳ�ʼ��
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
 * ִ���ַ�������
 */
int cli_cmd_exec(int cli, const char *str)
{
    int  retval;

    if(!CLI_ID_IS_VALID(cli) || (str == NULL)) {
        return( -1 );               // ��������
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
 * ִ�ж���������
 */
int cli_bin_exec(int cli, const void* data, int size)
{
    int  retval;

    if(!CLI_ID_IS_VALID(cli) || (data == NULL) || (size <= 0)) {
        return( -1 );               // ��������
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

