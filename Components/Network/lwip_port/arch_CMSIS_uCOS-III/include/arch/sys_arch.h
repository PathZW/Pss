/*******************************************************************************
 * @file    sys_arch.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/11/13
 * @brief   The operating system emulation layer
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2021/5/7 -- Linghu -- the first version, support for uCOS-III.
 ******************************************************************************/
#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H
/******************************************************************************/

#if !NO_SYS
#include "os.h"
#endif

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/

#define SYS_ARCH_DECL_PROTECT(lev)    CPU_SR_ALLOC()
#define SYS_ARCH_PROTECT(lev)         CPU_CRITICAL_ENTER()
#define SYS_ARCH_UNPROTECT(lev)       CPU_CRITICAL_EXIT()

/*----------------------------------------------------------------------------*/
#if !NO_SYS
typedef OS_MUTEX               sys_mutex_t;                     /*
err_t   sys_mutex_new         (sys_mutex_t *mutex);
void    sys_mutex_free        (sys_mutex_t *mutex);
void    sys_mutex_lock        (sys_mutex_t *mutex);
void    sys_mutex_unlock      (sys_mutex_t *mutex);
int     sys_mutex_valid       (sys_mutex_t *mutex);
void    sys_mutex_set_invalid (sys_mutex_t *mutex);             */
#endif

/*----------------------------------------------------------------------------*/
#if !NO_SYS
//lint -esym(534, sys_arch_sem_wait)

typedef OS_SEM               sys_sem_t;                         /*
err_t   sys_sem_new         (sys_sem_t *sem, u8_t count);
void    sys_sem_free        (sys_sem_t *sem);
u32_t   sys_arch_sem_wait   (sys_sem_t *sem, u32_t timeout);
void    sys_sem_signal      (sys_sem_t *sem);
int     sys_sem_valid       (sys_sem_t *sem);
void    sys_sem_set_invalid (sys_sem_t *sem);                   */
#endif

#if !NO_SYS     // Not lwIP port, calls differ between task level and ISR level
void    sys_sem_signal_fromisr(sys_sem_t *sem);
#else
#define sys_sem_signal_fromisr(sem)
#endif

#if !NO_SYS
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_thread_sem_new ()
#define LWIP_NETCONN_THREAD_SEM_FREE()  sys_thread_sem_free()
#define LWIP_NETCONN_THREAD_SEM_GET()   sys_thread_sem_get ()

void       sys_thread_sem_new (void);
void       sys_thread_sem_free(void);
sys_sem_t *sys_thread_sem_get (void);
#endif

/*----------------------------------------------------------------------------*/
#if !NO_SYS
//lint -esym(534, sys_mbox_trypost)
//lint -esym(534, sys_mbox_trypost_fromisr)
//lint -esym(534, sys_arch_mbox_fetch)
//lint -esym(534, sys_arch_mbox_tryfetch)

typedef OS_Q                    sys_mbox_t;                     /*
err_t   sys_mbox_new           (sys_mbox_t *mbox, int size);
void    sys_mbox_free          (sys_mbox_t *mbox);
void    sys_mbox_post          (sys_mbox_t *mbox, void  *msg);
err_t   sys_mbox_trypost       (sys_mbox_t *mbox, void  *msg);
u32_t   sys_arch_mbox_fetch    (sys_mbox_t *mbox, void **msg, u32_t timeout);
u32_t   sys_arch_mbox_tryfetch (sys_mbox_t *mbox, void **msg);
int     sys_mbox_valid         (sys_mbox_t *mbox);
void    sys_mbox_set_invalid   (sys_mbox_t *mbox);              */
#endif

#if !NO_SYS     // calls differ between task level and ISR level
err_t   sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void  *msg);
#else
#define sys_mbox_trypost_fromisr(mbox, msg)
#endif

/*----------------------------------------------------------------------------*/
#if !NO_SYS
//lint -esym(534, sys_thread_new)

typedef OS_TCB   *sys_thread_t;                             /*
sys_thread_t      sys_thread_new(  const char    *name
                                 , lwip_thread_fn thread
                                 , void          *arg
                                 , int            stacksize
                                 , int             prio
                                );                          */
void              sys_thread_exit(void);        // Not lwIP port

void    sys_arch_msleep(u32_t ms);
#define sys_msleep(ms)  sys_arch_msleep(ms)     // lwIP not used
#endif

u32_t   sys_jiffies(void);                      // lwIP not used
u32_t   sys_now    (void);
void    sys_init   (void);

/*----------------------------------------------------------------------------*/
#if !NO_SYS
void sys_mark_tcpip_thread(void);
int  sys_core_is_locked   (void);
#endif


/*****************************  END OF FILE  **********************************/
#ifdef  __cplusplus
}
#endif
#endif  // LWIP_ARCH_SYS_ARCH_H
/*****************************  END OF FILE  **********************************/

