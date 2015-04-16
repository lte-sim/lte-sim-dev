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

#include "Street.h"
#include "Building.h"
#include "../core/cartesianCoodrdinates/CartesianCoordinates.h"
#include <assert.h>


Street::Street( int id,
		        CartesianCoordinates* center,
		        double theta,
		        int nbBuilding,
		        int building_type,
		        double street_width, double building_distance,
		        double apartmentSide)
{
  m_streetID = id;

  m_centerPosition = center;

  m_thetaStreet = theta;
  assert (m_thetaStreet == 0 || m_thetaStreet == 90);

  m_buildingTypeInStreet = building_type;
  m_nbBuildings = nbBuilding;
  m_streetWidth = street_width;
  m_buildingDistance = building_distance;
  m_apartmentSide = apartmentSide;
}

Street::~Street()
{
  delete m_centerPosition;
}

std::vector<CartesianCoordinates*>*
Street::GetBuildingDistributionInStreet ( void )
{
  std::vector<CartesianCoordinates*> *vectorOfCoordinates = new std::vector<CartesianCoordinates*>;

  if (m_buildingTypeInStreet == 0 )
  {//Building::TYPE_3GPP_5x5_grid

	  double street_length = ( m_nbBuildings * 5 * m_apartmentSide ) +
								  ( (m_nbBuildings - 1) * m_buildingDistance );

	  for (int i = 0; i < m_nbBuildings; i++)
	  {
		  double x_1, x_2, y_1, y_2;

		  if ( m_thetaStreet == 0 )
		  {
			  x_1 = m_centerPosition->GetCoordinateX() - street_length/2 + m_apartmentSide*2.5 + ( i * ( 5 * m_apartmentSide + m_buildingDistance ) );
			  y_1 = m_centerPosition->GetCoordinateY() + m_streetWidth/2 + m_apartmentSide*2.5;

			  x_2 = x_1;
			  y_2 = m_centerPosition->GetCoordinateY() - m_streetWidth/2 - m_apartmentSide*2.5;
		  }
		  else if ( m_thetaStreet == 90 )
		  {
			  x_1 = m_centerPosition->GetCoordinateX() - m_streetWidth/2 - m_apartmentSide*2.5;
			  y_1 = m_centerPosition->GetCoordinateY() - street_length/2 + m_apartmentSide*2.5 + ( i * ( 5 * m_apartmentSide + m_buildingDistance ) );;

			  x_2 = m_centerPosition->GetCoordinateX() + m_streetWidth/2 + m_apartmentSide*2.5;
			  y_2 = y_1;
		  }


		  CartesianCoordinates *newCoordinates_1 = new CartesianCoordinates( x_1, y_1);
		  CartesianCoordinates *newCoordinates_2 = new CartesianCoordinates( x_2, y_2);

		  vectorOfCoordinates->push_back(newCoordinates_1);
		  vectorOfCoordinates->push_back(newCoordinates_2);
	  }
  }
  else
  {
	 ////XXX: TO BE IMPLEMENTED
  }

  return vectorOfCoordinates;
}
