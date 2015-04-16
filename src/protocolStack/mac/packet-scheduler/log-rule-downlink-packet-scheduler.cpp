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

#include "log-rule-downlink-packet-scheduler.h"
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
#include "../../../core/idealMessages/ideal-control-messages.h"
#include "../../../flows/QoS/QoSParameters.h"
#include "../../../flows/MacQueue.h"

LogRuleDownlinkPacketScheduler::LogRuleDownlinkPacketScheduler()
{
  SetMacEntity (0);
  CreateFlowsToSchedule ();
}

LogRuleDownlinkPacketScheduler::~LogRuleDownlinkPacketScheduler()
{
  Destroy ();
}

void
LogRuleDownlinkPacketScheduler::DoSchedule ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << "Start LOG RULE packet scheduler for node "
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
LogRuleDownlinkPacketScheduler::ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel)
{
  double metric;

  if ((bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_INFINITE_BUFFER)
	  ||
	  (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_CBR))
	{
	  metric = (spectralEfficiency * 180000.)
				/
				bearer->GetAverageTransmissionRate();
	}
  else
	{
	   QoSParameters *qos = bearer->GetQoSParameters ();
	   double HOL = bearer->GetHeadOfLinePacketDelay ();
	   double targetDelay = qos->GetMaxDelay ();

	   //COMPUTE METRIC USING EXP RULE:
	   double logTerm = log (1.1 + ( (5 * HOL) / targetDelay ));
	   double weight = (spectralEfficiency * 180000.)
		  		       /
	    	           bearer->GetAverageTransmissionRate();

  	   metric = logTerm * weight;
	}

  return metric;
}

