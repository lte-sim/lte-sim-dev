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


#include "LteChannel.h"
#include "../core/spectrum/transmitted-signal.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../device/NetworkNode.h"
#include "../phy/lte-phy.h"
#include "../core/eventScheduler/simulator.h"
#include "../load-parameters.h"
#include "propagation-model/propagation-loss-model.h"

LteChannel::LteChannel()
{
  m_attachedDevices = new std::vector<NetworkNode*> ();
  m_propagationLossModel = new PropagationLossModel ();
}

LteChannel::~LteChannel()
{
  m_attachedDevices->clear ();
  delete m_attachedDevices;
  delete m_propagationLossModel;
}

void
LteChannel::StartTx (PacketBurst* p, TransmittedSignal* txSignal, NetworkNode* src)
{
#ifdef TEST_DEVICE_ON_CHANNEL

  std::cout << "LteChannel::StartTx ch " << GetChannelId () << std::endl;
#endif

  Simulator::Init()->Schedule(0.001,
	 						  &LteChannel::StartRx,
	  						  this,
	 						  p,
	  						  txSignal,
	  						  src);
#ifdef TRANSMISSION_DEBUG
  std::cout << "   =======  channel  =======" << std::endl;
#endif

}

void
LteChannel::StartRx (PacketBurst* p, TransmittedSignal* txSignal, NetworkNode* src)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "LteChannel::StartRx ch " << GetChannelId () << std::endl;
#endif

  for (std::vector<NetworkNode*>::iterator it = GetDevices ()->begin();
		  it != GetDevices ()->end (); it++)
    {
	  NetworkNode* dst = *it;

#ifdef TEST_DEVICE_ON_CHANNEL
	  std::cout << "\t Node " << dst->GetIDNetworkNode () << " is attached" << std::endl;
#endif

	  //APPLY THE PROPAGATION LOSS MODEL
	  TransmittedSignal* rxSignal;
	  if (m_propagationLossModel != NULL)
	    {
#ifdef TEST_DEVICE_ON_CHANNEL
          std::cout << "LteChannel::StartRx add propagation loss" << std::endl;
#endif
		  rxSignal = GetPropagationLossModel ()->AddLossModel (src, dst, txSignal);
	    }
	  else
	    {
		  rxSignal = txSignal->Copy ();
	    }

	  //DELIVERY THE BURST OF PACKETS
	  dst->GetPhy ()->StartRx (p->Copy (), rxSignal);
    }

  delete p;
}

void
LteChannel::AddDevice (NetworkNode* d)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "LteChannel " << m_channelId << " ADD Node " << d->GetIDNetworkNode ()<< std::endl;
#endif

  m_attachedDevices->push_back(d);
}

void
LteChannel::DelDevice (NetworkNode* d)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "LteChannel " << m_channelId << " DEL Node " << d->GetIDNetworkNode ()<< std::endl;
#endif

  std::vector<NetworkNode*> *new_list = new std::vector<NetworkNode*> ();
  for (std::vector<NetworkNode*>::iterator it = GetDevices ()->begin();
		  it != GetDevices ()->end (); it++)
    {
	  NetworkNode* node = *it;

	  if (node->GetIDNetworkNode () != d->GetIDNetworkNode ())
	    {
		  new_list->push_back (node);
	    }
    }

  m_attachedDevices->clear ();
  delete m_attachedDevices;
  m_attachedDevices = new_list;
}

bool
LteChannel::IsAttached (NetworkNode* d)
{
  for (std::vector<NetworkNode*>::iterator it = GetDevices ()->begin();
		  it != GetDevices ()->end (); it++)
    {
	  NetworkNode* node = *it;

	  if (node->GetIDNetworkNode () == d->GetIDNetworkNode ())
	    {
#ifdef TEST_DEVICE_ON_CHANNEL
          std::cout << "LteChannel find Node " << d->GetIDNetworkNode ()<< std::endl;
#endif
		  return true;
	    }
    }
  return false;
}

std::vector<NetworkNode*>*
LteChannel::GetDevices (void)
{
  return m_attachedDevices;
}

void
LteChannel::SetPropagationLossModel (PropagationLossModel* m)
{
  m_propagationLossModel = m;
}

PropagationLossModel*
LteChannel::GetPropagationLossModel (void)
{
  return m_propagationLossModel;
}

void
LteChannel::SetChannelId (int id)
{
  m_channelId = id;
}

int
LteChannel::GetChannelId (void)
{
  return m_channelId;
}
