/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010,2011,2012,2013 TELEMATICS LAB, Politecnico di Bari
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


#include "dl-mlwdf-packet-scheduler.h"
#include "../mac-entity.h"
#include "../../packet/Packet.h"
#include "../../packet/packet-burst.h"
#include "../../../device/NetworkNode.h"
#include "../../../flows/radio-bearer.h"
#include "../../../protocolStack/rrc/rrc-entity.h"
#include "../../../flows/application/Application.h"
#include "../../../device/ENodeB.h"
#include "../../../protocolStack/mac/AMCModule.h"
#include "../../../phy/lte-phy.h"
#include "../../../core/spectrum/bandwidth-manager.h"
#include "../../../flows/QoS/QoSForM_LWDF.h"
#include "../../../flows/MacQueue.h"

DL_MLWDF_PacketScheduler::DL_MLWDF_PacketScheduler()
{
  SetMacEntity (0);
  CreateFlowsToSchedule ();
}

DL_MLWDF_PacketScheduler::~DL_MLWDF_PacketScheduler()
{
  Destroy ();
}


void
DL_MLWDF_PacketScheduler::DoSchedule ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << "Start DL packet scheduler for node "
			<< GetMacEntity ()->GetDevice ()->GetIDNetworkNode()<< std::endl;
#endif

  UpdateAverageTransmissionRate ();
  CheckForDLDropPackets ();
  SelectFlowsToSchedule ();

  if (GetFlowsToSchedule ()->size() == 0)
	{}
  else
	{
	  RBsAllocation ();
	}

  StopSchedule ();
}

double
DL_MLWDF_PacketScheduler::ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel)
{
  /*
   * For the M-LWDF scheduler the metric is computed
   * as follows:
   *
   * metric = -log(dropProbability)/targetDelay *
   *  			* HOL * availableRate/averageRate
   */

  double metric;

  if ((bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_INFINITE_BUFFER)
	  ||
	  (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_CBR))
    {
	  metric = (spectralEfficiency * 180000.)
				/
	    	    bearer->GetAverageTransmissionRate();

#ifdef SCHEDULER_DEBUG
	  std::cout << "METRIC: " << bearer->GetApplication ()->GetApplicationID ()
			 << " " << spectralEfficiency
			 << " " << bearer->GetAverageTransmissionRate ()
			 << " --> " << metric
			 << std::endl;
#endif

    }
  else
    {

     QoSForM_LWDF *qos = (QoSForM_LWDF*) bearer->GetQoSParameters ();

     double a = (-log10 (qos->GetDropProbability())) / qos->GetMaxDelay ();
     double HOL = bearer->GetHeadOfLinePacketDelay ();

	 metric = (a * HOL)
			 *
			 ((spectralEfficiency * 180000.)
			 /
			 bearer->GetAverageTransmissionRate ());

#ifdef SCHEDULER_DEBUG
	 std::cout << "METRIC: " << bearer->GetApplication ()->GetApplicationID ()
			 << " " << a
			 << " " << Simulator::Init()->Now()
			 << " " << bearer->GetMacQueue()->Peek().GetTimeStamp()
			 << " " << HOL
			 << " " << spectralEfficiency
			 << " " << bearer->GetAverageTransmissionRate ()
			 << " --> " << metric
			 << std::endl;
#endif

    }

  return metric;
}

