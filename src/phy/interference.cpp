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


#include "interference.h"
#include "../componentManagers/NetworkManager.h"
#include "../device/UserEquipment.h"
#include "../device/ENodeB.h"
#include "../device/HeNodeB.h"
#include "../utility/ComputePathLoss.h"
#include "lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"

Interference::Interference()
{}

Interference::~Interference()
{}

double
Interference::ComputeInterference (UserEquipment *ue)
{
  ENodeB *node;

  double interference = 0;

  std::vector<ENodeB*> *eNBs = NetworkManager::Init ()->GetENodeBContainer ();
  std::vector<ENodeB*>::iterator it;

  //std::cout << "Compute interference for UE " << ue->GetIDNetworkNode () << " ,target node " << ue->GetTargetNode ()->GetIDNetworkNode ()<< std::endl;

  for (it = eNBs->begin (); it != eNBs->end (); it++)
    {
	  node = (*it);
	  if (node->GetIDNetworkNode () != ue->GetTargetNode ()->GetIDNetworkNode () &&
			  node->GetPhy ()->GetBandwidthManager ()->GetDlOffsetBw () ==
			  ue->GetTargetNode ()->GetPhy ()->GetBandwidthManager ()->GetDlOffsetBw ())
	    {
		  double powerTXForSubBandwidth = 10 * log10 (
				  pow (10., (node->GetPhy()->GetTxPower() - 30)/10)
				  /
				  node->GetPhy()->GetBandwidthManager ()->GetDlSubChannels().size ());


          double nodeInterference_db = powerTXForSubBandwidth - 10 - ComputePathLossForInterference (node, ue); // in dB
          double nodeInterference = pow(10, nodeInterference_db/10);

          interference += nodeInterference;

          /*
		  std::cout << "\t add interference from eNB " << node->GetIDNetworkNode ()
				  << " " << powerTXForSubBandwidth << " "  << ComputePathLossForInterference (node, ue)
				  << " " << nodeInterference_db << " " << nodeInterference
				  << " --> tot: " << interference
				  << std::endl;
		  */
	    }
    }

  std::vector<HeNodeB*> *HeNBs = NetworkManager::Init ()->GetHomeENodeBContainer();
  std::vector<HeNodeB*>::iterator it2;
  HeNodeB *henb;

  //std::cout << "Compute interference for UE " << ue->GetIDNetworkNode () << " ,target node " << ue->GetTargetNode ()->GetIDNetworkNode ()<< std::endl;

  for (it2 = HeNBs->begin (); it2 != HeNBs->end (); it2++)
    {
	  henb = (*it2);
	  if (henb->GetIDNetworkNode () != ue->GetTargetNode ()->GetIDNetworkNode () &&
			  henb->GetPhy ()->GetBandwidthManager ()->GetDlOffsetBw () ==
			  ue->GetTargetNode ()->GetPhy ()->GetBandwidthManager ()->GetDlOffsetBw ())
	    {
		  double powerTXForSubBandwidth = 10 * log10 (
				  pow (10., (henb->GetPhy()->GetTxPower() - 30)/10)
				  /
				  henb->GetPhy()->GetBandwidthManager ()->GetDlSubChannels().size ());


          //ATTENZIONE double nodeInterference_db = powerTXForSubBandwidth - 10 - ComputePathLossForInterference (henb, ue); // in dB
		  double nodeInterference_db = powerTXForSubBandwidth - ComputePathLossForInterference (henb, ue); // in dB
          double nodeInterference = pow(10, nodeInterference_db/10);

          interference += nodeInterference;

          /*
		  std::cout << "\t add interference from eNB " << node->GetIDNetworkNode ()
				  << " " << powerTXForSubBandwidth << " "  << ComputePathLossForInterference (node, ue)
				  << " " << nodeInterference_db << " " << nodeInterference
				  << " --> tot: " << interference
				  << std::endl;
		  */
	    }
    }

  return interference;
}
