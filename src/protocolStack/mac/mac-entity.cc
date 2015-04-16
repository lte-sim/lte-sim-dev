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


#include "mac-entity.h"
#include "../packet/Packet.h"
#include "../../device/NetworkNode.h"
#include "AMCModule.h"
#include "harq-manager.h"

MacEntity::MacEntity ()
{}


MacEntity::~MacEntity ()
{
  delete m_amcModule;
  m_device = NULL;
}

void
MacEntity::Destroy (void)
{
  delete m_amcModule;
  delete m_harqmanager;
  m_device = NULL;
}

void
MacEntity::SetDevice (NetworkNode* d)
{
  m_device = d;
}

NetworkNode*
MacEntity::GetDevice ()
{
  return m_device;
}

void
MacEntity::SetAmcModule (AMCModule* amc)
{
  m_amcModule = amc;
}

AMCModule*
MacEntity::GetAmcModule (void) const
{
  return m_amcModule;
}

HarqManager*
MacEntity::GetHarqManager (void)
{
  return m_harqmanager;
}
