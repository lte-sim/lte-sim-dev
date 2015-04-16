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



#include "rlc-entity.h"
#include "../../device/NetworkNode.h"
#include "../packet/Packet.h"
#include "../../flows/radio-bearer-instance.h"

RlcEntity::RlcEntity ()
  : m_device (NULL),
    m_bearer (NULL)
{
}


RlcEntity::RlcEntity (NetworkNode* d)
  : m_device (NULL),
    m_bearer (NULL)
{
}

void
RlcEntity::Destroy (void)
{
  m_device = NULL;
  m_bearer = NULL;
}


RlcEntity::~RlcEntity ()
{
}


void
RlcEntity::SetDevice (NetworkNode* d)
{
  m_device = d;
}


NetworkNode*
RlcEntity::GetDevice (void)
{
  return m_device;
}

void
RlcEntity::SetRadioBearer (RadioBearerInstance *b)
{
  m_bearer = b;
}

RadioBearerInstance*
RlcEntity::GetRadioBearerInstance (void)
{
  return m_bearer;
}


void
RlcEntity::SetRlcEntityIndex (int i)
{
  m_rlcEntityIndex = i;
}

int
RlcEntity::GetRlcEntityIndex (void)
{
  return m_rlcEntityIndex;
}

void
RlcEntity::SetRlcPduSequenceNumber (int sn)
{
  m_rlcPduSequenceNumber = sn;
}

int
RlcEntity::GetRlcPduSequenceNumber (void)
{
  return m_rlcPduSequenceNumber;
}

void
RlcEntity::SetRlcMode (RlcMode mode)
{
  m_rlcMode = mode;
}

RlcEntity::RlcMode
RlcEntity::GetRlcModel (void)
{
  return m_rlcMode;
}
