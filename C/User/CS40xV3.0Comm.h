
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_COMM_H__
#define __CS40X_V3_0_COMM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/*********************************************************************
 * @}
 * @addtogroup              Comm
 * @{
 ********************************************************************/
//!< SMU Func 类型
typedef enum SMU_func_{         //!< 源类型     SOUR:FUNC {VOLT | CURR | MEM}
    SMU_VOLT = 0,               //!< - 电压源
    SMU_CURR = 1,               //!< - 电流源
    SMU_MEM  = 2,               //!< - Memory   (暂不实现)
} SMU_func_e;

//!< SMU Shap 类型
typedef enum SMU_shap_{         //!< SMU 类型   :SOUR:FUNC:SHAP {DC | PULS}
    SMU_DC    = 0,              //!< - 直流
    SMU_PULS  = 1,              //!< - 脉冲
} SMU_shap_e;

//!< SMU mode 类型
typedef enum SMU_mode_{         //!< SMU 模式类型
    OUT_FIX ,                   //!< - 恒定输出     :SOUR:{VOLT | CURR}:MODE {FIX | LIST SWE}
    OUT_SWE ,                   //!< - 扫描输出
    SWE_LIST,                   //!< - 扫描:自定义(列表)
    SWE_LIN ,                   //!< - 扫描:线性    :SOUR:SWE:SPAC {LIN | LOG}
    SWE_LOG ,                   //!< - 扫描:对数
    SWE_SIN ,                   //!< - 扫描:正弦波  (暂不实现)
    SWE_SQU ,                   //!< - 扫描:方波    (暂不实现)
    SWE_TRI ,                   //!< - 扫描:三角波  (暂不实现)
    SWE_RIMP,                   //!< - 扫描:锯齿波  (暂不实现)
} SMU_mode_e;


#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_COMM_H__ */
