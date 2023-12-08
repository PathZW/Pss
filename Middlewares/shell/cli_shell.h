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
#define CLI_INVALID                        -1               // 无效的 CLI ID

#define CLI_ECHO_ON                       0x01              // 开启命令行回显
#define CLI_EXEC_STR                      0x02              // 执行字符串命令
#define CLI_EXEC_BIN                      0x04              // 执行二进制命令
#define CLI_NO_SEARCH                     0x08              // 不搜索命令执行函数(执行未注册命令处理)
#define CLI_NO_THREAD                     0x40              // 不创建命令行处理线程

//lint -emacro(835, CLI_ADDR_M_*)
#define CLI_ADDR_T_DEFAULT                  0               // 默认地址
#define CLI_ADDR_T_BROADCAST                0               // 广播地址
#define CLI_ADDR_T_MULITCAST                0               // 组播地址
#define CLI_ADDR_T_UNICAST                  3               // 单播地址
#define CLI_ADDR_M_DEFAULT     (1 << CLI_ADDR_T_DEFAULT)    // 默认地址(掩码)
#define CLI_ADDR_M_BROADCAST   (1 << CLI_ADDR_T_BROADCAST)  // 广播地址(掩码)
#define CLI_ADDR_M_MULITCAST   (1 << CLI_ADDR_T_MULITCAST)  // 组播地址(掩码)
#define CLI_ADDR_M_UNICAST     (1 << CLI_ADDR_T_UNICAST)    // 单播地址(掩码)


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
 * 命令处理回调函数(字符串)
 *
 * @param[in]  cli    命令行接口
 * @param[in]  args   命令字符串, 包括命令名, 包括命令结束符('\n')
 *
 * @retval  (>= 0) 命令执行成功
 * @retval  ( < 0) 命令执行异常, 会导致命令行解析处理结束
 */
typedef int (*cli_cmd_fn)(int cli, const char *args);

/**
 * 二进制命令处理回调函数
 *
 * @param[in]  cli    命令行接口
 * @param[in]  data   二进制数据指针
 * @param[in]  size   二进制数据大小, 以字节为单位
 *
 * @retval  ( > 0) 成功处理的二进制数据长度
 * @retval  (== 0) 无法识别的二进制命令数据
 * @retval  ( < 0) 命令执行异常, 会导致命令行解析处理结束
 *
 * @note    二进制命令函数必须注意效率, 因为每从设备读取一次数据就会调用该函数一次
 */
typedef int (*cli_bin_fn)(int cli, const void *data, int size);

/**
 * 未注册命令处理回调函数
 *
 * @param[in]  cli    命令行接口
 * @param[in]  args   命令字符串, 包括命令名, 包括命令结束符('\n')
 *
 * @retval  ( > 0) 成功识别处理
 * @retval  (== 0) 无法识别处理
 * @retval  ( < 0) 命令执行异常, 会导致命令行解析处理结束
 */
typedef int (*cli_ufd_fn)(int cli, const char *args);

/**
 * 接收到命令指示处理回调函数
 *
 * @param[in]  cli    命令行接口
 * @param[in]  data   命令数据指针
 * @param[in]  size   命令数据大小, 以字节为单位
 *
 * @retval  (>= 0) 执行成功
 * @retval  ( < 0) 执行异常, 会导致命令行解析处理结束
 */
typedef int (*cli_prmt_fn)(int cli, const void *data, int size);


/*----------------------------------------------------------------------------*/
/**
 * CLI设备
 */
typedef struct cli_device_  cli_device_t;

/**
 * 命令行接口设备打开函数, 仅在 cli_shell_new() 中调用
 *
 * @param[in]  device  设备指针
 *
 * @retval  (>= 0) 成功
 * @retval  ( < 0) 失败
 */
typedef int (*cli_open_fn)(cli_device_t *device, int cli);

/**
 * 命令行接口设备关闭函数, 仅在 cli_shell_free() 中调用
 *
 * @param[in]  device  设备指针
 *
 * @retval  (>= 0) 成功
 * @retval  ( < 0) 失败
 */
typedef int (*cli_close_fn)(cli_device_t *device, int cli);

/**
 * 命令行接口设备读函数
 *
 * @param[in]  device   设备指针
 * @param[out] buff     缓存指针, 保存读到的数据
 * @param[in]  size     缓存大小, 以字节为单位
 *
 * @retval  ( > 0) 读取到的数据长度
 * @retval  (== 0) 已读取到一条完整命令, 用于以数据包(非'\n')分隔的命令
 * @retval  ( < 0) 设备操作异常
 */
typedef int (*cli_read_fn)(cli_device_t *device, void *buff, int size);

/**
 * 命令行接口设备写函数
 *
 * @param[in]  device   设备指针
 * @param[in]  data     保存需写入的数据指针
 * @param[in]  size     需写入数据大小, 以字节为单位
 *
 * @retval  (>= 0) 已写入的数据长度
 * @retval  ( < 0) 设备操作异常
 */
typedef int (*cli_write_fn)(cli_device_t *device, const void *data, int size);


/**
 * CLI设备结构体
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
 * 命令行接口初始化
 */
void cli_shell_init(void);

/**
 * 创建命令行接口
 *
 * @param[in]  device   命令行输入输出设备
 * @param[in]  name     命令行接口名字符串, 不可释放与修改, (NULL)使用默认名
 * @param[in]  flag     打开方式, CLI_ECHO_ON  CLI_EXEC_STR  CLI_EXEC_BIN  CLI_NO_THREAD 的组合
 *
 * @retval  (>= 0) 成功, 返回命令行接口 ID
 * @retval  ( < 0) 失败
 *
 * @note    1. 命令行处理线程仅在设备操作异常、命令行接口被关闭时才会终止
 * @note    2. 命令行处理线程在终止前会调用 cli_shell_free() 释放命令行接口
 */
int cli_shell_new(const cli_device_t *device, const char *name, int flag);

/**
 * 释放命令行接口
 *
 * @param[in]  cli  命令行接口
 *
 * @retval  ( > 0) 命令行接口正在使用中, 结束后会自动关闭
 * @retval  (== 0) 命令行接口正常关闭
 * @retval  ( < 0) 无效的参数
 */
int cli_shell_free(int cli);

/**
 * 设置命令行接口控制标志
 * 
 * @param[in]  cli   命令行接口
 * @param[in]  set   需被设置的标志
 * @param[in]  clr   需被清除的标志
 *
 * @retval  (>= 0): 修改前的标志
 * @retval  ( < 0): 失败
 */
int cli_shell_flag(int cli, int set, int clr);

/**
 * 获取命令行接口名
 *
 * @param[in]  cli   命令行接口
 *
 * @return  命令行接口名指针, (NULL)表示错误
 */
const char *cli_shell_name(int cli);

/**
 * 命令执行轮询, 从设备读数据, 解析为命令与参数, 然后执行
 *
 * @param[in]  cli  命令行接口
 *
 * @return  (>= 0): 成功, 可继续轮询
 * @return  ( < 0): 参数错误, 或设备操作异常
 *
 * @note  1. 该函数仅在参数错误、设备操作异常(如设备关闭)时才返回
 * @note  2. 该函数返回后, 一般调用 cli_shell_free() 释放命令行接口
 */
int cli_shell_poll(int cli);

#if 0
int cli_set_prompt (int cli, const char *prompt);
int cli_set_welcome(int cli, const char *welcome);

int cli_write(int cli, const void *buf, size_t count);
int cli_read (int cli,       void *buf, size_t count);

char *cli_fgets  (char *str, int size, int cli);
int   cli_fputc  (int ch, int cli);
int   cli_fgetc  (int cli); // 遇到回车才可读取, 有回显
int   cli_fgetch (int cli); // 不需回车也可读取, 无回显
int   cli_fgetche(int cli); // 不需回车也可读取, 有回显
int   cli_fscanf (int cli, const char* format, ...);
int   cli_vfscanf(int cli, const char* format, va_list arg);
#endif

/**
 * 读数据, 类似 stdio.h fread(), 但没有 feof() 相关特性
 *
 * @param[out] ptr    返回读取的元素数据
 * @param[in]  size   要读取的元素大小
 * @param[in]  nmemb  要读取的元素个数, 每个大小都为 size 字节
 * @param[in]  cli    命令行接口
 *
 * @retval  (== nmemb) 数据完全读取
 * @retval  (!= nmemb) 参数错误, 或设备操作异常, 此时返回已经读取的元素个数
 *
 * @note    该函数确保每次读为一个或多个完整元素
 */
size_t cli_fread(void *ptr, size_t size, size_t nmemb, int cli);

/**
 * 写数据(全缓冲模式), 类似 stdio.h fwrite(), 但没有 feof() 相关特性
 *
 * @param[in]  ptr    要写入的元素指针
 * @param[in]  size   要写入的元素大小
 * @param[in]  nmemb  要写入的元素个数, 每个大小都为 size 字节
 * @param[in]  cli    命令行接口
 *
 * @retval  (== nmemb) 数据完全写入
 * @retval  (!= nmemb) 参数错误, 或设备操作异常, 此时返回已经写入的元素个数
 *
 * @note    该函数确保每次写为一个或多个完整元素, size 大于输出缓存时直接写设备
 * @note    STM32F429 ---- 29 CLK/Byte
 */
size_t cli_fwrite(const void *ptr, size_t size, size_t nmemb, int cli);

/**
 * 字符串输出, 类似 stdio.h fputs(), 但没有 feof() 相关特性
 *
 * @param[in]  str  要输出的字符串
 * @param[in]  cli  命令行接口
 *
 * @retval  (>= 0) 输出的字符数
 * @retval  ( < 0) 参数错误, 或设备操作异常
 */
int cli_fputs(const char *str, int cli);

/**
 * 格式化输出(行缓存模式), 类似 stdio.h fprintf(), 但没有 feof() 相关特性
 *
 * @param[in]  cli     命令行接口
 * @param[in]  format  格式字符串
 *
 * @retval  (>= 0) 输出的字符数
 * @retval  ( < 0) 参数错误, 编码错误, 或设备操作异常
 *
 * @note    只有返回值非负, 且小于输出缓存大小时, 字符串才被完全输出
 * @note    STM32F429 ---- 256 CLK/Byte
 */
int cli_fprintf(int cli, const char *format, ...);

/**
 * 回显开启才格式化输出(行缓存模式), 类似 stdio.h fprintf(), 但没有 feof() 相关特性
 *
 * @param[in]  cli     命令行接口
 * @param[in]  format  格式字符串
 *
 * @retval  (>= 0) 输出的字符数
 * @retval  ( < 0) 参数错误, 编码错误, 或设备操作异常
 *
 * @note    只有返回值非负, 且小于输出缓存大小时, 字符串才被完全输出
 */
int cli_fechof(int cli, const char *format, ...);

/**
 * 格式化输出(行缓存模式), 类似 stdio.h vfprintf(), 但没有 feof() 相关特性
 *
 * @param[in]  cli     命令行接口
 * @param[in]  format  格式字符串
 * @param[in]  arg     用 va_start() 初始化的参数列表
 *
 * @retval  (>= 0) 输出的字符数
 * @retval  ( < 0) 参数错误, 编码错误, 或设备操作异常
 *
 * @note    只有返回值非负, 且小于输出缓存大小时, 字符串才被完全输出
 */
int cli_vfprintf(int cli, const char *format, va_list arg);

/**
 * 刷新输出缓存, 类似 stdio.h fflush(), 但没有 feof() 相关特性
 */
int cli_fflush(int cli);

/**
 * 删除字符串首尾空白
 */
char *cli_strdelspace(char *str);

/**
 * 返回字符串 n 次定界后的子串
 *
 * @param[in]  s      字符串
 * @param[in]  n      定界次数
 * @param[in]  delim  定界符, NULL 表示空白符
 *
 * @retval  (!= NULL) n 次定界后的子串
 * @retval  (== NULL) n 次定界失败
 *
 * @note    例如 cli_strdelim(":PSS:Volt  CHN2,0.5", 1, NULL) --> "CHN2,0.5"
 */
const char *cli_strdelim(const char *s, unsigned n, const char *delim);

/**
 * 字符串比较, 忽略行首空白和大小写, 遇到非行首空白和终止字符终止
 *
 * @param[in]  s1  将比较的字符串1.
 * @param[in]  s2  将比较的字符串2.
 *
 * @retval  ( > 0) 字符串1 大于 字符串2(第一个不匹配的字符)
 * @retval  (== 0) 两个字符串相等
 * @retval  ( < 0) 字符串1 小于 字符串2(第一个不匹配的字符)
 */
int cli_strtrimcmp(const char *s1, const char *s2);

/**
 * 字符串比较, 忽略行首空白和大小写, 遇到非行首空白和终止字符终止
 *
 * @param[in]  s1  将比较的字符串1.
 * @param[in]  s2  将比较的字符串2.
 * @param[in]  n   比较的字符最大个数.
 *
 * @retval  ( > 0) 字符串1 大于 字符串2(第一个不匹配的字符)
 * @retval  (== 0) 两个字符串相等
 * @retval  ( < 0) 字符串1 小于 字符串2(第一个不匹配的字符)
 */
int cli_strntrimcmp(const char *s1, const char *s2, size_t n);

/**
 * 在字符串 n 次定界后的子串中, 识别定关键字后的正整数
 *
 * @param[in]  s      字符串
 * @param[in]  key    关键字, 忽略大小写
 * @param[in]  n      定界次数
 * @param[in]  delim  定界符, NULL 表示空白符
 *
 * @retval  ( > 0) 关键字后的正整数
 * @retval  (== 0) 关键字未找到
 * @retval  ( < 0) 字符串格式错误 
 *
 * @note    例如 cli_strsplitnum(":PSS:VOLT CHN2,0.15", "chn", 1, NULL) ---> 2
 */
int cli_strsplitnum(const char *s, const char *key, unsigned n, const char *delim);

/**
 * 在一个内存块中搜索匹配另一个内存块的第一个位置.
 *
 * @param[in]  haystack 搜索的内存块地址
 * @param[in]  hs_len   搜索的内存块长度
 * @param[in]  needle   匹配的内存块地址
 * @param[in]  ne_len   匹配的内存块长度
 *
 * @retval  (!= NULL) 返回找到位置的指针
 * @retval  (== NULL) 未找到
 */
const void *cli_memmem(const void *haystack, size_t hs_len, const void *needle, size_t ne_len);

/**
 * 修改命令地址, 对所有命令有效.
 *
 * @param[in]  type  地址类型(CLI_ADDR_T_*)
 * @param[in]  addr  地址值, (0)为无效地址, (1)通常为默认地址
 *
 * @retval  (>= 0) 返回修改前地址值
 * @retval  ( < 0) 失败
 */
int32_t cli_cmd_addr(size_t type, uint16_t addr);

/**
 * 修改命令选项, 仅对指定命令有效.
 *
 * @param[in]  name  命令名
 * @param[in]  clr   清除的命令选项(CLI_ADDR_M_*)
 * @param[in]  set   开启的命令选项(CLI_ADDR_M_*)
 *
 * @retval  (>= 0) 返回修改前命令选项值
 * @retval  ( < 0) 失败
 */
int32_t cli_cmd_flag(const char *name, uint16_t clr, uint16_t set);

/**
 * 命令注册
 *
 * @param[in]  name  命令名, 不可释放与修改
 * @param[in]  func  命令执行函数
 * @param[in]  desc  命令描叙, 不可释放与修改
 *
 * @retval  (>= 0) 成功, 返回已注册命令数量
 * @retval  ( < 0) 失败
 */
int cli_cmd_reg(const char *name, cli_cmd_fn func, const char *desc);

/**
 * 命令注册取消
 *
 * @param[in]  name  命令名, 不可释放与修改
 *
 * @retval  (>= 0) 成功, 返回已注册命令数量
 * @retval  ( < 0) 失败
 */
int cli_cmd_unreg(const char *name);

/**
 * 执行字符串命令
 *
 * @param[in]  cli  命令行接口
 * @param[in]  str  命令字符串
 *
 * @retval  ( > 0) 无效的命令, 或命令参数错误
 * @retval  (== 0) 命令执行成功
 * @retval  ( < 0) 设备操作异常
 */
int cli_cmd_exec(int cli, const char *str);

/**
 * 注册二进制命令处理函数
 *
 * @param[in]  func   处理函数指针, (NULL)表示取消注册
 *
 * @return  旧处理函数指针, (NULL)表示失败
 */
cli_bin_fn cli_bin_reg(cli_bin_fn func);

/**
 * 执行二进制命令
 *
 * @param[in]  cli   命令行接口
 * @param[in]  data  二进制数据指针
 * @param[in]  size  二进制数据长度
 *
 * @retval  ( > 0) 执行成功, 返回已处理的数据长度
 * @retval  (== 0) 无效的命令
 * @retval  ( < 0) 设备操作异常
 */
int cli_bin_exec(int cli, const void *data, int size);

/**
 * 注册未知命令的处理函数
 *
 * @param[in]  func   处理函数指针, (NULL)表示取消注册
 *
 * @return  旧处理函数指针, (NULL)表示失败
 */
cli_ufd_fn cli_ufd_reg(cli_ufd_fn func);

/**
 * 注册识别到有效命令(字符串与二进制)提示处理函数
 *
 * @param[in]  func   处理函数指针, (NULL)表示取消注册
 *
 * @return  旧处理函数指针, (NULL)表示失败
 *
 * @note    一般用于辅助处理, 如闪烁LED指示接收到有效命令
 */
cli_prmt_fn cli_prmt_reg(cli_prmt_fn func);

/**
 * 显示所有命令帮助信息; "*Help [1]" 掩藏 "**"开头命令, "*Help 0" 显示所有命令
 *
 * @param[in]  cli    命令行接口
 * @param[in]  args   命令字符串, 包括命令名, 包括命令结束符('\n')
 *
 * @retval  ( > 0) 命令参数错误
 * @retval  (== 0) 命令执行成功
 * @retval  ( < 0) 设备操作异常
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

