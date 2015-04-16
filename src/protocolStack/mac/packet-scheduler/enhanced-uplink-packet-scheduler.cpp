/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012
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
 * Author: Khaled Elsayed <khaled@ieee.org>
 */

#include "enhanced-uplink-packet-scheduler.h"
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
#include "../../../utility/eesm-effective-sinr.h"

EnhancedUplinkPacketScheduler::EnhancedUplinkPacketScheduler()
{
  SetMacEntity (0);
  CreateUsersToSchedule ();
}

EnhancedUplinkPacketScheduler::~EnhancedUplinkPacketScheduler()
{
  Destroy ();
}

double
EnhancedUplinkPacketScheduler::ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel)
{
  double metric;
  return metric;
}

double
EnhancedUplinkPacketScheduler::ComputeSchedulingMetric (UserToSchedule* user, int subchannel)
{
  double metric;

  int channelCondition = user->m_channelContition.at (subchannel);
  double spectralEfficiency = GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (channelCondition);

  metric = spectralEfficiency * 180000;

  return metric;
}

void
EnhancedUplinkPacketScheduler::RBsAllocation ()
{
	/* This is an implementation of an algorithm based on  first maximum expansion algorithm reported in
	 * L. Temiño, G. Berardinelli, S. Frattasi,  and P.E. Mogensen,
	 * "Channel-aware scheduling algorithms for SC-FDMA in LTE uplink",  in Proc. PIMRC 2008
	 * The main difference is that here we have a given number of RB's to allocate to the UE
	 * based on its pending queue status whereas the original attempts to allocate till another
	 * UE has a better channel response
	 */
#ifdef SCHEDULER_DEBUG
	std::cout << " ---- UL RBs Allocation";
#endif

	UsersToSchedule *users = GetUsersToSchedule ();
	UserToSchedule* scheduledUser;
	int nbOfRBs = GetMacEntity ()->GetDevice ()->GetPhy ()->GetBandwidthManager ()->GetUlSubChannels ().size ();

	int availableRBs;     // No of RB's not allocated
	int unallocatedUsers; // No of users who remain unallocated
	int selectedUser;     // user to be selected for allocation
	int selectedPRB;      // PRB to be selected for allocation
	double bestMetric;    // best metric to identify user/RB combination
	int left, right;      // index of left and left PRB's to check
	bool Allocated[nbOfRBs];
	bool allocationMade;
	double metrics[nbOfRBs][users->size ()];
	int requiredPRBs[users->size ()];


	//Some initialization
	availableRBs = nbOfRBs;
	unallocatedUsers = users->size ();
	for(int i=0; i < nbOfRBs; i++)
		Allocated[i] = false;

	//create a matrix of flow metrics
	for (int i = 0; i < nbOfRBs; i++)
	{
		for (int j = 0; j < users->size (); j++)
		{
			metrics[i][j] = ComputeSchedulingMetric (users->at (j), i);
		}
	}

	//create number of required PRB's per scheduled users
	for(int j=0; j < users->size(); j++)
	{
		scheduledUser = users->at(j);
#ifdef SCHEDULER_DEBUG
		cout << "\n" << "User "  << j; // << "CQI Vector";
#endif

		std::vector<double> sinrs;
		for (std::vector<int>::iterator c = scheduledUser->m_channelContition.begin ();
				c != scheduledUser->m_channelContition.end (); c++)
		{
			//cout << *c <<" ";
			sinrs.push_back (GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (*c));
		}


		double effectiveSinr =  GetEesmEffectiveSinr (sinrs);

		int mcs = GetMacEntity ()->GetAmcModule ()->GetMCSFromCQI (
			  GetMacEntity ()->GetAmcModule ()->GetCQIFromSinr (effectiveSinr));
		scheduledUser->m_selectedMCS = mcs;
		requiredPRBs[j] = (floor) (scheduledUser->m_dataToTransmit /
				  (GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, 1) / 8));
#ifdef SCHEDULER_DEBUG
		cout << " EffSINR = " << effectiveSinr << "  MCS = " << mcs << "\n";
#endif
	}

#ifdef SCHEDULER_DEBUG
  //std::cout << ", available RBs " << nbOfRBs << ", users " << users->size () << std::endl;
  for (int ii = 0; ii < users->size (); ii++)
    {
	  std::cout << "Metrics for user "
			  << users->at (ii)->m_userToSchedule->GetIDNetworkNode () << "\n";
	  for (int jj = 0; jj < nbOfRBs; jj++)
	    {
		  //std::cout  << setw(3) << metrics[jj][ii]/1000 << " ";
		  printf("%3d  ", (int) (metrics[jj][ii]/1000.0));
	    }
	  std::cout << std::endl;
    }
#endif

  //RBs allocation

  while(availableRBs > 0 && unallocatedUsers > 0) //
  {
	  // First step: find the best user-RB combo
	  selectedPRB = -1;
	  selectedUser = -1;
	  bestMetric = (double) (-(1<<30));

	  for(int i=0; i < nbOfRBs; i++)
	  {
		  if (!Allocated[i]){ // check only unallocated PRB's
			  for(int j=0; j < users->size (); j++)
			  {
				  if ( users->at (j)->m_listOfAllocatedRBs.size() == 0
						  && requiredPRBs[j] > 0) //only unallocated users requesting some RB's
					  if (bestMetric < metrics[i][j]){
						  selectedPRB = i;
						  selectedUser = j;
						  bestMetric = metrics[i][j];
					  }
			  }

		  }
	  }
	  // Now start allocating for the selected user at the selected PRB the required blocks
	  // using how many PRB's are needed for the user
	  if (selectedUser != -1)
	  {
		  scheduledUser = users->at(selectedUser);
		  scheduledUser->m_listOfAllocatedRBs.push_back (selectedPRB);
		  Allocated[selectedPRB] = true;
		  left =  selectedPRB - 1;
		  right = selectedPRB + 1;
		  availableRBs--;
		  unallocatedUsers--;

		  allocationMade = true;
		  for(int i = 1; i < requiredPRBs[selectedUser] && availableRBs > 0 && allocationMade; i++ )
		  { // search right and left of initial allocation
			  allocationMade = false;
			  if (left >=0 && Allocated[left] && right < nbOfRBs && Allocated[right])
				  break; // nothing is available, since we need to have contiguous allocation

			  if (    (right < nbOfRBs) && (! Allocated[right]) &&
					  (
							  ((left >=0) &&
							  (metrics[right][selectedUser] >= metrics[left][selectedUser])) // right is better than left
							  || (left < 0) || Allocated[left]// OR no more left
					  )
				)
			  {
				  //Allocate PRB at right to the user
				  Allocated[right] = true;
				  scheduledUser->m_listOfAllocatedRBs.push_back (right);
				  right++;
				  allocationMade = true;
				  availableRBs--;
			  } else if ( (left >=0) && (! Allocated[left]) &&
						  (
							  ((right < nbOfRBs) &&
							  (metrics[left][selectedUser] > metrics[right][selectedUser])) //left better than right
							  || (right >= nbOfRBs) || Allocated[right]// OR no more right
						   )
						)
			  {
				  //Allocate PRB at left to the user
				  Allocated[left] = true;
				  scheduledUser->m_listOfAllocatedRBs.push_back (left);
				  left--;
				  allocationMade = true;
				  availableRBs--;
			  }
		  } // end of for
		  if (allocationMade){
			  scheduledUser->m_transmittedData = GetMacEntity ()->GetAmcModule ()->
					  GetTBSizeFromMCS (scheduledUser->m_selectedMCS, scheduledUser->m_listOfAllocatedRBs.size()) / 8;
#ifdef SCHEDULER_DEBUG
			  printf("Scheduled User = %d mcs = %d Required RB's = %d Allocated RB's= %d\n",
		 				  scheduledUser->m_userToSchedule->GetIDNetworkNode(),
		 				  scheduledUser->m_selectedMCS,
		 				  requiredPRBs[selectedUser], scheduledUser->m_listOfAllocatedRBs.size() );
			  for(int i=0; i<scheduledUser->m_listOfAllocatedRBs.size(); i++)
				  printf("%d ", scheduledUser->m_listOfAllocatedRBs.at(i));
			  printf("\n------------------\n");
#endif
		  }
	  } else { // nothing to do exit the allocation loop
		  break;
	  }
  } //while

}
