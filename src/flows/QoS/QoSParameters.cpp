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


#include "QoSParameters.h"
#include <iostream>

QoSParameters::QoSParameters()
{
  m_qci = 0;
  m_gbr = 0;
  m_mbr = 0;
  m_arpPreEmptionCapability = false;
  m_arpPreEmptionVulnerability = false;
  m_maxDelay = 0;
}

QoSParameters::QoSParameters (int qci,
							  double gbr,
				              double mbr)
{
  m_qci = qci;
  m_gbr = gbr;
  m_mbr = mbr;
  m_arpPreEmptionCapability = false;
  m_arpPreEmptionVulnerability = false;
  m_maxDelay = 0;
}

QoSParameters::QoSParameters (int qci,
							  bool arpPreEmptionCapability,
							  bool arpPreEmptionVulnerability,
							  double gbr,
							  double mbr)
{
  m_qci = qci;
  m_gbr = gbr;
  m_mbr = mbr;
  m_arpPreEmptionCapability = arpPreEmptionCapability;
  m_arpPreEmptionVulnerability = arpPreEmptionVulnerability;
  m_maxDelay = 0;
}


QoSParameters::~QoSParameters()
{}

void
QoSParameters::SetQCI (int qci)
{
  m_qci = qci;
}

int
QoSParameters::GetQCI (void) const
{
  return m_qci;
}

void
QoSParameters::SetArpPreEmptionCapability (bool flag)
{
  m_arpPreEmptionCapability = flag;
}

bool
QoSParameters::GetArpPreEmptionCapability (void) const
{
  return m_arpPreEmptionCapability;
}

void
QoSParameters::SetArpPreEmptionVulnerability (bool flag)
{
  m_arpPreEmptionVulnerability = flag;
}

bool
QoSParameters::GetArpPreEmptionVulnerability (void) const
{
  return m_arpPreEmptionVulnerability;
}

void
QoSParameters::SetGBR (double gbr)
{
  m_gbr = gbr;
}

double
QoSParameters::GetGBR (void) const
{
  return m_gbr;
}

void
QoSParameters::SetMBR (double mbr)
{
  m_mbr = mbr;
}

double
QoSParameters::GetMBR (void) const
{
  return m_mbr;
}

void
QoSParameters::SetMaxDelay (double targetDelay)
{
  m_maxDelay = targetDelay;
}

double
QoSParameters::GetMaxDelay (void) const
{
  return m_maxDelay;
}
