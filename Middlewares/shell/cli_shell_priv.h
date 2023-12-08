/*******************************************************************************
 * @file    cli_shell_priv.h
 * @author  Linghu(Linghu0060@qq.com)
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
 *      2019/6/21 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __CLI_SHELL_DEF_H__
#define __CLI_SHELL_DEF_H__
/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup          CLI_Shell
 * @{
 ******************************************************************************/

#ifndef CLI_THREAD_STKB
#define CLI_THREAD_STKB             640u        //!< �����д����߳�ջ��С(���ֽ�Ϊ��λ)
#endif
#ifndef CLI_THREAD_PRIO
#define CLI_THREAD_PRIO         OS_PRIO_NORMAL  //!< �����д����߳����ȼ�
#endif

#ifndef CLI_MAX_COMMAND
#define CLI_MAX_COMMAND              64         //!< ��ע��������������
#endif
#ifndef CLI_MAX_INTERFACE
#define CLI_MAX_INTERFACE            4          //!< �ɴ򿪵���������нӿ�����
#endif

#ifndef CLI_LINE_BUFF_SIZE
#define CLI_LINE_BUFF_SIZE          128         //!< ���������뻺���С(���ֽ�Ϊ��λ)
#endif
#ifndef CLI_OUT_BUFF_SIZE
#define CLI_OUT_BUFF_SIZE           128         //!< ��������������С(���ֽ�Ϊ��λ)
#endif

// #ifndef CLI_CMD_SEPARATOR
// #define CLI_CMD_SEPARATOR        ""          //!< �����зָ���, Ref: scanf()
// #endif
#ifndef CLI_DEFAULT_PROMPT
#define CLI_DEFAULT_PROMPT       "CLI >> "      //!< Ĭ����������ʾ��
#endif
#ifndef CLI_DEFAULT_WELCOME
#define CLI_DEFAULT_WELCOME        NULL         //!< Ĭ�������л�ӭ��Ϣ
#endif

#ifndef CLI_BIN_PROMPT
#define CLI_BIN_PROMPT "Executed Binary Command!"ENDL""ENDL //!< ִ�ж���������ɹ���ʾ
#endif
#ifndef CLI_UFD_PROMPT
#define CLI_UFD_PROMPT        "Unknown Command!"ENDL        //!< ��Ч������ʾ
#endif

#ifndef ENDL
#define ENDL                    "\r\n"      //!< (C) End of line, start the new line
#endif


/*******************************************************************************
 * @}
 * @}
 * @addtogroup CLI_Shell
 * @{
 * @addtogroup          Private_Declaration
 * @{
 ******************************************************************************/

#ifndef CLI_EXTERN
#define CLI_EXTERN  extern
#endif


/*----------------------------------------------------------------------------*/
/**
 * �������߳�
 */
//lint -emacro(786, CLI_TNAME_R, CLI_TNAME_L)   " String concatenation "
#if (CLI_THREAD_STKB > 0)
#define CLI_TNAME_R(n, m)       "CLI " #n " thread",
#define CLI_TNAME_L(n, m)       "CLI " #n " thread"
CLI_EXTERN const char * const   _cli_thread_name[] = {
    REPEAT_GROW(CLI_MAX_INTERFACE, CLI_TNAME_R, CLI_TNAME_L, 0)
};
#define CLI_THREAD_NAME         _cli_thread_name[cli - 1]   // �߳���

OS_THREAD_STK_DEF(              _cli_thread_stk[CLI_MAX_INTERFACE], CLI_THREAD_STKB);
#define CLI_THREAD_STACK        _cli_thread_stk[cli - 1]    // �̶߳�ջ
#endif


/*----------------------------------------------------------------------------*/
/**
 * ����������
 */
#if 1
typedef struct cli_cmd_ {
    const char         *name;       // ������
    const char         *desc;       // ��������
    void               *func;       // �����
    uint16_t            flag;       // ����ѡ��
} cli_cmd_t;

CLI_EXTERN cli_cmd_t    _cli_cmd_tbl[CLI_MAX_COMMAND + 3];  // �����ڴ��
CLI_EXTERN OS_MUTEX_ID  _cli_cmd_mutex;                     // �������
CLI_EXTERN uint16_t     _cli_cmd_addr[4];                   // �����ַ, [0]Ĭ�ϵ�ַ
                                                            //           [1]�㲥��ַ
                                                            //           [2]�鲥��ַ
                                                            //           [3]������ַ
#define CLI_CMD_ADDR_INV        0                           // ��Ч��ֵַ
#define CLI_CMD_ADDR_DFT        1                           // Ĭ�ϵ�ֵַ

#define CLI_CMD_UFD     (&_cli_cmd_tbl[ARRAY_SIZE(_cli_cmd_tbl) - 3])
#define CLI_CMD_BIN     (&_cli_cmd_tbl[ARRAY_SIZE(_cli_cmd_tbl) - 2])
#define CLI_CMD_PRMT    (&_cli_cmd_tbl[ARRAY_SIZE(_cli_cmd_tbl) - 1])

//lint -emacro(611, CLI_FUNC_UFD, CLI_FUNC_BIN, CLI_FUNC_PRMT)  // Suspicious cast
#define CLI_FUNC_UFD    ((cli_ufd_fn )cli_command_find((const char *)CLI_CMD_UFD))
#define CLI_FUNC_BIN    ((cli_bin_fn )cli_command_find((const char *)CLI_CMD_BIN))
#define CLI_FUNC_PRMT   ((cli_prmt_fn)cli_command_find((const char *)CLI_CMD_PRMT))
#endif


/*----------------------------------------------------------------------------*/
/**
 * �����нӿ�
 */
#if 1
typedef struct cli_if_ {
    const char         *name;       // �����нӿ���
    const char         *prompt;     // ��������ʾ��
    const char         *welcome;    // �����л�ӭ��Ϣ
    const cli_device_t *device;     // �����нӿ��豸
#if (CLI_THREAD_STKB > 0)
    OS_THREAD_ID        thread;     // �������߳�
#endif
    OS_MUTEX_ID         i_lock;     // ���������뱣����
    OS_MUTEX_ID         o_lock;     // ���������������
    OS_MUTEX_ID         if_lock;    // �����нӿڱ�����
    uint16_t            if_refs;    // �����нӿ����ü���
    int16_t             if_id;      // �����нӿ�ID
    uint8_t             if_stat;    // ������״̬
    uint8_t             l_stat;                         // ������״̬
    uint16_t            l_pos;                          // �л���λ��
    uint16_t            l_cur;                          // �л�����
    uint16_t            i_pos;                          // ���뻺��λ��
    uint16_t            i_cur;                          // ���뻺����
    uint16_t            o_pos;                          // �������λ��
    char                l_buff[CLI_LINE_BUFF_SIZE];     // �л���
    uint8_t             i_buff[CLI_LINE_BUFF_SIZE];     // ���뻺��
    uint8_t             o_buff[CLI_OUT_BUFF_SIZE];      // �������
} cli_if_t;


CLI_EXTERN cli_if_t     _cli_if_tbl[CLI_MAX_INTERFACE]; // �����нӿ��ڴ��
CLI_EXTERN OS_MUTEX_ID  _cli_if_mutex;                  // �����нӿڻ�����

// �����нӿ�ID�Ƿ���Ч, ����(true)��Ч, ����(false)��Ч
#define CLI_ID_IS_VALID(cli)  ((0 < (cli)) && ((cli) <= (int)ARRAY_SIZE(_cli_if_tbl)))


//lint -emacro(786, CLI_INAME_R, CLI_INAME_L)   " String concatenation "
#define CLI_INAME_R(n, m)       "CLI " #n ,
#define CLI_INAME_L(n, m)       "CLI " #n
CLI_EXTERN const char * const   _cli_if_name[] = {
    REPEAT_GROW(CLI_MAX_INTERFACE, CLI_INAME_R, CLI_INAME_L, 0)
};
#define CLI_IF_DEF_NAME         _cli_if_name[cli - 1]   // �����нӿ���


//lint -emacro(786, CLI_LNAME_R, CLI_LNAME_L)   " String concatenation "
#define CLI_LNAME_R(n, m)       {"CLI " #n " i_lock", "CLI " #n " o_lock", "CLI " #n " if_lock"},
#define CLI_LNAME_L(n, m)       {"CLI " #n " i_lock", "CLI " #n " o_lock", "CLI " #n " if_lock"}
CLI_EXTERN const char * const   _cli_lock_name[][3] = {
    REPEAT_GROW(CLI_MAX_INTERFACE, CLI_LNAME_R, CLI_LNAME_L, 0)
};
#define CLI_LOCK_NAME           _cli_lock_name[cli - 1] // �����нӿڱ�������


#define CLI_IF_PTR          (&_cli_if_tbl[cli - 1])     // �����нӿ�ָ��
#define CLI_IF_NAME         (CLI_IF_PTR->name)          // �����нӿ���
#define CLI_IF_PROMPT       (CLI_IF_PTR->prompt)        // ��������ʾ��
#define CLI_IF_WELCOME      (CLI_IF_PTR->welcome)       // �����л�ӭ��Ϣ
#define CLI_IF_DEVICE       (CLI_IF_PTR->device)        // �����нӿ��豸
#define CLI_IF_THREAD       (CLI_IF_PTR->thread)        // �������߳�
#define CLI_IF_I_LOCK       (CLI_IF_PTR->i_lock)        // ���������뱣����
#define CLI_IF_O_LOCK       (CLI_IF_PTR->o_lock)        // ���������������
#define CLI_IF_LOCK         (CLI_IF_PTR->if_lock)       // �����нӿڱ�����
#define CLI_IF_REFS         (CLI_IF_PTR->if_refs)       // �����нӿ����ü���
#define CLI_IF_ID           (CLI_IF_PTR->if_id)         // �����нӿ�ID
#define CLI_IF_STAT         (CLI_IF_PTR->if_stat)       // ������״̬    Todo: ���ԭ�Ӳ���
#define CLI_IF_L_STAT       (CLI_IF_PTR->l_stat)        // ������״̬
#define CLI_IF_L_POS        (CLI_IF_PTR->l_pos)         // �л���λ��
#define CLI_IF_L_CUR        (CLI_IF_PTR->l_cur)         // �л�����
#define CLI_IF_I_POS        (CLI_IF_PTR->i_pos)         // ���뻺��λ��
#define CLI_IF_I_CUR        (CLI_IF_PTR->i_cur)         // ���뻺����
#define CLI_IF_O_POS        (CLI_IF_PTR->o_pos)         // �������λ��
#define CLI_IF_L_BUFF       (CLI_IF_PTR->l_buff)        // �л���
#define CLI_IF_I_BUFF       (CLI_IF_PTR->i_buff)        // ���뻺��
#define CLI_IF_O_BUFF       (CLI_IF_PTR->o_buff)        // �������
#define CLI_IF_L_SUB        (CLI_IF_L_POS - CLI_IF_L_CUR)


//lint -emacro(506, CLI_IF_DATA_INIT)   " Constant value Boolean "
//lint -emacro(734, CLI_IF_DATA_INIT)   " Loss of precision "
// �����нӿ����ݳ�ʼ��
#define CLI_IF_DATA_INIT(device, name, flags)                           \
            CLI_IF_NAME    =  (name == NULL) ? CLI_IF_DEF_NAME : name ; \
            CLI_IF_PROMPT  =  CLI_DEFAULT_PROMPT;                       \
            CLI_IF_WELCOME =  CLI_DEFAULT_WELCOME;                      \
            CLI_IF_DEVICE  =  device;                                   \
         /* CLI_IF_THREAD  =   ;  */                                    \
         /* CLI_IF_I_LOCK  =   ;  */                                    \
         /* CLI_IF_O_LOCK  =   ;  */                                    \
         /* CLI_IF_LOCK    =   ;  */                                    \
         /* CLI_IF_REFS    =  0;  */                                    \
         /* CLI_IF_ID      = -1;  */                                    \
            CLI_IF_STAT    =  flags;                                    \
            CLI_IF_L_STAT  =  CLI_LINE_STAT_NORMAL;                     \
            CLI_IF_L_POS   =  0;                                        \
            CLI_IF_L_CUR   =  0;                                        \
            CLI_IF_I_POS   =  0;                                        \
            CLI_IF_I_CUR   =  0;                                        \
            CLI_IF_O_POS   =  0
#endif


/*----------------------------------------------------------------------------*/
/**
 * �������豸
 */
#if 1
#define CLI_DEVICE_OPEN()            CLI_IF_DEVICE->open( (cli_device_t *)CLI_IF_DEVICE, cli)
#define CLI_DEVICE_CLOSE()           CLI_IF_DEVICE->close((cli_device_t *)CLI_IF_DEVICE, cli)
#define CLI_DEVICE_READ( ptr, size)  CLI_IF_DEVICE->read( (cli_device_t *)CLI_IF_DEVICE, (ptr), (size))
#define CLI_DEVICE_WRITE(ptr, size)  CLI_IF_DEVICE->write((cli_device_t *)CLI_IF_DEVICE, (ptr), (size))


// ���豸�� size �ֽڵ� ptr, ret <-- �ɹ���ȡ����[(!= size)�豸�����쳣], tmp Ϊ int ������ʱ����
#define CLI_IF_DEV_READ( ptr, size, tmp, ret)


//lint -emacro(681, CLI_IF_DEV_WRITE)   " Loop is not entered "
// ptr �� size �ֽ�д���豸, ret <-- �ɹ�д�볤��[(!= size)�豸�����쳣], tmp Ϊ int ������ʱ����
#define CLI_IF_DEV_WRITE(ptr, size, tmp, ret)                                                \
            for((ret) = 0;  (int)(ret) < (int)(size);) {                                     \
                (tmp) = CLI_DEVICE_WRITE(&((uint8_t*)(ptr))[(ret)], (int)(size)-(int)(ret)); \
                if ((tmp) < 0)  { break; }                  (ret) += (tmp);                  \
            }   assert((int)(size) >= (int)(ret))


//lint -emacro(506, CLI_IF_DEV_ECHO)    " Constant value Boolean "
//lint -emacro(509, CLI_IF_DEV_ECHO)    " Constant value Boolean "
//lint -emacro(713, CLI_IF_DEV_ECHO)    " Loss of precision "
//lint -emacro(774, CLI_IF_DEV_ECHO)    " Boolean within 'if' always evaluates to True "
// �����л���, ��ͬʱ���� ptr1��ptr2��ptr3 �ַ���, expt1��expt2 Ϊ�豸�쳣����
#define CLI_IF_DEV_ECHO(expt1, expt2, ptr1, size1, ptr2, size2, ptr3, size3)                               \
            if (CLI_IF_STAT & CLI_ECHO_ON)                                                                 \
            {   int  _t1_, _t2_, _t3_;    _t3_ = -1;                                                       \
                OS_MUTEX_ENTER(CLI_IF_O_LOCK, OS_WAIT_FOREVER)                                             \
                {                                                                                          \
                    if ((_t1_ = (size1)) != 0) {                                                           \
                        CLI_IF_DEV_WRITE((ptr1), _t1_, _t2_, _t3_);  if(_t3_ != _t1_){ _t3_ = -1; break; } \
                    }                                                                                      \
                    if ((_t1_ = (size2)) != 0) {                                                           \
                        CLI_IF_DEV_WRITE((ptr2), _t1_, _t2_, _t3_);  if(_t3_ != _t1_){ _t3_ = -1; break; } \
                    }                                                                                      \
                    if ((_t1_ = (size3)) != 0) {                                                           \
                        CLI_IF_DEV_WRITE((ptr3), _t1_, _t2_, _t3_);  if(_t3_ != _t1_){ _t3_ = -1; break; } \
                    }                                                                                      \
                }                                                                                          \
                OS_MUTEX_LEAVE(CLI_IF_O_LOCK, OS_MUTEX_TIMEOUT_HANDLER)                                    \
                if(_t3_ <= 0){ ##expt1##;  ##expt2##; }                                                    \
            }   ((void)0)
#endif


/*----------------------------------------------------------------------------*/
/**
 * ���̴���
 */
#if 1
#define CLI_KEY_BEL             '\a'        // 0x07 ����
#define CLI_KEY_BS              '\b'        // 0x08 �˸�(Backspace)
#define CLI_KEY_HT              '\t'        // 0x09 ˮƽ�Ʊ�(Tab)
#define CLI_KEY_LF              '\n'        // 0x0A ����(Enter)
#define CLI_KEY_VT              '\v'        // 0x0B ��ֱ�Ʊ�
#define CLI_KEY_FF              '\f'        // 0x0C ��ҳ
#define CLI_KEY_CR              '\r'        // 0x0D �س�(Enter)
#define CLI_KEY_UP              0x0E        // Up    Arrow <--- �Ƴ�
#define CLI_KEY_DOWN            0x0F        // Down  Arrow <--- ����
#define CLI_KEY_RIGHT           0x10        // Right Arrow <--- �������Ӷ�ʧ
#define CLI_KEY_LEFT            0x11        // Left  Arrow <--- �豸���� 1
#define CLI_KEY_HOME            0x12        // Home        <--- �豸���� 2
#define CLI_KEY_INSERT          0x13        // Insert      <--- �豸���� 3
#define CLI_KEY_END             0x14        // End         <--- �豸���� 4
#define CLI_KEY_PU              0x15        // Page Up     <--- �񶨽���
#define CLI_KEY_PD              0x16        // Page Down   <--- ͬ�����÷�
#define CLI_KEY_DEL             0x7F        // Delete


#define CLI_LINE_STAT_NORMAL    0x00        // �л���״̬Ϊ��ͨ
#define CLI_LINE_STAT_FEED1     0x01        // �ϴ�����Ϊ�н�����('\r' 0x0D)
#define CLI_LINE_STAT_FEED2     0x02        // �ϴ�����Ϊ�н�����('\n' 0x0A)
#define CLI_LINE_STAT_SEQUC     0x03        // �ϴ�����Ϊ���з��� 0x1B
#define CLI_LINE_STAT_SPEC1     0x04        // �ϴ�����Ϊ������� 0x5B '['
#define CLI_LINE_STAT_SPEC2_1   0x05        // �ϴ�����Ϊ������� 0x31 '1'
#define CLI_LINE_STAT_SPEC2_2   0x06        // �ϴ�����Ϊ������� 0x32 '2'
#define CLI_LINE_STAT_SPEC2_3   0x07        // �ϴ�����Ϊ������� 0x33 '3'
#define CLI_LINE_STAT_SPEC2_4   0x08        // �ϴ�����Ϊ������� 0x34 '4'
#define CLI_LINE_STAT_SPEC2_5   0x09        // �ϴ�����Ϊ������� 0x35 '5'
#define CLI_LINE_STAT_SPEC2_6   0x0A        // �ϴ�����Ϊ������� 0x36 '6'


#define CLI_LINE_BS(n, m)       "\b"
CLI_EXTERN const char   _cli_str_bs[] = {   // �����˸�(����)
    " \b\b\b\b"
#if (CLI_LINE_BUFF_SIZE <= 512)
    REPEAT_GROW(CLI_LINE_BUFF_SIZE, CLI_LINE_BS, CLI_LINE_BS, 0)
#else
#if (CLI_LINE_BUFF_SIZE > (512 * 1))
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
#endif
#if (CLI_LINE_BUFF_SIZE > (512 * 2))
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
#endif
#if (CLI_LINE_BUFF_SIZE > (512 * 3))
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
#endif
#if (CLI_LINE_BUFF_SIZE > (512 * 4))
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
#endif
#if (CLI_LINE_BUFF_SIZE > (512 * 5))
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
#endif
#if (CLI_LINE_BUFF_SIZE > (512 * 6))
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
#endif
#if (CLI_LINE_BUFF_SIZE > (512 * 7))
    REPEAT_GROW(512               , CLI_LINE_BS, CLI_LINE_BS, 0)
#endif
#endif  // (CLI_LINE_BUFF_SIZE <= 512)
};
#endif


/*******************************************************************************
 * @}
 * @addtogroup          Private_Prototypes
 * @{
 ******************************************************************************/

static int   cli_line_char(int cli, char ch, char *str, int size);

static int   cli_buff_read (int cli);
static int   cli_buff_get  (int cli, void *buff, int size);
static void  cli_buff_free(int cli, int size);
static int   cli_buff_flush(int cli, int all);

static int   cli_refs_inc(int cli);
static int   cli_refs_dec(int cli);

static int cli_cmd_unfound(int cli, const char *args);
static int cli_cmd_bin    (int cli, const void *data, int size);
static int cli_cmd_idc    (int cli, const void *data, int size);

static void  cli_command_init(void);
static void *cli_command_find(const char *str);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#endif  // __CLI_SHELL_DEF_H__
/*****************************  END OF FILE  **********************************/

