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


#include "lte-phy.h"
#include "../device/NetworkNode.h"
#include "../channel/LteChannel.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../core/spectrum/transmitted-signal.h"
#include "interference.h"
#include "error-model.h"

LtePhy::LtePhy()
{
  m_device = NULL;
  m_dlChannel = NULL;
  m_ulChannel = NULL;
  m_bandwidthManager = NULL;
  m_txSignal = NULL;
}

LtePhy::~LtePhy()
{
  m_device = NULL;
  m_dlChannel = NULL;
  m_ulChannel = NULL;
  m_bandwidthManager = NULL;
  delete m_txSignal;
}

void
LtePhy::Destroy (void)
{
  m_device = NULL;
  m_dlChannel = NULL;
  m_ulChannel = NULL;
  m_bandwidthManager = NULL;
  delete m_txSignal;
  delete m_interference;
  delete m_errorModel;
}


void
LtePhy::SetDevice (NetworkNode* d)
{
  m_device = d;
}

NetworkNode*
LtePhy::GetDevice (void)
{
  return m_device;
}

void
LtePhy::SetDlChannel (LteChannel* c)
{
  m_dlChannel = c;
}

LteChannel*
LtePhy::GetDlChannel (void)
{
  return m_dlChannel;
}

void
LtePhy::SetUlChannel (LteChannel* c)
{
  m_ulChannel = c;
}

LteChannel*
LtePhy::GetUlChannel (void)
{
  return m_ulChannel;
}

void
LtePhy::SetBandwidthManager (BandwidthManager* s)
{
  m_bandwidthManager = s;
  if (s != NULL)
    DoSetBandwidthManager ();
}

BandwidthManager*
LtePhy::GetBandwidthManager (void)
{
  return m_bandwidthManager;
}

void
LtePhy::SetTxPower (double p)
{
  m_txPower = p;
}

double
LtePhy::GetTxPower (void)
{
  return m_txPower;
}

void
LtePhy::SetTxSignal (TransmittedSignal* txSignal)
{
  m_txSignal = txSignal;
}

TransmittedSignal*
LtePhy::GetTxSignal (void)
{
  return m_txSignal;
}


void
LtePhy::SetInterference (Interference* i)
{
  m_interference = i;
}

void
LtePhy::SetErrorModel (ErrorModel* e)
{
  m_errorModel = e;
}


Interference*
LtePhy::GetInterference (void)
{
  return m_interference;
}


ErrorModel*
LtePhy::GetErrorModel (void)
{
  return m_errorModel;
}
