/*******************************************************************************
 * @file    emb_queue.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.2
 * @date    2015/6/3
 * @brief   FIFO 队列定义与操作. 单Producer/单Consumer 线程时为无锁队列(LockFree).
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
              队尾                                                    队头
             出(POP)                                                入(PUSH)
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
 * 队列长度类型, 可配置, 如小于256, 可定义为 unsigned char
 */
#ifndef __Q_SIZE
#define __Q_SIZE                unsigned short
#endif

/**
 * @brief       定义一个FIFO队列类型
 * @param[in]   type  队列元素类型
 * @param[in]   len   队列元素数组长度, 小于(__Q_SIZE)最大值, 必须为(2 ^ n)
 */
#define QUEUE_TYPE(type, len)   struct {                                            \
                                    type          item[len];     /* 队列 buffer  */ \
                                    volatile __Q_SIZE  head;     /* head -- 头入 */ \
                                    volatile __Q_SIZE  tail;     /* tail -- 尾出 */ \
                                }

//lint -emacro(716, QUEUE_INIT)     // while(1)
//lint -emacro(717, QUEUE_INIT)     // do ... while(0)
/**
 * @brief       队列初始化. 非 LockFree
 * @param[in]   q    队列对象
 * @return      NONE
 */
#define QUEUE_INIT(q)   do{ assert(IS_POWER_OF_TWO(__Q_LEN(q))); __Q_H_I(q); __Q_T_I(q); }while(0)

/**
 * @brief       获取队列大小(最多可保存的元素个数). LockFree条件: 无
 * @param[in]   q    队列对象
 * @return      (size_t)队列大小
 */
#define QUEUE_SIZE(q)           ( (__Q_SIZE)(__Q_LEN(q) - 1) )

/**
 * @brief       获取队列已入队元素个数. LockFree条件: 无
 * @param[in]   q    队列对象
 * @return      (size_t)已入队元素个数
 */
#define QUEUE_ITEM(q)           __Q_MOD(q, (__Q_H_R(q) - __Q_T_R(q)))

/**
 * @brief       获取队列中空闲元素个数. LockFree条件: 无
 * @param[in]   q    队列对象
 * @return      (size_t)空闲元素个数
 */
#define QUEUE_FREE(q)           __Q_MOD(q, (__Q_T_R(q) - __Q_H_R(q)) - 1)

/**
 * @brief       判断队列是否已满. LockFree条件: 无
 * @param[in]   q    队列对象
 * @return      (true)已满, (false)非满
 */
#define QUEUE_IS_FULL(q)        ( QUEUE_FREE(q) == 0 )

/**
 * @brief       判断队列是否为空. LockFree条件: 无
 * @param[in]   q    队列对象
 * @return      (true)为空, (false)非空
 */
#define QUEUE_IS_EMPTY(q)       ( QUEUE_ITEM(q) == 0 )

/**
 * @brief       查看队列指定位置元素.
 * @param[in]   q    队列对象
 * @param[in]   ofs  元素相对位置
 *                   @arg ( 0 )  最先入队元素位置(POP).  LockFree条件: Consumer
 *                   @arg (-1 )  最后入队元素位置(PUSH). LockFree条件: Consumer
 *                   @arg (>= 0) 相对于最先入队元素位置的偏移. LockFree条件: Consumer
 *                   @arg ( < 0) 相对于最后入队元素位置的偏移. LockFree条件: Producer
 * @param[out]  ptr  元素指针, 返回获取的元素
 * @return      NONE
 * @note        必须保证队列(ofs)位置元素存在
 */
#define QUEUE_PEEK(q, ofs, ptr)  ( *(ptr) = (q).item[ __Q_MOD(q, __Q_OFFS(q, ofs))] )

/**
 * @brief       队列入队. LockFree条件: Producer
 * @param[in]   q    队列对象
 * @param[in]   itm  插入队列的元素
 * @return      NONE
 * @note        必须保证队列非满, 否则可能丢失队列中所有元素
 */
#define QUEUE_PUSH(q, itm)      ( (q).item[__Q_MOD(q, __Q_H_R(q))] = (itm), __Q_H_M(q, 1) )

/**
 * @brief       队列出队. LockFree条件: Consumer
 * @param[in]   q    队列对象
 * @param[out]  ptr  返回出队的元素
 * @return      NONE
 * @note        必须保证队列非空, 否则可能丢失队列中所有元素
 */
#define QUEUE_POP(q, ptr)       ( *(ptr) = (q).item[__Q_MOD(q, __Q_T_R(q))], __Q_T_M(q, 1) )

//lint -emacro(522, QUEUE_I_BUF)
/**
 * @brief       获取队列入队缓存. LockFree条件: Producer
 * @param[in]   q    队列对象
 * @param[out]  ptr  返回入队缓存的指针
 * @return      入队缓存长度(非循环), 以队列元素(非字节)为单位
 */
#define QUEUE_I_BUF(q, ptr)     ( *(ptr) = &((q).item[__Q_MOD(q, __Q_H_C(q))]), __Q_FREE(q) )

//lint -emacro(522, QUEUE_O_BUF)
/**
 * @brief       获取队列出队缓存. LockFree条件: Consumer
 * @param[in]   q    队列对象
 * @param[out]  ptr  返回出队缓存的指针
 * @return      出队缓存长度(非循环), 以队列元素(非字节)为单位
 */
#define QUEUE_O_BUF(q, ptr)     ( *(ptr) = &((q).item[__Q_MOD(q, __Q_T_C(q))]), __Q_ITEM(q) )

/**
 * @brief       调整队列元素数量(增加队列中元素个数). LockFree条件: Producer
 * @param[in]   q    队列对象
 * @param[in]   num  入队元素个数
 */
#define QUEUE_I_ADJ(q, num)     __Q_H_M(q, num)

/**
 * @brief       调整队列元素数量(减少队列中元素个数). LockFree条件: Consumer
 * @param[in]   q    队列对象
 * @param[in]   num  出队元素个数
 */
#define QUEUE_O_ADJ(q, num)     __Q_T_M(q, num)


/******************************************************************************/
#define __Q_LEN(q)          ( (__Q_SIZE)(sizeof((q).item) / sizeof((q).item[0])) )
#define __Q_MOD(q, val)     (((__Q_SIZE)(val)) & (__Q_LEN(q) - 1))

/**
 * __Q_OFFS(): 队列中 ofs 偏移元素位置
 * __Q_ITEM(): 队列中连续可出队元素数量
 * __Q_FREE(): 队列中连续可入队元素数量
 */
//lint -emacro(506, __Q_OFFS)           // Constant value Boolean
//lint -emacro(835, __Q_OFFS)           // A zero has been given to operator
//lint -emacro(834, __Q_ITEM, __Q_FREE) // Operator '-' followed by operator '-' is confusing
#define __Q_OFFS(q, ofs) ( (ofs) + (((ofs) >= 0) ? __Q_T_R(q) : __Q_H_R(q)) )
#define __Q_ITEM(q)      MIN(__Q_LEN(q) - __Q_MOD(q, __Q_T_C(q)), __Q_MOD(q, __Q_H_C(q) - __Q_T_C(q)    ))
#define __Q_FREE(q)      MIN(__Q_LEN(q) - __Q_MOD(q, __Q_H_C(q)), __Q_MOD(q, __Q_T_C(q) - __Q_H_C(q) - 1))

/**
 * 队首索引操作
 */
#define __Q_H_I(q)       ATOMIC_INIT(         (q).head, 0    )   // Initialize
#define __Q_H_C(q)       ATOMIC_LOAD(__Q_DVAD((q).head)      )   // Read (const)
#define __Q_H_R(q)       ATOMIC_LOAD(         (q).head       )   // Read
#define __Q_H_W(q, val)  ATOMIC_STORE(        (q).head, (val))   // Write
#define __Q_H_M(q, val)  ATOMIC_ADD(          (q).head, (val))   // Addition

/**
 * 队尾索引操作
 */
#define __Q_T_I(q)       ATOMIC_INIT(         (q).tail, 0    )   // Initialize
#define __Q_T_C(q)       ATOMIC_LOAD(__Q_DVAD((q).tail)      )   // Read (const)
#define __Q_T_R(q)       ATOMIC_LOAD(         (q).tail       )   // Read
#define __Q_T_W(q, val)  ATOMIC_STORE(        (q).tail, (val))   // Write
#define __Q_T_M(q, val)  ATOMIC_ADD(          (q).tail, (val))   // Addition

#define __Q_DVAD(m)      ( *((const __Q_SIZE *)(&(m))) ) // del [volatile] and add [const]


/******************************************************************************/
#ifndef IS_POWER_OF_TWO
#define IS_POWER_OF_TWO(n)  (((n) & ((n) - 1)) == 0)    // (true): n 为 2 的幂次方
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

