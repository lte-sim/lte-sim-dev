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


#include "dl-exp-packet-scheduler.h"
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
#include "../../../flows/QoS/QoSForEXP.h"
#include "../../../flows/MacQueue.h"

DL_EXP_PacketScheduler::DL_EXP_PacketScheduler()
{
  SetMacEntity (0);
  CreateFlowsToSchedule ();
}

DL_EXP_PacketScheduler::~DL_EXP_PacketScheduler()
{
  Destroy ();
}

void
DL_EXP_PacketScheduler::DoSchedule ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << "Start DL packet scheduler for node "
			<< GetMacEntity ()->GetDevice ()->GetIDNetworkNode()<< std::endl;
#endif

  UpdateAverageTransmissionRate ();
  CheckForDLDropPackets ();
  SelectFlowsToSchedule ();
  ComputeAW ();

  if (GetFlowsToSchedule ()->size() == 0)
	{}
  else
	{
	  RBsAllocation ();
	}

  StopSchedule ();
}

double
DL_EXP_PacketScheduler::ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel)
{
  /*
   * For the EXP scheduler the metric is computed
   * as follows:
   *                -log(dropProbability)/targetDelay * HOL - aW
   * metric = exp ( --------------------------------------------) * availableRate/averageRate
   *                            1 - sqrt (aW)
   */

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

     QoSForEXP *qos = (QoSForEXP*) bearer->GetQoSParameters ();

	 double HOL = bearer->GetHeadOfLinePacketDelay ();
     double alfa = -log10(qos->GetDropProbability()) / qos->GetMaxDelay ();
     double avgAW = GetAW ();
	 double AW = alfa * HOL;

	  if (AW < 0.000001)
		  AW=0;

	  double AW_avgAW = AW - avgAW;

	  if (AW_avgAW < 0.000001)
		  AW_avgAW=0;

	  metric = exp ( AW_avgAW /
					 (1 + sqrt (GetAW ())) )
	               *
	               ((spectralEfficiency * 180000.)
	               /
	               bearer->GetAverageTransmissionRate());
    }

  return metric;
}


void
DL_EXP_PacketScheduler::ComputeAW ()
{
  FlowsToSchedule *flowsToSchedule = GetFlowsToSchedule ();
  FlowsToSchedule::iterator iter;
  FlowToSchedule *flow;

#ifdef SCHEDULER_DEBUG
  std::cout << "ComputeAW" << std::endl;
#endif

  m_aW = 0;
  int nbFlow = 0;
  for (iter = flowsToSchedule->begin(); iter != flowsToSchedule->end(); iter++)
    {
	  flow = *iter;
	  RadioBearer *bearer = flow->GetBearer ();

	  if (bearer->HasPackets ())
	    {
		  if ((bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_TRACE_BASED)
			  ||
			  (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_VOIP))
			{
			  QoSForEXP *qos = (QoSForEXP*) bearer->GetQoSParameters ();
			  double aWi =  - (log10 (qos->GetDropProbability())
							  /
							  qos->GetMaxDelay ());
			  double HOL = bearer->GetHeadOfLinePacketDelay ();
			  aWi = aWi * HOL;
			  m_aW += aWi;
			  nbFlow++;
			}
	    }
    }

  m_aW = m_aW/nbFlow;

  if (m_aW < 0.000001)
	  m_aW=0;
}

double
DL_EXP_PacketScheduler::GetAW (void) const
{
  return m_aW;
}
