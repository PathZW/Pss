/*******************************************************************************
 * @file    cli_shell.h
 * @author  Linghu
 * @version v1.0.0
 * @date    2019/5/23
 * @brief   Command Line Interface of Shell application Module.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/5/23 -- Linghu -- the first version
 ******************************************************************************/

#include "stdio.h"
#include "stdarg.h"

/******************************************************************************/
#ifndef __CLI_SHELL_H__
#define __CLI_SHELL_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup CLI_Shell
 * @{
 * @addtogroup          Exported_Constants
 * @{
 ******************************************************************************/
////
#define CLI_INVALID                        -1               // ��Ч�� CLI ID

#define CLI_ECHO_ON                       0x01              // ���������л���
#define CLI_EXEC_STR                      0x02              // ִ���ַ�������
#define CLI_EXEC_BIN                      0x04              // ִ�ж���������
#define CLI_NO_SEARCH                     0x08              // ����������ִ�к���(ִ��δע�������)
#define CLI_NO_THREAD                     0x40              // �����������д����߳�

//lint -emacro(835, CLI_ADDR_M_*)
#define CLI_ADDR_T_DEFAULT                  0               // Ĭ�ϵ�ַ
#define CLI_ADDR_T_BROADCAST                0               // �㲥��ַ
#define CLI_ADDR_T_MULITCAST                0               // �鲥��ַ
#define CLI_ADDR_T_UNICAST                  3               // ������ַ
#define CLI_ADDR_M_DEFAULT     (1 << CLI_ADDR_T_DEFAULT)    // Ĭ�ϵ�ַ(����)
#define CLI_ADDR_M_BROADCAST   (1 << CLI_ADDR_T_BROADCAST)  // �㲥��ַ(����)
#define CLI_ADDR_M_MULITCAST   (1 << CLI_ADDR_T_MULITCAST)  // �鲥��ַ(����)
#define CLI_ADDR_M_UNICAST     (1 << CLI_ADDR_T_UNICAST)    // ������ַ(����)


/*******************************************************************************
 * @}
 * @addtogroup          Exported_Macros
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup          Exported_Types
 * @{
 ******************************************************************************/
/**
 * �����ص�����(�ַ���)
 *
 * @param[in]  cli    �����нӿ�
 * @param[in]  args   �����ַ���, ����������, �������������('\n')
 *
 * @retval  (>= 0) ����ִ�гɹ�
 * @retval  ( < 0) ����ִ���쳣, �ᵼ�������н����������
 */
typedef int (*cli_cmd_fn)(int cli, const char *args);

/**
 * �����������ص�����
 *
 * @param[in]  cli    �����нӿ�
 * @param[in]  data   ����������ָ��
 * @param[in]  size   ���������ݴ�С, ���ֽ�Ϊ��λ
 *
 * @retval  ( > 0) �ɹ�����Ķ��������ݳ���
 * @retval  (== 0) �޷�ʶ��Ķ�������������
 * @retval  ( < 0) ����ִ���쳣, �ᵼ�������н����������
 *
 * @note    ���������������ע��Ч��, ��Ϊÿ���豸��ȡһ�����ݾͻ���øú���һ��
 */
typedef int (*cli_bin_fn)(int cli, const void *data, int size);

/**
 * δע�������ص�����
 *
 * @param[in]  cli    �����нӿ�
 * @param[in]  args   �����ַ���, ����������, �������������('\n')
 *
 * @retval  ( > 0) �ɹ�ʶ����
 * @retval  (== 0) �޷�ʶ����
 * @retval  ( < 0) ����ִ���쳣, �ᵼ�������н����������
 */
typedef int (*cli_ufd_fn)(int cli, const char *args);

/**
 * ���յ�����ָʾ����ص�����
 *
 * @param[in]  cli    �����нӿ�
 * @param[in]  data   ��������ָ��
 * @param[in]  size   �������ݴ�С, ���ֽ�Ϊ��λ
 *
 * @retval  (>= 0) ִ�гɹ�
 * @retval  ( < 0) ִ���쳣, �ᵼ�������н����������
 */
typedef int (*cli_prmt_fn)(int cli, const void *data, int size);


/*----------------------------------------------------------------------------*/
/**
 * CLI�豸
 */
typedef struct cli_device_  cli_device_t;

/**
 * �����нӿ��豸�򿪺���, ���� cli_shell_new() �е���
 *
 * @param[in]  device  �豸ָ��
 *
 * @retval  (>= 0) �ɹ�
 * @retval  ( < 0) ʧ��
 */
typedef int (*cli_open_fn)(cli_device_t *device, int cli);

/**
 * �����нӿ��豸�رպ���, ���� cli_shell_free() �е���
 *
 * @param[in]  device  �豸ָ��
 *
 * @retval  (>= 0) �ɹ�
 * @retval  ( < 0) ʧ��
 */
typedef int (*cli_close_fn)(cli_device_t *device, int cli);

/**
 * �����нӿ��豸������
 *
 * @param[in]  device   �豸ָ��
 * @param[out] buff     ����ָ��, �������������
 * @param[in]  size     �����С, ���ֽ�Ϊ��λ
 *
 * @retval  ( > 0) ��ȡ�������ݳ���
 * @retval  (== 0) �Ѷ�ȡ��һ����������, ���������ݰ�(��'\n')�ָ�������
 * @retval  ( < 0) �豸�����쳣
 */
typedef int (*cli_read_fn)(cli_device_t *device, void *buff, int size);

/**
 * �����нӿ��豸д����
 *
 * @param[in]  device   �豸ָ��
 * @param[in]  data     ������д�������ָ��
 * @param[in]  size     ��д�����ݴ�С, ���ֽ�Ϊ��λ
 *
 * @retval  (>= 0) ��д������ݳ���
 * @retval  ( < 0) �豸�����쳣
 */
typedef int (*cli_write_fn)(cli_device_t *device, const void *data, int size);


/**
 * CLI�豸�ṹ��
 */
struct cli_device_ {
    cli_open_fn     open;
    cli_close_fn    close;
    cli_read_fn     read;
    cli_write_fn    write;
};


/*******************************************************************************
 * @}
 * @addtogroup          Exported_Functions
 * @{
 ******************************************************************************/
/**
 * �����нӿڳ�ʼ��
 */
void cli_shell_init(void);

/**
 * ���������нӿ�
 *
 * @param[in]  device   ��������������豸
 * @param[in]  name     �����нӿ����ַ���, �����ͷ����޸�, (NULL)ʹ��Ĭ����
 * @param[in]  flag     �򿪷�ʽ, CLI_ECHO_ON  CLI_EXEC_STR  CLI_EXEC_BIN  CLI_NO_THREAD �����
 *
 * @retval  (>= 0) �ɹ�, ���������нӿ� ID
 * @retval  ( < 0) ʧ��
 *
 * @note    1. �����д����߳̽����豸�����쳣�������нӿڱ��ر�ʱ�Ż���ֹ
 * @note    2. �����д����߳�����ֹǰ����� cli_shell_free() �ͷ������нӿ�
 */
int cli_shell_new(const cli_device_t *device, const char *name, int flag);

/**
 * �ͷ������нӿ�
 *
 * @param[in]  cli  �����нӿ�
 *
 * @retval  ( > 0) �����нӿ�����ʹ����, ��������Զ��ر�
 * @retval  (== 0) �����нӿ������ر�
 * @retval  ( < 0) ��Ч�Ĳ���
 */
int cli_shell_free(int cli);

/**
 * ���������нӿڿ��Ʊ�־
 * 
 * @param[in]  cli   �����нӿ�
 * @param[in]  set   �豻���õı�־
 * @param[in]  clr   �豻����ı�־
 *
 * @retval  (>= 0): �޸�ǰ�ı�־
 * @retval  ( < 0): ʧ��
 */
int cli_shell_flag(int cli, int set, int clr);

/**
 * ��ȡ�����нӿ���
 *
 * @param[in]  cli   �����нӿ�
 *
 * @return  �����нӿ���ָ��, (NULL)��ʾ����
 */
const char *cli_shell_name(int cli);

/**
 * ����ִ����ѯ, ���豸������, ����Ϊ���������, Ȼ��ִ��
 *
 * @param[in]  cli  �����нӿ�
 *
 * @return  (>= 0): �ɹ�, �ɼ�����ѯ
 * @return  ( < 0): ��������, ���豸�����쳣
 *
 * @note  1. �ú������ڲ��������豸�����쳣(���豸�ر�)ʱ�ŷ���
 * @note  2. �ú������غ�, һ����� cli_shell_free() �ͷ������нӿ�
 */
int cli_shell_poll(int cli);

#if 0
int cli_set_prompt (int cli, const char *prompt);
int cli_set_welcome(int cli, const char *welcome);

int cli_write(int cli, const void *buf, size_t count);
int cli_read (int cli,       void *buf, size_t count);

char *cli_fgets  (char *str, int size, int cli);
int   cli_fputc  (int ch, int cli);
int   cli_fgetc  (int cli); // �����س��ſɶ�ȡ, �л���
int   cli_fgetch (int cli); // ����س�Ҳ�ɶ�ȡ, �޻���
int   cli_fgetche(int cli); // ����س�Ҳ�ɶ�ȡ, �л���
int   cli_fscanf (int cli, const char* format, ...);
int   cli_vfscanf(int cli, const char* format, va_list arg);
#endif

/**
 * ������, ���� stdio.h fread(), ��û�� feof() �������
 *
 * @param[out] ptr    ���ض�ȡ��Ԫ������
 * @param[in]  size   Ҫ��ȡ��Ԫ�ش�С
 * @param[in]  nmemb  Ҫ��ȡ��Ԫ�ظ���, ÿ����С��Ϊ size �ֽ�
 * @param[in]  cli    �����нӿ�
 *
 * @retval  (== nmemb) ������ȫ��ȡ
 * @retval  (!= nmemb) ��������, ���豸�����쳣, ��ʱ�����Ѿ���ȡ��Ԫ�ظ���
 *
 * @note    �ú���ȷ��ÿ�ζ�Ϊһ����������Ԫ��
 */
size_t cli_fread(void *ptr, size_t size, size_t nmemb, int cli);

/**
 * д����(ȫ����ģʽ), ���� stdio.h fwrite(), ��û�� feof() �������
 *
 * @param[in]  ptr    Ҫд���Ԫ��ָ��
 * @param[in]  size   Ҫд���Ԫ�ش�С
 * @param[in]  nmemb  Ҫд���Ԫ�ظ���, ÿ����С��Ϊ size �ֽ�
 * @param[in]  cli    �����нӿ�
 *
 * @retval  (== nmemb) ������ȫд��
 * @retval  (!= nmemb) ��������, ���豸�����쳣, ��ʱ�����Ѿ�д���Ԫ�ظ���
 *
 * @note    �ú���ȷ��ÿ��дΪһ����������Ԫ��, size �����������ʱֱ��д�豸
 * @note    STM32F429 ---- 29 CLK/Byte
 */
size_t cli_fwrite(const void *ptr, size_t size, size_t nmemb, int cli);

/**
 * �ַ������, ���� stdio.h fputs(), ��û�� feof() �������
 *
 * @param[in]  str  Ҫ������ַ���
 * @param[in]  cli  �����нӿ�
 *
 * @retval  (>= 0) ������ַ���
 * @retval  ( < 0) ��������, ���豸�����쳣
 */
int cli_fputs(const char *str, int cli);

/**
 * ��ʽ�����(�л���ģʽ), ���� stdio.h fprintf(), ��û�� feof() �������
 *
 * @param[in]  cli     �����нӿ�
 * @param[in]  format  ��ʽ�ַ���
 *
 * @retval  (>= 0) ������ַ���
 * @retval  ( < 0) ��������, �������, ���豸�����쳣
 *
 * @note    ֻ�з���ֵ�Ǹ�, ��С����������Сʱ, �ַ����ű���ȫ���
 * @note    STM32F429 ---- 256 CLK/Byte
 */
int cli_fprintf(int cli, const char *format, ...);

/**
 * ���Կ����Ÿ�ʽ�����(�л���ģʽ), ���� stdio.h fprintf(), ��û�� feof() �������
 *
 * @param[in]  cli     �����нӿ�
 * @param[in]  format  ��ʽ�ַ���
 *
 * @retval  (>= 0) ������ַ���
 * @retval  ( < 0) ��������, �������, ���豸�����쳣
 *
 * @note    ֻ�з���ֵ�Ǹ�, ��С����������Сʱ, �ַ����ű���ȫ���
 */
int cli_fechof(int cli, const char *format, ...);

/**
 * ��ʽ�����(�л���ģʽ), ���� stdio.h vfprintf(), ��û�� feof() �������
 *
 * @param[in]  cli     �����нӿ�
 * @param[in]  format  ��ʽ�ַ���
 * @param[in]  arg     �� va_start() ��ʼ���Ĳ����б�
 *
 * @retval  (>= 0) ������ַ���
 * @retval  ( < 0) ��������, �������, ���豸�����쳣
 *
 * @note    ֻ�з���ֵ�Ǹ�, ��С����������Сʱ, �ַ����ű���ȫ���
 */
int cli_vfprintf(int cli, const char *format, va_list arg);

/**
 * ˢ���������, ���� stdio.h fflush(), ��û�� feof() �������
 */
int cli_fflush(int cli);

/**
 * ɾ���ַ�����β�հ�
 */
char *cli_strdelspace(char *str);

/**
 * �����ַ��� n �ζ������Ӵ�
 *
 * @param[in]  s      �ַ���
 * @param[in]  n      �������
 * @param[in]  delim  �����, NULL ��ʾ�հ׷�
 *
 * @retval  (!= NULL) n �ζ������Ӵ�
 * @retval  (== NULL) n �ζ���ʧ��
 *
 * @note    ���� cli_strdelim(":PSS:Volt  CHN2,0.5", 1, NULL) --> "CHN2,0.5"
 */
const char *cli_strdelim(const char *s, unsigned n, const char *delim);

/**
 * �ַ����Ƚ�, �������׿հ׺ʹ�Сд, ���������׿հ׺���ֹ�ַ���ֹ
 *
 * @param[in]  s1  ���Ƚϵ��ַ���1.
 * @param[in]  s2  ���Ƚϵ��ַ���2.
 *
 * @retval  ( > 0) �ַ���1 ���� �ַ���2(��һ����ƥ����ַ�)
 * @retval  (== 0) �����ַ������
 * @retval  ( < 0) �ַ���1 С�� �ַ���2(��һ����ƥ����ַ�)
 */
int cli_strtrimcmp(const char *s1, const char *s2);

/**
 * �ַ����Ƚ�, �������׿հ׺ʹ�Сд, ���������׿հ׺���ֹ�ַ���ֹ
 *
 * @param[in]  s1  ���Ƚϵ��ַ���1.
 * @param[in]  s2  ���Ƚϵ��ַ���2.
 * @param[in]  n   �Ƚϵ��ַ�������.
 *
 * @retval  ( > 0) �ַ���1 ���� �ַ���2(��һ����ƥ����ַ�)
 * @retval  (== 0) �����ַ������
 * @retval  ( < 0) �ַ���1 С�� �ַ���2(��һ����ƥ����ַ�)
 */
int cli_strntrimcmp(const char *s1, const char *s2, size_t n);

/**
 * ���ַ��� n �ζ������Ӵ���, ʶ�𶨹ؼ��ֺ��������
 *
 * @param[in]  s      �ַ���
 * @param[in]  key    �ؼ���, ���Դ�Сд
 * @param[in]  n      �������
 * @param[in]  delim  �����, NULL ��ʾ�հ׷�
 *
 * @retval  ( > 0) �ؼ��ֺ��������
 * @retval  (== 0) �ؼ���δ�ҵ�
 * @retval  ( < 0) �ַ�����ʽ���� 
 *
 * @note    ���� cli_strsplitnum(":PSS:VOLT CHN2,0.15", "chn", 1, NULL) ---> 2
 */
int cli_strsplitnum(const char *s, const char *key, unsigned n, const char *delim);

/**
 * ��һ���ڴ��������ƥ����һ���ڴ��ĵ�һ��λ��.
 *
 * @param[in]  haystack �������ڴ���ַ
 * @param[in]  hs_len   �������ڴ�鳤��
 * @param[in]  needle   ƥ����ڴ���ַ
 * @param[in]  ne_len   ƥ����ڴ�鳤��
 *
 * @retval  (!= NULL) �����ҵ�λ�õ�ָ��
 * @retval  (== NULL) δ�ҵ�
 */
const void *cli_memmem(const void *haystack, size_t hs_len, const void *needle, size_t ne_len);

/**
 * �޸������ַ, ������������Ч.
 *
 * @param[in]  type  ��ַ����(CLI_ADDR_T_*)
 * @param[in]  addr  ��ֵַ, (0)Ϊ��Ч��ַ, (1)ͨ��ΪĬ�ϵ�ַ
 *
 * @retval  (>= 0) �����޸�ǰ��ֵַ
 * @retval  ( < 0) ʧ��
 */
int32_t cli_cmd_addr(size_t type, uint16_t addr);

/**
 * �޸�����ѡ��, ����ָ��������Ч.
 *
 * @param[in]  name  ������
 * @param[in]  clr   ���������ѡ��(CLI_ADDR_M_*)
 * @param[in]  set   ����������ѡ��(CLI_ADDR_M_*)
 *
 * @retval  (>= 0) �����޸�ǰ����ѡ��ֵ
 * @retval  ( < 0) ʧ��
 */
int32_t cli_cmd_flag(const char *name, uint16_t clr, uint16_t set);

/**
 * ����ע��
 *
 * @param[in]  name  ������, �����ͷ����޸�
 * @param[in]  func  ����ִ�к���
 * @param[in]  desc  ��������, �����ͷ����޸�
 *
 * @retval  (>= 0) �ɹ�, ������ע����������
 * @retval  ( < 0) ʧ��
 */
int cli_cmd_reg(const char *name, cli_cmd_fn func, const char *desc);

/**
 * ����ע��ȡ��
 *
 * @param[in]  name  ������, �����ͷ����޸�
 *
 * @retval  (>= 0) �ɹ�, ������ע����������
 * @retval  ( < 0) ʧ��
 */
int cli_cmd_unreg(const char *name);

/**
 * ִ���ַ�������
 *
 * @param[in]  cli  �����нӿ�
 * @param[in]  str  �����ַ���
 *
 * @retval  ( > 0) ��Ч������, �������������
 * @retval  (== 0) ����ִ�гɹ�
 * @retval  ( < 0) �豸�����쳣
 */
int cli_cmd_exec(int cli, const char *str);

/**
 * ע��������������
 *
 * @param[in]  func   ������ָ��, (NULL)��ʾȡ��ע��
 *
 * @return  �ɴ�����ָ��, (NULL)��ʾʧ��
 */
cli_bin_fn cli_bin_reg(cli_bin_fn func);

/**
 * ִ�ж���������
 *
 * @param[in]  cli   �����нӿ�
 * @param[in]  data  ����������ָ��
 * @param[in]  size  ���������ݳ���
 *
 * @retval  ( > 0) ִ�гɹ�, �����Ѵ�������ݳ���
 * @retval  (== 0) ��Ч������
 * @retval  ( < 0) �豸�����쳣
 */
int cli_bin_exec(int cli, const void *data, int size);

/**
 * ע��δ֪����Ĵ�����
 *
 * @param[in]  func   ������ָ��, (NULL)��ʾȡ��ע��
 *
 * @return  �ɴ�����ָ��, (NULL)��ʾʧ��
 */
cli_ufd_fn cli_ufd_reg(cli_ufd_fn func);

/**
 * ע��ʶ����Ч����(�ַ����������)��ʾ������
 *
 * @param[in]  func   ������ָ��, (NULL)��ʾȡ��ע��
 *
 * @return  �ɴ�����ָ��, (NULL)��ʾʧ��
 *
 * @note    һ�����ڸ�������, ����˸LEDָʾ���յ���Ч����
 */
cli_prmt_fn cli_prmt_reg(cli_prmt_fn func);

/**
 * ��ʾ�������������Ϣ; "*Help [1]" �ڲ� "**"��ͷ����, "*Help 0" ��ʾ��������
 *
 * @param[in]  cli    �����нӿ�
 * @param[in]  args   �����ַ���, ����������, �������������('\n')
 *
 * @retval  ( > 0) �����������
 * @retval  (== 0) ����ִ�гɹ�
 * @retval  ( < 0) �豸�����쳣
 */
int cli_cmd_help(int cli, const char *args);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __CLI_SHELL_H__
/*****************************  END OF FILE  **********************************/

