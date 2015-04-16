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


#include "ideal-control-messages.h"
#include "../../device/NetworkNode.h"


IdealControlMessage::IdealControlMessage (void)
  : m_source (0),
    m_destination (0)
{
}


IdealControlMessage::~IdealControlMessage (void)
{
}


void
IdealControlMessage::SetSourceDevice (NetworkNode* src)
{
  m_source = src;
}


void
IdealControlMessage::SetDestinationDevice (NetworkNode* dst)
{
  m_destination = dst;
}


NetworkNode*
IdealControlMessage::GetSourceDevice (void)
{
  return m_source;
}


NetworkNode*
IdealControlMessage::GetDestinationDevice (void)
{
  return m_destination;
}


void
IdealControlMessage::SetMessageType (IdealControlMessage::MessageType type)
{
  m_type = type;
}


IdealControlMessage::MessageType
IdealControlMessage::GetMessageType (void)
{
  return m_type;
}



// ----------------------------------------------------------------------------------------------------------


PdcchMapIdealControlMessage::PdcchMapIdealControlMessage (void)
{
  m_idealPdcchMessage =  new IdealPdcchMessage ();
  SetMessageType (IdealControlMessage::ALLOCATION_MAP);
}


PdcchMapIdealControlMessage::~PdcchMapIdealControlMessage (void)
{
  m_idealPdcchMessage->clear ();
  delete m_idealPdcchMessage;
}


void
PdcchMapIdealControlMessage::AddNewRecord (Direction direction,
                                           int subChannel, NetworkNode* ue, double mcs)
{
  IdealPdcchRecord record;
  record.m_direction = direction;
  record.m_idSubChannel = subChannel;
  record.m_mcsIndex = mcs;
  record.m_ue = ue;
  m_idealPdcchMessage->push_back (record);
}


PdcchMapIdealControlMessage::IdealPdcchMessage*
PdcchMapIdealControlMessage::GetMessage (void)
{
  return m_idealPdcchMessage;
}



// ----------------------------------------------------------------------------------------------------------


CqiIdealControlMessage::CqiIdealControlMessage (void)
{
  m_cqiFeedbacks = new CqiFeedbacks ();
  SetMessageType (IdealControlMessage::CQI_FEEDBACKS);
}


CqiIdealControlMessage::~CqiIdealControlMessage (void)
{
  m_cqiFeedbacks->clear ();
  delete m_cqiFeedbacks;
}


void
CqiIdealControlMessage::AddNewRecord (int subChannel, double cqi)
{
  CqiFeedback c;
  c.m_idSubChannel = subChannel;
  c.m_cqi = cqi;

  m_cqiFeedbacks->push_back (c);

}


CqiIdealControlMessage::CqiFeedbacks*
CqiIdealControlMessage::GetMessage (void)
{
  return m_cqiFeedbacks;
}



// ----------------------------------------------------------------------------------------------------------


ArqRlcIdealControlMessage::ArqRlcIdealControlMessage (void)
{
  SetMessageType (IdealControlMessage::ARQ_RLC_ACK);
}


ArqRlcIdealControlMessage::~ArqRlcIdealControlMessage (void)
{}

void
ArqRlcIdealControlMessage::SetAck (int ack)
{
  m_ack = ack;
}

int
ArqRlcIdealControlMessage::GetAck (void)
{
  return m_ack;
}

void
ArqRlcIdealControlMessage::SetStartByte (int b)
{
  m_startByte = b;
}

void
ArqRlcIdealControlMessage::SetEndByte (int b)
{
  m_endByte = b;
}

int
ArqRlcIdealControlMessage::GetStartByte (void)
{
  return m_startByte;
}

int
ArqRlcIdealControlMessage::GetEndByte (void)
{
  return m_endByte;
}




// ----------------------------------------------------------------------------------------------------------


SchedulingRequestIdealControlMessage::SchedulingRequestIdealControlMessage (void)
{
  SetMessageType (IdealControlMessage::SCHEDULING_REQUEST);
}


SchedulingRequestIdealControlMessage::~SchedulingRequestIdealControlMessage (void)
{}

int
SchedulingRequestIdealControlMessage::GetBufferStatusReport ()
{
  return m_bufferStatusReport;
}

void
SchedulingRequestIdealControlMessage::SetBufferStatusReport (int b)
{
  m_bufferStatusReport = b;
}
