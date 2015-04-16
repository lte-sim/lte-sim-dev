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

#include "DualStripe.h"
#include "Femtocell.h"
#include "../core/cartesianCoodrdinates/CartesianCoordinates.h"
#include "../device/UserEquipment.h"

DualStripe::DualStripe (int idBuilding,
					int type,
	     	 	    int nbFloors,
	     	 	    double side_X,
	     	 	    double side_Y,
	     	 	    double center_X,
	     	 	    double center_Y)
{
	SetIdBuilding(idBuilding);

	SetBuildingType(TYPE_3GPP_dual_stripe);

	SetFloors(nbFloors);

	double* side = new double[2];
	side [0] = side_X;
	side [1] = side_Y;
	SetSide(side);

	CartesianCoordinates *Position = new CartesianCoordinates(center_X,center_Y);
	SetCenterPosition(Position);

	std::vector<Femtocell*>* femtoCellsInBuilding = new std::vector<Femtocell*>;
	SetFemtoCellsInBuilding(femtoCellsInBuilding);
}

DualStripe::~DualStripe()
{
  ClearAll();
}
