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
 * Driver:       Driver_ETH_PHYn (default: Driver_ETH_PHY0)
 * Project:      Ethernet Physical Layer Transceiver (PHY)
 *               Driver for DM8203EP
 * -----------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                     Value
 *   ---------------------                     -----
 *   Connect to hardware via Driver_ETH_PHY# = n (default: 0)
 * -------------------------------------------------------------------- */

#include "PHY_DM8203EP.h"

#define ARM_ETH_PHY_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(6,2) /* driver version */


#ifndef ETH_PHY_NUM
#define ETH_PHY_NUM     0        /* Default driver number */
#endif

#ifndef ETH_PHY_ADDR
#define ETH_PHY_ADDR    0x00     /* Default device address */
#endif


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_ETH_PHY_API_VERSION,
  ARM_ETH_PHY_DRV_VERSION
};

/* Ethernet PHY control structure */
static PHY_CTRL PHY = { NULL, NULL, 0, 0, 0 };

#define DM8203EP_CONV(phy_addr, reg_addr)          \
        (  ((uint8_t)((phy_addr) & 0x03U) << 3U)   \
         | ((uint8_t)((reg_addr) & 0xE0U) >> 5U)   \
        ), ((uint8_t)((reg_addr) & 0x1FU))


/**
  \fn          ARM_DRIVER_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          int32_t Initialize (ARM_ETH_PHY_Read_t  fn_read,
                                   ARM_ETH_PHY_Write_t fn_write)
  \brief       Initialize Ethernet PHY Device.
  \param[in]   fn_read   Pointer to \ref ARM_ETH_MAC_PHY_Read
  \param[in]   fn_write  Pointer to \ref ARM_ETH_MAC_PHY_Write
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write) {

  if ((fn_read == NULL) || (fn_write == NULL)) { return ARM_DRIVER_ERROR_PARAMETER; }

  if ((PHY.flags & PHY_INIT) == 0U) {
    /* Register PHY read/write functions. */
    PHY.reg_rd = fn_read;
    PHY.reg_wr = fn_write;

    PHY.bmcr   = 0U;
    PHY.flags  = PHY_INIT;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Uninitialize (void)
  \brief       De-initialize Ethernet PHY Device.
  \return      \ref execution_status
*/
static int32_t Uninitialize (void) {

  PHY.reg_rd = NULL;
  PHY.reg_wr = NULL;
  PHY.bmcr   = 0U;
  PHY.flags  = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet PHY Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {
  uint16_t val;

  switch ((int32_t)state) {
    case ARM_POWER_OFF:
      if ((PHY.flags & PHY_INIT) == 0U) {
        /* Initialize must provide register access function pointers */
        return ARM_DRIVER_ERROR;
      }

      /* Per Port Control/Status Index Register */
      PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_P_INDEX), 2);

      /* Initialize Basic Mode Control Register */
      PHY.flags &= ~PHY_POWER;
      PHY.bmcr   =  BMCR_POWER_DOWN;
      return (PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_BMCR), PHY.bmcr));

    case ARM_POWER_FULL:
      if ((PHY.flags & PHY_INIT) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if (PHY.flags & PHY_POWER) {
        return ARM_DRIVER_OK;
      }

      /* Check Device Identification. */
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, REG_VID_L), &val);
      if (val != (PHY_VID & 0xFF)) {
        return ARM_DRIVER_ERROR_UNSUPPORTED;  /* Invalid VID */
      }
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, REG_VID_H), &val);
      if (val != (PHY_VID >> 8)) {
        return ARM_DRIVER_ERROR_UNSUPPORTED;  /* Invalid VID */
      }

      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, REG_PID_L), &val);
      if (val != (PHY_PID & 0xFF)) {
        return ARM_DRIVER_ERROR_UNSUPPORTED;  /* Invalid PID */
      }
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, REG_PID_H), &val);
      if (val != (PHY_PID >> 8)) {
        return ARM_DRIVER_ERROR_UNSUPPORTED;  /* Invalid PID */
      }

      /* SWITCH Address Table Aging set to 256 sec */
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, REG_SWITCHCR), &val);
      val = ((val & ~SWITCHCR_AGE_MASK) | SWITCHCR_AGE_256S);
      PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_SWITCHCR),  val);

      /* Per Port Control/Status Index Register */
      PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_P_INDEX), 2);

      /* Initialize Basic Mode Control Register */
      PHY.bmcr = 0U;
      if (PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_BMCR), PHY.bmcr) != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
      }
      PHY.flags |=  PHY_POWER;

      return ARM_DRIVER_OK;

    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
}

/**
  \fn          int32_t SetInterface (uint32_t interface)
  \brief       Set Ethernet Media Interface.
  \param[in]   interface  Media Interface type
  \return      \ref execution_status
*/
static int32_t SetInterface (uint32_t interface) {
  ((void)interface);
  return( ARM_DRIVER_OK );
}

/**
  \fn          int32_t SetMode (uint32_t mode)
  \brief       Set Ethernet PHY Device Operation mode.
  \param[in]   mode  Operation Mode
  \return      \ref execution_status
*/
static int32_t SetMode (uint32_t mode) {
  uint16_t val;

  if ((PHY.flags & PHY_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  val = PHY.bmcr & BMCR_POWER_DOWN;

  switch (mode & ARM_ETH_PHY_SPEED_Msk) {
    case ARM_ETH_PHY_SPEED_10M:
      break;
    case ARM_ETH_PHY_SPEED_100M:
      val |= BMCR_SPEED_SEL;
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  switch (mode & ARM_ETH_PHY_DUPLEX_Msk) {
    case ARM_ETH_PHY_DUPLEX_HALF:
      break;
    case ARM_ETH_PHY_DUPLEX_FULL:
      val |= BMCR_DUPLEX;
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  if (mode & ARM_ETH_PHY_AUTO_NEGOTIATE) {
    val |= BMCR_ANEG_EN;
  }

  if (mode & ARM_ETH_PHY_LOOPBACK) {
    val |= BMCR_LOOPBACK;
  }

  if (mode & ARM_ETH_PHY_ISOLATE) {
    val |= BMCR_ISOLATE;
  }

  PHY.bmcr = val;

  return (PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_BMCR), PHY.bmcr));
}

/**
  \fn          ARM_ETH_LINK_STATE GetLinkState (void)
  \brief       Get Ethernet PHY Device Link state.
  \return      current link status \ref ARM_ETH_LINK_STATE
*/
static ARM_ETH_LINK_STATE GetLinkState (void) {
  ARM_ETH_LINK_STATE state;
  uint16_t           val0 = 0U;
  uint16_t           val1 = 0U;

  #ifdef _PHY_DEBUG_
  {        uint16_t  val, i;
    static uint16_t  reg_14H = 0;
    static uint32_t  mib_count[10];

    for(i = 0;  i < 9;  i++) {
      PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, 0x80), i);
      do{ PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, 0x80), &val); }while(!(val & 0x80));
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, 0x81), &val);  mib_count[i]  =  ((uint32_t)val);
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, 0x82), &val);  mib_count[i] |= (((uint32_t)val) << 8);
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, 0x83), &val);  mib_count[i] |= (((uint32_t)val) << 16);
      PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, 0x84), &val);  mib_count[i] |= (((uint32_t)val) << 24);
    }
    PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, 0x14), &reg_14H);
  }
  #endif

  if (PHY.flags & PHY_POWER) {
    PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_P_INDEX), 0);
    PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, REG_P_STUS), &val0);
    PHY.reg_wr(DM8203EP_CONV(ETH_PHY_ADDR, REG_P_INDEX), 1);
    PHY.reg_rd(DM8203EP_CONV(ETH_PHY_ADDR, REG_P_STUS), &val1);
  }
  state = ((val0 | val1) & P_STUS_LINK) ? ARM_ETH_LINK_UP : ARM_ETH_LINK_DOWN;

  return (state);
}

/**
  \fn          ARM_ETH_LINK_INFO GetLinkInfo (void)
  \brief       Get Ethernet PHY Device Link information.
  \return      current link parameters \ref ARM_ETH_LINK_INFO
*/
static ARM_ETH_LINK_INFO GetLinkInfo (void) {
  ARM_ETH_LINK_INFO info;

  info.speed  = ARM_ETH_SPEED_100M;
  info.duplex = ARM_ETH_DUPLEX_FULL;

  return (info);
}


/* PHY Driver Control Block */
extern
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM);
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM) = {
  GetVersion,
  Initialize,
  Uninitialize,
  PowerControl,
  SetInterface,
  SetMode,
  GetLinkState,
  GetLinkInfo
};
