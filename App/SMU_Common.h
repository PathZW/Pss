/*******************************************************************************
 * @file    SMU_Common.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/29
 * @brief   Source Meter Unit common
 *******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) Wuhan Precise Co.,Ltd</center></h2>
 *
 * The information and source code contained herein is the exclusive
 * property of Wuhan Precise Electronic Technology Co.,Ltd. And may
 * not be disclosed, examined or reproduced in whole or in part without
 * explicit written authorization from the company.
 *
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/3/29 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __SMU_COMMON_H__
#define __SMU_COMMON_H__
/******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "SMU_Range.h"

#ifndef SMU_LIST_SIZE
#define SMU_LIST_SIZE       (1024ul * 2)    // ���ɨ���б��С
#endif

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup SMU_Common
 * @{
 * @addtogroup              Common
 * @{
 ******************************************************************************/

//!< SMU Shap ����
typedef enum {                  //!< SMU ����
    SMU_DC    = 0,              //!< - ֱ��
    SMU_PULS  = 1,              //!< - ����
} SMU_SHAP_E;

//!< SMU Func ����
typedef enum {                  //!< Դ����
    SMU_VOLT = 0,               //!< - ��ѹԴ
    SMU_CURR = 1,               //!< - ����Դ
} SMU_FUNC_E;

typedef enum {                  //!< ���ģʽ
    OUT_FIX   = 0,              //!< - �㶨���
    SWE_LIST  = 1,              //!< - �б����
    SWE_LIN   = 2,              //!< - ɨ��:����
    SWE_LOG   = 4,              //!< - ɨ��:����
    SWE_SIN   = 6,              //!< - ɨ��:���Ҳ�
    SWE_SQU   = 7,              //!< - ɨ��:����
    SWE_TRI   = 8,              //!< - ɨ��:���ǲ�
    SWE_RIMP  = 9,              //!< - ɨ��:��ݲ�
} SMU_MODE_E;

//lint -emacro(835, SMU_CHNL)
typedef uint32_t      SMU_CHNL_T;           //!< ͨ����������
#define SMU_CHNL(n) ((SMU_CHNL_T)1 << (n))  //!< ͨ�� n ����
#define SMU_CHNL_ALL   (0xFFFFFFFFul)       //!< ����ͨ��


/*******************************************************************************
 * @}
 * @addtogroup              Range
 * @{
 ******************************************************************************/

//!< ��������
typedef enum {                      //!< ��������
    RANG_FIX  = 0,                  //!< - �̶�
    RANG_AUTO = 1,                  //!< - �Զ�
    RANG_BEST = 2,                  //!< - ���
    RANG_UP   = 3,                  //!< - ����
    RANG_DOWN = 4,                  //!< - ����
} RANG_MODE_E;

typedef const struct {                      //
    void              (*fun)(uint32_t arg); //
    uint32_t            arg;                //
} RANG_SWITCH_T;

typedef struct {                    //!< ������Ϣ
    float               mVal;       //!< - ���ֵ
    float               mHiOver;    //!< - ���л�ֵ
    float               mLoOver;    //!< - ���л�ֵ
    double              mCalK;      //!< - У׼ϵ�� K(C1)
    double              mCalB;      //!< - У׼ϵ�� B(C0)
    int32_t             mCalM;      //!< - У׼ϵ�� M
    int64_t             mCalKmN;    //!< - У׼ϵ�� Km * N
    int64_t             mCalBmN;    //!< - У׼ϵ�� Bm * N
    RANG_SWITCH_T      *mSwList;    //!< - �л�������, ������{ NULL, 0 }
} RANG_INFO_T;

typedef struct RANG_STAT_T_ {       //!< ����״̬(Run-Time)
    unsigned            mIdxCur;    //!< - ��ǰ����
    unsigned            mIdxCfg;    //!< - ��������
    RANG_MODE_E         mMode;      //!< - ��������
    unsigned      const mIdxMax;    //!< - �������
    RANG_INFO_T  *const mInfoList;  //!< - ������Ϣ��
    void        (*const mExFun)(struct RANG_STAT_T_ *rang); //!< ��չ�л�����������
} RANG_STAT_T;
 
void  SMU_RangCfg   (RANG_INFO_T *info, float val, float hiOver, float loOver);
void  SMU_RangCalSet(RANG_INFO_T *info, double K, double B);

void        SMU_RangRst    (RANG_STAT_T *rang);
RANG_MODE_E SMU_RangModeGet(RANG_STAT_T *rang);
unsigned    SMU_RangIdxGet (RANG_STAT_T *rang);
float       SMU_RangValGet (RANG_STAT_T *rang);
double      SMU_RangCalVal (RANG_STAT_T *rang, double val);
bool        SMU_RangChange (RANG_STAT_T *rang, RANG_MODE_E mode, float val);
bool        SMU_RangSwitch (RANG_STAT_T *rang);


/*******************************************************************************
 * @}
 * @addtogroup              Sweeps
 * @{
 ******************************************************************************/

//!< ����ɨ�跽��
typedef enum {                  //!< ����ɨ�跽��
    ZONE_UP   = 0,              //!< - ����, ��� --> �յ�
    ZONE_DOW  = 1,              //!< - ����, �յ� --> ���
    ZONE_DUAL = 2,              //!< - ˫��, ��� --> �յ� --> ���
} SWE_ZONE_E;

typedef struct {                //!< ����ɨ�����
    RANG_MODE_E     mRangM;     //!< - ɨ����������
    uint32_t        mCount;     //!< - ɨ���ظ�����
    bool            bCAbort;    //!< - ����ֹͣ
    float           mStart;     //!< - ɨ�����
    float           mStop;      //!< - ɨ���յ�
    uint32_t        mPoin;      //!< - ɨ�����
    SWE_ZONE_E      mDire;      //!< - ɨ�跽��
} SWE_ZONE_T;

#define SWE_ZONE_DFT {          /*!< Ĭ��(����ɨ�����) */ \
      RANG_FIX                  /*!< - ɨ����������     */ \
    , 1u                        /*!< - ɨ���ظ�����     */ \
    , false                     /*!< - ����ֹͣ         */ \
    , 0.0f                      /*!< - ɨ�����         */ \
    , 0.0f                      /*!< - ɨ���յ�         */ \
    , 1u                        /*!< - ɨ�����         */ \
    , ZONE_UP                   /*!< - ɨ�跽��         */ \
}


/*******************************************************************************
 * @}
 * @addtogroup              Source
 * @{
 ******************************************************************************/
//typedef enum {                //!< ���״̬
//    SOUR_CLOSED   = 0,        //!< - �Ѿ��ر�
//    SOUR_STARTED  = 1,        //!< - �Ѿ�����
//    SOUR_CLOSING  = 2,        //!< - ���ڹر�
//    SOUR_STARTING = 3,        //!< - ��������
//} SOUR_STAT_E;

typedef struct {                            //!< SENS Info
    SMU_FUNC_E          mFunc;              //!< - ����{ SMU_VOLT | SMU_CURR }
    bool                bRsens;             //!< - �Ƿ�ʹ��4���Ʋ���
    bool                bAZero;             //!< - �Ƿ��Զ�����
    float               mRang;              //!< - ����
    bool                bRangA;             //!< - �Ƿ��Զ�����
    float               mRangHi;            //!< - �Զ����̸�ֵ
    float               mRangLo;            //!< - �Զ����̵�ֵ
    bool                bRangRe;            //!< - �Զ������Ƿ�ָ�
    float               mNplc;              //!< - NPLC ֵ    - �� SMU_DC ģʽ
    uint32_t            mAvgCnt;            //!< - �˲�ƽ���� - �� SMU_DC ģʽ
    uint32_t            mSRate;             //!< - ��������
} SENS_INFO_T;

typedef struct {                            //!< SOUR Info
    SMU_FUNC_E          mFunc;              //!< - ����{ SMU_VOLT | SMU_CURR }
    float               mLevVal;            //!< - Դֵ(V or A)
    RANG_STAT_T  *const mLevRang;           //!< - Դ����
    float               mLimVal;            //!< - ��ֵ(V or A)
    RANG_STAT_T  *const mLimRang;           //!< - ������
    float               mDlyDC;             //!< - ֱ���ȶ�ʱ��(S)
    float               mDlyPuls;           //!< - �½����ȶ�ʱ��(S)
    float               mPulsWidt;          //!< - ������(S)
    float               mPulsPeri;          //!< - ��������(S)
    uint32_t            mPulsNum;           //!< - ����������
    bool                bPulsAll;           //!< - �Ƿ�ȫ���ڲ���
    float               mBiasLev;           //!< - ƫ��Դֵ(V or A)
    float               mBiasLim;           //!< - ƫ����ֵ(V or A)
} SOUR_INFO_T;

//!< SOUR Info ����
#define SOUR_INFO_DFT(levRang, limRang) {   /*!< Ĭ��(SOUR Info)     */ \
      SMU_VOLT       /* mFunc     */        /*!< - ����              */ \
    , 0.0f           /* mLevVal   */        /*!< - Դֵ(V or A)      */ \
    , levRang        /* mLevRang  */        /*!< - Դ����            */ \
    , 0.0f           /* mLimVal   */        /*!< - ��ֵ(V or A)      */ \
    , limRang        /* mLimRang  */        /*!< - ������            */ \
    , 1e-6f          /* mDlyDC    */        /*!< - ֱ���ȶ�ʱ��(S)   */ \
    , 1e-6f          /* mDlyPuls  */        /*!< - �½����ȶ�ʱ��(S) */ \
    , 1e-3f          /* mPulsWidt */        /*!< - ������(S)       */ \
    , 2e-3f          /* mPulsPeri */        /*!< - ��������(S)       */ \
    , 1u             /* mPulsNum  */        /*!< - ����������        */ \
    , false          /* bPulsAll  */        /*!< - �Ƿ�ȫ���ڲ���    */ \
    , 0.0f           /* mBiasLev  */        /*!< - ƫ��Դֵ(V or A)  */ \
    , 0.0f           /* mBiasLim  */        /*!< - ƫ����ֵ(V or A)  */ \
}

typedef struct {                            //!< ����б�
    uint32_t    mCnt;                       //!< - �������
    double      mVal[SMU_LIST_SIZE];        //!< - ���Դֵ
} SOUR_LIST_T;

typedef struct {                            //!< SOUR Stat for Run-Time
    SOUR_INFO_T        *mInfo;              //!< - SOUR Info
    SMU_SHAP_E          mShap;              //!< - �������{ SMU_DC | SMU_PULS }
    bool                bOutp;              //!< - ���״̬[ ON(1) | OFF(0) ]
    SMU_MODE_E          mMode;              //!< - ���ģʽ
    SWE_ZONE_T   *const mZone;              //!< - ����ɨ��
    SOUR_LIST_T  *const mList;              //!< - ����б�
} SOUR_STAT_T;

#define SOUR_STAT_DFT(info, zone, list) {   /*!< Ĭ��(SOUR Stat)    */ \
      info           /* mInfo;  */          /*!< - SOUR Info        */ \
    , SMU_DC         /* mShap;  */          /*!< - �������         */ \
    , false          /* bOutp;  */          /*!< - ���״̬         */ \
    , OUT_FIX        /* mMode;  */          /*!< - ���ģʽ         */ \
    , zone           /* mZone;  */          /*!< - ����ɨ��         */ \
    , list           /* mList   */          /*!< - ����б�         */ \
}


/*******************************************************************************
 * @}
 * @addtogroup              Sense
 * @{
 ******************************************************************************/



/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __SMU_COMMON_H__
/*****************************  END OF FILE  **********************************/

