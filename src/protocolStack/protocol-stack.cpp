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


#include "protocol-stack.h"
#include "../device/NetworkNode.h"

ProtocolStack::ProtocolStack()
{
  m_nas = NULL;
  m_rrc = NULL;
  m_mac = NULL;
  m_pdcp = NULL;
}

ProtocolStack::ProtocolStack(NetworkNode* device)
{
  m_rrc = new RrcEntity ();
  m_rrc->SetDevice (device);

  m_pdcp = new PdcpEntity ();

  m_nas = new NasEntity ();
  m_nas->SetDevice (device);

  if (device->GetNodeType() == NetworkNode::TYPE_ENODEB)
    {
      m_mac = new EnbMacEntity ();
      m_mac->SetDevice(device);
    }
  else if (device->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION)
  {
    m_mac = new HenbMacEntity ();
    m_mac->SetDevice(device);
  }

  else if (device->GetNodeType() == NetworkNode::TYPE_UE)
    {
	  m_mac = new UeMacEntity ();
	  m_mac->SetDevice(device);
    }
  else
    {
	  //ADD other devices
	  m_mac = NULL;
    }

}

ProtocolStack::~ProtocolStack()
{
  delete m_rrc;
  delete m_mac;
  delete m_nas;
  delete m_pdcp;
}

RrcEntity*
ProtocolStack::GetRrcEntity (void)
{
  return m_rrc;
}

MacEntity*
ProtocolStack::GetMacEntity (void)
{
  return m_mac;
}

NasEntity*
ProtocolStack::GetNasEntity (void)
{
  return m_nas;
}

PdcpEntity*
ProtocolStack::GetPdcpEntity (void)
{
  return m_pdcp;
}

void
ProtocolStack::SetNasEntity (NasEntity* e)
{
  m_nas = e;
}

void
ProtocolStack::SetRrcEntity (RrcEntity* e)
{
  m_rrc = e;
}

void
ProtocolStack::SetMacEntity (MacEntity* e)
{
  m_mac = e;
}

void
ProtocolStack::SetPdcpEntity (PdcpEntity* e)
{
  m_pdcp = e;
}
