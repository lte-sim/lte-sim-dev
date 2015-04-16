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



#include "Application.h"
#include "../radio-bearer.h"
#include "../../device/NetworkNode.h"
#include "../QoS/QoSParameters.h"
#include "../../componentManagers/NetworkManager.h"
#include "../radio-bearer-sink.h"
#include "application-sink.h"
#include "../../phy/lte-phy.h"
#include "../../channel/LteChannel.h"
#include "../../device/UserEquipment.h"
#include "../../device/ENodeB.h"
#include "../../load-parameters.h"

Application::Application()
{
  m_classifierParameters = NULL;
  m_qosParameters = NULL;
  m_source = NULL;
  m_destination = NULL;
  m_radioBearer = NULL;
  m_bearerSink = NULL;
  m_applicationSink = NULL;
}

void
Application::Destroy (void)
{
  m_classifierParameters = NULL;
  m_qosParameters = NULL;
  m_source = NULL;
  m_destination = NULL;
  m_radioBearer = NULL;
  m_bearerSink = NULL;
  m_applicationSink = NULL;
}

Application::Application(ApplicationType applicationType)
{
  m_applicationType = applicationType;
}

Application::~Application()
{
  Destroy ();
}

void
Application::Start ()
{
#ifdef TEST_START_APPLICATION
  std::cout << "Start Application: src: " << GetSource ()->GetIDNetworkNode ()
		  << " dst: " << GetDestination ()->GetIDNetworkNode () << std::endl;
#endif

  // 1 - create radio bearer
  m_radioBearer = new RadioBearer ();
  m_radioBearer->GetRlcEntity ()->SetRlcEntityIndex (GetApplicationID ());


  if (GetSource ()->GetNodeType() == NetworkNode::TYPE_UE)
    {
	  //create an UL radio bearer between UE and targetENB
	  UserEquipment* ue = (UserEquipment*) GetSource ();
	  ue->SetNodeState (NetworkNode::STATE_ACTIVE);

#ifdef TEST_START_APPLICATION
      std::cout << "Create UL radio bearer bewtween: " << GetSource ()->GetIDNetworkNode ()
		  << " and " << ue->GetTargetNode ()->GetIDNetworkNode () << std::endl;
#endif

	  m_radioBearer->SetSource (ue);
	  m_radioBearer->SetDestination (ue->GetTargetNode ());
	  m_radioBearer->SetClassifierParameters (GetClassifierParameters ());
	  m_radioBearer->SetApplication (this);
	  m_radioBearer->SetQoSParameters (GetQoSParameters ());
    }
  else if (GetSource ()->GetNodeType() == NetworkNode::TYPE_GW
		  ||
		  GetSource ()->GetNodeType() == NetworkNode::TYPE_ENODEB
		  ||
		  GetSource ()->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION)
    {
	  //create an DL radio bearer between targetENB and UE
	  UserEquipment* ue = (UserEquipment*) GetDestination ();
	  ue->SetNodeState (NetworkNode::STATE_ACTIVE);

#ifdef TEST_START_APPLICATION
      std::cout << "Create DL radio bearer bewtween: " << ue->GetTargetNode ()->GetIDNetworkNode ()
    		  << " and " << ue->GetIDNetworkNode ()  << std::endl;
#endif

	  m_radioBearer->SetSource (ue->GetTargetNode ());
	  m_radioBearer->SetDestination (ue);
	  m_radioBearer->SetClassifierParameters (GetClassifierParameters ());
	  m_radioBearer->SetApplication (this);
	  m_radioBearer->SetQoSParameters (GetQoSParameters ());
    }

  m_radioBearer->GetSource ()->GetProtocolStack ()->GetRrcEntity ()->AddRadioBearer (m_radioBearer);


  // 2 - create application sink
  m_applicationSink = new ApplicationSink ();
  m_applicationSink->SetClassifierParameters (GetClassifierParameters ());
  m_applicationSink->SetSourceApplication (this);


  // 3 - create radio bearer sink
  m_bearerSink = new RadioBearerSink ();
  m_bearerSink->GetRlcEntity ()->SetRlcEntityIndex (GetApplicationID ());
  m_bearerSink->SetApplication (m_applicationSink);
  m_bearerSink->SetClassifierParameters (GetClassifierParameters ());
  m_bearerSink->SetQoSParameters (GetQoSParameters ());
  if (GetSource ()->GetNodeType() == NetworkNode::TYPE_UE)
    {
	  UserEquipment* ue = (UserEquipment*) GetSource ();
	  ue->SetNodeState (NetworkNode::STATE_ACTIVE);
	  m_bearerSink->SetSource (ue);
	  m_bearerSink->SetDestination (ue->GetTargetNode ());
    }
  else if (GetSource ()->GetNodeType() == NetworkNode::TYPE_GW || GetSource ()->GetNodeType() == NetworkNode::TYPE_ENODEB
		  || GetSource ()->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION)
    {
	  UserEquipment* ue = (UserEquipment*) GetDestination ();
	  ue->SetNodeState (NetworkNode::STATE_ACTIVE);
	  m_bearerSink->SetSource (ue->GetTargetNode ());
	  m_bearerSink->SetDestination (ue);
    }


  // 4 - add in radio bearer a pointer to the radio bearer sink
  m_radioBearer->GetDestination() ->GetProtocolStack ()->GetRrcEntity ()->AddRadioBearerSink(m_bearerSink);
  m_applicationSink->SetRadioBearerSink (m_bearerSink);


  // 4 attach UE on the UL or DL channel
  if (GetSource ()->GetNodeType() == NetworkNode::TYPE_UE)
    {
	  UserEquipment* ue = (UserEquipment*) GetSource ();
	  /*
	  LteChannel *ch = ue->GetTargetNode ()->GetPhy ()->GetUlChannel ();

	  if (!ch->IsAttached (ue))
	    {
		  ch->AddDevice (ue);
	    }
	  */

	  ue->MakeActive ();
    }
  else if (GetSource ()->GetNodeType() == NetworkNode::TYPE_GW || GetSource ()->GetNodeType() == NetworkNode::TYPE_ENODEB
		  || GetSource ()->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION)
    {
	  UserEquipment* ue = (UserEquipment*) GetDestination ();
	  LteChannel *ch = ue->GetTargetNode ()->GetPhy ()->GetDlChannel ();

	  if (!ch->IsAttached (ue))
	    {
		  ch->AddDevice (ue);
	    }

	  ue->MakeActive ();
    }

#ifdef TEST_START_APPLICATION
  std::cout << "CREATED RADIO BEARER " << m_radioBearer->GetApplication ()->GetApplicationID ()
		  << " BETWEEN "
		  << m_radioBearer->GetSource ()->GetIDNetworkNode () << " and "
		  << m_radioBearer->GetDestination () ->GetIDNetworkNode ()<< std::endl;
#endif

  DoStart ();
}

void
Application::Stop ()
{}


RadioBearer*
Application::GetRadioBearer (void)
{
  return m_radioBearer;
}

void
Application::SetApplicationID (int id)
{
  m_applicationID = id;
}

int
Application::GetApplicationID (void)
{
  return m_applicationID;
}


void
Application::SetApplicationType (ApplicationType applicationType)
{
  m_applicationType = applicationType;
}

Application::ApplicationType
Application::GetApplicationType (void) const
{
  return m_applicationType;
}

void
Application::SetClassifierParameters (ClassifierParameters* cp)
{
  m_classifierParameters = cp;
}

ClassifierParameters*
Application::GetClassifierParameters (void)
{
  return m_classifierParameters;
}

void
Application::SetQoSParameters (QoSParameters* qos)
{
  m_qosParameters = qos;
}

QoSParameters*
Application::GetQoSParameters (void)
{
  return m_qosParameters;
}

NetworkNode*
Application::GetSource (void)
{
  return m_source;
}

NetworkNode*
Application::GetDestination (void)
{
  return m_destination;
}

int
Application::GetSourcePort (void) const
{
  return m_sourcePort;
}

int
Application::GetDestinationPort (void) const
{
  return m_destinationPort;
}

void
Application::SetSource (NetworkNode *source)
{
  m_source = source;
}

void
Application::SetDestination (NetworkNode *destination)
{
  m_destination = destination;
}

void
Application::SetSourcePort (int port)
{
  m_sourcePort = port;
}

void
Application::SetDestinationPort (int port)
{
  m_destinationPort = port;
}

TransportProtocol::TransportProtocolType
Application::GetTransportProtocol (void) const
{
  return m_transportProtocol;
}

void
Application::SetTransportProtocol (TransportProtocol::TransportProtocolType protocol)
{
  m_transportProtocol = protocol;
}


void
Application::SetStartTime (double time)
{
  m_startTime = time;
  Simulator::Init()->Schedule(time,
							  &Application::Start,
							  this);
}

double
Application::GetStartTime (void) const
{
  return m_startTime;
}

void
Application::SetStopTime (double time)
{
  m_stopTime = time;
  Simulator::Init()->Schedule(time + 0.1,
							  &Application::Stop,
							  this);
}

double
Application::GetStopTime (void) const
{
  return m_stopTime;
}

void
Application::Trace (Packet* p)
{

 if (!_APP_TRACING_) return;

 /*
  * Trace format:
  *
  * TX   APPLICATION_TYPE   BEARER_ID  SIZE   SRC_ID   DST_ID   TIME
  */
  std::cout << "TX";
  switch (m_applicationType)
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

  if (GetDestination ()->GetNodeType() == NetworkNode::TYPE_UE)
    {
      UserEquipment* ue = (UserEquipment*) GetDestination ();
      std::cout << " ID " << p->GetID ()
		    << " B " << GetApplicationID ()
			<< " SIZE " << p->GetSize ()
			<< " SRC " << GetSource ()->GetIDNetworkNode ()
			<< " DST " << GetDestination ()->GetIDNetworkNode ()
			<< " T " << Simulator::Init()->Now()
	        << " " << ue->IsIndoor () << std::endl;
    }
  else
    {
	  std::cout << " ID " << p->GetID ()
		<< " B " << GetApplicationID ()
		<< " SIZE " << p->GetSize ()
		<< " SRC " << GetSource ()->GetIDNetworkNode ()
		<< " DST " << GetDestination ()->GetIDNetworkNode ()
		<< " T " << Simulator::Init()->Now() << std::endl;
    }
}

void
Application::Print (void)
{
  cout << " Application object: "
      "\n\t m_applicationType = " << m_applicationType <<
      endl;
}

