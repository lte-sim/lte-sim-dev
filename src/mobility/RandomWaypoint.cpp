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



#include "RandomWaypoint.h"
#include "../componentManagers/NetworkManager.h"
#include "../device/Gateway.h"
#include "../device/ENodeB.h"
#include "../device/UserEquipment.h"
#include "../load-parameters.h"


RandomWaypoint::RandomWaypoint()
{
  SetMobilityModel(Mobility::RANDOM_WAYPOINT);
  SetSpeed (0);
  SetSpeedDirection (0.0);
  SetPositionLastUpdate (0.0);
  SetHandover (false);
  SetLastHandoverTime (0.0);
  SetAbsolutePosition (NULL);
}

RandomWaypoint::~RandomWaypoint()
{
  DeleteAbsolutePosition ();
}

void
RandomWaypoint::UpdatePosition(double time)
{

}

