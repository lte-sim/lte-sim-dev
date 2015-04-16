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



#ifndef CLASSIFIERPARAMETERS_H_
#define CLASSIFIERPARAMETERS_H_

#include "../../protocolStack/protocols/TransportProtocol.h"


class ClassifierParameters {
public:
	ClassifierParameters();
	ClassifierParameters(int sourceID,
						 int destinationID,
						 int sourcePort,
						 int destinationPort,
						 TransportProtocol::TransportProtocolType protocol);
	virtual ~ClassifierParameters();

	void
	SetSourceID(int id);
	int
	GetSourceID(void);
	void
	SetDestinationID(int id);
	int
	GetDestinationID(void);
	void
	SetSourcePort(int port);
	int
	GetSourcePort(void);
	void
	SetDestinationPort(int port);
	int
	GetDestinationPort(void);
	void
	SetTransportProtocol(TransportProtocol::TransportProtocolType protocol);
	TransportProtocol::TransportProtocolType
	GetTransportProtocol(void);

//Debug
	void
	Print();

private:
	int m_sourceID;
	int m_destinationID;
	int m_sourcePort;
	int m_destinationPort;
	TransportProtocol::TransportProtocolType m_protocolType;

};

#endif /* CLASSIFIERPARAMETERS_H_ */
