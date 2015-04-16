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



#include "ClassifierParameters.h"
#include <cstdlib>
#include <iostream>

ClassifierParameters::ClassifierParameters()
{

}

ClassifierParameters::ClassifierParameters(int sourceID,
										   int destinationID,
										   int sourcePort,
										   int destinationPort,
										   TransportProtocol::TransportProtocolType protocol)
{
  m_sourceID = sourceID;
  m_destinationID = destinationID;
  m_sourcePort = sourcePort;
  m_destinationPort = destinationPort;
  m_protocolType = protocol;
}

ClassifierParameters::~ClassifierParameters()
{}

void
ClassifierParameters::SetSourceID(int id)
{
  m_sourceID = id;
}

int
ClassifierParameters::GetSourceID(void)
{
  return  m_sourceID;
}

void
ClassifierParameters::SetDestinationID(int id)
{
  m_destinationID = id;
}

int
ClassifierParameters::GetDestinationID(void)
{
  return m_destinationID;
}

void
ClassifierParameters::SetSourcePort(int port)
{
  m_sourcePort = port;
}

int
ClassifierParameters::GetSourcePort(void)
{
  return m_sourcePort;
}

void
ClassifierParameters::SetDestinationPort(int port)
{
  m_destinationPort = port;
}

int
ClassifierParameters::GetDestinationPort(void)
{
  return m_destinationPort;
}

void
ClassifierParameters::SetTransportProtocol(TransportProtocol::TransportProtocolType protocol)
{
  m_protocolType = protocol;
}

TransportProtocol::TransportProtocolType
ClassifierParameters::GetTransportProtocol(void)
{
  return m_protocolType;
}

void
ClassifierParameters::Print()
{
  std::cout <<
		  "\t Classifier Parameters:"
		  "\n\t\t src = " << m_sourceID <<
		  "\n\t\t dst = " << m_destinationID <<
		  "\n\t\t srcPort = " << m_sourcePort <<
   	      "\n\t\t dstPort = " << m_destinationPort <<
   	      "\n\t\t protocol = " << m_protocolType
   	      << std::endl;
}
