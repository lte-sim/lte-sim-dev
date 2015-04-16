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

#include "am-rlc-entity.h"
#include "../packet/packet-burst.h"
#include "../../flows/radio-bearer.h"
#include "../../flows/radio-bearer-sink.h"
#include "../../flows/MacQueue.h"
#include "../../flows/application/Application.h"
#include "../../flows/application/application-sink.h"
#include "../../flows/QoS/QoSParameters.h"
#include "../../device/NetworkNode.h"
#include "amd-record.h"
#include "../../core/idealMessages/ideal-control-messages.h"
#include "../../load-parameters.h"

#define MAX_AMD_RETX 5

AmRlcEntity::AmRlcEntity()
{
  SetDevice (NULL);
  SetRadioBearer (NULL);
  SetRlcMode(RlcEntity::AM_RLC_MODE);
  SetRlcPduSequenceNumber (0);
  m_amStateVariables = new AmStateVariables ();
  m_sentAMDs = new std::vector<AmdRecord*> ();
  m_receivedAMDs = new std::vector<AmdRecord*> ();

}

AmRlcEntity::~AmRlcEntity()
{
  delete m_amStateVariables;
  ClearPacketList ();
  Destroy ();
}

void
AmRlcEntity::ClearPacketList (void)
{
  std::vector <AmdRecord*>::iterator it;
  for (it = m_sentAMDs->begin (); it != m_sentAMDs->end (); it++)
	{
	  delete *it;
	}
  m_sentAMDs->clear ();

  for (it = m_receivedAMDs->begin (); it != m_receivedAMDs->end (); it++)
	{
	  delete *it;
	}
  m_receivedAMDs->clear ();
}

void
AmRlcEntity::PrintSentAMDs (void)
{
  std::cout << "\t PrintSentAMDs" << std::endl;
  std::vector <AmdRecord*>::iterator it;
  for (it = m_sentAMDs->begin (); it != m_sentAMDs->end (); it++)
	{
	  Packet* packet = (*it)->m_packet;
	  std::cout << "\t\t *** pkt " << packet->GetID() << " frag " << packet->GetRLCHeader ()->GetFragmentNumber () <<
			  " sn " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber () <<
			  " startB " << packet->GetRLCHeader ()->GetStartByte () <<
			  " endB " << packet->GetRLCHeader ()->GetEndByte () << std::endl;

	}
}

void
AmRlcEntity::PrintReceivedAMDs (void)
{
  std::cout << "\t PrintReceivedAMDs" << std::endl;
  std::vector <AmdRecord*>::iterator it;
  for (it = m_receivedAMDs->begin (); it != m_receivedAMDs->end (); it++)
	{
	  Packet* packet = (*it)->m_packet;
	  std::cout << "\t\t *** pkt " << packet->GetID() << " frag " << packet->GetRLCHeader ()->GetFragmentNumber () <<
			  " sn " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber () <<
			  " startB " << packet->GetRLCHeader ()->GetStartByte () <<
			  " endB " << packet->GetRLCHeader ()->GetEndByte () << std::endl;

	}
}


AmStateVariables*
AmRlcEntity::GetAmStateVariables ()
{
  return m_amStateVariables;
}

std::vector<AmdRecord*>*
AmRlcEntity::GetSentAMDs (void)
{
  return m_sentAMDs;
}

std::vector<AmdRecord*>*
AmRlcEntity::GetReceivedAMDs (void)
{
  return m_receivedAMDs;
}

void
AmRlcEntity::InsertAMDIntoReceptionList (AmdRecord* amd)
{
#ifdef RLC_DEBUG
  std::cout  << "InsertAMDIntoReceptionList" << std::endl;
  PrintReceivedAMDs ();
#endif

  int amds = m_receivedAMDs->size ();

  if (amds == 0)
    {
	  //std::cout << "no AMD into the list" << std::endl;
	  m_receivedAMDs->insert (m_receivedAMDs->begin (), amd);
	  return;
    }

  for (int i = 0; i < amds; i++)
	{
	  Packet* packet = m_receivedAMDs->at (i)->m_packet;

	  if (amd->m_packet->GetRLCHeader ()->GetRlcPduSequenceNumber () < packet->GetRLCHeader ()->GetRlcPduSequenceNumber ())
	    {
		  //std::cout << "insert the AMD" << std::endl;
		  m_receivedAMDs->insert (m_receivedAMDs->begin () + i, amd);
		  return;
	    }
	  else if (amd->m_packet->GetRLCHeader ()->GetRlcPduSequenceNumber () > packet->GetRLCHeader ()->GetRlcPduSequenceNumber ()
			  && i == amds - 1)
	    {
		  //std::cout << "insert the AMD at the end of the queue" << std::endl;
		  m_receivedAMDs->insert (m_receivedAMDs->end (), amd);
          return;
	    }
	  else if (amd->m_packet->GetRLCHeader ()->GetRlcPduSequenceNumber () == packet->GetRLCHeader ()->GetRlcPduSequenceNumber ())
	    {
		  if (amd->m_packet->GetRLCHeader ()->GetEndByte () < packet->GetRLCHeader ()->GetStartByte ())
		    {
			  //std::cout << "insert before an AMD with the same SN" << std::endl;
			  m_receivedAMDs->insert (m_receivedAMDs->begin () + i, amd);
			  return;
		    }
		  else if (i == amds - 1)
		    {
			  //std::cout << "insert the AMD at the end of the queue" << std::endl;
			  m_receivedAMDs->insert (m_receivedAMDs->end (), amd);
	          return;
		    }
	    }
	  else
	    {}
	}

#ifdef RLC_DEBUG
  PrintReceivedAMDs ();
#endif
}

PacketBurst*
AmRlcEntity::TransmissionProcedure (int availableBytes)
{
#ifdef RLC_DEBUG
  std::cout << "AM RLC tx procedure for node " << GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode ()<< " bearer "<<  GetRlcEntityIndex () << std::endl;
#endif

  PacketBurst* pb = new PacketBurst ();
  AmStateVariables* amRlcState = GetAmStateVariables ();

  //STEP 1 CHECK FOR RE-TRANSMISSION
#ifdef RLC_DEBUG
  std::cout << "--> STEP 1:  CHECK FOR RE-TRANSMISSION" << std::endl;
  PrintSentAMDs ();
#endif

  int nbSentAMDs = GetSentAMDs()->size();
  if (nbSentAMDs != 0)
    {
      int amdId = 0;
	  while (availableBytes > 0 && amdId < nbSentAMDs)
        {
    	  AmdRecord* amdRecord = GetSentAMDs()->at (amdId);

    	  if (amdRecord->m_packet->GetSize () + 6 <= availableBytes) //6 = MAC  + CRC overhead
    	    {
    	      amdRecord->m_retx_count++;
#ifdef RLC_DEBUG
    		  std::cout << "send the whole unacknowledged AMD PDU" << std::endl;
#endif
    		  Packet* p = amdRecord->m_packet->Copy ();
    		  MACHeader *mac = new MACHeader (GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode (),
    	    		                          GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode ());
    	      p->AddMACHeader(mac);
    	      p->AddHeaderSize (3);

    	      if (_RLC_TRACING_)
    	        {
	              std::cout << "TX AM_RLC SIZE" << p->GetSize () <<
	        		    " B " << GetRlcEntityIndex () <<
	        		    " PDU_SN " << p->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
    	        }

    	      pb->AddPacket (p);
    	      availableBytes -= p->GetSize ();
    	      amdId++;
    	    }
    	  else if (availableBytes > 8) // 8 = RLC + MAC + CRC
    	    {

    	      amdRecord->m_retx_count++;
#ifdef RLC_DEBUG
    		  std::cout << "there is not enough bandwidth for this AMD PDU. Do another fragmentation"
    				  << "--> bytes: " << availableBytes << std::endl;
#endif
    		  Packet* p1 = amdRecord->m_packet->Copy ();
    		  Packet* p2 = amdRecord->m_packet;

    		  int sentBytes = availableBytes - 7;
    		  p1->GetRLCHeader ()->SetEndByte (p1->GetRLCHeader ()->GetStartByte () + sentBytes - 1);
              p1->SetSize (2 + sentBytes); //consider both RLC and sent bytes
    		  p2->GetRLCHeader ()->SetStartByte (p1->GetRLCHeader ()->GetStartByte () + sentBytes);
    		  p2->SetSize (p2->GetSize () - sentBytes); //decrease the packet size by the quota of sent bytes
    		  p1->GetRLCHeader ()->SetTheLatestFragment (false);

    		  p1->GetRLCHeader ()->SetAFragment (true);
    		  p2->GetRLCHeader ()->SetAFragment (true);

       		  MACHeader *mac = new MACHeader (GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode (),
      	    		                          GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode ());
       	      p1->AddMACHeader(mac);
       	      p1->AddHeaderSize (3); //CRC

       	      if (_RLC_TRACING_)
       	        {
                  std::cout << "TX AM_RLC SIZE" << p1->GetSize () <<
       	    		 " B " << GetRlcEntityIndex () <<
       	    		 " PDU_SN " << p1->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
       	        }

       	      pb->AddPacket (p1->Copy ());

    		  AmdRecord* newAmdRecord = new AmdRecord (p1, p1->GetRLCHeader ()->GetRlcPduSequenceNumber ());
    		  newAmdRecord->m_retx_count = amdRecord->m_retx_count;
    		  GetSentAMDs ()->insert(GetSentAMDs ()->begin() + amdId, newAmdRecord);

#ifdef RLC_DEBUG
       	      PrintSentAMDs ();
#endif
       	      availableBytes = 0;
    	    }
    	  else
    	    {
    		  availableBytes = 0;
    	    }

        }
    }



  if (availableBytes <= 0 ) return pb;


  //STEP 2 NEW TRANSMISSION
#ifdef RLC_DEBUG
  std::cout << "--> STEP 2:  CHECK FOR NEW TRANSMISSION" << std::endl;
#endif

  RadioBearer *bearer = (RadioBearer*) GetRadioBearerInstance ();
  MacQueue *queue = bearer->GetMacQueue ();

  if (bearer->GetApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_INFINITE_BUFFER)
    {
	  //CREATE PACKET FOR THE INFINITE BUFFER SOURCE
	  while (true)
	    {
	      Packet *packet = bearer->CreatePacket (availableBytes);
	      packet->GetRLCHeader ()->SetRlcEntityIndex (GetRlcEntityIndex ());

          // set sn
	      int currentSN = amRlcState->m_vt_s;
	      packet->GetRLCHeader ()->SetRlcPduSequenceNumber (currentSN);

	      //update am rlc state variables
	      amRlcState->m_vt_s++;

	      //Add MAC header
	      MACHeader *mac = new MACHeader (GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode (),
	    		                          GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode ());
	      packet->AddMACHeader(mac);
          packet->AddHeaderSize (3);

	      if (availableBytes > 1503)
	        {
	    	  packet->SetSize (1503);
	    	  packet->GetPacketTags ()->SetApplicationSize (1490);
	    	  availableBytes -= 1503;

	    	  if (_RLC_TRACING_)
	    	    {
	              std::cout << "TX AM_RLC SIZE " << packet->GetSize () <<
	        		  " B " << GetRlcEntityIndex () <<
	        		  " PDU_SN " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber() <<
	        		  " Frag " << packet->GetRLCHeader ()->IsAFragment () <<
	        		  " LastFrag " << packet->GetRLCHeader ()->IsTheLatestFragment() <<
	        		  " startB " << packet->GetRLCHeader ()->GetStartByte () <<
	        		  " endB " << packet->GetRLCHeader ()->GetEndByte () <<
	        		  std::endl;
	    	    }

	          pb->AddPacket (packet);
	        }
	      else if (availableBytes > 13)
	        {
	    	  packet->SetSize (availableBytes);
	    	  packet->GetPacketTags ()->SetApplicationSize (availableBytes - 13);
	    	  availableBytes = 0;

	    	  if (_RLC_TRACING_)
	    	    {
	              std::cout << "TX 2 AM_RLC SIZE " << packet->GetSize () <<
	        		  " B " << GetRlcEntityIndex () <<
	        		  " PDU_SN " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber() <<
	        		  " Frag " << packet->GetRLCHeader ()->IsAFragment () <<
	        		  " LastFrag " << packet->GetRLCHeader ()->IsTheLatestFragment() <<
					  " startB " << packet->GetRLCHeader ()->GetStartByte () <<
					  " endB " << packet->GetRLCHeader ()->GetEndByte () <<
					  std::endl;
	    	    }

	    	  pb->AddPacket (packet);
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

#ifdef RLC_DEBUG
	    	  std::cout << "SEND PACKET id " << packet->GetID() << " frag n " << packet->GetRLCHeader ()->GetFragmentNumber ()<< std::endl;
#endif

	    	  //Set the id of the receiver RLC entity
		      packet->GetRLCHeader ()->SetRlcEntityIndex (GetRlcEntityIndex ());

		      //amRlcState->PrintTxVariables ();

	          // set sn
		      int currentSN = amRlcState->m_vt_s;
		      packet->GetRLCHeader ()->SetRlcPduSequenceNumber (currentSN);

		      //update am rlc state variables
		      amRlcState->m_vt_s++;

		      //amRlcState->PrintTxVariables ();

#ifdef RLC_DEBUG
	    	  std::cout << "_____ pkt " << packet->GetID() << " frag " << packet->GetRLCHeader ()->GetFragmentNumber () <<
	    			  " sn " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber () <<
	    			  " startB " << packet->GetRLCHeader ()->GetStartByte () <<
	    			  " endB " << packet->GetRLCHeader ()->GetEndByte () << std::endl;
	    	  PrintSentAMDs ();
#endif


	    	  AmdRecord *amdRecord = new AmdRecord (packet->Copy (), currentSN);
		      GetSentAMDs ()->push_back (amdRecord);
#ifdef RLC_DEBUG
		      PrintSentAMDs ();
#endif
		      //Add MAC header
		      MACHeader *mac = new MACHeader (GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode (),
		    		                          GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode ());
		      packet->AddMACHeader(mac);
              packet->AddHeaderSize (3);

              if (_RLC_TRACING_)
                {
	              std::cout << "TX AM_RLC SIZE " << packet->GetSize () <<
	        		  " B " << GetRlcEntityIndex () <<
	        		  " PDU_SN " << packet->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
                }

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
AmRlcEntity::ReceptionProcedure (Packet* p)
{
#ifdef RLC_DEBUG
  std::cout << "AM RLC rx procedure for node " << GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode () << " bearer "<<  GetRlcEntityIndex () << std::endl;
  std::cout << "_____ pkt " << p->GetID() << " frag " << p->GetRLCHeader ()->GetFragmentNumber () <<
		  " sn " << p->GetRLCHeader ()->GetRlcPduSequenceNumber () <<
		  " startB " << p->GetRLCHeader ()->GetStartByte () <<
		  " endB " << p->GetRLCHeader ()->GetEndByte () << std::endl;
#endif

  if (_RLC_TRACING_)
    {
      std::cout << "RX AM_RLC SIZE " << p->GetSize () <<
		  " B " << GetRlcEntityIndex () <<
		  " PDU_SN " << p->GetRLCHeader ()->GetRlcPduSequenceNumber() << std::endl;
    }

  RadioBearerSink *bearer = (RadioBearerSink*) GetRadioBearerInstance ();
  AmStateVariables* amsState = GetAmStateVariables ();

#ifdef RLC_DEBUG
  PrintReceivedAMDs ();
  //amsState->PrintRxVariables();
#endif

  int seqNumber = p->GetRLCHeader ()->GetRlcPduSequenceNumber ();

  /* NOT IMPLEMENTED YET
  if (seqNumber < amsState->m_vr_r || seqNumber >= amsState->m_vr_mr)
    {
	  //discard the received packet and finish the rx procedure
	  ReceptionProcedureEnd ();
	  return;
    }
  */

  if (seqNumber >= amsState->m_vr_h)
    {
	  amsState->m_vr_h = seqNumber + 1;
	  amsState->m_vr_mr = amsState->m_vr_h + amsState->m_am_window_size;
    }

  //create an ack
  //if (!bearer->GetApplication ()->GetSourceApplication ()->GetApplicationType () == Application::APPLICATION_TYPE_INFINITE_BUFFER)
    //{
      ArqRlcIdealControlMessage msg;
      msg.SetAck (seqNumber);
      msg.SetStartByte (p->GetRLCHeader ()->GetStartByte ());
      msg.SetEndByte (p->GetRLCHeader ()->GetEndByte ());
      SendArqRlcIdealControlMessage (msg);
    //}

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
  else if (p->GetRLCHeader ()->IsAFragment ())
    {
#ifdef RLC_DEBUG
      std::cout << "\t received a fragment " << std::endl;
#endif
      AmdRecord *amdRecord = new AmdRecord (p, seqNumber);
      InsertAMDIntoReceptionList (amdRecord);

#ifdef RLC_DEBUG
      PrintReceivedAMDs ();
#endif
      ReceptionProcedureEnd ();
    }
}

void
AmRlcEntity::ReceptionProcedureEnd ()
{
#ifdef RLC_DEBUG
  std::cout << "AM RLC ReceptionProcedureEnd " << std::endl;
  PrintReceivedAMDs ();
#endif

  if (m_receivedAMDs->size () == 0) return;

  int currentPacket = -1;
  int expectedNextByte = 0;

  std::list<int> packetIdToDelete;

  std::vector <AmdRecord*>::iterator it;
  for (it = m_receivedAMDs->begin (); it != m_receivedAMDs->end (); it++)
	{
	  Packet* packet = (*it)->m_packet;

	  if (packet->GetID () != currentPacket && packet->GetRLCHeader ()->GetStartByte () == 0)
		{
		  //std::cout << "\t\t !! first fragment of the packet" << std::endl;
          currentPacket = packet->GetID ();
          expectedNextByte = packet->GetRLCHeader ()->GetEndByte () + 1;
		}
	  else if (packet->GetID () != currentPacket && packet->GetRLCHeader ()->GetStartByte () != 0)
	    {
		  //std::cout << "\t\t !! fragment of a new packet, but not he first" << std::endl;
		  expectedNextByte = 0;
	    }
	  else if (packet->GetID () == currentPacket && packet->GetRLCHeader ()->GetStartByte () ==  expectedNextByte)
	    {
		  if (packet->GetRLCHeader ()->IsTheLatestFragment ())
		    {
			  //std::cout << "\t\t !! consecutive fragment --> the packet can be reassembled" << std::endl;
			  RadioBearerSink *bearer = (RadioBearerSink*) GetRadioBearerInstance ();
              Packet* p = packet->Copy ();
              p->SetSize (packet->GetRLCHeader ()->GetEndByte () + 8);
			  bearer->Receive (p);

	          packetIdToDelete.push_front(packet->GetID ());

	    	  expectedNextByte = 0;
		    }
		  else
		    {
			  //std::cout << "\t\t !! consecutive fragment --> we expect another fragment" << std::endl;
			  expectedNextByte = packet->GetRLCHeader ()->GetEndByte () + 1;
		    }
	    }
	}


  //DELETE REASSEBLED PACKETS!
#ifdef RLC_DEBUG
  std::cout << "\t\t !! DELETE REASSEBLED PACKETS" << std::endl;
#endif

  for (std::list<int>::iterator iter = packetIdToDelete.begin (); iter != packetIdToDelete.end (); iter++)
    {
	  std::vector <AmdRecord*> *newAmdList = new std::vector <AmdRecord*>;
	  int id = (*iter);
	  for (std::vector <AmdRecord*>::iterator  itt = m_receivedAMDs->begin (); itt != m_receivedAMDs->end (); itt++)
		{
		  AmdRecord *amdRecord = (*itt);
		  double delay = amdRecord->m_packet->GetTimeStamp () + GetRadioBearerInstance ()->GetQoSParameters ()->GetMaxDelay ();
  		  if (amdRecord->m_packet->GetID () == id)
			{
				delete amdRecord;
			}
  		  else
  		    {
  			  newAmdList->push_back (amdRecord);
  		    }
		}
   	  m_receivedAMDs->clear();
   	  delete m_receivedAMDs;
   	  m_receivedAMDs = newAmdList;
    }


  //DELETE PACKETS or AMDs WHOSE DEADLINE is EXPIRED !
#ifdef RLC_DEBUG
  std::cout << "\t\t !! DELETE PACKETS or AMDs WHOSE DEADLINE is EXPIRED !" << std::endl;
#endif

  int currentpacketId = -1;
  std::vector <AmdRecord*> *newAmdList = new std::vector <AmdRecord*>;
  for (std::vector <AmdRecord*>::iterator  itt = m_receivedAMDs->begin (); itt != m_receivedAMDs->end (); itt++)
	{
	  AmdRecord *amdRecord = (*itt);
	  double delay = amdRecord->m_packet->GetTimeStamp () + GetRadioBearerInstance ()->GetQoSParameters ()->GetMaxDelay ();
	  if ((delay + 0.01) < Simulator::Init()->Now())
		{
		  //trace for statistical purposes
		  Packet *pp = amdRecord->m_packet;
		  if (pp->GetID () !=  currentpacketId)
		    {
			  currentpacketId = amdRecord->m_packet->GetID ();

			  if (_RLC_TRACING_)
			    {
				  std::cout << "DROP_RX_AM_RLC";
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
		    }
			delete amdRecord;
		}
	  else
		{
		  newAmdList->push_back (amdRecord);
		}
	}
  m_receivedAMDs->clear();
  delete m_receivedAMDs;
  m_receivedAMDs = newAmdList;



#ifdef RLC_DEBUG
  PrintReceivedAMDs ();
#endif
}

void
AmRlcEntity::SendArqRlcIdealControlMessage (ArqRlcIdealControlMessage msg)
{
#ifdef RLC_DEBUG
  std::cout << "AM RLC -- SendArqRlcIdealControlMessage, node " << GetRadioBearerInstance ()->GetDestination ()->GetIDNetworkNode () << " bearer "<<  GetRlcEntityIndex () << std::endl;
  std::cout << "\t ack " << msg.GetAck() << ", startB " << msg.GetStartByte () << ", endB " << msg.GetEndByte () << std::endl;
#endif

  //send AM RLC ACK to the receiver RLC under ideal control channel
  RadioBearerSink* thisBearer = (RadioBearerSink*) GetRadioBearerInstance ();
  ApplicationSink *thisApplication = (ApplicationSink*) thisBearer->GetApplication ();
  RadioBearer* remoteBearer = thisApplication->GetSourceApplication ()->GetRadioBearer ();

  AmRlcEntity* remoteRlc = (AmRlcEntity*) remoteBearer->GetRlcEntity();
  remoteRlc->ReceiveArqRlcIdealControlMessage (msg);
}

void
AmRlcEntity::ReceiveArqRlcIdealControlMessage (ArqRlcIdealControlMessage msg)
{
#ifdef RLC_DEBUG
  std::cout << "AM RLC -- ReceiveArqRlcIdealControlMessage, node " << GetRadioBearerInstance ()->GetSource ()->GetIDNetworkNode () << " bearer "<<  GetRlcEntityIndex () << std::endl;
  std::cout << "\t ack " << msg.GetAck() << ", startB " << msg.GetStartByte () << ", endB " << msg.GetEndByte () << std::endl;
  PrintSentAMDs ();
#endif

  //delete AMD PDU from m_sentAMDs
  std::vector<AmdRecord*>::iterator it;
  for (it = m_sentAMDs->begin(); it != m_sentAMDs->end(); it++)
	{
	  AmdRecord *amdRecord = (*it);
	  if (amdRecord->m_packet->GetRLCHeader ()->GetRlcPduSequenceNumber () == msg.GetAck()
			&&
			amdRecord->m_packet->GetRLCHeader ()->GetStartByte () == msg.GetStartByte ())
	   {
		 m_sentAMDs->erase (it);
		 delete amdRecord;

#ifdef RLC_DEBUG
        std::cout << " || deleted AMD ||"<< std::endl;
        PrintSentAMDs ();
#endif
		 return;
	   }

	}
}

void
AmRlcEntity::CheckForDropPackets (double maxDelay, int bearerID)
{
#ifdef RLC_DEBUG
  std::cout << "AmRlcEntity::CheckForDropPackets " << std::endl;
  PrintSentAMDs ();
#endif

  int currentpacket = -1;
  double now = Simulator::Init()->Now();
  std::vector <AmdRecord*> *newAmdlist = new std::vector <AmdRecord*> ();

  for (std::vector <AmdRecord*>::iterator  it = m_sentAMDs->begin (); it != m_sentAMDs->end (); it++)
	{
	  AmdRecord *amdRecord = (*it);
	  double HOL = now - amdRecord->m_packet->GetTimeStamp ();

	  if (HOL > maxDelay || amdRecord->m_retx_count > MAX_AMD_RETX)
	   {
		 if (currentpacket != amdRecord->m_packet->GetID ())
		   {
			 currentpacket = amdRecord->m_packet->GetID ();

			 if (_RLC_TRACING_)
			   {
				 /*
				  * TRACE
				  */
				 std::cout << "DROP";
				 if (amdRecord->m_packet->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_VOIP)
				   std::cout << " VOIP";
				 else if (amdRecord->m_packet->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_TRACE_BASED)
				   std::cout << " VIDEO";
				 else if (amdRecord->m_packet->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_CBR)
				   std::cout << " CBR";
				 else if (amdRecord->m_packet->GetPacketTags()->GetApplicationType() == PacketTAGs::APPLICATION_TYPE_INFINITE_BUFFER)
				   std::cout << " INF_BUF";
				 else
				   std::cout << " UNKNOW";

				   std::cout << " ID "<< amdRecord->m_packet->GetID()
					  << " B " << GetRlcEntityIndex ();
				  if (amdRecord->m_packet->GetPacketTags() != NULL
					  && amdRecord->m_packet->GetPacketTags()->GetApplicationType() ==
							  PacketTAGs::APPLICATION_TYPE_TRACE_BASED)
				    {
				      std::cout << " FRAME " <<  amdRecord->m_packet->GetPacketTags()->GetFrameNuber()
							<< " START " << amdRecord->m_packet->GetPacketTags()->GetStartByte()
							<< " END " << amdRecord->m_packet->GetPacketTags()->GetEndByte();
				    }
			      std::cout  <<  std::endl;
			   }
		   }
		 delete amdRecord;
	   }
	  else
	    {
		  newAmdlist->push_back (amdRecord);
	    }
	}

  m_sentAMDs->clear();
  delete m_sentAMDs;
  m_sentAMDs = newAmdlist;

#ifdef RLC_DEBUG
  PrintSentAMDs ();
#endif
}

int
AmRlcEntity::GetSizeOfUnaknowledgedAmd (void)
{
  int size = 0;
  for (std::vector <AmdRecord*>::iterator  it = m_sentAMDs->begin (); it != m_sentAMDs->end (); it++)
	{
	  size += (*it)->m_packet->GetSize () + 5; //add also MAC and CRC overhead
	}
  return size;
}
