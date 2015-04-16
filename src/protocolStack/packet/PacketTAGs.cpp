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


#include "PacketTAGs.h"

PacketTAGs::PacketTAGs()
{
  m_type = PacketTAGs::APPLICATION_TYPE_INFINITE_BUFFER;
  m_frameNumber = 0;
  m_startByte = 0;
  m_endBytes = 0;
}

PacketTAGs::PacketTAGs(PacketTAGs::ApplicationType type,
					   int frameNumber,
					   int startFrame,
					   int endFrame)
{
  m_type = type;
  m_frameNumber = frameNumber;
  m_startByte = startFrame;
  m_endBytes = endFrame;
}

PacketTAGs::~PacketTAGs()
{
}

void
PacketTAGs::SetFrameNumber (int frameNumber)
{
  m_frameNumber = frameNumber;
}

int
PacketTAGs::GetFrameNuber (void) const
{
  return m_frameNumber;
}

void
PacketTAGs::SetStartByte (int startByte)
{
  m_startByte = startByte;
}

int
PacketTAGs::GetStartByte (void) const
{
  return m_startByte;
}

void
PacketTAGs::SetEndByte (int endByte)
{
  m_endBytes = endByte;
}

int
PacketTAGs::GetEndByte (void) const
{
  return m_endBytes;
}

void
PacketTAGs::SetApplicationType (PacketTAGs::ApplicationType type)
{
  m_type = type;
}

PacketTAGs::ApplicationType
PacketTAGs::GetApplicationType (void)
{
  return m_type;
}

void
PacketTAGs::SetApplicationSize (int s)
{
  m_applicationSize = s;
}

int
PacketTAGs::GetApplicationSize ()
{
  return m_applicationSize;
}
