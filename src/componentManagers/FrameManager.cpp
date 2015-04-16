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



#include "FrameManager.h"
#include "../load-parameters.h"
#include "../device/ENodeB.h"
#include "../device/HeNodeB.h"

FrameManager* FrameManager::ptr=NULL;

FrameManager::FrameManager() {
  m_nbFrames = 0;
  m_nbSubframes = 0;
  m_TTICounter = 0;
  m_frameStructure = FrameManager::FRAME_STRUCTURE_FDD; //Default Value
  m_TDDFrameConfiguration = 1; //Default Value
  Simulator::Init()->Schedule(0.0, &FrameManager::Start, this);
}

FrameManager::~FrameManager()
{}

void
FrameManager::SetFrameStructure (FrameManager::FrameStructure frameStructure)
{
  m_frameStructure = frameStructure;
}

FrameManager::FrameStructure
FrameManager::GetFrameStructure (void) const
{
  return m_frameStructure;
}


void
FrameManager::SetTDDFrameConfiguration (int configuration)
{
  if (configuration < 0 && configuration > 6)
    {
	  m_TDDFrameConfiguration = 0; //Default Value
    }
  else
    {
	  m_TDDFrameConfiguration = configuration;
    }
}

int
FrameManager::GetTDDFrameConfiguration (void) const
{
  return m_TDDFrameConfiguration;
}

int
FrameManager::GetSubFrameType (int nbSubFrame)
{
  return TDDConfiguration [GetTDDFrameConfiguration ()][nbSubFrame-1];
}


void
FrameManager::UpdateNbFrames (void)
{
  m_nbFrames++;
}

int
FrameManager::GetNbFrames (void) const
{
  return m_nbFrames;
}

void
FrameManager::UpdateNbSubframes (void)
{
  m_nbSubframes++;
}

void
FrameManager::ResetNbSubframes (void)
{
  m_nbSubframes = 0;
}

int
FrameManager::GetNbSubframes (void) const
{
  return m_nbSubframes;
}

void
FrameManager::UpdateTTIcounter (void)
{
  m_TTICounter++;
}

unsigned long
FrameManager::GetTTICounter () const
{
  return m_TTICounter;
}

void
FrameManager::Start (void)
{
#ifdef FRAME_MANAGER_DEBUG
  std::cout << " LTE Simulation starts now! "<< std::endl;
#endif

  Simulator::Init()->Schedule(0.0, &FrameManager::StartFrame, this);
}

void
FrameManager::StartFrame (void)
{
  UpdateNbFrames ();

#ifdef FRAME_MANAGER_DEBUG
  std::cout << " +++++++++ Start Frame, time =  "
      << Simulator::Init()->Now() << " +++++++++" << std::endl;
#endif

  Simulator::Init()->Schedule(0.0,
							  &FrameManager::StartSubframe,
							  this);
}

void
FrameManager::StopFrame (void)
{
  Simulator::Init()->Schedule(0.0,
							  &FrameManager::StartFrame,
							  this);
}

void
FrameManager::StartSubframe (void)
{
#ifdef FRAME_MANAGER_DEBUG
  std::cout << " --------- Start SubFrame, time =  "
      << Simulator::Init()->Now() << " --------- " << std::endl;
#endif

  UpdateTTIcounter ();
  UpdateNbSubframes ();

  /*
   * At the beginning of each sub-frame the simulation
   * update user position. This function could be
   * implemented also every TTI.
   */
  //UpdateUserPosition(); --> moved to each UE class


#ifdef PLOT_USER_POSITION
  NetworkManager::Init()->PrintUserPosition();
#endif

  /*
   * According to the Frame Structure, the DW/UL link scheduler
   * will be called for each sub-frame.
   * (RBs allocation)
   */
  ResourceAllocation();
  Simulator::Init()->Schedule(0.001,
							  &FrameManager::StopSubframe,
							  this);
}

void
FrameManager::StopSubframe (void)
{
  if (GetNbSubframes () == 10)
    {
	  ResetNbSubframes ();
	  Simulator::Init()->Schedule(0.0,
								  &FrameManager::StopFrame,
								  this);
    }
  else
    {
	  Simulator::Init()->Schedule(0.0,
								  &FrameManager::StartSubframe,
								  this);
    }
}


NetworkManager*
FrameManager::GetNetworkManager (void)
{
  return NetworkManager::Init();
}

void
FrameManager::UpdateUserPosition(void)
{
  GetNetworkManager ()->UpdateUserPosition (GetTTICounter () / 1000.0);
}


void
FrameManager::ResourceAllocation(void)
{
  std::vector<ENodeB*> *records = GetNetworkManager ()->GetENodeBContainer ();
  std::vector<ENodeB*>::iterator iter;
  ENodeB *record;
  for (iter = records->begin (); iter != records->end (); iter++)
	{
	  record = *iter;

#ifdef FRAME_MANAGER_DEBUG
	  std::cout << " FRAME_MANAGER_DEBUG: RBs allocation for eNB " <<
		  record->GetIDNetworkNode() << std::endl;
#endif


	  if (GetFrameStructure () == FrameManager::FRAME_STRUCTURE_FDD)
		{
		  //record->ResourceBlocksAllocation ();
		  Simulator::Init()->Schedule(0.0, &ENodeB::ResourceBlocksAllocation,record);
		}
	  else
		{
		  //see frame configuration in TDDConfiguration
		  if (GetSubFrameType (GetNbSubframes ()) == 0)
			{
#ifdef FRAME_MANAGER_DEBUG
			  std::cout << " FRAME_MANAGER_DEBUG: SubFrameType = "
				  "	SUBFRAME_FOR_DOWNLINK " << std::endl;
#endif
			  //record->DownlinkResourceBlokAllocation();
			  Simulator::Init()->Schedule(0.0, &ENodeB::DownlinkResourceBlokAllocation,record);
			}
		  else if(GetSubFrameType (GetNbSubframes ()) == 1)
			{
#ifdef FRAME_MANAGER_DEBUG
			  std::cout << " FRAME_MANAGER_DEBUG: SubFrameType = "
				  "	SUBFRAME_FOR_UPLINK " << std::endl;
#endif
			  //record->UplinkResourceBlockAllocation();
			  Simulator::Init()->Schedule(0.0, &ENodeB::UplinkResourceBlockAllocation,record);
			}
		  else
			{
#ifdef FRAME_MANAGER_DEBUG
			  std::cout << " FRAME_MANAGER_DEBUG: SubFrameType = "
				  "	SPECIAL_SUBFRAME" << std::endl;
#endif
			}
		}
	}

  std::vector<HeNodeB*> *records_2 = GetNetworkManager ()->GetHomeENodeBContainer();
    std::vector<HeNodeB*>::iterator iter_2;
    HeNodeB *record_2;
    for (iter_2 = records_2->begin (); iter_2 != records_2->end (); iter_2++)
  	{
  	  record_2 = *iter_2;

  #ifdef FRAME_MANAGER_DEBUG
  	  std::cout << " FRAME_MANAGER_DEBUG: RBs allocation for eNB " <<
  		  record_2->GetIDNetworkNode() << std::endl;
  #endif


  	  if (GetFrameStructure () == FrameManager::FRAME_STRUCTURE_FDD)
  		{
  		  //record_2->ResourceBlocksAllocation ();
  		  Simulator::Init()->Schedule(0.0, &ENodeB::ResourceBlocksAllocation,record_2);
  		}
  	  else
  		{
  		  //see frame configuration in TDDConfiguration
  		  if (GetSubFrameType (GetNbSubframes ()) == 0)
  			{
  #ifdef FRAME_MANAGER_DEBUG
  			  std::cout << " FRAME_MANAGER_DEBUG: SubFrameType = "
  				  "	SUBFRAME_FOR_DOWNLINK " << std::endl;
  #endif
  			  //record_2->DownlinkResourceBlokAllocation();
  			  Simulator::Init()->Schedule(0.0, &ENodeB::DownlinkResourceBlokAllocation,record_2);
  			}
  		  else if(GetSubFrameType (GetNbSubframes ()) == 1)
  			{
  #ifdef FRAME_MANAGER_DEBUG
  			  std::cout << " FRAME_MANAGER_DEBUG: SubFrameType = "
  				  "	SUBFRAME_FOR_UPLINK " << std::endl;
  #endif
  			  //record_2->UplinkResourceBlockAllocation();
  			  Simulator::Init()->Schedule(0.0, &ENodeB::UplinkResourceBlockAllocation,record_2);
  			}
  		  else
  			{
  #ifdef FRAME_MANAGER_DEBUG
  			  std::cout << " FRAME_MANAGER_DEBUG: SubFrameType = "
  				  "	SPECIAL_SUBFRAME" << std::endl;
  #endif
  			}
  		}
  	}
}
