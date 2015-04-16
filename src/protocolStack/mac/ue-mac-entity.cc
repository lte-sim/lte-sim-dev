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


#include "ue-mac-entity.h"
#include "../packet/Packet.h"
#include "../packet/packet-burst.h"
#include "AMCModule.h"
#include "../../core/idealMessages/ideal-control-messages.h"
#include "../../device/NetworkNode.h"
#include "../../device/UserEquipment.h"
#include "../../phy/lte-phy.h"
#include "../../protocolStack/rrc/rrc-entity.h"
#include "../../flows/radio-bearer.h"
#include "../../flows/application/Application.h"
#include "../../flows/MacQueue.h"
#include "../../flows/QoS/QoSParameters.h"
#include "../rlc/am-rlc-entity.h"


UeMacEntity::UeMacEntity ()
{
  SetAmcModule (new AMCModule ());
  SetDevice (NULL);
  m_schedulingRequestInterval = 0.001;

  if (UPLINK)
    {
      Simulator::Init()->Schedule(GetSchedulingRequestInterval (),
  		                      &UeMacEntity::SendSchedulingRequest,
  		                      this);
    }
}


UeMacEntity::~UeMacEntity ()
{
  Destroy ();
}

double
UeMacEntity::GetSchedulingRequestInterval (void)
{
  return m_schedulingRequestInterval;
}

void
UeMacEntity::SetSchedulingRequestInterval (double t)
{
  m_schedulingRequestInterval = t;
}


void
UeMacEntity::SendSchedulingRequest ()
{

  ChechFroDropPackets ();

  //compute the buffer status report
  int bufferStaturReport = 0;
  RrcEntity *rrc = GetDevice ()->GetProtocolStack ()->GetRrcEntity ();

  if (rrc->GetRadioBearerContainer ()->size() > 0)
    {

	  for (RrcEntity::RadioBearersContainer::iterator it = rrc->GetRadioBearerContainer ()->begin();
			  it != rrc->GetRadioBearerContainer ()->end(); it++)
		{
		  RadioBearer *b = (*it);
		  if (b->GetApplication()->GetApplicationType ()
				  != Application::APPLICATION_TYPE_INFINITE_BUFFER)
			{
			  bufferStaturReport += b->GetQueueSize ();
			}
		  else
			{
			  bufferStaturReport += 10000000;
			}
		}
    }

  if (bufferStaturReport > 0)
    {
      //create the message
	  SchedulingRequestIdealControlMessage *msg = new SchedulingRequestIdealControlMessage ();
	  UserEquipment* thisNode = (UserEquipment*) GetDevice ();
	  msg->SetSourceDevice (thisNode);
	  msg->SetDestinationDevice (thisNode->GetTargetNode ());
      msg->SetBufferStatusReport (bufferStaturReport);

      //send the message
      GetDevice ()->GetPhy ()->SendIdealControlMessage (msg);
    }


  //schedule the next event
  Simulator::Init()->Schedule(GetSchedulingRequestInterval (),
		                      &UeMacEntity::SendSchedulingRequest,
		                      this);

}

void
UeMacEntity::ScheduleUplinkTransmission (int nbPrb, int mcs)
{
  int availableBytes = GetAmcModule ()->GetTBSizeFromMCS (mcs, nbPrb)/8;

  PacketBurst *pb = new PacketBurst ();
  RrcEntity *rrc = GetDevice ()->GetProtocolStack ()->GetRrcEntity ();

  if (rrc->GetRadioBearerContainer ()->size() > 0)
    {
	  for (RrcEntity::RadioBearersContainer::iterator it = rrc->GetRadioBearerContainer ()->begin();
			  it != rrc->GetRadioBearerContainer ()->end(); it++)
		{
		  RadioBearer *b = (*it);

		  if (availableBytes > 0)
		    {
			  RlcEntity *rlc = b->GetRlcEntity ();
			  PacketBurst* pb2 = rlc->TransmissionProcedure (availableBytes);
			  if (pb2->GetNPackets () > 0)
				{
				  std::list<Packet*> packets = pb2->GetPackets ();
				  std::list<Packet* >::iterator it;
				  for (it = packets.begin (); it != packets.end (); it++)
					{
					  Packet *p = (*it);
					  pb->AddPacket (p->Copy ());
					}
				}
			  availableBytes -= pb2->GetSize ();
			  delete pb2;
		    }
		}

	  GetDevice ()->SendPacketBurst (pb);
    }
}

void
UeMacEntity::ChechFroDropPackets (void)
{
  RrcEntity *rrc = GetDevice ()->GetProtocolStack ()->GetRrcEntity ();
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
