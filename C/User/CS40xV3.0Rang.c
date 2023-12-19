

#include "CS40xV3.0Rang.h"


/******************** 函数声明 ******************/
/**
  * @功能: 量程参数默认配置
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 配置索引
           [in ] txData 配置参数
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: 例如根据加密信息，生成量程信息列表 
  **/
int SMU_rangeCfgPara(const RANG_stat_t *anlgRang, void *txIndex, void *txData);

