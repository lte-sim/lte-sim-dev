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

#include "../../../device/ENodeB.h"
#include "uplink-packet-scheduler.h"
#include "../mac-entity.h"
#include "../../packet/Packet.h"
#include "../../packet/packet-burst.h"
#include "../../../device/NetworkNode.h"
#include "../../../flows/radio-bearer.h"
#include "../../../protocolStack/rrc/rrc-entity.h"
#include "../../../flows/application/Application.h"
#include "../../../protocolStack/mac/AMCModule.h"
#include "../../../phy/lte-phy.h"
#include "../../../core/spectrum/bandwidth-manager.h"
#include "../../../flows/MacQueue.h"
#include "../../../utility/eesm-effective-sinr.h"

UplinkPacketScheduler::UplinkPacketScheduler()
{}

UplinkPacketScheduler::~UplinkPacketScheduler()
{
  Destroy ();
  DeleteUsersToSchedule ();
}

void
UplinkPacketScheduler::CreateUsersToSchedule (void)
{
  m_usersToSchedule = new UsersToSchedule ();
}

void
UplinkPacketScheduler::DeleteUsersToSchedule (void)
{
  if (m_usersToSchedule != NULL)
    {
	  if (m_usersToSchedule->size () > 0)
        ClearUsersToSchedule ();
      delete m_usersToSchedule;
    }
}

void
UplinkPacketScheduler::ClearUsersToSchedule ()
{
  UsersToSchedule*  records = GetUsersToSchedule ();
  UsersToSchedule::iterator iter;

  for (iter = records->begin(); iter != records->end (); iter++)
	{
	  delete *iter;
	}

  GetUsersToSchedule ()->clear ();
}

UplinkPacketScheduler::UsersToSchedule*
UplinkPacketScheduler::GetUsersToSchedule (void)
{
  return m_usersToSchedule;
}

void
UplinkPacketScheduler::SelectUsersToSchedule ()
{
  CreateUsersToSchedule ();

  ENodeB *node = (ENodeB*) GetMacEntity ()->GetDevice ();
  ENodeB::UserEquipmentRecords *records = node->GetUserEquipmentRecords ();
  ENodeB::UserEquipmentRecord *record;
  ENodeB::UserEquipmentRecords::iterator iter;

#ifdef SCHEDULER_DEBUG
  std::cout << "UplinkPacketScheduler::SelectUsersToSchedule () "
		  " users " << node->GetUserEquipmentRecords ()->size ()<< std::endl;
#endif

  for (iter = records->begin (); iter != records->end (); iter++)
	{
	  record = *iter;
      if (record->GetSchedulingRequest () > 0)
        {
    	  UserToSchedule* user = new UserToSchedule ();
          user->m_userToSchedule = (NetworkNode*) record->GetUE ();
          user->m_dataToTransmit = record->GetSchedulingRequest ();
          user->m_listOfAllocatedRBs.clear ();
          user->m_selectedMCS = 0;
          user->m_transmittedData = 0;
          user->m_channelContition = record->GetCQI ();
          user->m_averageSchedulingGrant = record->GetSchedulingGrants ();

          GetUsersToSchedule ()->push_back (user);
        }
	}
#ifdef SCHEDULER_DEBUG
  std::cout << "users to be schedule = " << GetUsersToSchedule ()->size () << std::endl;
#endif
}

void
UplinkPacketScheduler::DoSchedule (void)
{
#ifdef SCHEDULER_DEBUG
	std::cout << "Start UPLINK packet scheduler for node "
			<< GetMacEntity ()->GetDevice ()->GetIDNetworkNode()<< std::endl;
#endif

   SelectUsersToSchedule ();

  if (GetUsersToSchedule ()->size () > 0)
    {
	  RBsAllocation ();
      DoStopSchedule ();
    }

  DeleteUsersToSchedule ();
}

void
UplinkPacketScheduler::RBsAllocation ()
{
#ifdef SCHEDULER_DEBUG
	std::cout << " ---- UL RBs Allocation";
#endif

	UsersToSchedule *users = GetUsersToSchedule ();
	int nbOfRBs = GetMacEntity ()->GetDevice ()->GetPhy ()->GetBandwidthManager ()->GetUlSubChannels ().size ();

	  //create a matrix of flow metrics
	  double metrics[nbOfRBs][users->size ()];
	  for (int i = 0; i < nbOfRBs; i++)
	    {
		  for (int j = 0; j < users->size (); j++)
		    {
			  metrics[i][j] = ComputeSchedulingMetric (users->at (j), i);
		    }
	    }

#ifdef SCHEDULER_DEBUG
  std::cout << ", available RBs " << nbOfRBs << ", users " << users->size () << std::endl;
  for (int ii = 0; ii < users->size (); ii++)
    {
	  std::cout << "\t metrics for user "
			  << users->at (ii)->m_userToSchedule->GetIDNetworkNode ();
	  for (int jj = 0; jj < nbOfRBs; jj++)
	    {
		  std::cout << " " << metrics[jj][ii];
	    }
	  std::cout << std::endl;
    }
#endif

  //RBs allocation
  int s = 0;
  while (s < nbOfRBs)
    {
	  double targetMetric = 0;
	  bool RBIsAllocated = false;
	  UserToSchedule* scheduledUser;

	  for (int k = 0; k < users->size (); k++)
	    {
		  if (metrics[s][k] > targetMetric)
			{
			  targetMetric = metrics[s][k];
			  RBIsAllocated = true;
			  scheduledUser = users->at (k);
			}
		}

	  if (RBIsAllocated)
	    {
		  int dataToTransmit = scheduledUser->m_dataToTransmit;
		  int availableRBs = nbOfRBs - s;
		  std::vector<double> sinrs;
		  for (std::vector<int>::iterator c = scheduledUser->m_channelContition.begin ();
				  c != scheduledUser->m_channelContition.end (); c++)
		    {
			  sinrs.push_back (GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (*c));
		    }
		  double effectiveSinr =  GetEesmEffectiveSinr (sinrs);
		  int mcs = GetMacEntity ()->GetAmcModule ()->GetMCSFromCQI (
				  GetMacEntity ()->GetAmcModule ()->GetCQIFromSinr (effectiveSinr));
		  int tbs = (GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, availableRBs)) / 8;

		  if (tbs <= dataToTransmit)
		    {
			  s = nbOfRBs;
			  scheduledUser->m_transmittedData = tbs;

			  for (int ss = 0; ss < availableRBs; ss++)
			    {
				  scheduledUser->m_listOfAllocatedRBs.push_back (ss); // XXX it doesn't describe correctly assigned PRBs
			    }
			  scheduledUser->m_selectedMCS = mcs;
		    }
		  else
		    {
			  int rbsNeeded = (floor) (scheduledUser->m_dataToTransmit /
					  (GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, 1) / 8));
		      scheduledUser->m_transmittedData = GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, rbsNeeded) / 8;
		      s += rbsNeeded;

			  for (int ss = 0; ss < rbsNeeded; ss++)
			    {
				  scheduledUser->m_listOfAllocatedRBs.push_back (ss); // XXX it doesn't describe correctly assigned PRBs
			    }
			  scheduledUser->m_selectedMCS = mcs;
		    }
	    }
    }
}

void
UplinkPacketScheduler::DoStopSchedule (void)
{
  //Finalize the allocation
  PdcchMapIdealControlMessage *pdcchMsg = new PdcchMapIdealControlMessage ();
  UsersToSchedule* users = GetUsersToSchedule ();

  for (UsersToSchedule::iterator it = users->begin (); it != users->end (); it++)
	{
	  UserToSchedule *user = (*it);
	  if (user->m_transmittedData > 0)
		{

		  //create PDCCH messages
		  for (int rb = 0; rb < user->m_listOfAllocatedRBs.size (); rb++ )
			{
			  pdcchMsg->AddNewRecord (PdcchMapIdealControlMessage::UPLINK,
					                  user->m_listOfAllocatedRBs.at (rb),
									  user->m_userToSchedule,
									  user->m_selectedMCS);
			}

		  //update users informations
		  ENodeB *enb = (ENodeB*) GetMacEntity ()->GetDevice ();
		  ENodeB::UserEquipmentRecord* record = enb->GetUserEquipmentRecord (user->m_userToSchedule->GetIDNetworkNode ());
		  record->m_schedulingRequest -= user->m_transmittedData;
		  if (record->m_schedulingRequest < 0 )
		    {
			  record->m_schedulingRequest = 0;
		    }
		  record->UpdateSchedulingGrants (user->m_dataToTransmit);

		}
	}

  if (pdcchMsg->GetMessage()->size () > 0)
	{
	  GetMacEntity ()->GetDevice ()->GetPhy ()->SendIdealControlMessage (pdcchMsg);
	}

  //delete pdcchMsg;
}
