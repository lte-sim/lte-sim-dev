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

#include "cqi-manager.h"
#include "../../core/eventScheduler/simulator.h"
#include "../NetworkNode.h"

CqiManager::CqiManager()
{
  m_device  = 0;
}

CqiManager::~CqiManager()
{
  m_device = 0;
}

void
CqiManager::SetDevice (NetworkNode* d)
{
  m_device = d;
}

NetworkNode*
CqiManager::GetDevice (void)
{
  return m_device;
}

void
CqiManager::SetCqiReportingMode (CqiManager::CQIReportingMode m)
{
  m_reportingMode = m;
  if (m_reportingMode == CqiManager::APERIODIC)
    {
	  m_sendCqi = false;
    }
  else
    {
	  m_sendCqi = true;
    }
}

CqiManager::CQIReportingMode
CqiManager::GetCqiReportingMode (void)
{
  return m_reportingMode;
}

void
CqiManager::SetSendCqi (bool b)
{
  m_sendCqi = b;
}

bool
CqiManager::GetSendCqi (void)
{
  return m_sendCqi;
}


void
CqiManager::SetReportingInterval (int i)
{
  m_reportingInterval = i;
}

int
CqiManager::GetReportingInterval (void)
{
  return m_reportingInterval;
}

void
CqiManager::SetLastSent ()
{
  m_lastSent = Simulator::Init ()->Now () * 1000; //TTI
}

long int
CqiManager::GetLastSent (void)
{
  return m_lastSent;
}

bool
CqiManager::NeedToSendFeedbacks (void)
{
//  std::cout <<"NeedToSendFeedbacks " << (Simulator::Init ()->Now ()*1000) << GetReportingInterval () << std::endl;

  if (GetSendCqi () && GetReportingInterval () == 1 )
    {
	  return true;
    }

  else if (GetSendCqi () && ((int)(Simulator::Init ()->Now ()*1000) - GetLastSent ()) >= GetReportingInterval ())
    {
      return true;
    }
  else
    {
	  return false;
    }
}
