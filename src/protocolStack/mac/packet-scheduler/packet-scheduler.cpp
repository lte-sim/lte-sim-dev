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


#include "packet-scheduler.h"
#include "../mac-entity.h"
#include "../../packet/Packet.h"
#include "../../packet/packet-burst.h"
#include "../../../device/NetworkNode.h"
#include "../../../flows/radio-bearer.h"
#include "../../../protocolStack/rrc/rrc-entity.h"
#include "../../../flows/application/Application.h"
#include "../../../flows/MacQueue.h"
#include "../../../flows/QoS/QoSParameters.h"
#include "../../rlc/am-rlc-entity.h"

PacketScheduler::PacketScheduler()
{
  m_mac = NULL;
  m_flowsToSchedule = NULL;
}

PacketScheduler::~PacketScheduler()
{
  ClearFlowsToSchedule ();
  delete m_flowsToSchedule;
  m_mac = NULL;
}

void
PacketScheduler::Destroy (void)
{
  ClearFlowsToSchedule ();
  delete m_flowsToSchedule;
  m_mac = NULL;
}

void
PacketScheduler::SetMacEntity (MacEntity* mac)
{
  m_mac = mac;
}

MacEntity*
PacketScheduler::GetMacEntity (void)
{
  return m_mac;
}

void
PacketScheduler::Schedule (void)
{
  DoSchedule ();
}

void
PacketScheduler::DoSchedule (void)
{}

void
PacketScheduler::StopSchedule ()
{
  DoStopSchedule ();
}

void
PacketScheduler::DoStopSchedule ()
{}


PacketScheduler::FlowToSchedule::FlowToSchedule(RadioBearer* bearer, int dataToTransmit)
{
  m_bearer = bearer;
  m_allocatedBits = 0;
  m_transmittedData = 0;
  m_dataToTransmit = dataToTransmit;
}

PacketScheduler::FlowToSchedule::~FlowToSchedule()
{}

void
PacketScheduler::CreateFlowsToSchedule (void)
{
  m_flowsToSchedule = new FlowsToSchedule ();
}

void
PacketScheduler::DeleteFlowsToSchedule (void)
{
  ClearFlowsToSchedule ();
  delete  m_flowsToSchedule;
}

PacketScheduler::FlowsToSchedule*
PacketScheduler::GetFlowsToSchedule (void) const
{
  return m_flowsToSchedule;
}

void
PacketScheduler::ClearFlowsToSchedule ()
{
  FlowsToSchedule*  records = GetFlowsToSchedule ();
  FlowsToSchedule::iterator iter;

  for (iter = records->begin(); iter != records->end (); iter++)
    {
	  delete *iter;
    }

  GetFlowsToSchedule ()->clear ();
}

RadioBearer*
PacketScheduler::FlowToSchedule::GetBearer (void)
{
  return m_bearer;
}

void
PacketScheduler::FlowToSchedule::SetSpectralEfficiency (std::vector<double> s)
{
  m_spectralEfficiency = s;
}

std::vector<double>
PacketScheduler::FlowToSchedule::GetSpectralEfficiency (void)
{
  return m_spectralEfficiency;
}

void
PacketScheduler::FlowToSchedule::UpdateAllocatedBits (int allocatedBits)
{
  m_allocatedBits += allocatedBits;
  int availableBytes = m_allocatedBits/8;

  int transmittedPackets = ceil
			  (availableBytes/1513.0);

  m_transmittedData = availableBytes - (transmittedPackets * 8);
  if (m_transmittedData < 0)
    {
	  m_transmittedData = 0;
    }
}

int
PacketScheduler::FlowToSchedule::GetAllocatedBits (void) const
{
  return m_allocatedBits;
}

int
PacketScheduler::FlowToSchedule::GetTransmittedData (void) const
{
  return m_transmittedData;
}

void
PacketScheduler::FlowToSchedule::SetDataToTransmit (int dataToTransmit)
{
  m_dataToTransmit = dataToTransmit;
}

int
PacketScheduler::FlowToSchedule::GetDataToTransmit (void) const
{
  return m_dataToTransmit;
}

std::vector<int>*
PacketScheduler::FlowToSchedule::GetListOfAllocatedRBs ()
{
  return &m_listOfAllocatedRBs;
}

std::vector<int>*
PacketScheduler::FlowToSchedule::GetListOfSelectedMCS ()
{
  return &m_listOfSelectedMCS;
}

void
PacketScheduler::FlowToSchedule::SetCqiFeedbacks (std::vector<int> cqiFeedbacks)
{
  m_cqiFeedbacks = cqiFeedbacks;
}

std::vector<int>
PacketScheduler::FlowToSchedule::GetCqiFeedbacks (void)
{
  return m_cqiFeedbacks;
}

void
PacketScheduler::InsertFlowToSchedule (RadioBearer* bearer, int dataToTransmit, std::vector<double> specEff, std::vector<int> cqiFeedbacks)
{
#ifdef SCHEDULER_DEBUG
	std::cout << "\t  --> selected flow: "
			<< bearer->GetApplication ()->GetApplicationID ()
			<< " " << dataToTransmit << std::endl;
#endif

  FlowToSchedule *flowToSchedule = new FlowToSchedule(bearer, dataToTransmit);
  flowToSchedule->SetSpectralEfficiency (specEff);
  flowToSchedule->SetCqiFeedbacks (cqiFeedbacks);

  GetFlowsToSchedule ()->push_back(flowToSchedule);
}

void
PacketScheduler::UpdateAllocatedBits (FlowToSchedule* scheduledFlow, int allocatedBits,  int allocatedRB, int selectedMCS)
{
  scheduledFlow->UpdateAllocatedBits (allocatedBits);
  scheduledFlow->GetListOfAllocatedRBs()->push_back(allocatedRB);
  scheduledFlow->GetListOfSelectedMCS()->push_back(selectedMCS);
}


void
PacketScheduler::CheckForDLDropPackets ()
{
  RrcEntity *rrc = GetMacEntity ()->GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
  RrcEntity::RadioBearersContainer* bearers = rrc->GetRadioBearerContainer ();

  for (std::vector<RadioBearer* >::iterator it = bearers->begin (); it != bearers->end (); it++)
    {
	  //delete packets from queue
	  (*it)->GetMacQueue ()->CheckForDropPackets (
			  (*it)->GetQoSParameters ()->GetMaxDelay (), (*it)->GetApplication ()->GetApplicationID ());

	  //delete fragment waiting in AM RLC entity
	  if ((*it)->GetRlcEntity()->GetRlcModel() == RlcEntity::AM_RLC_MODE)
	    {
		  AmRlcEntity* amRlc = (AmRlcEntity*) (*it)->GetRlcEntity();
		  amRlc->CheckForDropPackets (
				  (*it)->GetQoSParameters ()->GetMaxDelay (), (*it)->GetApplication ()->GetApplicationID ());
	    }
    }
}
