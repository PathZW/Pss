/*******************************************************************************
 * @file    emb_queue.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.2
 * @date    2015/6/3
 * @brief   FIFO ���ж��������. ��Producer/��Consumer �߳�ʱΪ��������(LockFree).
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2015/6/3 -- Linghu -- the first version
 *
 * @par Change Logs:
 *      2017/11/3 -- Linghu -- Explicit define lockfree operation.
 *
 * @par Change Logs:
 *      2019/7/5 -- Linghu -- Add queue pointer operations.
 ******************************************************************************/
#ifndef __EMB_QUEUE_H__
#define __EMB_QUEUE_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup FIFO_Queue
 * @{
 ******************************************************************************/
/*------------------------------------------------------------------------------
              ��β                                                    ��ͷ
             ��(POP)                                                ��(PUSH)
              tail                                                    head
            +-------+-------+-------+-------+-------+-------+-------+-------+
  Queue:    |   0   |   1   |   2   |  ...  |  n-2  |  n-1  |   n   |       |
            +-------+-------+-------+-------+-------+-------+-------+-------+
            |   T   |  T+1  |  T+2  |  ...  |  H-3  |  H-2  |  H-1  |   H   |
            +-------+-------+-------+-------+-------+-------+-------+-------+
  Position: |   0   |   1   |   2   |  ...  |  -3   |  -2   |  -1   |       |
            +-------+-------+-------+-------+-------+-------+-------+-------+
------------------------------------------------------------------------------*/
/**
 * ���г�������, ������, ��С��256, �ɶ���Ϊ unsigned char
 */
#ifndef __Q_SIZE
#define __Q_SIZE                unsigned short
#endif

/**
 * @brief       ����һ��FIFO��������
 * @param[in]   type  ����Ԫ������
 * @param[in]   len   ����Ԫ�����鳤��, С��(__Q_SIZE)���ֵ, ����Ϊ(2 ^ n)
 */
#define QUEUE_TYPE(type, len)   struct {                                            \
                                    type          item[len];     /* ���� buffer  */ \
                                    volatile __Q_SIZE  head;     /* head -- ͷ�� */ \
                                    volatile __Q_SIZE  tail;     /* tail -- β�� */ \
                                }

//lint -emacro(716, QUEUE_INIT)     // while(1)
//lint -emacro(717, QUEUE_INIT)     // do ... while(0)
/**
 * @brief       ���г�ʼ��. �� LockFree
 * @param[in]   q    ���ж���
 * @return      NONE
 */
#define QUEUE_INIT(q)   do{ assert(IS_POWER_OF_TWO(__Q_LEN(q))); __Q_H_I(q); __Q_T_I(q); }while(0)

/**
 * @brief       ��ȡ���д�С(���ɱ����Ԫ�ظ���). LockFree����: ��
 * @param[in]   q    ���ж���
 * @return      (size_t)���д�С
 */
#define QUEUE_SIZE(q)           ( (__Q_SIZE)(__Q_LEN(q) - 1) )

/**
 * @brief       ��ȡ���������Ԫ�ظ���. LockFree����: ��
 * @param[in]   q    ���ж���
 * @return      (size_t)�����Ԫ�ظ���
 */
#define QUEUE_ITEM(q)           __Q_MOD(q, (__Q_H_R(q) - __Q_T_R(q)))

/**
 * @brief       ��ȡ�����п���Ԫ�ظ���. LockFree����: ��
 * @param[in]   q    ���ж���
 * @return      (size_t)����Ԫ�ظ���
 */
#define QUEUE_FREE(q)           __Q_MOD(q, (__Q_T_R(q) - __Q_H_R(q)) - 1)

/**
 * @brief       �ж϶����Ƿ�����. LockFree����: ��
 * @param[in]   q    ���ж���
 * @return      (true)����, (false)����
 */
#define QUEUE_IS_FULL(q)        ( QUEUE_FREE(q) == 0 )

/**
 * @brief       �ж϶����Ƿ�Ϊ��. LockFree����: ��
 * @param[in]   q    ���ж���
 * @return      (true)Ϊ��, (false)�ǿ�
 */
#define QUEUE_IS_EMPTY(q)       ( QUEUE_ITEM(q) == 0 )

/**
 * @brief       �鿴����ָ��λ��Ԫ��.
 * @param[in]   q    ���ж���
 * @param[in]   ofs  Ԫ�����λ��
 *                   @arg ( 0 )  �������Ԫ��λ��(POP).  LockFree����: Consumer
 *                   @arg (-1 )  ������Ԫ��λ��(PUSH). LockFree����: Consumer
 *                   @arg (>= 0) ������������Ԫ��λ�õ�ƫ��. LockFree����: Consumer
 *                   @arg ( < 0) �����������Ԫ��λ�õ�ƫ��. LockFree����: Producer
 * @param[out]  ptr  Ԫ��ָ��, ���ػ�ȡ��Ԫ��
 * @return      NONE
 * @note        ���뱣֤����(ofs)λ��Ԫ�ش���
 */
#define QUEUE_PEEK(q, ofs, ptr)  ( *(ptr) = (q).item[ __Q_MOD(q, __Q_OFFS(q, ofs))] )

/**
 * @brief       �������. LockFree����: Producer
 * @param[in]   q    ���ж���
 * @param[in]   itm  ������е�Ԫ��
 * @return      NONE
 * @note        ���뱣֤���з���, ������ܶ�ʧ����������Ԫ��
 */
#define QUEUE_PUSH(q, itm)      ( (q).item[__Q_MOD(q, __Q_H_R(q))] = (itm), __Q_H_M(q, 1) )

/**
 * @brief       ���г���. LockFree����: Consumer
 * @param[in]   q    ���ж���
 * @param[out]  ptr  ���س��ӵ�Ԫ��
 * @return      NONE
 * @note        ���뱣֤���зǿ�, ������ܶ�ʧ����������Ԫ��
 */
#define QUEUE_POP(q, ptr)       ( *(ptr) = (q).item[__Q_MOD(q, __Q_T_R(q))], __Q_T_M(q, 1) )

//lint -emacro(522, QUEUE_I_BUF)
/**
 * @brief       ��ȡ������ӻ���. LockFree����: Producer
 * @param[in]   q    ���ж���
 * @param[out]  ptr  ������ӻ����ָ��
 * @return      ��ӻ��泤��(��ѭ��), �Զ���Ԫ��(���ֽ�)Ϊ��λ
 */
#define QUEUE_I_BUF(q, ptr)     ( *(ptr) = &((q).item[__Q_MOD(q, __Q_H_C(q))]), __Q_FREE(q) )

//lint -emacro(522, QUEUE_O_BUF)
/**
 * @brief       ��ȡ���г��ӻ���. LockFree����: Consumer
 * @param[in]   q    ���ж���
 * @param[out]  ptr  ���س��ӻ����ָ��
 * @return      ���ӻ��泤��(��ѭ��), �Զ���Ԫ��(���ֽ�)Ϊ��λ
 */
#define QUEUE_O_BUF(q, ptr)     ( *(ptr) = &((q).item[__Q_MOD(q, __Q_T_C(q))]), __Q_ITEM(q) )

/**
 * @brief       ��������Ԫ������(���Ӷ�����Ԫ�ظ���). LockFree����: Producer
 * @param[in]   q    ���ж���
 * @param[in]   num  ���Ԫ�ظ���
 */
#define QUEUE_I_ADJ(q, num)     __Q_H_M(q, num)

/**
 * @brief       ��������Ԫ������(���ٶ�����Ԫ�ظ���). LockFree����: Consumer
 * @param[in]   q    ���ж���
 * @param[in]   num  ����Ԫ�ظ���
 */
#define QUEUE_O_ADJ(q, num)     __Q_T_M(q, num)


/******************************************************************************/
#define __Q_LEN(q)          ( (__Q_SIZE)(sizeof((q).item) / sizeof((q).item[0])) )
#define __Q_MOD(q, val)     (((__Q_SIZE)(val)) & (__Q_LEN(q) - 1))

/**
 * __Q_OFFS(): ������ ofs ƫ��Ԫ��λ��
 * __Q_ITEM(): �����������ɳ���Ԫ������
 * __Q_FREE(): ���������������Ԫ������
 */
//lint -emacro(506, __Q_OFFS)           // Constant value Boolean
//lint -emacro(835, __Q_OFFS)           // A zero has been given to operator
//lint -emacro(834, __Q_ITEM, __Q_FREE) // Operator '-' followed by operator '-' is confusing
#define __Q_OFFS(q, ofs) ( (ofs) + (((ofs) >= 0) ? __Q_T_R(q) : __Q_H_R(q)) )
#define __Q_ITEM(q)      MIN(__Q_LEN(q) - __Q_MOD(q, __Q_T_C(q)), __Q_MOD(q, __Q_H_C(q) - __Q_T_C(q)    ))
#define __Q_FREE(q)      MIN(__Q_LEN(q) - __Q_MOD(q, __Q_H_C(q)), __Q_MOD(q, __Q_T_C(q) - __Q_H_C(q) - 1))

/**
 * ������������
 */
#define __Q_H_I(q)       ATOMIC_INIT(         (q).head, 0    )   // Initialize
#define __Q_H_C(q)       ATOMIC_LOAD(__Q_DVAD((q).head)      )   // Read (const)
#define __Q_H_R(q)       ATOMIC_LOAD(         (q).head       )   // Read
#define __Q_H_W(q, val)  ATOMIC_STORE(        (q).head, (val))   // Write
#define __Q_H_M(q, val)  ATOMIC_ADD(          (q).head, (val))   // Addition

/**
 * ��β��������
 */
#define __Q_T_I(q)       ATOMIC_INIT(         (q).tail, 0    )   // Initialize
#define __Q_T_C(q)       ATOMIC_LOAD(__Q_DVAD((q).tail)      )   // Read (const)
#define __Q_T_R(q)       ATOMIC_LOAD(         (q).tail       )   // Read
#define __Q_T_W(q, val)  ATOMIC_STORE(        (q).tail, (val))   // Write
#define __Q_T_M(q, val)  ATOMIC_ADD(          (q).tail, (val))   // Addition

#define __Q_DVAD(m)      ( *((const __Q_SIZE *)(&(m))) ) // del [volatile] and add [const]


/******************************************************************************/
#ifndef IS_POWER_OF_TWO
#define IS_POWER_OF_TWO(n)  (((n) & ((n) - 1)) == 0)    // (true): n Ϊ 2 ���ݴη�
#endif

#ifndef assert
#define assert(e)   do{ if(!(e))  while(1); }while(0)   // assert()
#endif

#ifndef MIN
#define MIN(a, b)   (((a) < (b)) ? (a) : (b))
#endif

#ifndef ATOMIC_INIT
#define ATOMIC_INIT(var, val)       ((var) = (val))
#endif
#ifndef ATOMIC_STORE
#define ATOMIC_STORE(var, val)      ((var) = (val))
#endif
#ifndef ATOMIC_LOAD
#define ATOMIC_LOAD(var)             (var)
#endif
#ifndef ATOMIC_ADD
#define ATOMIC_ADD(var, val)        ((var) += (val))
#endif


/*****************************  END OF FILE  ***********************************
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_QUEUE_H__
/*****************************  END OF FILE  **********************************/

