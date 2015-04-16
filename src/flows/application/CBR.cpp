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



#include "CBR.h"
#include <cstdlib>
#include "../../componentManagers/NetworkManager.h"
#include "../radio-bearer.h"

CBR::CBR()
{
  SetApplicationType (Application::APPLICATION_TYPE_CBR);
}

CBR::~CBR()
{
  Destroy ();
}

void
CBR::DoStart (void)
{
  Simulator::Init()->Schedule(0.0, &CBR::Send, this);
}

void
CBR::DoStop (void)
{
}

void
CBR::ScheduleTransmit (double time)
{
  if ( (Simulator::Init()->Now () + time) < GetStopTime () )
    {
      Simulator::Init()->Schedule(time, &CBR::Send, this);
    }
}

void
CBR::Send (void)
{
  //CREATE A NEW PACKET (ADDING UDP, IP and PDCP HEADERS)
  Packet *packet = new Packet ();
  int uid = Simulator::Init()->GetUID ();

  packet->SetID(uid);
  packet->SetTimeStamp (Simulator::Init()->Now ());
  packet->SetSize (GetSize ());

  PacketTAGs *tags = new PacketTAGs ();
  tags->SetApplicationType(PacketTAGs::APPLICATION_TYPE_CBR);
  tags->SetApplicationSize (packet->GetSize ());
  packet->SetPacketTags(tags);


  UDPHeader *udp = new UDPHeader (GetClassifierParameters ()->GetSourcePort (),
		                          GetClassifierParameters ()->GetDestinationPort ());
  packet->AddUDPHeader (udp);

  IPHeader *ip = new IPHeader (GetClassifierParameters ()->GetSourceID (),
                               GetClassifierParameters ()->GetDestinationID ());
  packet->AddIPHeader (ip);

  PDCPHeader *pdcp = new PDCPHeader ();
  packet->AddPDCPHeader (pdcp);

  Trace (packet);

  GetRadioBearer()->Enqueue (packet);

  ScheduleTransmit (GetInterval ());

}


int
CBR::GetSize (void) const
{
  return m_size;
}

void
CBR::SetSize(int size)
{
  m_size = size;
}
void
CBR::SetInterval(double interval)
{
  m_interval = interval;
}

double
CBR::GetInterval (void) const
{
  return m_interval;
}
