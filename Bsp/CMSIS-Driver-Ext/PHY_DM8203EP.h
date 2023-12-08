/*
 * Copyright (c) 2013-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -----------------------------------------------------------------------
 *
 * Project:      Ethernet Physical Layer Transceiver (PHY)
 *               Definitions for DM8203EP
 * -------------------------------------------------------------------- */

#ifndef __PHY_DM8203EP_H
#define __PHY_DM8203EP_H

#include "Driver_ETH_PHY.h"


/*----------------------------------------------------------------------------*/
/* Basic Registers */
#define REG_BMCR            0x00        /* Basic Mode Control Register       */
#define REG_BMSR            0x01        /* Basic Mode Status Register        */
#define REG_PHYIDR1         0x02        /* PHY Identifier 1                  */
#define REG_PHYIDR2         0x03        /* PHY Identifier 2                  */
#define REG_ANAR            0x04        /* Auto-Negotiation Advertisement    */
#define REG_ANLPAR          0x05        /* Auto-Neg. Link Partner Abitily    */
#define REG_ANER            0x06        /* Auto-Neg. Expansion Register      */

/* Extended Registers */
#define REG_VID_L           0x28        /* Vendor ID  Low  Byte              */
#define REG_VID_H           0x29        /* Vendor ID  High Byte              */
#define REG_PID_L           0x2A        /* Product ID Low  Byte              */
#define REG_PID_H           0x2B        /* Product ID High Byte              */
#define REG_SWITCHCR        0x52        /* SWITCH Control Register           */
#define REG_P_INDEX         0x60        /* Per Port Index Register           */
#define REG_P_STUS          0x62        /* Per Port Status Data Register     */


/*----------------------------------------------------------------------------*/
/* Basic Mode Control Register */
#define BMCR_RESET          0x8000      /* Software Reset                    */
#define BMCR_LOOPBACK       0x4000      /* Loopback mode                     */
#define BMCR_SPEED_SEL      0x2000      /* Speed Select (1=100Mb/s)          */
#define BMCR_ANEG_EN        0x1000      /* Auto Negotiation Enable           */
#define BMCR_POWER_DOWN     0x0800      /* Power Down                        */
#define BMCR_ISOLATE        0x0400      /* Isolate Media interface           */
#define BMCR_REST_ANEG      0x0200      /* Restart Auto Negotiation          */
#define BMCR_DUPLEX         0x0100      /* Duplex Mode (1=Full duplex)       */
#define BMCR_COL_TEST       0x0080      /* Collision Test                    */

/* Basic Mode Status Register */
#define BMSR_100B_T4        0x8000      /* 100BASE-T4 Capable                */
#define BMSR_100B_TX_FD     0x4000      /* 100BASE-TX Full Duplex Capable    */
#define BMSR_100B_TX_HD     0x2000      /* 100BASE-TX Half Duplex Capable    */
#define BMSR_10B_T_FD       0x1000      /* 10BASE-T Full Duplex Capable      */
#define BMSR_10B_T_HD       0x0800      /* 10BASE-T Half Duplex Capable      */
#define BMSR_MF_PRE_SUP     0x0040      /* Preamble suppression Capable      */
#define BMSR_ANEG_COMPL     0x0020      /* Auto Negotiation Complete         */
#define BMSR_REM_FAULT      0x0010      /* Remote Fault                      */
#define BMSR_ANEG_ABIL      0x0008      /* Auto Negotiation Ability          */
#define BMSR_LINK_STAT      0x0004      /* Link Status (1=established)       */
#define BMSR_JABBER_DET     0x0002      /* Jaber Detect                      */
#define BMSR_EXT_CAPAB      0x0001      /* Extended Capability               */

/* PHY Identifier Registers */
#define PHY_ID1             0x0181      /* Device Identifier MSB             */
#define PHY_ID2             0xB8B0      /* Device Identifier LSB             */
#define PHY_VID             0x0A46      /* Device Vendor  ID                 */
#define PHY_PID             0x8203      /* Device Product ID                 */

/* SWITCH Control Register */
#define SWITCHCR_MEM_BIST   0x0080      /* Address Table Memory Test BIST    */
#define SWITCHCR_RST_SW     0x0040      /* Reset Switch Core                 */
#define SWITCHCR_RST_ANLG   0x0020      /* Reset Analog PHY Core             */
#define SWITCHCR_SNF_PORT_M 0x0018      /* Sniffer Port Number: Bits Mask    */
#define SWITCHCR_SNF_PORT_0 0x0000      /* Sniffer Port Number: Port 0       */
#define SWITCHCR_SNF_PORT_1 0x0008      /* Sniffer Port Number: Port 1       */
#define SWITCHCR_SNF_PORT_2 0x0010      /* Sniffer Port Number: Port 2       */
#define SWITCHCR_CRC_DIS    0x0004      /* CRC Checking Disable              */
#define SWITCHCR_AGE_MASK   0x0003      /* Address Table Aging: Bits Mask    */
#define SWITCHCR_AGE_NONE   0x0000      /* Address Table Aging: no aging     */
#define SWITCHCR_AGE_64S    0x0001      /* Address Table Aging: 64 ¡À 32 sec  */
#define SWITCHCR_AGE_128S   0x0002      /* Address Table Aging: 128 ¡À 64 sec */
#define SWITCHCR_AGE_256S   0x0003      /* Address Table Aging: 256 ¡À128 sec */

/* Per Port Status Data Register */
#define P_STUS_SPEED        0x0004      /* 0: 10Mbps, 1:100Mbps              */
#define P_STUS_FDX          0x0002      /* 0: half-duplex, 1:full-duplex     */
#define P_STUS_LINK         0x0001      /* 0: link fail, 1: link OK          */


/*----------------------------------------------------------------------------*/
/* PHY Driver State Flags */
#define PHY_INIT            0x01U       /* Driver initialized                */
#define PHY_POWER           0x02U       /* Driver power is on                */

/* PHY Driver Control Structure */
typedef struct phy_ctrl {
  ARM_ETH_PHY_Read_t  reg_rd;           /* PHY register read function        */
  ARM_ETH_PHY_Write_t reg_wr;           /* PHY register write function       */
  uint16_t            bmcr;             /* BMCR register value               */
  uint8_t             flags;            /* Control flags                     */
  uint8_t             rsvd;             /* Reserved                          */
} PHY_CTRL;

#endif /* __PHY_DM8203EP_H */
