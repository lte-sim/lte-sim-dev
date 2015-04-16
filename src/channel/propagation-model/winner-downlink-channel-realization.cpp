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


#include "winner-downlink-channel-realization.h"
#include "../../device/UserEquipment.h"
#include "../../device/ENodeB.h"
#include "../../device/HeNodeB.h"
#include "../../utility/RandomVariable.h"
#include "../../utility/IndoorScenarios.h"
#include "shadowing-trace.h"
#include "../../core/spectrum/bandwidth-manager.h"
#include "../../phy/lte-phy.h"
#include "../../core/eventScheduler/simulator.h"
#include "../../load-parameters.h"
#include <assert.h>


WinnerDownlinkChannelRealization::WinnerDownlinkChannelRealization (NetworkNode* src, NetworkNode* dst)
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

WinnerDownlinkChannelRealization::~WinnerDownlinkChannelRealization()
{
}

void
WinnerDownlinkChannelRealization::SetPenetrationLoss (double pnl)
{
  m_penetrationLoss = pnl;
}

double WinnerDownlinkChannelRealization::GetPenetrationLoss (void)
{
  return m_penetrationLoss;
}

double
WinnerDownlinkChannelRealization::GetPathLoss (void)
{
  /*
   * Path Loss Model For Indoor Environment.
   * "WINNER II channel models, ver 1.1, Tech Report"
   * PL = A*log10(r) + B + C*log10(fc/5) + X; [r in meters; fc in GHz]
   * I = 128.1 – 2GHz
   * X depends on the number of walls in between
   * FL = 17 + 4 (Nfloors - 1) --- floor loss
   */

  double distance;
  UserEquipment* ue;
  ENodeB* enb;

  assert (GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION  || GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION);

  if (GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_UE
		  && GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION )
    {
	  ue = (UserEquipment*) GetSourceNode ();
	  enb = (ENodeB*) GetDestinationNode ();

	  distance =  ue->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (enb->GetMobilityModel ()->GetAbsolutePosition ());
    }

  else if (GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_UE
		  && GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION )
    {
	  ue = (UserEquipment*) GetDestinationNode ();
	  enb = (ENodeB*) GetSourceNode ();

	  distance =  ue->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (enb->GetMobilityModel ()->GetAbsolutePosition ());
    }

  int* nbWalls = GetWalls( (Femtocell*) (enb->GetCell()), ue);

  double A, B, C;
  double ExternalWallsAttenuation = 20.0;
  double InternalWallsAttenuation = 10.0;

  if (nbWalls[0] == 0 && nbWalls[1] == 0)
    { //LOS
      A = 18.7;
      B = 46.8;
      C = 20.0;
    }
  else
    { //NLOS
      A = 20.0;
      B = 46.4;
      C = 20.0;
    }

  m_pathLoss = A * log10( distance ) +
                       B +
                       C * log10(2. / 5.0) +
                       InternalWallsAttenuation * nbWalls[1] +
                       ExternalWallsAttenuation * nbWalls[0];

  delete [] nbWalls;
  return m_pathLoss;
}


void
WinnerDownlinkChannelRealization::SetShadowing (double sh)
{
  m_shadowing = sh;
}

double
WinnerDownlinkChannelRealization::GetShadowing (void)
{
  return m_shadowing;
}


void
WinnerDownlinkChannelRealization::UpdateModels ()
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
WinnerDownlinkChannelRealization::GetLoss ()
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
