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


#include "NetworkNode.h"
#include "UserEquipment.h"
#include "ENodeB.h"
#include "Gateway.h"
#include "../protocolStack/mac/packet-scheduler/packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/mt-uplink-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-pf-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-mlwdf-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-exp-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-fls-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/exp-rule-downlink-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/log-rule-downlink-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/enhanced-uplink-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/roundrobin-uplink-packet-scheduler.h"
#include "../phy/enb-lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../protocolStack/packet/packet-burst.h"

ENodeB::ENodeB ()
{}

ENodeB::ENodeB (int idElement,
			    Cell *cell)
{
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_ENODEB);
  SetCell (cell);

  CartesianCoordinates *position = new CartesianCoordinates(cell->GetCellCenterPosition ()->GetCoordinateX (),
		                                                    cell->GetCellCenterPosition ()->GetCoordinateY ());
  Mobility* m = new ConstantPosition ();
  m->SetAbsolutePosition (position);
  SetMobilityModel (m);
  delete position;

  m_userEquipmentRecords = new UserEquipmentRecords;

  EnbLtePhy *phy = new EnbLtePhy ();
  phy->SetDevice (this);
  SetPhy (phy);

  ProtocolStack *stack = new ProtocolStack (this);
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  classifier->SetDevice (this);
  SetClassifier (classifier);
}

ENodeB::ENodeB (int idElement,
				Cell *cell,
				double posx,
				double posy)
{
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_ENODEB);
  SetCell (cell);

  CartesianCoordinates *position = new CartesianCoordinates(posx, posy);
  Mobility* m = new ConstantPosition ();
  m->SetAbsolutePosition (position);
  SetMobilityModel (m);


  m_userEquipmentRecords = new UserEquipmentRecords;

  EnbLtePhy *phy = new EnbLtePhy ();
  phy->SetDevice(this);
  SetPhy (phy);

  ProtocolStack *stack = new ProtocolStack (this);
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  classifier->SetDevice (this);
  SetClassifier (classifier);
}

ENodeB::~ENodeB()
{
  Destroy ();
  m_userEquipmentRecords->clear();
  delete m_userEquipmentRecords;
}

void
ENodeB::RegisterUserEquipment (UserEquipment *UE)
{
  UserEquipmentRecord *record = new UserEquipmentRecord (UE);
  GetUserEquipmentRecords ()->push_back(record);
}

void
ENodeB::DeleteUserEquipment (UserEquipment *UE)
{
  UserEquipmentRecords *records = GetUserEquipmentRecords ();
  UserEquipmentRecord *record;
  UserEquipmentRecords::iterator iter;

  UserEquipmentRecords *new_records = new UserEquipmentRecords ();

  for (iter = records->begin (); iter != records->end (); iter++)
    {
	  record = *iter;
	  if (record->GetUE ()->GetIDNetworkNode () != UE->GetIDNetworkNode ())
	    {
          //records->erase(iter);
          //break;
		  new_records->push_back (record);
	    }
	  else
	    {
		  delete record;
	    }
    }

  m_userEquipmentRecords->clear ();
  delete m_userEquipmentRecords;
  m_userEquipmentRecords = new_records;
}

int
ENodeB::GetNbOfUserEquipmentRecords (void)
{
  return GetUserEquipmentRecords ()->size();
}

void
ENodeB::CreateUserEquipmentRecords (void)
{
  m_userEquipmentRecords = new UserEquipmentRecords ();
}

void
ENodeB::DeleteUserEquipmentRecords (void)
{
  m_userEquipmentRecords->clear ();
  delete m_userEquipmentRecords;
}

ENodeB::UserEquipmentRecords*
ENodeB::GetUserEquipmentRecords (void)
{
  return m_userEquipmentRecords;
}

ENodeB::UserEquipmentRecord*
ENodeB::GetUserEquipmentRecord (int idUE)
{
  UserEquipmentRecords *records = GetUserEquipmentRecords ();
  UserEquipmentRecord *record;
  UserEquipmentRecords::iterator iter;

  for (iter = records->begin (); iter != records->end (); iter++)
	{
	  record = *iter;
	  if (record->GetUE ()->
			  GetIDNetworkNode () == idUE)
		{
		  return record;
		}
	}
  return false;
}


ENodeB::UserEquipmentRecord::UserEquipmentRecord ()
{
  m_UE = NULL;
  //Create initial CQI values:
  m_cqiFeedback.clear ();
  m_uplinkChannelStatusIndicator.clear ();
  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

ENodeB::UserEquipmentRecord::~UserEquipmentRecord ()
{
  m_cqiFeedback.clear ();
  m_uplinkChannelStatusIndicator.clear();
}

ENodeB::UserEquipmentRecord::UserEquipmentRecord (UserEquipment *UE)
{
  m_UE = UE;
  BandwidthManager *s = m_UE->GetPhy ()->GetBandwidthManager ();

  int nbRbs = s->GetDlSubChannels ().size ();
  m_cqiFeedback.clear ();
  for (int i = 0; i < nbRbs; i++ )
    {
	  m_cqiFeedback.push_back (10);
    }

  nbRbs = s->GetUlSubChannels ().size ();
  m_uplinkChannelStatusIndicator.clear ();
  for (int i = 0; i < nbRbs; i++ )
    {
	  m_uplinkChannelStatusIndicator.push_back (10.);
    }

  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

void
ENodeB::UserEquipmentRecord::SetUE (UserEquipment *UE)
{
  m_UE = UE;
}

UserEquipment*
ENodeB::UserEquipmentRecord::GetUE (void) const
{
  return m_UE;
}

void
ENodeB::UserEquipmentRecord::SetCQI (std::vector<int> cqi)
{
  m_cqiFeedback = cqi;
}

std::vector<int>
ENodeB::UserEquipmentRecord::GetCQI (void) const
{
 return m_cqiFeedback;
}

int
ENodeB::UserEquipmentRecord::GetSchedulingRequest (void)
{
  return m_schedulingRequest;
}

void
ENodeB::UserEquipmentRecord::SetSchedulingRequest (int r)
{
  m_schedulingRequest = r;
}

void
ENodeB::UserEquipmentRecord::UpdateSchedulingGrants (int b)
{
  m_averageSchedulingGrants = (0.9 * m_averageSchedulingGrants)	+ (0.1 * b);
}

int
ENodeB::UserEquipmentRecord::GetSchedulingGrants (void)
{
  return m_averageSchedulingGrants;
}

void
ENodeB::UserEquipmentRecord::SetUlMcs (int mcs)
{
  m_ulMcs = mcs;
}

int
ENodeB::UserEquipmentRecord::GetUlMcs (void)
{
  return m_ulMcs;
}

void
ENodeB::UserEquipmentRecord::SetUplinkChannelStatusIndicator (std::vector<double> vet)
{
  m_uplinkChannelStatusIndicator = vet;
}

std::vector<double>
ENodeB::UserEquipmentRecord::GetUplinkChannelStatusIndicator (void) const
{
 return m_uplinkChannelStatusIndicator;
}

void
ENodeB::SetDLScheduler (ENodeB::DLSchedulerType type)
{
  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();
  PacketScheduler *scheduler;
  switch (type)
    {
      case ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR:
    	scheduler = new  DL_PF_PacketScheduler ();
    	scheduler->SetMacEntity (mac);
    	mac->SetDownlinkPacketScheduler (scheduler);
	    break;

      case ENodeB::DLScheduler_TYPE_FLS:
      	scheduler = new  DL_FLS_PacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
        break;

      case ENodeB::DLScheduler_TYPE_EXP:
      	scheduler = new  DL_EXP_PacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case ENodeB::DLScheduler_TYPE_MLWDF:
      	scheduler = new  DL_MLWDF_PacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case ENodeB::DLScheduler_EXP_RULE:
      	scheduler = new  ExpRuleDownlinkPacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case ENodeB::DLScheduler_LOG_RULE:
      	scheduler = new  LogRuleDownlinkPacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

	  default:
	    std::cout << "ERROR: invalid scheduler type" << std::endl;
    	scheduler = new  DL_PF_PacketScheduler ();
    	scheduler->SetMacEntity (mac);
    	mac->SetDownlinkPacketScheduler (scheduler);
	    break;
    }
}

PacketScheduler*
ENodeB::GetDLScheduler (void) const
{
  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();
  return mac->GetDownlinkPacketScheduler ();
}

void
ENodeB::SetULScheduler (ULSchedulerType type)
{
  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();
  PacketScheduler *scheduler;
  switch (type)
	{
	  case ENodeB::ULScheduler_TYPE_MAXIMUM_THROUGHPUT:
		scheduler = new MaximumThroughputUplinkPacketScheduler ();
		scheduler->SetMacEntity (mac);
		mac->SetUplinkPacketScheduler (scheduler);
		break;
	  case ENodeB::ULScheduler_TYPE_FME:
	    scheduler = new EnhancedUplinkPacketScheduler();
	    scheduler->SetMacEntity (mac);
	    mac->SetUplinkPacketScheduler (scheduler);
	    break;
	  case ENodeB::ULScheduler_TYPE_ROUNDROBIN:
	    scheduler = new RoundRobinUplinkPacketScheduler ();
	    scheduler->SetMacEntity (mac);
	    mac->SetUplinkPacketScheduler (scheduler);
	    break;
	  default:
		std::cout << "ERROR: invalid scheduler type" << std::endl;
		scheduler = new  MaximumThroughputUplinkPacketScheduler ();
		scheduler->SetMacEntity (mac);
		mac->SetUplinkPacketScheduler (scheduler);
		break;
	}
}

PacketScheduler*
ENodeB::GetULScheduler (void) const
{
  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();
  return mac->GetUplinkPacketScheduler ();
}

void
ENodeB::ResourceBlocksAllocation (void)
{
  DownlinkResourceBlokAllocation ();
  UplinkResourceBlockAllocation ();
}

void
ENodeB::UplinkResourceBlockAllocation (void)
{
  if (GetULScheduler () != NULL && GetNbOfUserEquipmentRecords () > 0)
   {
	  GetULScheduler ()->Schedule();
   }
}

void
ENodeB::DownlinkResourceBlokAllocation (void)
{
  if (GetDLScheduler () != NULL && GetNbOfUserEquipmentRecords () > 0)
   {
	  GetDLScheduler ()->Schedule();
   }
  else
    {
	  //send only reference symbols
	  //PacketBurst *pb = new PacketBurst ();
	  //SendPacketBurst (pb);
    }
}

//Debug
void
ENodeB::Print (void)
{
  std::cout << " ENodeB object:"
      "\n\t m_idNetworkNode = " << GetIDNetworkNode () <<
	  "\n\t m_idCell = " << GetCell ()->GetIdCell () <<
	  "\n\t Served Users: " <<
  std::endl;

  vector<UserEquipmentRecord*>* records = GetUserEquipmentRecords ();
  UserEquipmentRecord *record;
  vector<UserEquipmentRecord*>::iterator iter;
  for (iter = records->begin (); iter != records->end (); iter++)
    {
  	  record = *iter;
	  std::cout << "\t\t idUE = " << record->GetUE ()->
			  GetIDNetworkNode () << std::endl;
    }
}
