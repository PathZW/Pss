/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_debug.h
 * Purpose: File System Debug Configuration
 *----------------------------------------------------------------------------*/
#if (FS_DEBUG_EVR_ENABLE != 0)
#include <stdint.h>

#include "rl_fs_lib.h"
#include "EventRecorder.h"              // Keil.ARM Compiler::Compiler:Event Recorder
#include "fs_evr.h"

/*
  Debug Configuration values

  0 (Off):          EventRecordNone
  1 (Errors):       EventRecordError
  2 (Errors + API): EventRecordError | EventRecordAPI
  3 (All):          EventRecordAll
*/

#define FS_DEBUG_EVR_CFG (FS_DEBUG_EVR_CORE   | \
                          FS_DEBUG_EVR_FAT    | \
                          FS_DEBUG_EVR_EFS    | \
                          FS_DEBUG_EVR_IOC    | \
                          FS_DEBUG_EVR_NFTL   | \
                          FS_DEBUG_EVR_NAND   | \
                          FS_DEBUG_EVR_MC_MCI | \
                          FS_DEBUG_EVR_MC_SPI )

#if (FS_DEBUG_EVR_CFG > 3)
  #error "FS_Debug.c contains invalid Event Recorder filter configuration."
#endif

#define Level_Msk(cfg)                             \
 ((cfg == 0) ? EventRecordNone  :                  \
  (cfg == 1) ? EventRecordError :                  \
  (cfg == 2) ? EventRecordError | EventRecordAPI : \
               EventRecordAll)

/*-----------------------------------------------------------------------------
 *  Configure Event Recorder event level
 *----------------------------------------------------------------------------*/
void fs_config_evr (void) {
  uint32_t msk;
  uint32_t comp;
  uint32_t i;

  EventRecorderDisable(EventRecordAll, EvtFsCore_No, EvtFsMcSPI_No);

  /* Select error level, component and apply configuration */
  for (i = 0U; i < 8U; i++) {
    switch (i) {
      default:
      case 0: msk = Level_Msk(FS_DEBUG_EVR_CORE);   comp = EvtFsCore_No;  break;
      case 1: msk = Level_Msk(FS_DEBUG_EVR_FAT);    comp = EvtFsFAT_No;   break;
      case 2: msk = Level_Msk(FS_DEBUG_EVR_EFS);    comp = EvtFsEFS_No;   break;
      case 3: msk = Level_Msk(FS_DEBUG_EVR_IOC);    comp = EvtFsIOC_No;   break;
      case 4: msk = Level_Msk(FS_DEBUG_EVR_NFTL);   comp = EvtFsNFTL_No;  break;
      case 5: msk = Level_Msk(FS_DEBUG_EVR_NAND);   comp = EvtFsNAND_No;  break;
      case 6: msk = Level_Msk(FS_DEBUG_EVR_MC_MCI); comp = EvtFsMcMCI_No; break;
      case 7: msk = Level_Msk(FS_DEBUG_EVR_MC_SPI); comp = EvtFsMcSPI_No; break;
    }
  
    EventRecorderEnable (msk, comp, comp);
  }
}
#endif /* (FS_DEBUG_EVR_ENABLE != 0) */
