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


#ifndef USERSDISTRIBTION_H_
#define USERSDISTRIBTION_H_

#include "../core/cartesianCoodrdinates/CartesianCoordinates.h"
#include "CellPosition.h"
#include "../componentManagers/NetworkManager.h"

#include <vector>
#include <iostream>

static CartesianCoordinates*
GetCartesianCoordinatesFromPolar (double r, double angle)
{
  double x = r * cos (angle);
  double y = r * sin (angle);

  CartesianCoordinates *coordinates = new CartesianCoordinates ();
  coordinates->SetCoordinates(x,y);
  return coordinates;
}

static vector<CartesianCoordinates*>*
GetUniformUsersDistribution (int idCell, int nbUE)
{
  NetworkManager * networkManager = NetworkManager::Init();
  vector<CartesianCoordinates*> *vectorOfCoordinates = new vector<CartesianCoordinates*>;

  Cell *cell = networkManager->GetCellByID(idCell);

  double radius = (cell->GetRadius()*1000)*0.8;

  CartesianCoordinates *cellCoordinates = cell->GetCellCenterPosition();
  double r; double angle;

  for (int i = 0; i < nbUE; i++)
    {
	  r = (double)(rand() %(int)radius);
	  angle = (double)(rand() %360) * ((2*3.14)/360);

	  CartesianCoordinates *newCoordinates = GetCartesianCoordinatesFromPolar (r, angle);

	  //Compute absoluteCoordinates
	  newCoordinates->SetCoordinateX (cellCoordinates->GetCoordinateX () + newCoordinates->GetCoordinateX ());
	  newCoordinates->SetCoordinateY (cellCoordinates->GetCoordinateY () + newCoordinates->GetCoordinateY ());

	  vectorOfCoordinates->push_back(newCoordinates);
    }

  return vectorOfCoordinates;
}

static vector<CartesianCoordinates*>*
GetUniformUsersDistributionInFemtoCell (int idCell, int nbUE)
{
  NetworkManager * networkManager = NetworkManager::Init();
  vector<CartesianCoordinates*> *vectorOfCoordinates = new vector<CartesianCoordinates*>;

  Femtocell *cell = networkManager->GetFemtoCellByID(idCell);

  double side = cell->GetSide();

  CartesianCoordinates *cellCoordinates = cell->GetCellCenterPosition();
  double r; double angle;

  for (int i = 0; i < nbUE; i++)
    {
	  r = (double)(rand() %(int)side);
	  angle = (double)(rand() %360) * ((2*3.14)/360);

	  CartesianCoordinates *newCoordinates = GetCartesianCoordinatesFromPolar (r, angle);

	  //Compute absoluteCoordinates
	  newCoordinates->SetCoordinateX (cellCoordinates->GetCoordinateX () + newCoordinates->GetCoordinateX ());
	  newCoordinates->SetCoordinateY (cellCoordinates->GetCoordinateY () + newCoordinates->GetCoordinateY ());

	  vectorOfCoordinates->push_back(newCoordinates);
    }

  return vectorOfCoordinates;
}


#endif /* USERSDISTRIBTION_H_ */
