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


#include "um-rlc-entity.h"
#include "../packet/packet-burst.h"
#include "../../flows/radio-bearer.h"
#include "../../flows/radio-bearer-sink.h"
#include "../../flows/MacQueue.h"
#include "../../flows/application/Application.h"
#include "../../device/NetworkNode.h"
#include "../../load-parameters.h"


UmRlcEntity::UmRlcEntity()
{
  SetDevice (NULL);
  SetRadioBearer (NULL);
  SetRlcPduSequenceNumber (0);
  SetRlcMode(RlcEntity::UM_RLC_MODE);
}

UmRlcEntity::~UmRlcEntity()
{
  ClearIncomingPackets ();
  Destroy ();
}

PacketBurst*
UmRlcEntity::TransmissionProcedure (int availableBytes)
{
#ifdef RLC_DEBUG
  std::cout << "UM RLC tx procedure for node " << GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode ()<< std::endl;
#endif

  PacketBurst* pb = new PacketBurst ();

  RadioBearer *bearer = (RadioBearer*) GetRadioBearerInstance ();
  MacQueue *queue = bearer->GetMacQueue ();

  if (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_INFINITE_BUFFER)
    {
	  //CREATE PACKET FOR THE INFINITE BUFFER SOURCE
	  while (true)
	    {
	      Packet *packet = bearer->CreatePacket (availableBytes);

	      //Set the id of the receiver RLC entity
	      packet->GetRLCHeader ()->SetRlcEntityIndex (GetRlcEntityIndex ());

	      packet->GetRLCHeader ()->SetRlcPduSequenceNumber (GetRlcPduSequenceNumber ());
	      int newSequenceNumber = GetRlcPduSequenceNumber () + 1;
	      SetRlcPduSequenceNumber (newSequenceNumber);

	      //Add MAC header
	      MACHeader *mac = new MACHeader (GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode (),
	    		                          GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode ());
	      packet->AddMACHeader(mac);
          packet->AddHeaderSize (3); //CRC

	      if (availableBytes > 1503)
	        {
	    	  packet->SetSize (1503);
	    	  packet->GetPacketTags ()->SetApplicationSize (1490);
	    	  //Set the id of the receiver RLC entity
		      packet->GetRLCHeader ()->SetRlcEntityIndex (GetRlcEntityIndex ());
		      packet->GetRLCHeader ()->SetRlcPduSequenceNumber (GetRlcPduSequenceNumber ());
		      int newSequenceNumber = GetRlcPduSequenceNumber () + 1;
		      SetRlcPduSequenceNumber (newSequenceNumber);

	    	  availableBytes -= 1503;
	          pb->AddPacket (packet);

	          if (_RLC_TRACING_)
	            {
	              std::cout << "TX UM_RLC SIZE" << packet->GetSize () <<
	        		  " B " << GetRlcEntityIndex () <<
	        		  " PDU_SN " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
	            }
	        }
	      else if (availableBytes > 13)
	        {
	    	  packet->SetSize (availableBytes);
	    	  packet->GetPacketTags ()->SetApplicationSize (availableBytes - 13);
	    	  //Set the id of the receiver RLC entity
		      packet->GetRLCHeader ()->SetRlcEntityIndex (GetRlcEntityIndex ());
		      packet->GetRLCHeader ()->SetRlcPduSequenceNumber (GetRlcPduSequenceNumber ());
		      int newSequenceNumber = GetRlcPduSequenceNumber () + 1;
		      SetRlcPduSequenceNumber (newSequenceNumber);

	    	  availableBytes = 0;
	    	  pb->AddPacket (packet);

	          if (_RLC_TRACING_)
	            {
	              std::cout << "TX UM_RLC SIZE" << packet->GetSize () <<
	        		  " B " << GetRlcEntityIndex () <<
	        		  " PDU_SN " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
	            }

	    	  break;
	        }
	      else
	        {
	    	  availableBytes = 0;
	    	  break;
	        }
	    }
    }
  else
    {
      while (availableBytes > 0 && !queue->IsEmpty ())
        {
	      Packet* packet = queue->GetPacketToTramsit (availableBytes);

	      if (packet != NULL)
	        {
	    	  //Set the id of the receiver RLC entity
		      packet->GetRLCHeader ()->SetRlcEntityIndex (GetRlcEntityIndex ());
		      packet->GetRLCHeader ()->SetRlcPduSequenceNumber (GetRlcPduSequenceNumber ());
		      int newSequenceNumber = GetRlcPduSequenceNumber () + 1;
		      SetRlcPduSequenceNumber (newSequenceNumber);

	          if (_RLC_TRACING_)
	            {
	              std::cout << "TX UM_RLC SIZE" << packet->GetSize () <<
	        		  " B " << GetRlcEntityIndex () <<
	        		  " PDU_SN " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
	            }

		      //Add MAC header
		      MACHeader *mac = new MACHeader (GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode (),
		    		                          GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode ());
		      packet->AddMACHeader(mac);
              packet->AddHeaderSize (3); //CRC

	    	  pb->AddPacket (packet);
	    	  availableBytes -= packet->GetSize ();
	        }
	      else
	        {
	    	  availableBytes = 0;
	        }
        }
    }

  return pb;
}


void
UmRlcEntity::ReceptionProcedure (Packet* p)
{
#ifdef RLC_DEBUG
  std::cout << "UM RLC rx procedure for node " << GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode ()<< std::endl;
  std::cout << "RECEIVE PACKET id " << p->GetID() << " frag n " << p->GetRLCHeader ()->GetFragmentNumber ()<< std::endl;
#endif

  if (_RLC_TRACING_)
    {
      std::cout << "RX UM_RLC SIZE " << p->GetSize () <<
		  " B " << GetRlcEntityIndex () <<
		  " PDU_SN " << p->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
    }

  if (m_incomingPacket.size() > 0 && p->GetID () != m_incomingPacket.at (0)->GetID ())
    {
#ifdef RLC_DEBUG
		  std::cout << "received a new packet, delete enqueued fragments"<< std::endl;
#endif

      Packet *pp = m_incomingPacket.at (0);

      if (_RLC_TRACING_)
        {
		  std::cout << "DROP_RX_UM_RLC";
		  if (pp->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_VOIP)
			  std::cout << " VOIP";
		  else if (pp->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_TRACE_BASED)
			  std::cout << " VIDEO";
		  else if (pp->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_CBR)
			  std::cout << " CBR";
		  else if (pp->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_INFINITE_BUFFER)
			  std::cout << " INF_BUF";
		  else
			  std::cout << " UNKNOW";

		  std::cout << " ID "<< pp->GetID()
				  << " B " << GetRlcEntityIndex ();

		  if (pp->GetPacketTags() != NULL
				  && pp->GetPacketTags()->GetApplicationType() ==
						  PacketTAGs::APPLICATION_TYPE_TRACE_BASED)
			{
			   std::cout << " FRAME " <<  pp->GetPacketTags()->GetFrameNuber()
						<< " START " << pp->GetPacketTags()->GetStartByte()
						<< " END " << pp->GetPacketTags()->GetEndByte();
			}
		  std::cout  <<  std::endl;
        }

	  ClearIncomingPackets ();
    }

  //The received packet is not a fragment
  if (!p->GetRLCHeader ()->IsAFragment ())
    {
#ifdef RLC_DEBUG
      std::cout << "\t received a packet " << std::endl;
#endif


      RadioBearerSink *bearer = (RadioBearerSink*) GetRadioBearerInstance ();
	  bearer->Receive (p);
    }

  //The received packet is a fragment
  if (p->GetRLCHeader ()->IsAFragment () && !p->GetRLCHeader ()->IsTheLatestFragment())
    {

#ifdef RLC_DEBUG
      std::cout << "\t received a fragment " << std::endl;
#endif
      m_incomingPacket.push_back (p);
    }

  //The received packet is the latest fragment
  if (p->GetRLCHeader ()->IsAFragment () && p->GetRLCHeader ()->IsTheLatestFragment())
    {

#ifdef RLC_DEBUG
       std::cout << "\t received the latest fragment " << std::endl;
#endif
	  m_incomingPacket.push_back (p);

	  //check if all fragment have been received
	  int numberOfPackets = p->GetRLCHeader ()->GetFragmentNumber () + 1;

	  if (m_incomingPacket.size () == numberOfPackets)
	    {
		  RadioBearerSink *bearer = (RadioBearerSink*) GetRadioBearerInstance ();
		  bearer->Receive (p->Copy ());

		  ClearIncomingPackets ();
	    }
	  else
	    {

#ifdef RLC_DEBUG
		  std::cout << "list of fragment incomplete -> delete all!"<< std::endl;
#endif

		  if (_RLC_TRACING_)
		    {
			  std::cout << "DROP_RX_UM_RLC";
			  if (p->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_VOIP)
				  std::cout << " VOIP";
			  else if (p->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_TRACE_BASED)
				  std::cout << " VIDEO";
			  else if (p->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_CBR)
				  std::cout << " CBR";
			  else if (p->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_INFINITE_BUFFER)
				  std::cout << " INF_BUF";
			  else
				  std::cout << " UNKNOW";

			  std::cout << " ID "<< p->GetID()
					  << " B " << GetRlcEntityIndex ();

			  if (p->GetPacketTags() != NULL
					  && p->GetPacketTags()->GetApplicationType() ==
							  PacketTAGs::APPLICATION_TYPE_TRACE_BASED)
				{
				   std::cout << " FRAME " <<  p->GetPacketTags()->GetFrameNuber()
							<< " START " << p->GetPacketTags()->GetStartByte()
							<< " END " << p->GetPacketTags()->GetEndByte();
				}
			  std::cout  <<  std::endl;
		    }

		  ClearIncomingPackets ();
	    }
    }
}

void
UmRlcEntity::ClearIncomingPackets (void)
{
  std::vector <Packet*>::iterator it;
  for (it = m_incomingPacket.begin (); it != m_incomingPacket.end (); it++)
    {
	  delete *it;
    }
  m_incomingPacket.clear ();
}
