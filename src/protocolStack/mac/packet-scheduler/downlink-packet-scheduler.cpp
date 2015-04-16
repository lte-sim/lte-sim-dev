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
 *         Lukasz Rajewski <lukasz.rajewski@gmail.com> (optimized PRB allocation)
 */


#include "downlink-packet-scheduler.h"
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
#include "../../../flows/MacQueue.h"
#include "../../../utility/eesm-effective-sinr.h"

DownlinkPacketScheduler::DownlinkPacketScheduler()
{}

DownlinkPacketScheduler::~DownlinkPacketScheduler()
{
  Destroy ();
}

void DownlinkPacketScheduler::SelectFlowsToSchedule ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << "\t Select Flows to schedule" << std::endl;
#endif

  ClearFlowsToSchedule ();

  RrcEntity *rrc = GetMacEntity ()->GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
  RrcEntity::RadioBearersContainer* bearers = rrc->GetRadioBearerContainer ();


  for (std::vector<RadioBearer* >::iterator it = bearers->begin (); it != bearers->end (); it++)
	{
	  //SELECT FLOWS TO SCHEDULE
	  RadioBearer *bearer = (*it);
	  if (bearer->HasPackets () && bearer->GetDestination ()->GetNodeState () == NetworkNode::STATE_ACTIVE)
		{
		  //compute data to transmit
		  int dataToTransmit;
		  if (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_INFINITE_BUFFER)
			{
			  dataToTransmit = 100000000;
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

		  //create flow to scheduler record
		  InsertFlowToSchedule(bearer, dataToTransmit, spectralEfficiency, cqiFeedbacks);
		}
	  else
	    {}
	}
}

void
DownlinkPacketScheduler::DoSchedule (void)
{
#ifdef SCHEDULER_DEBUG
	std::cout << "Start DL packet scheduler for node "
			<< GetMacEntity ()->GetDevice ()->GetIDNetworkNode()<< std::endl;
#endif

  UpdateAverageTransmissionRate ();
  SelectFlowsToSchedule ();

  if (GetFlowsToSchedule ()->size() == 0)
	{}
  else
	{
	  RBsAllocation ();
	}

  StopSchedule ();
}

void
DownlinkPacketScheduler::DoStopSchedule (void)
{
#ifdef SCHEDULER_DEBUG
  std::cout << "\t Creating Packet Burst" << std::endl;
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

  //UpdateAverageTransmissionRate ();

  //SEND PACKET BURST

#ifdef SCHEDULER_DEBUG
  if (pb->GetNPackets () == 0)
    std::cout << "\t Send only reference symbols" << std::endl;
#endif

  GetMacEntity ()->GetDevice ()->SendPacketBurst (pb);
}

/*
void
DownlinkPacketScheduler::RBsAllocation ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << " ---- RBs Allocation";
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


  //RBs allocation
  for (int s = 0; s < nbOfRBs; s++)
    {
      double targetMetric = 0;
      bool RBIsAllocated = false;
      FlowToSchedule* scheduledFlow;

	  for (int k = 0; k < flows->size (); k++)
	    {
		  if (metrics[s][k] > targetMetric)
		    {
			  targetMetric = metrics[s][k];
			  RBIsAllocated = true;
			  scheduledFlow = flows->at (k);
		    }
	    }

	  if (RBIsAllocated)
	    {
		  scheduledFlow->GetListOfAllocatedRBs()->push_back (s); // the s RB has been allocated to that flow!

#ifdef SCHEDULER_DEBUG
		  std::cout << "\t *** RB " << s << " assigned to the "
				  " flow " << scheduledFlow->GetBearer ()->GetApplication ()->GetApplicationID ()
				  << std::endl;
#endif
	    }
    }


  //Finalize the allocation
  AMCModule *amc = GetMacEntity ()->GetAmcModule ();
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
*/

void
DownlinkPacketScheduler::RBsAllocation ()
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
  for (int s = 0; s < nbOfRBs; s++)
    {
      if (l_iScheduledFlows == flows->size ())
          break;

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


void
DownlinkPacketScheduler::UpdateAverageTransmissionRate (void)
{
  RrcEntity *rrc = GetMacEntity ()->GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
  RrcEntity::RadioBearersContainer* bearers = rrc->GetRadioBearerContainer ();

  for (std::vector<RadioBearer* >::iterator it = bearers->begin (); it != bearers->end (); it++)
    {
      RadioBearer *bearer = (*it);
      bearer->UpdateAverageTransmissionRate ();
    }
}
