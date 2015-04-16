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



#include "rrc-entity.h"
#include "../packet/Packet.h"
#include "../../device/IPClassifier/ClassifierParameters.h"
#include "../../flows/radio-bearer.h"
#include "../../flows/radio-bearer-sink.h"
#include "../../load-parameters.h"
#include "../../device/NetworkNode.h"
#include "ho/handover-entity.h"

RrcEntity::RrcEntity ()
{
  m_bearers = new RadioBearersContainer ();
  m_sink = new RadioBearersSinkContainer ();
  m_device = NULL;
  m_handover = new HandoverEntity ();
}

RrcEntity::~RrcEntity ()
{
  m_bearers->clear ();
  delete m_bearers;

  m_sink->clear ();
  delete m_sink;

  delete m_handover;

  m_device = NULL;
}

void
RrcEntity::SetDevice (NetworkNode* d)
{
  m_device = d;
  m_handover->SetDevice (d);
}


NetworkNode*
RrcEntity::GetDevice ()
{
  return m_device;
}

RrcEntity::RadioBearersContainer*
RrcEntity::GetRadioBearerContainer (void)
{
  return m_bearers;
}

RrcEntity::RadioBearersSinkContainer*
RrcEntity::GetRadioBearerSinkContainer (void)
{
  return m_sink;
}


void
RrcEntity::AddRadioBearer (RadioBearer* bearer)
{
#ifdef TEST_START_APPLICATION
      std::cout << "Add radio bearer on "  << GetDevice ()->GetIDNetworkNode ()  << std::endl;
#endif

  m_bearers->push_back (bearer);
}

void
RrcEntity::DelRadioBearer (RadioBearer* bearer)
{
  RadioBearersContainer* newContainer = new RadioBearersContainer ();
  RadioBearersContainer::iterator it;
  for (it = GetRadioBearerContainer ()->begin(); it != GetRadioBearerContainer ()->end(); it++)
	{
	  RadioBearer *b = (*it);
	  if (b->GetRlcEntity ()->GetRlcEntityIndex () != bearer->GetRlcEntity ()->GetRlcEntityIndex ())
		{
		  newContainer->push_back (b);
		}
	}
  m_bearers->clear ();
  delete m_bearers;
  m_bearers = newContainer;
}

void
RrcEntity::AddRadioBearerSink (RadioBearerSink* bearer)
{
#ifdef TEST_START_APPLICATION
      std::cout << "Add sink radio bearer on "  << GetDevice ()->GetIDNetworkNode () << std::endl;
#endif

  m_sink->push_back (bearer);
}

void
RrcEntity::DelRadioBearerSink (RadioBearerSink* bearer)
{
  RadioBearersSinkContainer* newContainer = new RadioBearersSinkContainer ();
  RadioBearersSinkContainer::iterator it;
  for (it = GetRadioBearerSinkContainer ()->begin(); it != GetRadioBearerSinkContainer ()->end(); it++)
	{
	  RadioBearerSink *b = (*it);
	  if (b->GetRlcEntity ()->GetRlcEntityIndex () != bearer->GetRlcEntity ()->GetRlcEntityIndex ())
	    {
		  newContainer->push_back (b);
	    }
	}
  m_sink->clear ();
  delete m_sink;
  m_sink = newContainer;
}

RadioBearer*
RrcEntity::GetRadioBearer (ClassifierParameters* ipc)
{
  RadioBearersContainer::iterator it;
  for (it = GetRadioBearerContainer ()->begin(); it != GetRadioBearerContainer ()->end(); it++)
    {
	  RadioBearer *bearer = (*it);

	  if (bearer->GetClassifierParameters ()->GetSourcePort () == ipc->GetSourcePort ()
			  &&
			  bearer->GetClassifierParameters ()->GetDestinationPort () == ipc->GetDestinationPort ())
	    {
		  return bearer;
	    }
    }
  return 0;
}

RadioBearerSink*
RrcEntity::GetRadioBearerSink (ClassifierParameters* ipc)
{
  RadioBearersSinkContainer::iterator it;
  for (it = GetRadioBearerSinkContainer ()->begin(); it != GetRadioBearerSinkContainer ()->end(); it++)
	{
	  RadioBearerSink *bearer = (*it);

	  if (bearer->GetClassifierParameters ()->GetSourcePort () == ipc->GetSourcePort ()
			  &&
			  bearer->GetClassifierParameters ()->GetDestinationPort () == ipc->GetDestinationPort ())
		{
		  return bearer;
		}
	}
  return 0;
}


RadioBearerInstance*
RrcEntity::GetRadioBearer (int rlcIndex)
{
  for (RadioBearersSinkContainer::iterator it = GetRadioBearerSinkContainer ()->begin();
		  it != GetRadioBearerSinkContainer ()->end(); it++)
	{
	  RadioBearerSink *bearer = (*it);

	  if (bearer->GetRlcEntity ()->GetRlcEntityIndex () == rlcIndex)
		{
		  return bearer;
		}
	}

  for (RadioBearersContainer::iterator it = GetRadioBearerContainer ()->begin();
		  it != GetRadioBearerContainer ()->end(); it++)
	{
	  RadioBearer *bearer = (*it);

	  if (bearer->GetRlcEntity ()->GetRlcEntityIndex () == rlcIndex)
	    {
		  return bearer;
		}
	}

  return 0;
}


void
RrcEntity::SetHandoverEntity (HandoverEntity* h)
{
  m_handover = h;
}

HandoverEntity*
RrcEntity::GetHandoverEntity (void)
{
  return m_handover;
}
