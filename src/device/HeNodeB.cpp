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
 * Author: Francesco Capozzi <f.capozzi@poliba.it>
 */

#include "HeNodeB.h"
#include "../phy/henb-lte-phy.h"


HeNodeB::HeNodeB ( int idElement,
	    		   Femtocell *cell)
{
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_HOME_BASE_STATION);
  SetFemtoCell (cell);

  double pos_X = cell->GetCellCenterPosition()->GetCoordinateX();
  double pos_Y = cell->GetCellCenterPosition()->GetCoordinateY();

  CartesianCoordinates *position = new CartesianCoordinates(pos_X, pos_Y);
  Mobility* m = new ConstantPosition ();
  m->SetAbsolutePosition (position);
  SetMobilityModel (m);

  CreateUserEquipmentRecords();

  HenbLtePhy *phy = new HenbLtePhy ();
  phy->SetDevice(this);
  SetPhy (phy);

  ProtocolStack *stack = new ProtocolStack (this);
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  classifier->SetDevice (this);
  SetClassifier (classifier);
}

HeNodeB::~HeNodeB()
{
  Destroy ();
  DeleteUserEquipmentRecords ();
}

void
HeNodeB::SetFemtoCell (Femtocell *cell)
{
  m_femtocell = cell;
  SetCell( (Cell*) cell);
}

Femtocell*
HeNodeB::GetFemtoCell (void)
{
  return m_femtocell;
}

