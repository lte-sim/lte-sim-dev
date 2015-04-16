/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010,2011,2012,2013 TELEMATICS LAB, Politecnico di Bari
 *
 * This file is part of LTE-Sim
 *
 * LTE-Sim is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation;
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

#include "dl-fls-packet-scheduler.h"
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
#include "../../../flows/QoS/QoSForFLS.h"
#include "../../../flows/MacQueue.h"
#include "../../../utility/eesm-effective-sinr.h"


DL_FLS_PacketScheduler::DL_FLS_PacketScheduler()
{
  SetMacEntity (0);
  CreateFlowsToSchedule ();
  m_runControlLaw = true;
  m_subFrameCounter = 0;
  m_startPrbAllocation = 0;
  m_freePrb = true;
  m_rtAllocation = false;
  m_lowerLevelSchedulerType = DL_FLS_PacketScheduler::PROPORTIONAL_FAIR;
}

DL_FLS_PacketScheduler::~DL_FLS_PacketScheduler()
{
  Destroy ();
}

void
DL_FLS_PacketScheduler::SetLowerLevelSchedulerType (LowerLevelSchedulerType type)
{
  m_lowerLevelSchedulerType = type;
}

DL_FLS_PacketScheduler::LowerLevelSchedulerType
DL_FLS_PacketScheduler::GetLowerLevelSchedulerType (void) const
{
  return m_lowerLevelSchedulerType;
}

void
DL_FLS_PacketScheduler::DoSchedule ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << "Start DL packet (FLS) scheduler for node "
			<< GetMacEntity ()->GetDevice ()->GetIDNetworkNode()<< std::endl;
#endif

  UpdateDataToTransmitAndAverageDataRate ();
  CheckForDLDropPackets ();
  m_startPrbAllocation = 0;
  m_freePrb = true;
  m_rtAllocation = false;

  m_subFrameCounter ++;
  if (m_runControlLaw)
	{
	  RunControlLaw ();
	}
  if (m_subFrameCounter == 10)
	{
	  m_runControlLaw = true;
	  m_subFrameCounter = 0;
	}

  Select_RT_FlowsToSchedule ();

  if (GetFlowsToSchedule ()->size() != 0)
    {
	  m_rtAllocation = true;
	  m_freePrb = false;
	  RBsAllocation ();
	  StopSchedule ();
    }
  if (m_freePrb)
    {
	  m_rtAllocation = false;
	  Select_NRT_FlowsToSchedule ();
	  if (GetFlowsToSchedule ()->size() != 0)
	    {
		  RBsAllocation ();
		  StopSchedule ();
	    }
	  else
	    {

#ifdef SCHEDULER_DEBUG
			  std::cout << " FLS_DEBUG: RBs Have NOT BEEN allocated"<< std::endl;
#endif
	    }
    }

}

void
DL_FLS_PacketScheduler::DoStopSchedule (void)
{
#ifdef SCHEDULER_DEBUG
  std::cout << "\t Do Stop Schedule (FLS) Creating Packet Burst" << std::endl;
#endif

  PacketBurst* pb = new PacketBurst ();

  //Create Packet Burst
  FlowsToSchedule *flowsToSchedule = GetFlowsToSchedule ();

  for (FlowsToSchedule::iterator it = flowsToSchedule->begin (); it != flowsToSchedule->end (); it++)
    {
	  FlowToSchedule *flow = (*it);

	  int availableBytes = flow->GetAllocatedBits ()/8;

	  if (availableBytes > 0)
	    {
		  flow->GetBearer ()->UpdateTransmittedBytes (availableBytes);
#ifdef SCHEDULER_DEBUG
	      std::cout << "\t  --> add packets for flow "
	    		  << flow->GetBearer ()->GetApplication ()->GetApplicationID () << std::endl;
#endif

	      //flow->GetBearer ()->GetMacQueue ()->PrintQueueInfo ();
	      RlcEntity *rlc = flow->GetBearer ()->GetRlcEntity ();
	      PacketBurst* pb2 = rlc->TransmissionProcedure (availableBytes);


#ifdef SCHEDULER_DEBUG
	      std::cout << "\t\t  nb of packets: " << pb2->GetNPackets () << std::endl;
#endif
	      if (pb2->GetNPackets () > 0)
	        {
	    	  std::list<Packet*> packets = pb2->GetPackets ();
	    	  std::list<Packet* >::iterator it;
	    	  for (it = packets.begin (); it != packets.end (); it++)
	    	    {
#ifdef SCHEDULER_DEBUG
	    		  std::cout << "\t\t  added packet of bytes " << (*it)->GetSize () << std::endl;
	    		  //(*it)->Print ();
#endif

	    		  Packet *p = (*it);
	    		  pb->AddPacket (p->Copy ());
	    	    }
	        }
	      delete pb2;
	    }
	  else
	    {}
    }
  //SEND PACKET BURST
#ifdef SCHEDULER_DEBUG
  if (pb->GetNPackets () == 0)
    std::cout << "\t Send only reference symbols" << std::endl;
#endif

  GetMacEntity ()->GetDevice ()->SendPacketBurst (pb);
}

double
DL_FLS_PacketScheduler::ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel)
{
  double metric;

  if (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_TRACE_BASED ||
		  bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_VOIP)
    {

      if (GetLowerLevelSchedulerType () == DL_FLS_PacketScheduler::MAXIMUM_THROUGHPUT)
        {
	      metric = spectralEfficiency * 180000.;
        }

      if (GetLowerLevelSchedulerType () == DL_FLS_PacketScheduler::PROPORTIONAL_FAIR)
        {
	      metric = (spectralEfficiency * 180000.)
			    	/
			       bearer->GetAverageTransmissionRate();
        }
    }
  else
    {
      metric = (spectralEfficiency * 180000.)
		    	/
		       bearer->GetAverageTransmissionRate();
    }

  return metric;
}


void
DL_FLS_PacketScheduler::RunControlLaw ()
{
  m_runControlLaw = false;

#ifdef SCHEDULER_DEBUG
  std::cout << " FLS_DEBUG:Run Control LOW"<< std::endl;
#endif
  RrcEntity *rrc = GetMacEntity ()->GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
  RrcEntity::RadioBearersContainer* bearers = rrc->GetRadioBearerContainer ();

  for (std::vector<RadioBearer* >::iterator it = bearers->begin (); it != bearers->end (); it++)
	{

	  RadioBearer *bearer = (*it);
	  if (bearer->GetApplication ()->GetApplicationType() == Application::APPLICATION_TYPE_TRACE_BASED
			  ||
			  bearer->GetApplication ()->GetApplicationType() == Application::APPLICATION_TYPE_VOIP )
		{

#ifdef SCHEDULER_DEBUG
		  bearer->GetMacQueue ()->PrintQueueInfo ();
#endif

		  //Frame Level Scheduler Control Low!!!
		  QoSForFLS* qos = (QoSForFLS*) bearer->GetQoSParameters ();

		  int queueSize = bearer->GetQueueSize ();
	      int* q = qos->GetQ ();
	      int* u = qos->GetU ();
	      double* c = qos->GetFilterCoefficients();
	      int M = qos->GetNbOfCoefficients ();

	      double dataToTransmit = ((double)(1-c[2]) * queueSize);
	      for (int i = 0; i < M-1; i++)
	        {
	    	  dataToTransmit += (double)q[i]*c[i+2];
	        }
	      for (int i = 0; i < M-2; i++)
			{
	      	  dataToTransmit -= (double)q[i]*c[i+3];
			}
	      for (int i = 0; i < M-1; i++)
			{
			  dataToTransmit -= (double)u[i]*c[i+2];
			}

	      if (dataToTransmit < 0)
	        {
	    	  dataToTransmit = 0;
	        }


	      if (bearer->HasPackets())
	        {
	    	  int minData = 8 + bearer->GetHeadOfLinePacketSize ();
    		  int maxData = bearer->GetMacQueue ()->GetByte (dataToTransmit);
#ifdef SCHEDULER_DEBUG
	      std::cout << "\t dataToTransmit " << dataToTransmit << " minData " << minData  << " maxData " <<  maxData  << std::endl;
#endif
    		  if (dataToTransmit < minData)
	    	    {
#ifdef SCHEDULER_DEBUG
	      std::cout << "\t selected minData " << std::endl;
#endif
	    		  dataToTransmit = minData;
	    	    }
	    	  else
	    	    {
	    		  if (dataToTransmit < maxData)
	    			{
	    			     dataToTransmit=maxData;
#ifdef SCHEDULER_DEBUG
	      std::cout << "\t selected maxData " << std::endl;
#endif
	    			}
	    	    }
	        }


#ifdef SCHEDULER_DEBUG
	      std::cout << "\t selected flow " << (*it)->GetApplication ()->GetApplicationID () << std::endl;
	      qos->Print ();
	      std::cout << "\t queue size  " <<  queueSize << "\n\t DATA TO TRANSMIT " << (ceil)(dataToTransmit) << std::endl;
#endif

	      qos->UpdateQ (queueSize);
	      qos->UpdateU ((ceil)(dataToTransmit));
	      qos->SetDataToTransmit ((ceil)(dataToTransmit));
		}
	}
}


void
DL_FLS_PacketScheduler::Select_RT_FlowsToSchedule ()
{
  ClearFlowsToSchedule ();

#ifdef SCHEDULER_DEBUG
	std::cout << "\t Select RT Flows to schedule" << std::endl;
#endif

  RrcEntity *rrc = GetMacEntity ()->GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
  RrcEntity::RadioBearersContainer* bearers = rrc->GetRadioBearerContainer ();

  for (std::vector<RadioBearer* >::iterator it = bearers->begin (); it != bearers->end (); it++)
	{
	  //SELECT FLOWS TO SCHEDULE
	  RadioBearer *bearer = (*it);
	  QoSForFLS* qos = (QoSForFLS*) bearer->GetQoSParameters ();

	  if (bearer->HasPackets ()
			  &&
			  bearer->GetDestination ()->GetNodeState () == NetworkNode::STATE_ACTIVE
			  &&
			  qos->GetDataToTransmit () > 0
			  &&
			      (bearer->GetApplication ()->GetApplicationType() == Application::APPLICATION_TYPE_TRACE_BASED
			  	  ||
                  bearer->GetApplication ()->GetApplicationType() == Application::APPLICATION_TYPE_VOIP ))
		{
		  //data to transmit
		  int dataToTransmit = qos->GetDataToTransmit ();

		  //compute spectral efficiency
		  ENodeB *enb = (ENodeB*) GetMacEntity ()->GetDevice ();
		  ENodeB::UserEquipmentRecord *ueRecord = enb->GetUserEquipmentRecord (bearer->GetDestination ()->GetIDNetworkNode ());
		  std::vector<double> spectralEfficiency;
		  std::vector<int> cqiFeedbacks = ueRecord->GetCQI ();
		  int numberOfCqi = cqiFeedbacks.size ();
		  for (int i = 0; i < numberOfCqi; i++)
			{
			  double sEff = GetMacEntity ()->GetAmcModule ()->GetEfficiencyFromCQI (cqiFeedbacks.at (i));
			  spectralEfficiency.push_back (sEff);
			}

		  //create flow to schedule record
		  InsertFlowToSchedule(bearer, dataToTransmit, spectralEfficiency, cqiFeedbacks);
		}
	  else
	    {}
	}
}


void
DL_FLS_PacketScheduler::Select_NRT_FlowsToSchedule ()
{
  ClearFlowsToSchedule ();

#ifdef SCHEDULER_DEBUG
	std::cout << "\t Select NRT Flows to schedule" << std::endl;
#endif

  RrcEntity *rrc = GetMacEntity ()->GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
  RrcEntity::RadioBearersContainer* bearers = rrc->GetRadioBearerContainer ();


  for (std::vector<RadioBearer* >::iterator it = bearers->begin (); it != bearers->end (); it++)
	{
	  //SELECT FLOWS TO SCHEDULE
	  RadioBearer *bearer = (*it);
	  if (bearer->HasPackets ()
			  &&
			  bearer->GetDestination ()->GetNodeState () == NetworkNode::STATE_ACTIVE
			  &&
			      (bearer->GetApplication ()->GetApplicationType() == Application::APPLICATION_TYPE_CBR
			       ||
			       bearer->GetApplication ()->GetApplicationType() == Application::APPLICATION_TYPE_INFINITE_BUFFER ))
		{

		  //compute data to transmit
		  int dataToTransmit;

		  if (bearer->GetApplication ()->GetApplicationType() == Application::APPLICATION_TYPE_INFINITE_BUFFER)
		    {
			  dataToTransmit = 100000;
		    }
		  else
		    {
			  dataToTransmit = bearer->GetQueueSize ();
		    }

		  //compute spectral efficiency
		  ENodeB *enb = (ENodeB*) GetMacEntity ()->GetDevice ();
		  ENodeB::UserEquipmentRecord *ueRecord = enb->GetUserEquipmentRecord (bearer->GetDestination ()->GetIDNetworkNode ());
		  std::vector<double> spectralEfficiency;
		  std::vector<int> cqiFeedbacks = ueRecord->GetCQI ();
		  int numberOfCqi = cqiFeedbacks.size ();
		  for (int i = 0; i < numberOfCqi; i++)
			{
			  double sEff = GetMacEntity ()->GetAmcModule ()->GetEfficiencyFromCQI (cqiFeedbacks.at (i));
			  spectralEfficiency.push_back (sEff);
			}

		  //create flow to schedule record
		  InsertFlowToSchedule(bearer, dataToTransmit, spectralEfficiency, cqiFeedbacks);
		}
	  else
	    {}
	}
}

void
DL_FLS_PacketScheduler::UpdateDataToTransmitAndAverageDataRate (void)
{
  RrcEntity *rrc = GetMacEntity ()->GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
  RrcEntity::RadioBearersContainer* bearers = rrc->GetRadioBearerContainer ();

  for (std::vector<RadioBearer* >::iterator it = bearers->begin (); it != bearers->end (); it++)
	{
	  RadioBearer *bearer = (*it);

	  // UPDATE DATA TO TRASMIT FOR RT FLOWS
	  if (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_TRACE_BASED
			  ||
			  bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_VOIP)
	    {
		  QoSForFLS *qos = (QoSForFLS*) bearer->GetQoSParameters ();
		  int dataToTransmit = qos->GetDataToTransmit ();
		  int transmittedData = bearer->GetTransmittedBytes ();
		  if (transmittedData >= dataToTransmit)
	        {
		      qos->SetDataToTransmit (0);
	        }
	      else
	        {
		      qos->SetDataToTransmit (dataToTransmit - transmittedData);
	        }
	    }

	  // UPDATE AVERAGE TRANSMISSION RATE
	  bearer->UpdateAverageTransmissionRate ();
	}
}
void
DL_FLS_PacketScheduler::RBsAllocation ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << " ---- DownlinkPacketScheduler::RBsAllocation";
#endif


  FlowsToSchedule* flows = GetFlowsToSchedule ();
  int nbOfRBs = GetMacEntity ()->GetDevice ()->GetPhy ()->GetBandwidthManager ()->GetDlSubChannels ().size ();

  //create a matrix of flow metrics
  double metrics[nbOfRBs][flows->size ()];
  for (int i = 0; i < nbOfRBs; i++)
    {
	  for (int j = 0; j < flows->size (); j++)
	    {
		  metrics[i][j] = ComputeSchedulingMetric (flows->at (j)->GetBearer (),
				                                   flows->at (j)->GetSpectralEfficiency ().at (i),
	    		                                   i);
	    }
    }

#ifdef SCHEDULER_DEBUG
  std::cout << ", available RBs " << nbOfRBs << ", flows " << flows->size () << std::endl;
  for (int ii = 0; ii < flows->size (); ii++)
    {
	  std::cout << "\t metrics for flow "
			  << flows->at (ii)->GetBearer ()->GetApplication ()->GetApplicationID () << ":";
	  for (int jj = 0; jj < nbOfRBs; jj++)
	    {
		  std::cout << " " << metrics[jj][ii];
	    }
	  std::cout << std::endl;
    }
#endif


  AMCModule *amc = GetMacEntity ()->GetAmcModule ();
  double l_dAllocatedRBCounter = 0;

  int l_iNumberOfUsers = ((ENodeB*)this->GetMacEntity()->GetDevice())->GetNbOfUserEquipmentRecords();

  bool * l_bFlowScheduled = new bool[flows->size ()];
  int l_iScheduledFlows = 0;
  std::vector<double> * l_bFlowScheduledSINR = new std::vector<double>[flows->size ()];
  for (int k = 0; k < flows->size (); k++)
      l_bFlowScheduled[k] = false;

  //RBs allocation
  for (int s = m_startPrbAllocation; s < nbOfRBs; s++)
    {
      if (l_iScheduledFlows == flows->size ())
        {
    	  if (m_rtAllocation)
    	    {
    		  m_freePrb = true;
    		  m_startPrbAllocation = s;
    	    }
          break;
        }

      double targetMetric = 0;
      bool RBIsAllocated = false;
      FlowToSchedule* scheduledFlow;
      int l_iScheduledFlowIndex = 0;

      for (int k = 0; k < flows->size (); k++)
        {
          if (metrics[s][k] > targetMetric && !l_bFlowScheduled[k])
            {
              targetMetric = metrics[s][k];
              RBIsAllocated = true;
              scheduledFlow = flows->at (k);
              l_iScheduledFlowIndex = k;
            }
        }

      if (RBIsAllocated)
        {
          l_dAllocatedRBCounter++;

          scheduledFlow->GetListOfAllocatedRBs()->push_back (s); // the s RB has been allocated to that flow!

#ifdef SCHEDULER_DEBUG
          std::cout << "\t *** RB " << s << " assigned to the "
                  " flow " << scheduledFlow->GetBearer ()->GetApplication ()->GetApplicationID ()
                  << std::endl;
#endif
          double sinr = amc->GetSinrFromCQI (scheduledFlow->GetCqiFeedbacks ().at (s));
          l_bFlowScheduledSINR[l_iScheduledFlowIndex].push_back(sinr);

          double effectiveSinr = GetEesmEffectiveSinr (l_bFlowScheduledSINR[l_iScheduledFlowIndex]);
          int mcs = amc->GetMCSFromCQI (amc->GetCQIFromSinr (effectiveSinr));
          int transportBlockSize = amc->GetTBSizeFromMCS (mcs, scheduledFlow->GetListOfAllocatedRBs ()->size ());
          if (transportBlockSize >= scheduledFlow->GetDataToTransmit() * 8)
          {
              l_bFlowScheduled[l_iScheduledFlowIndex] = true;
              l_iScheduledFlows++;
          }

        }
    }

  delete [] l_bFlowScheduled;
  delete [] l_bFlowScheduledSINR;


  //Finalize the allocation
  PdcchMapIdealControlMessage *pdcchMsg = new PdcchMapIdealControlMessage ();

  for (FlowsToSchedule::iterator it = flows->begin (); it != flows->end (); it++)
    {
      FlowToSchedule *flow = (*it);
      if (flow->GetListOfAllocatedRBs ()->size () > 0)
        {
          //this flow has been scheduled
          std::vector<double> estimatedSinrValues;
          for (int rb = 0; rb < flow->GetListOfAllocatedRBs ()->size (); rb++ )

            {
              double sinr = amc->GetSinrFromCQI (
                      flow->GetCqiFeedbacks ().at (flow->GetListOfAllocatedRBs ()->at (rb)));

              estimatedSinrValues.push_back (sinr);
            }

          //compute the effective sinr
          double effectiveSinr = GetEesmEffectiveSinr (estimatedSinrValues);

          //get the MCS for transmission

          int mcs = amc->GetMCSFromCQI (amc->GetCQIFromSinr (effectiveSinr));

          //define the amount of bytes to transmit
          //int transportBlockSize = amc->GetTBSizeFromMCS (mcs);
          int transportBlockSize = amc->GetTBSizeFromMCS (mcs, flow->GetListOfAllocatedRBs ()->size ());
          double bitsToTransmit = transportBlockSize;
          flow->UpdateAllocatedBits (bitsToTransmit);

#ifdef SCHEDULER_DEBUG
		  std::cout << "\t\t --> flow "	<< flow->GetBearer ()->GetApplication ()->GetApplicationID ()
				  << " has been scheduled: " <<
				  "\n\t\t\t nb of RBs " << flow->GetListOfAllocatedRBs ()->size () <<
				  "\n\t\t\t effectiveSinr " << effectiveSinr <<
				  "\n\t\t\t tbs " << transportBlockSize <<
				  "\n\t\t\t bitsToTransmit " << bitsToTransmit
				  << std::endl;
#endif

		  //create PDCCH messages
		  for (int rb = 0; rb < flow->GetListOfAllocatedRBs ()->size (); rb++ )
		    {
			  pdcchMsg->AddNewRecord (PdcchMapIdealControlMessage::DOWNLINK,
					  flow->GetListOfAllocatedRBs ()->at (rb),
									  flow->GetBearer ()->GetDestination (),
									  mcs);
		    }
	    }
    }

  if (pdcchMsg->GetMessage()->size () > 0)
    {
      GetMacEntity ()->GetDevice ()->GetPhy ()->SendIdealControlMessage (pdcchMsg);
    }
  delete pdcchMsg;
}
