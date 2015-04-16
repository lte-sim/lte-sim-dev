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


#include "FlowsManager.h"
#include "NetworkManager.h"
#include "../device/NetworkNode.h"
#include "../device/IPClassifier/ClassifierParameters.h"
#include "../flows/application/Application.h"
#include "../flows/application/CBR.h"
#include "../flows/application/InfiniteBuffer.h"
#include "../flows/application/VoIP.h"
#include "../flows/application/TraceBased.h"
#include "../load-parameters.h"

FlowsManager* FlowsManager::ptr=NULL;

FlowsManager::FlowsManager()
{}

FlowsManager::~FlowsManager()
{}

Application*
FlowsManager::CreateApplication (int applicationID,
								 NetworkNode* src, NetworkNode* dst,
		                         int srcPort, int destPort,
		                         TransportProtocol::TransportProtocolType protocol,
		                         Application::ApplicationType type,
		                         QoSParameters* qos,
		                         double startTime, double duration)
{

  Application* app;

  if (type == Application::APPLICATION_TYPE_CBR)
    {
	  app = new CBR ();
    }
  if (type == Application::APPLICATION_TYPE_INFINITE_BUFFER)
    {
	  app = new InfiniteBuffer ();
    }
  if (type == Application::APPLICATION_TYPE_VOIP)
    {
	  app = new VoIP ();
    }
  if (type == Application::APPLICATION_TYPE_TRACE_BASED)
    {
	  app = new TraceBased ();
    }

  app->SetApplicationID (applicationID);
  app->SetSource (src);
  app->SetDestination (dst);
  app->SetSourcePort (srcPort);
  app->SetDestinationPort (destPort);

  ClassifierParameters *cp = new ClassifierParameters (src->GetIDNetworkNode(),
      	                                               dst->GetIDNetworkNode(),
      	                                               srcPort,
      	                                               destPort,
      	                                               protocol);

  app->SetClassifierParameters (cp);


  app->SetQoSParameters (qos);

  app->SetStartTime (startTime);
  app->SetStopTime (startTime + duration);

  return app;

}

