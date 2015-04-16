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


#ifndef FLOWSMANAGER_H_
#define FLOWSMANAGER_H_

#include <iostream>
#include <queue>
#include "../protocolStack/protocols/TransportProtocol.h"
#include "../flows/application/Application.h"

class ClassifierParameters;
class QoSParameters;
class NetworkNode;


class FlowsManager {
public:

private:

	FlowsManager();
	static FlowsManager *ptr;

public:
	virtual ~FlowsManager();

	static FlowsManager*
	Init (void)
	  {
		if (ptr==NULL)
	      {
		    ptr = new FlowsManager;
	   	  }
		return ptr;
	  }

	Application*  CreateApplication (int applicationID,
									 NetworkNode* src, NetworkNode* dst,
			                         int srcPort, int destPort,
			                         TransportProtocol::TransportProtocolType protocol,
			                         Application::ApplicationType type,
			                         QoSParameters* qos,
			                         double startTime, double duration);
};

#endif /* FLOWSMANAGER_H_ */
