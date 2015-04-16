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

#include "exp-rule-downlink-packet-scheduler.h"
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

ExpRuleDownlinkPacketScheduler::ExpRuleDownlinkPacketScheduler()
{
  SetMacEntity (0);
  CreateFlowsToSchedule ();
}

ExpRuleDownlinkPacketScheduler::~ExpRuleDownlinkPacketScheduler()
{
  Destroy ();
}



void
ExpRuleDownlinkPacketScheduler::DoSchedule ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << "Start EXP RULE packet scheduler for node "
			<< GetMacEntity ()->GetDevice ()->GetIDNetworkNode()<< std::endl;
#endif

  UpdateAverageTransmissionRate ();
  CheckForDLDropPackets ();
  SelectFlowsToSchedule ();
  ComputeAverageOfHOLDelays ();

  if (GetFlowsToSchedule ()->size() == 0)
	{}
  else
	{
	  RBsAllocation ();
	}

  StopSchedule ();
}


void
ExpRuleDownlinkPacketScheduler::ComputeAverageOfHOLDelays (void)
{
  double avgHOL = 0.;
  int nbFlows = 0;
  FlowsToSchedule *flowsToSchedule = GetFlowsToSchedule ();
  FlowsToSchedule::iterator iter;
  FlowToSchedule *flow;

  for (iter = flowsToSchedule->begin (); iter != flowsToSchedule->end (); iter++)
	{
	  flow = (*iter);
	  if (flow->GetBearer ()->HasPackets ())
	    {
		  if ((flow->GetBearer ()->GetApplication ()->GetApplicationType ()
				  == Application::APPLICATION_TYPE_TRACE_BASED)
				  ||
				  (flow->GetBearer ()->GetApplication ()->GetApplicationType ()
						  == Application::APPLICATION_TYPE_VOIP))
			{
			  avgHOL += flow->GetBearer ()->GetHeadOfLinePacketDelay ();
			  nbFlows++;
			}
		}
	}

  m_avgHOLDelayes = avgHOL/nbFlows;
}


double
ExpRuleDownlinkPacketScheduler::ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel)
{
#ifdef SCHEDULER_DEBUG
	std::cout << "\t ComputeSchedulingMetric for flow "
			<< bearer->GetApplication ()->GetApplicationID () << std::endl;
#endif

  double metric;

  if ((bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_INFINITE_BUFFER)
	  ||
	  (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_CBR))
	{
	  metric = (spectralEfficiency * 180000.)
				/
				bearer->GetAverageTransmissionRate();

#ifdef SCHEDULER_DEBUG
	std::cout << "\t\t non real time flow: metric = " << metric << std::endl;
#endif

	}
  else
	{
	  QoSParameters *qos = bearer->GetQoSParameters ();
	  double HOL = bearer->GetHeadOfLinePacketDelay ();
	  double targetDelay = qos->GetMaxDelay ();

	  //COMPUTE METRIC USING EXP RULE:
	  double numerator = (6/targetDelay) * HOL;
	  double denominator = (1 + sqrt (m_avgHOLDelayes));
	  double weight = (spectralEfficiency * 180000.)
				      /
	    	          bearer->GetAverageTransmissionRate();

	  metric = (exp (numerator / denominator)) * weight;

#ifdef SCHEDULER_DEBUG
	  std::cout << "\t\t real time flow: "
			  "\n\t\t\t HOL = " << HOL <<
			  "\n\t\t\t target delay = " << targetDelay <<
			  "\n\t\t\t m_avgHOLDelayes = " << m_avgHOLDelayes <<
			  "\n\t\t\t spectralEfficiency = " << spectralEfficiency <<
			  "\n\t\t\t avg rate = " << bearer->GetAverageTransmissionRate() <<
	          "\n\t\t\t numerator " << numerator <<
              "\n\t\t\t denominator " << denominator <<
              "\n\t\t\t weight = " << weight <<
	          "\n\t\t --> metric = " << metric << std::endl;
#endif
	}

  return metric;
}

