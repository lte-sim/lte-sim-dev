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


#include "TraceBased.h"
#include "Trace/default-trace.h"
#include <cstring>
#include "../../componentManagers/NetworkManager.h"
#include "../radio-bearer.h"

#define MAXMTUSIZE 1490

TraceBased::TraceBased()
{
  m_sent = 0;
  m_entries = new std::vector<TraceEntry> ();
  LoadDefaultTrace ();
  m_frameCounter = 0;
  SetApplicationType (Application::APPLICATION_TYPE_TRACE_BASED);
}

TraceBased::~TraceBased()
{
  m_entries->clear ();
  delete m_entries;
  Destroy ();
}

void
TraceBased::DoStart (void)
{
  Simulator::Init()->Schedule(0.0, &TraceBased::Send, this);
}

void
TraceBased::DoStop (void)
{}

void
TraceBased::SetTraceFile(std::string traceFile)
{
  LoadTrace (traceFile);

#ifdef APPLICATION_DEBUG
  std::cout << " TRACE_BASED_DEBUG: Add file trace to Trace-Based "
		  "Application Client.....  DONE" << std::endl;
#endif

}

void
TraceBased::LoadTrace (std::string traceFile)
{
  if (m_entries->size() > 0)
    {
      m_entries->clear ();
    }

  uint32_t time, index, prevTime = 0;
  uint16_t size;
  char frameType;
  TraceEntry entry;
  std::ifstream ifTraceFile;
  ifTraceFile.open (traceFile.c_str (), std::ifstream::in);

  if (!ifTraceFile.good ())
    {
	  std::cout << " TRACE_BASED_APPLICATION ERROR BAD FILE"<< std::endl;
      LoadDefaultTrace ();
    }
  while (ifTraceFile.good ())
    {
      ifTraceFile >> index >> frameType >> time >> size;
      if (frameType == 'B' || frameType == 'PB')
        {
          entry.TimeToSend = 0;
        }
      else
        {
          entry.TimeToSend = time - prevTime;
          prevTime = time;
        }
      entry.PacketSize = size;
      entry.FrameIndex = index;
      entry.FrameType = frameType;
      m_entries->push_back (entry);
    }
  ifTraceFile.close ();
  iter = m_entries->begin ();
}

void
TraceBased::LoadDefaultTrace (void)
{
  uint32_t time, index, prevTime = 0;
  uint16_t size;
  char frameType;
  TraceEntry entry;
  for (int i = 0; i < NB_FRAME; i++)
    {
      index = i + 1;
      frameType = frameType_Tab[i];
      time = time_Tab[i];
      size = size_Tab[i];

      if (frameType == 'B')
        {
          entry.TimeToSend = 0;
        }
      else
        {
          entry.TimeToSend = time - prevTime;
          prevTime = time;
        }
      entry.PacketSize = size;
      entry.FrameIndex = index;
      entry.FrameType = frameType;
      m_entries->push_back (entry);
    }
  iter = m_entries->begin ();

}

void
TraceBased::ScheduleTransmit (double time)
{
  if ( (Simulator::Init()->Now () + time) < GetStopTime () )
    {
      Simulator::Init()->Schedule(time, &TraceBased::Send, this);
    }
}

void
TraceBased::Send (void)
{

  int dataOfFrameAlreadySent = 0;

  while (true)
    {
      for (int i = 0; i < (iter->PacketSize) / MAXMTUSIZE; i++)
		{
    	  //CREATE A NEW PACKET (ADDING UDP, IP and PDCP HEADERS)
    	  Packet *packet = new Packet ();
    	  int uid = Simulator::Init()->GetUID ();

    	  packet->SetID(uid);
    	  packet->SetTimeStamp (Simulator::Init()->Now ());
    	  packet->SetSize (MAXMTUSIZE);

      	  Trace (packet);

		  PacketTAGs *tags = new PacketTAGs ();
		  tags->SetApplicationType(PacketTAGs::APPLICATION_TYPE_TRACE_BASED);
		  tags->SetFrameNumber(GetFrameCounter());
		  tags->SetStartByte(dataOfFrameAlreadySent);
		  tags->SetEndByte(dataOfFrameAlreadySent+MAXMTUSIZE-1);
		  tags->SetApplicationSize (packet->GetSize ());
		  dataOfFrameAlreadySent+=MAXMTUSIZE;
		  packet->SetPacketTags(tags);


    	  UDPHeader *udp = new UDPHeader (GetClassifierParameters ()->GetSourcePort (),
    			                          GetClassifierParameters ()->GetDestinationPort ());
    	  packet->AddUDPHeader (udp);

    	  IPHeader *ip = new IPHeader (GetClassifierParameters ()->GetSourceID (),
    	                               GetClassifierParameters ()->GetDestinationID ());
    	  packet->AddIPHeader (ip);

    	  PDCPHeader *pdcp = new PDCPHeader ();
    	  packet->AddPDCPHeader (pdcp);

    	  GetRadioBearer()->Enqueue (packet);
		}

      uint16_t sizetosend = (iter->PacketSize) % MAXMTUSIZE;

      if (sizetosend > 0)
        {
    	  //CREATE A NEW PACKET (ADDING UDP, IP and PDCP HEADERS)
	      Packet *packet = new Packet ();
	      int uid = Simulator::Init()->GetUID ();

   	      packet->SetID(uid);
	      packet->SetTimeStamp (Simulator::Init()->Now ());
	      packet->SetSize (sizetosend);

	      Trace (packet);

	      PacketTAGs *tags = new PacketTAGs ();
	      tags->SetApplicationType(PacketTAGs::APPLICATION_TYPE_TRACE_BASED);
	      tags->SetFrameNumber(GetFrameCounter());
	      tags->SetStartByte(dataOfFrameAlreadySent);
	      tags->SetEndByte(dataOfFrameAlreadySent+sizetosend-1);
		  tags->SetApplicationSize (packet->GetSize ());
	      dataOfFrameAlreadySent+=sizetosend;
	      packet->SetPacketTags(tags);


	      UDPHeader *udp = new UDPHeader (GetClassifierParameters ()->GetSourcePort (),
										  GetClassifierParameters ()->GetDestinationPort ());
	      packet->AddUDPHeader (udp);

	      IPHeader *ip = new IPHeader (GetClassifierParameters ()->GetSourceID (),
									   GetClassifierParameters ()->GetDestinationID ());
	      packet->AddIPHeader (ip);

	      PDCPHeader *pdcp = new PDCPHeader ();
	      packet->AddPDCPHeader (pdcp);

	      GetRadioBearer()->Enqueue (packet);
        }

      iter++;
	  m_frameCounter++;
	  dataOfFrameAlreadySent = 0;

	  if (iter == m_entries->end ())
		{
		  iter = m_entries->begin ();
		}

	  if (iter->TimeToSend != 0)
		{
		  ScheduleTransmit ((iter->TimeToSend)*0.001);
		  break;
		}
    }
}

void
TraceBased::UpdateFrameCounter (void)
{
  m_frameCounter++;
}

int
TraceBased::GetFrameCounter (void)
{
  return m_frameCounter;
}

void
TraceBased::PrintTrace(void)
{
  std::cout << "Print Trace "<< std::endl;
  std::vector<TraceEntry>::iterator it;

  int n=0;
  for (it = m_entries->begin(); it != m_entries->end(); it++)
    {
	  std::cout << it->FrameIndex << " " <<it->FrameType
			  << " " << it->TimeToSend << " " << it->PacketSize << std::endl;
	  n++;
	  if (n==10)
		  return;
    }
}
