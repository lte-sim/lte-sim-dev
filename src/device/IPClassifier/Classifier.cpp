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



#include "Classifier.h"
#include "../../core/eventScheduler/simulator.h"
#include "ClassifierParameters.h"
#include "../NetworkNode.h"
#include "../../protocolStack/rrc/rrc-entity.h"
#include "../../protocolStack/protocol-stack.h"
#include "../../flows/radio-bearer-sink.h"
#include "../../flows/radio-bearer.h"

Classifier::Classifier()
{
  m_device = NULL;
}

Classifier::~Classifier()
{
  m_device = NULL;
}


void
Classifier::SetDevice (NetworkNode* d)
{
  m_device = d;
}

NetworkNode*
Classifier::GetDevice (void)
{
  return m_device;
}

void
Classifier::Classify (Packet* p)
{
  //CLASSIFY THE PACKET
  RrcEntity *rrc = GetDevice ()->GetProtocolStack ()->GetRrcEntity ();

  ClassifierParameters *cp = new ClassifierParameters (p->GetSourceID(),
		                                               p->GetDestinationID(),
		                                               p->GetSourcePort(),
		                                               p->GetDestinationPort(),
		                                               TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);

  RadioBearerSink *sink = rrc->GetRadioBearerSink (cp);

  if (sink != NULL)
    {
	  sink->Receive (p);
    }
  else
  {
    RadioBearer *bearer = rrc->GetRadioBearer (cp);
    if (bearer != NULL)
      {
	    //enqueue
    	bearer->Enqueue (p);
      }
  }


}

