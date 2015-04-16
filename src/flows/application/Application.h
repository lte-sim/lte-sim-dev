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



#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "../../core/eventScheduler/simulator.h"
#include "../../protocolStack/packet/Packet.h"
#include "../../protocolStack/protocols/TransportProtocol.h"
#include "../../load-parameters.h"
#include "../../device/IPClassifier/ClassifierParameters.h"
#include "../../protocolStack/packet/PacketTAGs.h"


#include <iostream>
#include <stdint.h>

class NetworkNode;
class ClassifierParameters;
class RadioBearer;
class QoSParameters;
class ApplicationSink;
class RadioBearerSink;

class Application {
public:
	enum ApplicationType
	  {
	    APPLICATION_TYPE_VOIP,
	    APPLICATION_TYPE_TRACE_BASED,
	    APPLICATION_TYPE_INFINITE_BUFFER,
	    APPLICATION_TYPE_CBR,
	    APPLICATION_TYPE_WEB
	  };

	Application();
	Application(ApplicationType applicationType);
	virtual ~Application();

	void Destroy (void);

	void SetApplicationType (ApplicationType applicationType);
	ApplicationType GetApplicationType (void) const;

	void SetClassifierParameters (ClassifierParameters* cp);
	ClassifierParameters* GetClassifierParameters (void);

	void SetQoSParameters (QoSParameters* qos);
	QoSParameters* GetQoSParameters (void);

	NetworkNode* GetSource (void);
	void SetSource (NetworkNode *source);
	NetworkNode* GetDestination (void);
	void SetDestination (NetworkNode *destination);
	int GetSourcePort (void) const;
	void SetSourcePort (int port);
	int GetDestinationPort (void) const;
    void SetDestinationPort (int port);
    TransportProtocol::TransportProtocolType GetTransportProtocol (void) const;
    void SetTransportProtocol (TransportProtocol::TransportProtocolType protocol);


	void SetStartTime (double time);
	double GetStartTime (void) const;
	void SetStopTime (double time);
	double GetStopTime (void) const;

	void Start (void);
	void Stop (void);
	virtual void DoStart (void) = 0;
	virtual void DoStop (void) = 0;

    void SetApplicationID (int id);
    int GetApplicationID (void);

    RadioBearer* GetRadioBearer (void);

	void Trace (Packet* packet);

	//Debug
	void Print (void);

private:
	ApplicationType m_applicationType;

	NetworkNode *m_source;
	NetworkNode *m_destination;
	int m_sourcePort;
	int m_destinationPort;
	TransportProtocol::TransportProtocolType m_transportProtocol;

	ClassifierParameters* m_classifierParameters;
	QoSParameters *m_qosParameters;

	RadioBearer* m_radioBearer;
	RadioBearerSink *m_bearerSink;
	ApplicationSink *m_applicationSink;

	double m_startTime;
	double m_stopTime;

	int m_applicationID;
};

#endif /* APPLICATION_H_ */
