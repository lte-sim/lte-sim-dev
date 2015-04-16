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
#include "../core/eventScheduler/simulator.h"
#include "../componentManagers/NetworkManager.h"
#include "../load-parameters.h"
#include "../protocolStack/packet/Packet.h"
#include "../protocolStack/packet/packet-burst.h"
#include "stdlib.h"
#include "../phy/lte-phy.h"
#include "../phy/enb-lte-phy.h"
#include "../phy/ue-lte-phy.h"
#include "../flows/radio-bearer-instance.h"


NetworkNode::NetworkNode()
{
  m_cell = NULL;
  m_protocolStack = NULL;
  m_phy = NULL;
  m_mobility = NULL;
}

NetworkNode::~NetworkNode()
{
  Destroy ();
}

void
NetworkNode::Destroy ()
{
  delete m_classifier;
  delete m_phy;
  delete m_protocolStack;
  delete m_mobility;
}

void
NetworkNode::SetIDNetworkNode (int id)
{
  m_idNetworkNode = id;
}

int
NetworkNode::GetIDNetworkNode (void) const
{
  return m_idNetworkNode;
}

void
NetworkNode::SetNodeType (NodeType type)
{
  m_nodeType = type;
}

NetworkNode::NodeType
NetworkNode::GetNodeType (void) const
{
  return m_nodeType;
}

void
NetworkNode::SetCell (Cell *cell)
{
  m_cell = cell;
}

Cell*
NetworkNode::GetCell (void)
{
  return m_cell;
}

void
NetworkNode::SetProtocolStack (ProtocolStack *stack)
{
  m_protocolStack = stack;
}

ProtocolStack*
NetworkNode::GetProtocolStack (void) const
{
  return m_protocolStack;
}


void
NetworkNode::SetClassifier (Classifier *classifier)
{
  m_classifier = classifier;
}

Classifier*
NetworkNode::GetClassifier (void) const
{
  return m_classifier;
}

void
NetworkNode::SetPhy (LtePhy *phy)
{
  m_phy = phy;
}


LtePhy*
NetworkNode::GetPhy (void)
{
  return m_phy;
}

void
NetworkNode::SetMobilityModel (Mobility* m)
{
  m_mobility = m;
}

Mobility*
NetworkNode::GetMobilityModel (void)
{
  return m_mobility;
}


void
NetworkNode::SetNodeState (NodeState state)
{
  m_state = state;
}

void
NetworkNode::MakeActive (void)
{
  SetNodeState (STATE_ACTIVE);
}

NetworkNode::NodeState
NetworkNode::GetNodeState (void) const
{
  return m_state;
}


void
NetworkNode::SendPacketBurst (PacketBurst* p)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "Node " << GetIDNetworkNode () << " sends burst" << std::endl;
#endif

  GetPhy ()->StartTx (p);
}

void NetworkNode::ReceivePacketBurst (PacketBurst* p)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "Node " << GetIDNetworkNode () << " receives burst" << std::endl;
#endif

  std::list<Packet* > packets = p->GetPackets ();
  std::list<Packet* >::iterator it;

  for (it = packets.begin (); it != packets.end (); it++)
    {
	  Packet* packet = (*it);

	  if (packet->GetDestinationMAC () == GetIDNetworkNode ())
	    {
		  if (packet->GetRLCHeader () == NULL)
            {
        	  // the received packet have not a RLC header (TM RLC Mode). It is not a fragment
			  // --> forward it to the classifier
        	  GetClassifier ()->Classify (packet->Copy ());
            }
          else
            {
        	  //The received packet could be a fragment. Forward it to a proper RLC entity
        	  int rlcEntityIndex = packet->GetRLCHeader ()->GetRlcEntityIndex ();
        	  RadioBearerInstance* bearer =
        			  GetProtocolStack ()->GetRrcEntity ()->GetRadioBearer (rlcEntityIndex);

        	  bearer->GetRlcEntity ()->ReceptionProcedure (packet->Copy ());
            }
	    }
	  else
	    {
		  //NOT FOR ME
	    }
    }
}
