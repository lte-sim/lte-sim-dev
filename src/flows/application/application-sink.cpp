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


#include "application-sink.h"
#include "../../device/IPClassifier/ClassifierParameters.h"
#include "../../componentManagers/NetworkManager.h"
#include "../../core/eventScheduler/simulator.h"
#include "../../load-parameters.h"
#include "../../device/UserEquipment.h"

ApplicationSink::ApplicationSink()
{
  m_classifierParameters = NULL;
  m_radioBearer = NULL;
  m_sourceApplication = NULL;
}

ApplicationSink::~ApplicationSink()
{
  m_classifierParameters = NULL;
  m_radioBearer = NULL;
  m_sourceApplication = NULL;
}

void
ApplicationSink::SetClassifierParameters (ClassifierParameters* cp)
{
  m_classifierParameters = cp;
}

ClassifierParameters*
ApplicationSink::GetClassifierParameters (void)
{
  return m_classifierParameters;
}


void
ApplicationSink::SetRadioBearerSink (RadioBearerSink* r)
{
  m_radioBearer = r;
}

RadioBearerSink*
ApplicationSink::GetRadioBearerSink (void)
{
  return m_radioBearer;
}

void
ApplicationSink::SetSourceApplication (Application* a)
{
  m_sourceApplication = a;
}

Application*
ApplicationSink::GetSourceApplication (void)
{
  return m_sourceApplication;
}

void
ApplicationSink::Receive (Packet* p)
{
  /*
   * Trace format:
   *
   * TX   APPLICATION_TYPE   BEARER_ID  SIZE   SRC_ID   DST_ID   TIME
   */

  if (!_APP_TRACING_) return;

  std::cout << "RX";

  switch (m_sourceApplication->GetApplicationType ())
	{
	  case Application::APPLICATION_TYPE_VOIP:
		{
		  std::cout << " VOIP";
		  break;
		}
	  case Application::APPLICATION_TYPE_TRACE_BASED:
		{
		  std::cout << " VIDEO";
		  break;
		}
	  case Application::APPLICATION_TYPE_CBR:
		{
		  std::cout << " CBR";
		  break;
		}
	  case Application::APPLICATION_TYPE_INFINITE_BUFFER:
		{
		  std::cout << " INF_BUF";
		  break;
		}
	  default:
		{
		  std::cout << " UNDEFINED";
		  break;
		}
	}

  double delay = ((Simulator::Init()->Now() *10000) - (p->GetTimeStamp () *10000)) /10000;
  if (delay < 0.000001) delay = 0.000001;

  UserEquipment* ue = (UserEquipment*) GetSourceApplication ()->GetDestination ();

  std::cout << " ID " << p->GetID ()
                        << " B " << m_sourceApplication->GetApplicationID ()
                        << " SIZE " << p->GetPacketTags ()->GetApplicationSize ()
                        << " SRC " << p->GetSourceID ()
                        << " DST " << p->GetDestinationID ()
                        << " D " << delay
                        << " " << ue->IsIndoor () << std::endl;


  delete p;
}
