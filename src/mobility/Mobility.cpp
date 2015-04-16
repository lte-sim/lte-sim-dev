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


#include "Mobility.h"
#include "../componentManagers/NetworkManager.h"
#include "../networkTopology/Cell.h"
#include "../load-parameters.h"

Mobility::Mobility()
{
  m_AbsolutePosition = NULL;
}

Mobility::~Mobility()
{
  delete m_AbsolutePosition;
}

void
Mobility::SetNodeID (int id)
{
  m_nodeID = id;
}

int
Mobility::GetNodeID (void) const
{
  return m_nodeID;
}

void
Mobility::SetMobilityModel(MobilityModel model)
{
  m_mobilityModel = model;
}

Mobility::MobilityModel
Mobility::GetMobilityModel(void) const
{
  return m_mobilityModel;
}

void
Mobility::SetAbsolutePosition (CartesianCoordinates *position)
{
  if (position == NULL)
    {
	  m_AbsolutePosition = NULL;
      return;
    }

  if (m_AbsolutePosition == NULL)
    {
	  m_AbsolutePosition = new CartesianCoordinates ();
    }

  m_AbsolutePosition->SetCoordinateX (position->GetCoordinateX ());
  m_AbsolutePosition->SetCoordinateY (position->GetCoordinateY ());
}

CartesianCoordinates*
Mobility::GetAbsolutePosition (void) const
{
  return m_AbsolutePosition;
}

void
Mobility::DeleteAbsolutePosition (void)
{
  delete  m_AbsolutePosition;
}

void
Mobility::SetSpeed (int speed)
{
  m_speed = speed;
}

int
Mobility::GetSpeed (void) const
{
  return m_speed;
}

void
Mobility::SetSpeedDirection (double speedDirection)
{
  m_speedDirection = speedDirection;
}

double
Mobility::GetSpeedDirection (void) const
{
  return m_speedDirection;
}

void
Mobility::SetPositionLastUpdate (double time)
{
  m_positionLastUpdate = time;
}

double
Mobility::GetPositionLastUpdate (void) const
{
  return m_positionLastUpdate;
}

void
Mobility::SetHandover (bool handover)
{
  m_handover = handover;
}

bool
Mobility::GetHandover (void) const
{
  return m_handover;
}

void
Mobility::SetLastHandoverTime (double lastHOtime)
{
  m_handoverLastRun = lastHOtime;
}

double
Mobility::GetLastHandoverTime (void) const
{
  return m_handoverLastRun;
}

double
Mobility::GetTopologyBorder (void)
{
  int nbCell = NetworkManager::Init()->GetNbCell();

  switch (nbCell)
    {
      case 1:
    	  return (NetworkManager::Init()->GetCellByID (0)->GetRadius () * 1000);
    	  break;
      case 7:
    	  return ((2.6 * NetworkManager::Init()->GetCellByID (0)->GetRadius ()) * 1000);
    	  break;
      case 19:
          return ((4. * NetworkManager::Init()->GetCellByID (0)->GetRadius ()) * 1000);
          break;
      default:
    	  return 1000.0;
    	  break;
    }
}
