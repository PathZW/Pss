/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_fs_lib.h
 * Purpose: File System Library Definitions
 *----------------------------------------------------------------------------*/

#ifndef __RL_FS_LIB_H
#define __RL_FS_LIB_H

#include <stdarg.h>

#if defined(RTE_FileSystem_Drive_MC_0) || defined(RTE_FileSystem_Drive_MC_1)
  #include "Driver_MCI.h"
  #include "Driver_SPI.h"
#else
  #define ARM_DRIVER_MCI          uint32_t
  #define ARM_MCI_SignalEvent_t   uint32_t
  #define ARM_MCI_CAPABILITIES    uint32_t
  #define ARM_DRIVER_SPI          uint32_t
  #define ARM_SPI_SignalEvent_t   uint32_t
  #define ARM_SPI_CAPABILITIES    uint32_t
#endif

#if defined(RTE_FileSystem_Drive_NAND_0) || defined(RTE_FileSystem_Drive_NAND_1)
  #include "Driver_NAND.h"
#else
  #define ARM_DRIVER_NAND         uint32_t
  #define ARM_NAND_SignalEvent_t  uint32_t
  #define ARM_NAND_CAPABILITIES   uint32_t
  #define ARM_NAND_ECC_INFO       uint32_t
#endif

#if defined(RTE_FileSystem_Drive_NOR_0) || defined(RTE_FileSystem_Drive_NOR_1)
  #include "Driver_Flash.h"
#else
  #define ARM_DRIVER_FLASH        uint32_t
  #define ARM_Flash_SignalEvent_t uint32_t
  #define ARM_FLASH_CAPABILITIES  uint32_t
  #define ARM_FLASH_SECTOR        uint32_t
#endif

#include "rl_fs.h"

/* Device System Type attribute */
#define FS_CUR                    ((uint8_t)0x01)  /* Initial Current Drive attribute    */
#define FS_EMB                    ((uint8_t)0x02)  /* Embedded File System (F:)          */
#define FS_FAT                    ((uint8_t)0x04)  /* FAT File System      (M:,N:,R:,U:) */
#define FS_IOC                    ((uint8_t)0x08)  /* IO Control allowed   (M:,N:)       */

/* Driver Init/Uninit Parameter Codes */
#define DM_IO                       0U  /* Init/Uninit IO peripherals         */
#define DM_MEDIA                    1U  /* Init/Uninit storage Media          */

/* NAND Configuration macros */
#define NAND_ROW_CYCLES(b,p)  ((b*p > 65536) ? 3 : 2)
#define NAND_COL_CYCLES(s)    ((s   >   528) ? 2 : 1)
#define NAND_BTT_SIZE(b)      ((b*3)/100)
#define NAND_SEC_PAGE(p)      (p/512)
#define NAND_SEC_BLOCK(b,p)   ((p/512)*b)
#define _DS_(b)               (b-(b*3)/100-1)
#define _EP_(p)               ((p/512)*128)
#define NAND_TSN_SIZE(b,p)    ((_DS_(b)+_EP_(p)-1)/_EP_(p))

#ifdef __cplusplus
extern "C"  {
#endif

/* Mutex for FileSystem */
typedef void *FS_MUTEX;

/* Device driver mapping type */
typedef struct {
  void   *dcb;                          /* Device Control block               */
  char    id[3];                        /* Drive Name Identification          */
  uint8_t attr;                         /* Drive attributes                   */
} const FS_DEV;

/* RAM interface control block */
typedef struct {
  uint8_t *Base;                        /* RAM buffer base address            */
  uint32_t Size;                        /* RAM buffer size                    */
} RAM_DEV;

/* Memory Card Device Control block (MCI) */
typedef struct {
  ARM_DRIVER_MCI       *Driver;         /* Registered MCI driver              */
  ARM_MCI_SignalEvent_t Callback;       /* Driver Callback                    */
  ARM_MCI_CAPABILITIES  Capabilities;   /* Driver Capabilities                */
  uint8_t              *ExtCSD;         /* Pointer to 512 byte buffer         */
  uint32_t              Property;       /* Card Properties                    */
  uint32_t              SerialNumber;   /* Memory card serial number          */
  uint32_t              SectorCount;    /* Device density in 512B sectors     */
  uint16_t              RCA;            /* Relative Card Address              */
  uint16_t volatile     Event;          /* Driver event flags                 */
  uint8_t  volatile     MediaStatus;    /* Media status                       */
  uint8_t               Status;         /* Device status                      */
  uint8_t               Instance;       /* Memory Card Instance number        */
  uint8_t               Reserved;       /* Reserved for future use            */
} MC_MCI;

/* Memory Card Device Control block (SPI) */
typedef struct {
  ARM_DRIVER_SPI       *Driver;         /* Registered SPI driver              */
  ARM_SPI_SignalEvent_t Callback;       /* Driver Callback                    */
  uint8_t              *ExtCSD;         /* Pointer to 512 byte buffer         */
  uint32_t              Property;       /* Card Properties                    */
  uint32_t              SerialNumber;   /* Memory card serial number          */
  uint32_t              SectorCount;    /* Device density in 512B sectors     */
  uint16_t              RCA;            /* Relative Card Address              */
  uint8_t volatile      Event;          /* Driver event flags                 */
  uint8_t volatile      MediaStatus;    /* Media status                       */
  uint8_t               Status;         /* Device status                      */
  uint8_t               Instance;       /* Memory Card Instance number        */
  uint8_t               Reserved[2];    /* Reserved for future use            */
} MC_SPI;

/* NAND Page Layout configuration */
typedef struct _NAND_PAGE_LAYOUT {
  struct {
    uint8_t  ofs_lsn;                   /* LSN position, where logical sector number (LSN) is placed. Usually, this is the first byte of Spare and has the value 0. LSN is a 32-bit value. */
    uint8_t  ofs_dcm;                   /* Page Data corrupted marker. Usually, this byte is the 5-th byte of Spare and has the value 4. */
    uint8_t  ofs_bbm;                   /* Bad Block marker position. Is usually placed as the 6-th byte of Spare and has the value 5. */
    uint8_t  ofs_ecc;                   /* Position of the first byte of Error Correction Code (ECC). Is usually the 7-th byte of Spare and has the value 6. This value is used by flash translation layer only if ECC is encoded and decoded in software. */
  } spare;                              /* Structure spare */
  uint16_t spare_ofs;                   /* Spare area offset from beginning of the page. */
  uint16_t spare_inc;                   /* Column increment till next spare. If page contains multiple sectors, then the first byte of the first spare area is determined by reading spare_ofs value. Location of the first byte of the second spare is (spare_inc + spare_ofs). */
  uint16_t sector_inc;                  /* Column increment till next sector. If page contains multiple sectors, then the first sector always starts at the beginning of the page (byte zero). Second sector starts at sect_inc, third sector at (sect_inc + sect_inc) and so on. */
} NAND_PAGE_LAYOUT;

/* NAND Flash Device information */
typedef struct _NAND_DEVICE {
  NAND_PAGE_LAYOUT *page_layout;        /* Page Layout configuration          */
  uint8_t           bus_width;          /* Device bus width (0=8bit, 1=16bit  */
  uint8_t           device_number;      /* Device number (chip select)        */
  uint16_t          page_size;          /* Page Size in bytes                 */
  uint32_t          block_count;        /* Number of Blocks in Device         */
  uint16_t          page_count;         /* Number of Pages per Block          */
  uint16_t          block_sectors;      /* Number of Sectors per Block        */
  uint8_t           page_sectors;       /* Number of Sectors per Page         */
  uint8_t           row_cycles;         /* Number of Row address cycles       */
  uint8_t           col_cycles;         /* Number of Column address cycles    */
  uint8_t           sw_ecc;             /* Type of software ECC operation     */
  ARM_NAND_ECC_INFO
  const            *ecc;                /* Device ECC configuration           */
} NAND_DEVICE;

/* NAND Driver Info */
typedef struct _NAND_HW_DRIVER {
  ARM_NAND_SignalEvent_t hw_cb;         /* Hardware event handler (callback)  */
  ARM_DRIVER_NAND       *drv;           /* NAND driver                        */
  ARM_NAND_CAPABILITIES  capabilities;  /* NAND driver capabilities           */
  uint8_t                init;          /* Initialization counter             */
  uint8_t                reserved[3];   /* Reserved for future use            */
} NAND_HW_DRIVER;

/* Signal NAND Media Driver event */
typedef void (*NAND_Media_SignalEvent_t) (uint32_t dev_num, uint32_t event);

/* NAND Media Driver Instance */
typedef struct _NFTL_DRIVER_HANDLE {
  NAND_Media_SignalEvent_t media_cb;    /* NAND Media event handler (callback)*/
  NAND_HW_DRIVER          *hw;          /* Hardware driver handle             */
  NAND_DEVICE const       *dev;         /* NAND Device description            */
  uint32_t                 seq;         /* Sequence number                    */
  uint16_t                 ecc;         /* ECC algorithm index and ECC flags  */
  uint8_t volatile         event;       /* Event flags                        */
  uint8_t                  status;      /* Device status byte                 */
  uint8_t                  instance;    /* Media handle instance              */
  uint8_t                  jedec_id;    /* JEDEC Manufacturer ID              */
  uint8_t                  ecc_req;     /* ECC correctability requirement     */
  uint8_t                  rsvd;        /* Reserved for future use            */
} NAND_MEDIA_HANDLE;

/* NAND Feature Parameters */
typedef struct _NAND_FEATURE_PARAMETER {
  uint8_t P1;
  uint8_t P2;
  uint8_t P3;
  uint8_t P4;
} NAND_FEATURE_PARAMETER;

/* NAND Media Interface Driver */
typedef struct _NAND_MEDIA_DRIVER {
  int32_t (*Initialize)     (NAND_MEDIA_HANDLE *h);
  int32_t (*Uninitialize)   (NAND_MEDIA_HANDLE *h);
  int32_t (*ResetDevice)    (NAND_MEDIA_HANDLE *h);
  int32_t (*ReadID)         (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len);
  int32_t (*ReadParamPage)  (NAND_MEDIA_HANDLE *h, uint32_t col, uint8_t *buf, uint32_t len);
  int32_t (*ReadPage)       (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, uint8_t *buf, uint32_t len);
  int32_t (*WritePage)      (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, const uint8_t *buf, uint32_t len);
  int32_t (*CopyPage)       (NAND_MEDIA_HANDLE *h, uint32_t row_src, uint32_t row_dst, uint32_t row_cnt);
  int32_t (*EraseBlock)     (NAND_MEDIA_HANDLE *h, uint32_t row);
  int32_t (*ReadStatus)     (NAND_MEDIA_HANDLE *h, uint8_t *stat);
  int32_t (*GetFeatures)    (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len);
  int32_t (*SetFeatures)    (NAND_MEDIA_HANDLE *h, uint8_t addr, const uint8_t *buf, uint32_t len);
} const NAND_MEDIA_DRIVER;

/* NAND FTL Block Index Cache */
typedef struct {
  uint8_t  *pgIdx;                      /* Index buffer                       */
  uint16_t pbn;                         /* Physical block number              */
  uint16_t lbn;                         /* Logical block number               */
  uint8_t  typ;                         /* Type of block in cache             */
  uint8_t  nextPg;                      /* Next page to be indexed            */
  uint8_t  rsvd[2];                     /* Reserved for future use            */
} BLOCK_CACHE;

/* NAND FTL Page Cache */
typedef struct {
  uint32_t row;                         /* Row address of the page in cache   */
  uint8_t *buf;                         /* Cached page buffer                 */
} PAGE_CACHE;

/* NAND FTL Buffer Pointers Structure */
typedef struct {
  uint32_t CacheBS;                     /* Block slot for block caching       */
  uint32_t CachePS;                     /* Page slot for page caching         */
  uint16_t CachedBlocks;                /* Number of indexed blocks           */
  uint16_t CachedPages;                 /* Number of cached pages             */
  BLOCK_CACHE *Block;                   /* Block indexing cache info struct   */
  PAGE_CACHE  *Page;                    /* Page data cache info structure     */
} NAND_FTL_CACHE;

/* NAND FTL Configuration structure */
typedef struct nand_ftl_cfg {
  /* Block Translation Table Space */
  uint16_t BttStartBn;                  /* First Physical Block               */
  uint16_t BttEndBn;                    /* Last Physical Block                */
  /* Data Space */
  uint16_t DataStartBn;                 /* First Physical Block               */
  uint16_t DataEndBn;                   /* Last Physical Block                */
  /* Buffer Sizes */  
  uint16_t NumCacheBlocks;              /* Number of indexed data blocks      */
  uint16_t NumCachePages;               /* Number of cached data pages        */
  uint16_t TsnTableSize;                /* Translation table cache size       */
  uint8_t  Reserved[2];                 /* Reserved for future use            */
  /* Page buffer & Caches */
  uint8_t     *PgBuf;                   /* Page data buffer                   */
  BLOCK_CACHE *BlockCache;              /* Block indexing cache info struct   */
  uint8_t     *BlockCacheBuf;           /* Block indexing cache buffer        */
  PAGE_CACHE  *PageCache;               /* Page data cache info structure     */
  uint8_t     *PageCacheBuf;            /* Page data cache buffer             */
  uint32_t    *TsnTable;                /* Translation table cache buffer     */
} const NAND_FTL_CFG;

/* NAND Device Control block */
typedef struct {
  NAND_MEDIA_HANDLE *Media;             /* NAND Media Driver Instance         */
  NAND_FTL_CFG      *Cfg;               /* NAND FTL configuration structure   */
  NAND_FTL_CACHE     Ca;                /* FTL cache info structure           */
  NAND_PAGE_LAYOUT   PgLay;             /* Page layout definition             */

  uint8_t   PageSectors;                /* Number of sectors/page 1,4,8, ...  */
  uint8_t   SPP;                        /* Number of sectors/page             */
                                        /*  (2^n, n = SPP)                    */
  uint8_t   PPB;                        /* Number of pages/block              */
                                        /*  (2^n, n = PPB)                    */
  uint8_t   SPB;                        /* Number of sectors/block            */
                                        /*  (2^n, n = SPB)                    */
  uint8_t   EPS;                        /* Entries/table sector               */
                                        /*  (2^n, n = EPS)                    */
  uint8_t   LastECC;                    /* ECC status after last page read    */
  uint16_t  LastDBN;                    /* Last allocated phy. block          */
                                        /*  in data area                      */
  uint16_t  LastTBN;                    /* Last allocated phy. block          */
                                        /*  in table area                     */
  uint32_t  CurrLBN;                    /* Current logical block number       */
  uint32_t  GcLBN;                      /* Current logical block number used  */
                                        /*  by forced GC                      */
  uint16_t  PbnQ[3];                    /* Empty block queue                  */
  uint16_t  BadBlockCnt;                /* Bad Block Counter                  */
  uint16_t  NumDataBlocks;              /* Number of data blocks reported     */
  uint16_t  TsnTableSize;               /* Translation table cache size       */
  uint32_t *TsnTable;                   /* Translation table cache buffer     */
  uint8_t  *PgBuf;                      /* Page data buffer                   */
  uint8_t   Status;                     /* FTL Status Flags                   */
  uint8_t   Reserved[3];                /* Reserved for future use            */
} NAND_FTL_DEV;

/* FAT Name Cache Configuration */
typedef struct fat_nca_cfg {
  uint8_t   MaxPathDepth;               /* Maximum path depth                 */
  uint8_t   ControlBlockCnt;            /* Number of control blocks           */
  uint32_t  NameMemPoolSize;            /* Name cache memory pool size        */
  uint32_t *NameMemPool;                /* Name cache memory pool             */
} const FAT_NCACHE_CFG;

/* FAT Sector Caching structure */
typedef struct fcache {
  uint32_t sect;                        /* Cached FAT sector number           */
  uint8_t  *buf;                        /* FAT sector cache buffer            */
  uint8_t  dirty;                       /* FAT table content modified         */
  uint8_t  cfat;                        /* Current FAT                        */
  uint8_t  rsvd[2];                     /* Reserved for future use            */
} FCACHE;

/* Data Sector Caching structure */
typedef struct dcache {
  uint32_t sect;                        /* Working Data sector number         */
  uint32_t csect;                       /* Data Cache starting sector number  */
  uint8_t  *buf;                        /* Working Data sector buffer         */
  uint8_t  *cbuf;                       /* Data Cache sector buffer           */
  uint8_t  nwr;                         /* Number of buffered write sectors   */
  uint8_t  nrd;                         /* Number of cached read sectors      */
  uint8_t  rsvd[2];                     /* Reserved for future use            */
} DCACHE;

/* Name Caching structure */
typedef struct ncache {
  uint32_t  max_path_depth;             /* Maximum path depth                 */
  uint32_t  size;                       /* Name cache memory pool size        */
  uint32_t *buf;                        /* Name cache memory pool             */
} NCACHE;

/* FAT Volume Information struct */
typedef struct fatinfo {
  uint32_t FAT32_RootClus;              /* Root directory first cluster       */
  uint16_t FAT32_ExtFlags;              /* FAT extended flags                 */
  uint16_t FAT32_FSInfo;                /* FSInfo structure sector number     */
  uint16_t FAT32_BkBootSec;             /* Boot Record copy sector number     */
  uint8_t  FatType;                     /* File System Fat type.              */
  uint8_t  SecPerClus;                  /* Number of Sectors per Cluster      */
  uint32_t BootSector;                  /* Boot Sector Offset                 */
  uint16_t FatSize;                     /* Number of Sectors per FAT table    */
  uint16_t RsvdSecCnt;                  /* Number of Reserved Sectors         */
  uint32_t DskSize;                     /* Disk Size Sector Count             */
  uint32_t DataSecCnt;                  /* Number of Data Sectors             */
  uint16_t RootEntCnt;                  /* Maximum Root Directory entries     */
  uint16_t BytesPerSec;                 /* Sector Size in bytes               */
  uint32_t DataClusCnt;                 /* Data Cluster Count                 */
  uint32_t RootDirAddr;                 /* Root Dir First Sector              */
  uint32_t ClusSize;                    /* Cluster Size in bytes              */
  uint16_t RootSecCnt;                  /* Number of Sectors for Root dir     */
  uint16_t EntsPerClus;                 /* Number of entries per cluster      */
  uint8_t  NumOfFat;                    /* Number of Fat tables               */
  uint8_t  Reserved[3];                 /* Reserved for future use            */
} FATINFO;

/* FAT File System driver */
typedef struct {
  uint32_t (*Init)        (uint32_t mode);
  uint32_t (*UnInit)      (uint32_t mode);
  uint32_t (*ReadSect)    (uint32_t sect, uint8_t *buf, uint32_t cnt);
  uint32_t (*WriteSect)   (uint32_t sect, const uint8_t *buf, uint32_t cnt);
  uint32_t (*ReadInfo)    (fsMediaInfo *cfg);
  fsStatus (*DeviceCtrl)  (fsDevCtrlCode code, void *p);
} const FAT_DRV;

/* File System Journal Control block */
typedef struct fsjour {
  FAT_DRV *drv;                         /* Memory device driver               */
  uint8_t *buf;                         /* Journal buffer                     */
  uint32_t FirstSect;                   /* First sector of journal            */
  uint32_t JournSect;                   /* Number of sectors available        */
  uint32_t TrId;                        /* Transaction identifier             */
  uint32_t TrSect;                      /* Transaction sector                 */
  uint8_t  State;                       /* Journal state                      */
  uint8_t  Status;                      /* Journal status                     */
  uint8_t  Reserved[2];                 /* Reserved for future use            */
} FSJOUR;

/* FAT Volume Description */
typedef struct _fsFAT_Volume {
  uint32_t    DrvLet;                   /* 4-byte encoded drive letter string */
  FS_MUTEX    Mutex;                    /* Volume mutex                       */
  FAT_DRV    *Drv;                      /* Registered FAT Device Driver       */
  uint32_t    Status;                   /* Volume Status                      */
  uint32_t   *CaBuf;                    /* Cache Buffer (FAT + Data)          */
  uint32_t    CaSize;                   /* Cache Buffer size                  */
  uint32_t    free_clus_cnt;            /* FAT32: Number of free clusters     */
  uint32_t    free_clus;                /* FAT32: First free cluster          */
  uint32_t    cdir_clus;                /* Current directory cluster          */
  NCACHE     *ncache;                   /* Name cache memory                  */
  FSJOUR     *fsj;                      /* File System Journal                */
  FATINFO     cfg;                      /* FAT Volume configuration           */
  FCACHE      fat;                      /* FAT table cache control            */
  DCACHE      ca;                       /* Data cache control                 */
  uint16_t    RsvdS;                    /* Reserved sectors used by journal   */
  uint8_t     Reserved[2];              /* Reserved for future use            */
} fsFAT_Volume;

/* FAT File Handle Description */
typedef struct _fsFAT_Handle {
  fsFAT_Volume *vol;                    /* FAT volume pointer                 */
  uint32_t  fsize;                      /* File size                          */
  uint32_t  fcsz;                       /* Current file size                  */
  uint32_t  fpos;                       /* File position indicator            */
  uint16_t  flags;                      /* File status flags                  */
  uint16_t  short_ent_offs;             /* SFN: Short entry sector offset     */
  uint32_t  short_ent_clus;             /* SFN: Short entry cluster number    */
  uint32_t  first_clus;                 /* First data cluster                 */
  uint32_t  current_clus;               /* Current data cluster               */
  uint8_t   current_sect;               /* Current data sector                */
  uint8_t   rsvd[3];                    /* Reserved for future use            */
} fsFAT_Handle;

/* Flash Device Timeouts */
typedef struct {
  uint16_t EraseChip;                   /* Chip erase operation timeout (s)   */
  uint16_t EraseSector;                 /* Sector erase operation timeout (s) */
  uint16_t Program;                     /* Program data operation timeout (s) */
  uint16_t Read;                        /* Read data operation timeout (s)    */
} const FLASH_TIMEOUT;

/* Flash Device */
typedef struct {
  ARM_FLASH_SECTOR *SectArray;          /* Flash sectors array                */
  uint32_t          SectCnt;            /* Total number of sectors            */
  uint32_t          SectSize;           /* Uniform sector size                */
  uint32_t          InitVal;            /* Erased memory content value        */
  FLASH_TIMEOUT    *Timeout;            /* Common operation timeouts          */
} FLASH_DEVICE;

/* EFS Volume Description */
typedef struct _fsEFS_Volume {
  uint32_t                DrvLet;       /* 4-byte encoded drive letter string */
  FS_MUTEX                Mutex;        /* Volume mutex                       */
  FLASH_DEVICE            Dev;          /* Flash device properties            */
  ARM_DRIVER_FLASH       *Drv;          /* Registered Device Driver           */
  ARM_FLASH_CAPABILITIES  Capab;        /* Flash driver capabilities          */
  ARM_Flash_SignalEvent_t Callback;     /* Flash driver callback              */
  uint32_t volatile       Event;        /* Flash driver events                */
  uint32_t                Status;       /* Volume status                      */
  uint32_t                Size;         /* Volume size                        */
  uint16_t                TopID;        /* Top used FileID                    */
  uint8_t                 Reserved[2];  /* Reserved for future use            */
} fsEFS_Volume;

/* EFS File Handle Description */
typedef struct _fsEFS_Handle {
  fsEFS_Volume *vol;                    /* EFS volume pointer                 */
  uint32_t  fsize;                      /* File Size                          */
  uint32_t  fpos;                       /* File position indicator            */
  uint16_t  flags;                      /* File status flags                  */
  uint16_t  fileID;                     /* File identification number         */
  uint16_t  fidx;                       /* Current file chunk index           */
  uint16_t  fblock;                     /* Current block index                */
  uint32_t  fbot;                       /* Block bottom position              */
  uint32_t  ftop;                       /* Block top position                 */
} fsEFS_Handle;

/* File System Configuration function */
extern void     fs_config          (const char *drive);
extern void     fs_config_evr      (void);

/* RAM drive interface functions */
extern uint32_t ram_Init           (uint32_t mode, RAM_DEV *ram);
extern uint32_t ram_UnInit         (uint32_t mode, RAM_DEV *ram);
extern uint32_t ram_ReadSector     (uint32_t sect, uint8_t *buf, uint32_t cnt, RAM_DEV *ram);
extern uint32_t ram_WriteSector    (uint32_t sect, const uint8_t *buf, uint32_t cnt, RAM_DEV *ram);
extern uint32_t ram_ReadInfo       (fsMediaInfo *info, RAM_DEV *ram);
extern fsStatus ram_DevCtrl        (fsDevCtrlCode code, void *p, RAM_DEV *ram);

/* Memory Card SPI mode interface functions */
extern uint32_t mc_spi_Init        (uint32_t mode, MC_SPI *mc);
extern uint32_t mc_spi_UnInit      (uint32_t mode, MC_SPI *mc);
extern uint32_t mc_spi_ReadSector  (uint32_t sect, uint8_t *buf, uint32_t cnt, MC_SPI *mc);
extern uint32_t mc_spi_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt, MC_SPI *mc);
extern uint32_t mc_spi_ReadInfo    (fsMediaInfo *info, MC_SPI *mc);
extern fsStatus mc_spi_DevCtrl     (fsDevCtrlCode code, void *p, MC_SPI *mc);
extern void     mc_spi_DrvEvent    (uint32_t event, MC_SPI *mc);

/* Memory Card Native mode interface functions */
extern uint32_t mc_mci_Init        (uint32_t mode, MC_MCI *mc);
extern uint32_t mc_mci_UnInit      (uint32_t mode, MC_MCI *mc);
extern uint32_t mc_mci_ReadSector  (uint32_t sect, uint8_t *buf, uint32_t cnt, MC_MCI *mc);
extern uint32_t mc_mci_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt, MC_MCI *mc);
extern uint32_t mc_mci_ReadInfo    (fsMediaInfo *info, MC_MCI *mc);
extern fsStatus mc_mci_DevCtrl     (fsDevCtrlCode code, void *p, MC_MCI *mc);
extern void     mc_mci_DrvEvent    (uint32_t event, MC_MCI *mc);

/* NAND Flash Translation Layer interface functions */
extern uint32_t ftl_Init           (uint32_t mode, NAND_FTL_DEV *ftl);
extern uint32_t ftl_UnInit         (uint32_t mode, NAND_FTL_DEV *ftl);
extern uint32_t ftl_ReadSect       (uint32_t lsn, uint8_t *buf, uint32_t cnt, NAND_FTL_DEV *ftl);
extern uint32_t ftl_WriteSect      (uint32_t lsn, const uint8_t *buf, uint32_t cnt, NAND_FTL_DEV *ftl);
extern uint32_t ftl_ReadInfo       (fsMediaInfo *info, NAND_FTL_DEV *ftl);
extern fsStatus ftl_DevCtrl        (fsDevCtrlCode code, void *p, NAND_FTL_DEV *ftl);

/* USB Mass Storage interface functions */
extern bool     FS_USBH_MSC_Initialize  (uint8_t instance, uint32_t mode);
extern bool     FS_USBH_MSC_Uninitialize(uint8_t instance, uint32_t mode);
extern bool     FS_USBH_MSC_ReadSectors (uint8_t instance, uint32_t sector_addr, uint32_t sector_num,       uint8_t *buf);
extern bool     FS_USBH_MSC_WriteSectors(uint8_t instance, uint32_t sector_addr, uint32_t sector_num, const uint8_t *buf);
extern bool     FS_USBH_MSC_ReadInfo    (uint8_t instance, fsMediaInfo *info);
extern fsStatus FS_USBH_MSC_DeviceCtrl  (uint8_t instance, fsDevCtrlCode code, void *p);

/* File System Journal interface functions */
extern uint32_t fsj_init      (FSJOUR *fsj, FAT_DRV *drv);
extern uint32_t fsj_set_space (FSJOUR *fsj, uint32_t start_sect, uint32_t cnt);
extern uint32_t fsj_write     (FSJOUR *fsj, uint32_t sect, uint32_t rsec, uint8_t *buf);

/* Embedded File System interface functions */
extern int32_t  efs_handle_get(fsEFS_Volume *vol);
extern fsStatus efs_init      (fsEFS_Volume *vol);
extern fsStatus efs_uninit    (fsEFS_Volume *vol);
extern fsStatus efs_mount     (fsEFS_Volume *vol);
extern fsStatus efs_unmount   (fsEFS_Volume *vol);
extern fsStatus efs_open      (int32_t handle, const char *fn, int32_t openmode);
extern fsStatus efs_close     (int32_t handle);
extern int32_t  efs_read      (int32_t handle, uint8_t *buf, uint32_t len);
extern int32_t  efs_write     (int32_t handle, const uint8_t *buf, uint32_t len);
extern fsStatus efs_flush     (int32_t handle);
extern int32_t  efs_flen      (int32_t handle);
extern fsStatus efs_seek      (int32_t handle, uint32_t pos);

/* EFS Maintenance Routines */
extern fsStatus efs_delete    (const char *filename, fsEFS_Volume *vol);
extern fsStatus efs_ffind     (fsFileInfo *info, fsEFS_Volume *vol);
extern fsStatus efs_rename    (const char *filename, const char *newname, fsEFS_Volume *vol);

/* EFS Utility Routines */
extern int64_t  efs_free      (fsEFS_Volume *vol);
extern fsStatus efs_format    (fsEFS_Volume *vol);
extern int32_t  efs_analyse   (fsEFS_Volume *vol);
extern fsStatus efs_check     (fsEFS_Volume *vol);
extern fsStatus efs_defrag    (fsEFS_Volume *vol);
extern fsStatus efs_info      (fsDriveInfo *info, fsEFS_Volume *vol);

/* EFS Event Callback handler for driver events */
extern void     efs_DrvEvent  (uint32_t event, fsEFS_Volume *vol);

/* FAT File System interface functions */
extern int32_t  fat_handle_get(fsFAT_Volume *vol);
extern fsStatus fat_init      (fsFAT_Volume *vol);
extern fsStatus fat_uninit    (fsFAT_Volume *vol);
extern fsStatus fat_mount     (fsFAT_Volume *vol);
extern fsStatus fat_unmount   (fsFAT_Volume *vol);
extern fsStatus fat_open      (int32_t handle, const char *name, int32_t openmode);
extern fsStatus fat_close     (int32_t handle);
extern int32_t  fat_read      (int32_t handle, uint8_t *buf, uint32_t len);
extern int32_t  fat_write     (int32_t handle, const uint8_t *buf, uint32_t len);
extern int32_t  fat_flush     (int32_t handle);
extern int32_t  fat_flen      (int32_t handle);
extern fsStatus fat_seek      (int32_t handle, uint32_t pos);

/* FAT File Maintenance Routines */
extern fsStatus fat_delete_l  (const char *fn, fsFAT_Volume *vol);
extern fsStatus fat_delete    (const char *fn, const char *options, fsFAT_Volume *vol);
extern fsStatus fat_ffind     (const char *fn, fsFileInfo *info, fsFAT_Volume *vol);
extern fsStatus fat_rename    (const char *path, const char *newname, fsFAT_Volume *vol);
extern fsStatus fat_attrib    (fsFAT_Volume *vol, const char *path, uint32_t attrib);
extern fsStatus fat_vol       (char *label, uint32_t *serial, fsFAT_Volume *vol);

/* FAT Directory Support Routines */
extern fsStatus fat_mkdir     (const char *path, fsFAT_Volume *vol);
extern fsStatus fat_rmdir     (const char *path, const char *options, fsFAT_Volume *vol);
extern fsStatus fat_chdir     (const char *path, fsFAT_Volume *vol);
extern fsStatus fat_pwd       (      char *path, uint32_t len, fsFAT_Volume *vol);

/* FAT Utility Routines */
extern int64_t  fat_free      (fsFAT_Volume *vol);
extern fsStatus fat_format    (fsFAT_Volume *vol, const char *param);
extern fsStatus fat_media     (fsFAT_Volume *vol);
extern fsStatus fat_info      (fsDriveInfo *info, fsFAT_Volume *vol);

/* FAT Journal System Routines */
extern uint32_t fat_jour_init (fsFAT_Volume *vol);
extern uint32_t fat_jour_prep (fsFAT_Volume *vol);

/* FileSystem Retarget Routines */
extern int      __sys_open    (const char *fname, int openmode);
extern int      __sys_close   (int handle);
extern int      __sys_read    (int handle, uint8_t *buf, uint32_t len);
extern int      __sys_write   (int handle, const uint8_t *buf, uint32_t len);
extern int      __sys_ensure  (int handle);
extern int      __sys_seek    (int handle, uint32_t pos);
extern long     __sys_flen    (int handle);

/* RTOS abstraction for FileSystem */
extern FS_MUTEX fs_mutex_new     (const void *arg);
extern uint32_t fs_mutex_acquire (FS_MUTEX mutex);
extern uint32_t fs_mutex_release (FS_MUTEX mutex);
extern uint32_t fs_mutex_delete  (FS_MUTEX mutex);

extern uint32_t fs_ms_rtos_tick;
extern uint32_t fs_get_rtos_tick_freq (void);
extern uint32_t fs_set_rtos_delay (uint32_t millisec);
extern uint32_t fs_get_sys_tick (void);
extern uint32_t fs_get_sys_tick_us (uint32_t microsec);

/* FAT File Handle array definition */
extern fsFAT_Handle  fs_fat_fh[];
extern uint8_t const fs_fat_fh_cnt;

/* EFS File Handle array definition */
extern fsEFS_Handle  fs_efs_fh[];
extern uint8_t const fs_efs_fh_cnt;

/* Drives array definition */
extern FS_DEV        fs_DevPool[];
extern uint8_t const fs_ndrv;

#ifdef RTE_FileSystem_Debug
/* Media objects */
extern MC_MCI fs_mc0_mci;
extern MC_SPI fs_mc0_spi;
extern MC_MCI fs_mc1_mci;
extern MC_SPI fs_mc1_spi;

extern NAND_FTL_DEV fs_nand0_handle;
extern NAND_FTL_DEV fs_nand1_handle;

extern RAM_DEV fs_ram0_dev;
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RL_FS_LIB_H */
