
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_ANLG_DEV_H__
#define __CS40X_V3_0_ANLG_DEV_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "CS40xV3.0Comm.h"
#include "CS40xV3.0Rang.h"
#include "CS40xV3.0Source.h"
#include "CS40xV3.0Sens.h"
#include "CS40xV3.0Sweeps.h"
#include "CS40xV3.0System.h"
#include "CS40xV3.0trig.h"





typedef struct SMU_AnlgDev_{
    SMU_func_e          mFunc;              //!< - 类型{ SMU_VOLT | SMU_CURR }
    SMU_shap_e          mShap;              //!< - 输出类型{ SMU_DC | SMU_PULS }
    SMU_mode_e          mMode;              //!< - 输出模式
    SOUR_info_t         *mSourInfo;         //!< - SOUR Info
    RANG_stat_t         *const mLevRang;    //!< - 源量程
    RANG_stat_t         *const mLimRang;    //!< - 限量程
    SENS_info_t         *mSensInfo;         //!< - SENS Info
    SYST_info_t         *mSystInfo;         //!< - SYST Info
    SWE_info_t          *mSweInfo;          //!< - SWE Info
} SMU_AnlgDev_t;




#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_ANLG_DEV_H__ */
