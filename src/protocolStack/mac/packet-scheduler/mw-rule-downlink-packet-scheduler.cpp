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

#include "mw-rule-downlink-packet-scheduler.h"
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

MwRulePacketScheduler::MwRulePacketScheduler()
{
  SetMacEntity (0);
  CreateFlowsToSchedule ();
}

MwRulePacketScheduler::~MwRulePacketScheduler()
{
  Destroy ();
}


double
MwRulePacketScheduler::ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel)
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

#ifdef SCHEDULER_DEBUG
	  std::cout << "\t\t real time flow: HOL = " << HOL << ", target delay = " << targetDelay;
#endif

	  //compute sum of HOL and average spectral efficiency
//#ifdef SCHEDULER_DEBUG
//	  std::cout << "\n\t\t\t compute sum HOL for all real time flows"<< std::endl;
//#endif

	  double sumHOL = 0;
	  int nbFlows = 0;
	  FlowsToSchedule *flowsToSchedule = GetFlowsToSchedule ();
	  FlowsToSchedule::iterator iter;
	  FlowToSchedule *flow;

	  for (iter = flowsToSchedule->begin (); iter != flowsToSchedule->end (); iter++)
	    {
		  flow = (*iter);
		  if (!flow->GetBearer ()->HasPackets ())
		    {}
		  else
			{
			  if ((flow->GetBearer ()->GetApplication ()->GetApplicationType ()
					  != Application::APPLICATION_TYPE_INFINITE_BUFFER)
			   		  &&
			          (flow->GetBearer ()->GetApplication ()->GetApplicationType ()
			        		  != Application::APPLICATION_TYPE_CBR))
			    {
			      sumHOL += flow->GetBearer ()->GetHeadOfLinePacketDelay ();
			      nbFlows++;

//#ifdef SCHEDULER_DEBUG
//	              std::cout << "\t\t\t --> HOL for flow " << flow->GetBearer ()->GetApplication ()->GetApplicationID ()
//	            		  << " = " << flow->GetBearer ()->GetHeadOfLinePacketDelay ()<< std::endl;
//#endif
			    }
			  else
			    {
//#ifdef SCHEDULER_DEBUG
//	              std::cout << "\t\t\t --> non real time flow, go on! " << std::endl;
//#endif
			    }
			}
	    }


	  //COMPUTE METRIC USING EXP RULE:
	  double numerator = (6/targetDelay) * HOL;
	  double denominator = (1 + sqrt (sumHOL/nbFlows));
	  double weight = (spectralEfficiency * 180000.)
				      /
	    	          bearer->GetAverageTransmissionRate();

	  metric = (exp (numerator / denominator)) * weight;

#ifdef SCHEDULER_DEBUG
	  std::cout << " --> metric = " << metric << std::endl;
#endif
	}

  return metric;
}

