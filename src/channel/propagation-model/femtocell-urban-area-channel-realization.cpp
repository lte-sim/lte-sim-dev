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
 * Author: Francesco Capozzi <f.capozzi@poliba.it>
 */


#include "femtocell-urban-area-channel-realization.h"
#include "../../device/UserEquipment.h"
#include "../../device/ENodeB.h"
#include "../../device/HeNodeB.h"
#include "../../utility/RandomVariable.h"
#include "shadowing-trace.h"
#include "../../core/spectrum/bandwidth-manager.h"
#include "../../phy/lte-phy.h"
#include "../../core/eventScheduler/simulator.h"
#include "../../load-parameters.h"


FemtoCellUrbanAreaChannelRealization::FemtoCellUrbanAreaChannelRealization (NetworkNode* src, NetworkNode* dst)
{
  SetSamplingPeriod (0.5);

  m_penetrationLoss = 0;
  m_shadowing = 0;
  m_pathLoss = 0;
  SetFastFading (new FastFading ());

  SetSourceNode (src);
  SetDestinationNode (dst);

#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "Created Channe Realization between "
		  << src->GetIDNetworkNode () << " and " << dst->GetIDNetworkNode () << std::endl;
#endif

  if (_simple_jakes_model_)
	  SetChannelType (ChannelRealization::CHANNEL_TYPE_JAKES);
  if (_PED_A_)
	  SetChannelType (ChannelRealization::CHANNEL_TYPE_PED_A);
  if (_PED_B_)
	  SetChannelType (ChannelRealization::CHANNEL_TYPE_PED_B);
  if (_VEH_A_)
	  SetChannelType (ChannelRealization::CHANNEL_TYPE_VEH_A);
  if (_VEH_B_)
	  SetChannelType (ChannelRealization::CHANNEL_TYPE_VEH_B);



  UpdateModels ();

}

FemtoCellUrbanAreaChannelRealization::~FemtoCellUrbanAreaChannelRealization()
{
}

void
FemtoCellUrbanAreaChannelRealization::SetPenetrationLoss (double pnl)
{
  m_penetrationLoss = pnl;
}

double FemtoCellUrbanAreaChannelRealization::GetPenetrationLoss (void)
{
  return m_penetrationLoss;
}

double
FemtoCellUrbanAreaChannelRealization::GetPathLoss (void)
{
	/*
	 * Path loss Models from sect. 5.2 in
	 * 3GPP TSG RAN WG4 R4-092042
	 *
	 * Alternative simplified model based on LTE-A evaluation methodology which avoids modeling any walls.
	 */

  double distance;
  double minimumCouplingLoss = 45; //[dB] - see 3GPP TSG RAN WG4 #42bis (R4-070456)
  double floorPenetration = 0.0;
  //18.3 n ((n+2)/(n+1)-0.46)

  if (GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_UE
		  && ( GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_ENODEB || GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION) )
    {
	  UserEquipment* ue = (UserEquipment*) GetSourceNode ();
	  ENodeB* enb = (ENodeB*) GetDestinationNode ();

	  if( enb->GetCell()->GetCellCenterPosition()->GetCoordinateZ() > 0
			  && ue->IsIndoor()
			     && enb->GetCell()->GetCellCenterPosition()->GetCoordinateZ() != ue->GetCell()->GetCellCenterPosition()->GetCoordinateZ())
	  {
		  int n = (int) abs( enb->GetCell()->GetCellCenterPosition()->GetCoordinateZ() - ue->GetCell()->GetCellCenterPosition()->GetCoordinateZ() );
		  floorPenetration = 18.3 * pow( n, ((n+2)/(n+1)-0.46));
	  }

	  distance =  ue->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (enb->GetMobilityModel ()->GetAbsolutePosition ());
    }

  else if (GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_UE
		  && ( GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_ENODEB || GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION) )
  {
	  UserEquipment* ue = (UserEquipment*) GetDestinationNode ();
	  ENodeB* enb = (ENodeB*) GetSourceNode ();

	  if( enb->GetCell()->GetCellCenterPosition()->GetCoordinateZ() > 0
			  && ue->IsIndoor()
			  && enb->GetCell()->GetCellCenterPosition()->GetCoordinateZ() != ue->GetCell()->GetCellCenterPosition()->GetCoordinateZ())
	  {
		  int n = (int) abs( enb->GetCell()->GetCellCenterPosition()->GetCoordinateZ() - ue->GetCell()->GetCellCenterPosition()->GetCoordinateZ() );
		  floorPenetration = 18.3 * pow( n, ((n+2)/(n+1)-0.46));
	  }

	  distance =  ue->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (enb->GetMobilityModel ()->GetAbsolutePosition ());
  }




  m_pathLoss = max( minimumCouplingLoss, 127 + ( 30 * log10 (distance * 0.001) ) + floorPenetration);


  return m_pathLoss;
}


void
FemtoCellUrbanAreaChannelRealization::SetShadowing (double sh)
{
  m_shadowing = sh;
}

double
FemtoCellUrbanAreaChannelRealization::GetShadowing (void)
{
  return m_shadowing;
}


void
FemtoCellUrbanAreaChannelRealization::UpdateModels ()
{

#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "\t --> UpdateModels" << std::endl;
#endif


  //update shadowing
  m_shadowing = 0;
  double probability = GetRandomVariable (101) / 100.0;
  for (int i = 0; i < 201; i++)
    {
	  if (probability <= shadowing_probability[i])
	    {
		  m_shadowing = shadowing_value[i];
          break;
	    }
    }

  UpdateFastFading ();

  SetLastUpdate ();
}


std::vector<double>
FemtoCellUrbanAreaChannelRealization::GetLoss ()
{
#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "\t  --> compute loss between "
		  << GetSourceNode ()->GetIDNetworkNode () << " and "
		  << GetDestinationNode ()->GetIDNetworkNode () << std::endl;
#endif

  if (NeedForUpdate ())
    {
	   UpdateModels ();
    }

  std::vector<double> loss;


  int now_ms = Simulator::Init()->Now () * 1000;
  int lastUpdate_ms = GetLastUpdate () * 1000;
  int index = now_ms - lastUpdate_ms;

  int nbOfSubChannels = GetSourceNode ()->GetPhy ()->GetBandwidthManager ()->GetDlSubChannels ().size ();

  for (int i = 0; i < nbOfSubChannels; i++)
    {
	  //ATTENZIONE double l = GetFastFading ()->at (i).at (index) - GetPathLoss () - GetPenetrationLoss () - GetShadowing ();
	  double l = - GetPathLoss ();
	  loss.push_back (l);

#ifdef TEST_PROPAGATION_LOSS_MODEL
       std::cout << "\t\t mlp = " << GetFastFading ()->at (i).at (index)
		  << " pl = " << GetPathLoss ()
          << " pnl = " << GetPenetrationLoss()
          << " sh = " << GetShadowing()
          << " LOSS = " << l
		  << std::endl;
#endif
    }

  return loss;
}


