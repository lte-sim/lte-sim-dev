/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, Politecnico di Bari
 *
 * This file is part of LTE-Sim
 *
 * LTE-Sim is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation;
 *
 * LTE-Sim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LTE-Sim; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Giuseppe Piro <g.piro@poliba.it>
 */


#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <iostream>
#include <string>


/* path */
static std::string path ("/home/alvin/Research/SmallCell/lte-sim-dev/");


/* tracing */
static bool _APP_TRACING_ = true;
static bool _RLC_TRACING_ = false;
static bool _MAC_TRACING_ = false;
static bool _PHY_TRACING_ = false;


/* activate uplink */
static bool UPLINK = true;



/* tests */
static bool _TEST_BLER_ = false;

/* channel model type*/
static bool _simple_jakes_model_ = true;
static bool _PED_A_ = false;
static bool _PED_B_ = false;
static bool _VEH_A_ = false;
static bool _VEH_B_ = false;
static bool _channel_TU_ = true;
static bool _channel_AWGN_ = false;

/* debugging */
//#define APPLICATION_DEBUG
//#define BEARER_DEBUG
//#define RLC_DEBUG
//#define MAC_QUEUE_DEBUG
//#define FLOW_MANAGER_DEBUG
//#define FRAME_MANAGER_DEBUG
//#define ENODEB_DEBUG
//#define UE_DEBUG
//#define PHY_DEBUG
//#define SINR_DEBUG
//#define BLER_DEBUG
//#define MOBILITY_DEBUG
//#define MOBILITY_DEBUG_TAB
//#define HANDOVER_DEBUG
//#define TRANSMISSION_DEBUG
//#define CHANNEL_REALIZATION_DEBUG
//#define TEST_DEVICE_ON_CHANNEL
//#define TEST_START_APPLICATION
//#define TEST_ENQUEUE_PACKETS
//#define TEST_PROPAGATION_LOSS_MODEL
//#define INTERFERENCE_DEBUG
//#define TEST_CQI_FEEDBACKS
//#define SCHEDULER_DEBUG
//#define AMC_MAPPING
//#define PLOT_USER_POSITION
//#define TEST_UL_SINR
//#define TEST_DL_SINR


#endif /* PARAMETERS_H_ */
                           
