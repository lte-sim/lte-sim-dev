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


#include "channel-realization.h"
#include "../../device/UserEquipment.h"
#include "../../device/ENodeB.h"
#include "../../device/HeNodeB.h"
#include "../../utility/RandomVariable.h"
#include "shadowing-trace.h"
#include "../../core/spectrum/bandwidth-manager.h"
#include "../../phy/lte-phy.h"
#include "../../core/eventScheduler/simulator.h"
#include "../../load-parameters.h"
#include "fast-fading-trace.h"

ChannelRealization::ChannelRealization()
{
  m_src = NULL;
  m_dst = NULL;
  m_samplingPeriod = 0.5;
  m_lastUpdate = NULL;
  m_fastFading = NULL;
}

ChannelRealization::~ChannelRealization()
{
  Destroy ();
}

void
ChannelRealization::Destroy ()
{
  if (m_fastFading != NULL)
    {
          FastFading::iterator it;
          for (it = m_fastFading->begin (); it != m_fastFading->end (); it ++)
                {
                  (*it).clear();
                }
          m_fastFading->clear ();
          delete m_fastFading;
    }
  m_src = NULL;
  m_dst = NULL;
}

void
ChannelRealization::SetSourceNode (NetworkNode* src)
{
  m_src = src;
}

NetworkNode*
ChannelRealization::GetSourceNode (void)
{
  return m_src;
}

void
ChannelRealization::SetDestinationNode (NetworkNode* dst)
{
  m_dst = dst;
}

NetworkNode*
ChannelRealization::GetDestinationNode (void)
{
  return m_dst;
}

void
ChannelRealization::SetLastUpdate (void)
{
  m_lastUpdate = Simulator::Init()->Now ();
}


double
ChannelRealization::GetLastUpdate (void)
{
  return m_lastUpdate;
}


void
ChannelRealization::SetSamplingPeriod (double sp)
{
  m_samplingPeriod = sp;
}


double
ChannelRealization::GetSamplingPeriod (void)
{
  return m_samplingPeriod;
}

bool
ChannelRealization::NeedForUpdate (void)
{

#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "Need for update ? Now " << Simulator::Init()->Now ()
		  << " GetLastUpdate () " << GetLastUpdate ()
		  << " GetSamplingPeriod () " << GetSamplingPeriod () << std::endl;
#endif

  if (Simulator::Init()->Now () >= (GetLastUpdate () + GetSamplingPeriod ()) - 0.001)
    {
      return true;
    }
  else
    {
      return false;
    }
}

void
ChannelRealization::SetChannelType (ChannelType t)
{
  m_channelType = t;
  //UpdateFastFading ();
}

ChannelRealization::ChannelType
ChannelRealization::GetChannelType (void)
{
  return m_channelType;
}


ChannelRealization::FastFading*
ChannelRealization::GetFastFading (void)
{
  return m_fastFading;
}

void
ChannelRealization::SetFastFading (ChannelRealization::FastFading* ff)
{
  m_fastFading = ff;
}

void
ChannelRealization::UpdateFastFading (void)
{
  //clear old realization
  FastFading::iterator it;
  for (it = m_fastFading->begin (); it != m_fastFading->end (); it ++)
	{
	  (*it).clear();
	}
  m_fastFading->clear ();

  int numbOfSubChannels = GetSourceNode ()->GetPhy ()->GetBandwidthManager ()->GetDlSubChannels ().size ();
  int samplingTime = GetSamplingPeriod () * 1000;
  double speed;

  if (GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_UE)
    {
	  UserEquipment* ue = (UserEquipment*) GetSourceNode ();
	  speed = ue->GetMobilityModel ()->GetSpeed ();
    }
  else if (GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_UE)
    {
	  UserEquipment* ue = (UserEquipment*) GetDestinationNode ();
	  speed = ue->GetMobilityModel ()->GetSpeed ();
    }
  else
    {
	  speed = 0;
    }


  //if (_simple_jakes_model_)
  if (GetChannelType () == ChannelRealization::CHANNEL_TYPE_JAKES)
   {
	  // number of path = M
	  //x = 1 -> M=6, x = 2 -> M=8, x = 3 -> M=10, x = 4 -> M=12
	  int x = 1 + GetRandomVariable (4);
	  for (int i = 0; i < numbOfSubChannels; i++)
		{
		  //StartJakes allow us to select a window of 0.5ms into the Jakes realization lasting 3s.
	      int startJakes = GetRandomVariable (2000);

		  FastFadingForTimeDomain ff_time;
		  if (x == 1)
			{
			  //SELECTED 6 MULTIPLE PATH FOR JAKES MODEL
			  if (abs(speed) == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M6_v_0 [j + startJakes]);
					}
				}
			  if (abs(speed) == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M6_v_3 [j + startJakes]);
					}
				}
			  if (abs(speed) == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M6_v_30 [j + startJakes]);
					}
				}
			  if (abs(speed) == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M6_v_120 [j + startJakes]);
					}
				}
			}

		  else if (x == 2)
			{
			  //SELECTED 8 MULTIPLE PATH FOR JAKES MODEL
			  if (abs(speed) == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M8_v_0 [j + startJakes]);
					}
				}
			  if (abs(speed) == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M8_v_3 [j + startJakes]);
					}
				}
			  if (abs(speed) == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M8_v_30 [j + startJakes]);
					}
				}
			  if (abs(speed) == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M8_v_120 [j + startJakes]);
					}
				}
			}

		  else if (x == 3)
			{
			  //SELECTED 10 MULTIPLE PATH FOR JAKES MODEL
			  if (abs(speed) == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M10_v_0 [j + startJakes]);
					}
				}
			  if (abs(speed) == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M10_v_3 [j + startJakes]);
					}
				}
			  if (abs(speed) == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M10_v_30 [j + startJakes]);
					}
				}
			  if (abs(speed) == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M10_v_120 [j + startJakes]);
					}
				}
			}

		  else if (x == 4)
			{
			  //SELECTED 12 MULTIPLE PATH FOR JAKES MODEL
			  if (abs(speed) == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M12_v_0 [j + startJakes]);
					}
				}
			  if (abs(speed) == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M12_v_3 [j + startJakes]);
					}
				}
			  if (abs(speed) == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M12_v_30 [j + startJakes]);
					}
				}
			  if (abs(speed) == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back(multipath_M12_v_120 [j + startJakes]);
					}
				}
			}

		  else
			{
			  std::cout << " ERROR: Jaks's Model, incorrect M value" << std::endl;
			  exit (1);
			}

		  m_fastFading->push_back (ff_time);
		}
    }


  else
    {
	  int start_point_freq = 0;
	  int start_point_time = GetRandomVariable (499);

	#ifdef TEST_PROPAGATION_LOSS_MODEL
	  std::cout << "UpdateFastFading, "
			  "\n\t speed " << speed <<
			  "\n\t RBs " << numbOfSubChannels <<
			  "\n\t samples " << samplingTime <<
			  "\n\t start_point_freq " << start_point_freq <<
			  "\n\t start_point_time " << start_point_time <<
			  std::endl;
	#endif

	  //CASE CHANNEL TYPE = PedA
	  if (GetChannelType () == ChannelRealization::CHANNEL_TYPE_PED_A)
		{
		  for (int i = 0; i < numbOfSubChannels; i++)
			{
			  FastFadingForTimeDomain ff_time;
			  if (speed == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_0[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_3[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_30[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_120[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 150)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_150[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 200)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_200[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 250)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_250[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 300)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_300[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 350)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedA_speed_350[start_point_freq+i][start_point_time+j]);
					}
				}
			  m_fastFading->push_back (ff_time);
			}
		}

	  //CASE CHANNEL TYPE = PedB
	  if (GetChannelType () == ChannelRealization::CHANNEL_TYPE_PED_B)
		{
		  for (int i = 0; i < numbOfSubChannels; i++)
			{
			  FastFadingForTimeDomain ff_time;
			  if (speed == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedB_speed_0[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedB_speed_3[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedB_speed_30[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_PedB_speed_120[start_point_freq+i][start_point_time+j]);
					}
				}
			  m_fastFading->push_back (ff_time);
			}
		}

	  //CASE CHANNEL TYPE = VehA
	  if (GetChannelType () == ChannelRealization::CHANNEL_TYPE_VEH_A)
		{
		  for (int i = 0; i < numbOfSubChannels; i++)
			{
			  FastFadingForTimeDomain ff_time;
			  if (speed == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_0[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_3[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_30[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_120[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 150)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_150[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 200)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_200[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 250)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_250[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 300)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_300[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 350)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehA_speed_350[start_point_freq+i][start_point_time+j]);
					}
				}
			  m_fastFading->push_back (ff_time);
			}
		}

	  //CASE CHANNEL TYPE = VehB
	  if (GetChannelType () == ChannelRealization::CHANNEL_TYPE_VEH_B)
		{
		  for (int i = 0; i < numbOfSubChannels; i++)
			{
			  FastFadingForTimeDomain ff_time;
			  if (speed == 0)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_0[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 3)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_3[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 30)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_30[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 120)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_120[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 150)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_150[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 200)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_200[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 250)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_250[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 300)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_300[start_point_freq+i][start_point_time+j]);
					}
				}
			  if (speed == 350)
				{
				  for (int j = 0; j < samplingTime; j++)
					{
					  ff_time.push_back (
							  ff_VehB_speed_350[start_point_freq+i][start_point_time+j]);
					}
				}
			  m_fastFading->push_back (ff_time);
			}
		}
    }
}
