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

#ifndef INDOORSCENARIOS_H_
#define INDOORSCENARIOS_H_

//#include "../core/cartesianCoodrdinates/CartesianCoordinates.h"
//#include "../componentManagers/NetworkManager.h"


#include <vector>
#include <iostream>
#include "../networkTopology/Building.h"
#include "../componentManagers/NetworkManager.h"

static vector<CartesianCoordinates*>*
GetUniformBuildingDistribution (int idCell, int nbBuilding)
{
  NetworkManager * networkManager = NetworkManager::Init();
  vector<CartesianCoordinates*> *vectorOfCoordinates = new vector<CartesianCoordinates*>;

  Cell *cell = networkManager->GetCellByID(idCell);

  double radius = (cell->GetRadius()*1000)*0.8;

  CartesianCoordinates *cellCoordinates = cell->GetCellCenterPosition();
  double r; double angle;

  for (int i = 0; i < nbBuilding; i++)
    {
	  r = (double)(rand() %(int)radius);
	  angle = (double)(rand() %360) * ((2*3.14)/360);

	  double x = r * cos (angle);
	  double y = r * sin (angle);

	  CartesianCoordinates *newCoordinates = new CartesianCoordinates( cellCoordinates->GetCoordinateX () + x,
																	   cellCoordinates->GetCoordinateY () + y);

	  vectorOfCoordinates->push_back(newCoordinates);
    }

  return vectorOfCoordinates;
}

static double*
GetFemtoCellOffsetInBuilding (Building::BuildingType type, int idCell, double side)
{
  double* offset;

  if ( type == Building::TYPE_3GPP_5x5_grid )
  {
	  int x_offset [25] = { -2,-1, 0, 1, 2,-2,-1, 0, 1, 2,-2,-1, 0, 1, 2,-2,-1, 0, 1, 2,-2,-1, 0, 1, 2};
	  int y_offset [25] = {  2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-2,-2,-2,-2,-2};

	  offset = new double[2];
	  offset[0] = (x_offset [idCell] * side);
	  offset[1] = (y_offset [idCell] * side);

  }
  else if ( type == Building::TYPE_3GPP_dual_stripe)
  {
	  double x_offset [10] = { -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5};
	  double y_offset [4] = {  2.5, 1.5, -1.5, -2.5};

	  offset = new double[2];
	  offset[0] = (x_offset [idCell%10] * side);
	  offset[1] = (y_offset [ (int) floor( (double) idCell / (double) 10 )%4 ] * side);

  }
  else
  {
	  std::cout << "ERROR: Wrong Building Type. Exiting...." << std::endl;
	  exit(1);
  }

  return offset;
}

static int cellsIndex_5x5grid [2][25] =
{
  {
    0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4,
  },
  {
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,
  },
};

static int*
GetWalls(Femtocell* henb_cell, UserEquipment* ue)
{
	int *nbWalls = new int[2];
	nbWalls[0] = 0; //External Walls
	nbWalls[1] = 0; //Internal Walls

	Building* henb_building = NetworkManager::Init()->GetBuildingByFemtoCellID(henb_cell->GetIdCell());
	int henb_cell_id = henb_building->GetFemtoIdInBuilding(henb_cell->GetIdCell());
	int ue_cell_id;

	nbWalls[0] = 0;


	if (henb_building->IsInThisBuilding(ue))
	{
		// UE and HeNB are in the same building

		// there are no external walls in between
		nbWalls[0] = 0;

		ue_cell_id = henb_building->GetFemtoIdInBuilding(NetworkManager::Init()->GetBelongingCellFromPosition(ue)->GetIdCell());

	}
	else if(ue->IsIndoor())
	{
		// UE is inside a different Building

		// there sure are 2 external walls in between
		nbWalls[0] = 2;

		// For internal wall calculation, we consider the closest femtocell to the ue
		ue_cell_id = henb_building->GetFemtoIdInBuilding(henb_building->GetClosestFemtoCellToUE(ue)->GetIdCell());

	}
	else
	{
		// UE is in the macro-cell

		// there sure is 1 external walls in between
		nbWalls[0] = 1;

		// For internal wall calculation, we consider the closest femtocell to the ue
		ue_cell_id = henb_building->GetFemtoIdInBuilding(henb_building->GetClosestFemtoCellToUE(ue)->GetIdCell());
	}

	if(henb_building->GetBuildingType() == Building::TYPE_3GPP_5x5_grid)
	{

		nbWalls[1] = abs( ( cellsIndex_5x5grid [0] [henb_cell_id] - cellsIndex_5x5grid [0] [ue_cell_id] ) )
										+ abs ( ( cellsIndex_5x5grid [1] [henb_cell_id] - cellsIndex_5x5grid [1] [ue_cell_id] ) );

	}
	else if (henb_building->GetBuildingType() == Building::TYPE_3GPP_dual_stripe)
	{
		//XXX: To BE REVISED
		/*int X_eNB_index = henb_cell_id%10;
		int Y_eNB_index = (int) floor( (double) eNodeBCellID / (double) 10 )%4;

		int X_ue_index = UserCellID%10;
		int Y_ue_index = (int) floor( (double) UserCellID / (double) 10 )%4;

		nbWalls[1] = abs( X_eNB_index - X_ue_index )
									+ abs ( Y_eNB_index - Y_ue_index );

		if ( ( Y_eNB_index<=1 && Y_ue_index>1 ) || ( Y_eNB_index>1 && Y_ue_index<=1 ) )
		{
			nbWalls[0]  = 2;
			nbWalls[1] -= 1;
		}*/

	}
	return nbWalls;
}

#endif /* INDOORSCENARIOS_H_ */
