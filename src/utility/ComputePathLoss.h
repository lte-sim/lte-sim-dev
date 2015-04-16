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


#ifndef COMPUTE_PATH_LOSS_H_
#define COMPUTE_PATH_LOSS_H_

#include "../device/NetworkNode.h"
#include <stdint.h>
#include "stdlib.h"
#include <math.h>
#include "IndoorScenarios.h"


static double
ComputePathLossForInterference (NetworkNode* src, NetworkNode* dst)
{
	/*
	 * Path loss Models from sect. 5.2 in
	 * 3GPP TSG RAN WG4 R4-092042
	 */
  double externalWallAttenuation = 20;
  double pathLoss = 0.0;

  if (src->GetNodeType() == NetworkNode::TYPE_ENODEB && dst->GetNodeType () == NetworkNode::TYPE_UE)
    {
	  //Path Loss Model For Urban Environment
	  double distance = src->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (
			  dst->GetMobilityModel ()->GetAbsolutePosition ());

	  // Urban Area
	  pathLoss = 128.1 + (37.6 * log10 (distance * 0.001));
	  // Sub-Urban area
	  // pathLoss = 128.1 + (37.6 * log10 (distance * 0.001));
	  // Rural Area
	  // pathLoss = 100.54 + (34.1 * log10 (distance * 0.001));

	  if ( ((UserEquipment*) dst)->IsIndoor() )
	  {
		  pathLoss = pathLoss + externalWallAttenuation;
	  }

	  return pathLoss;
    }

  if (src->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION && dst->GetNodeType () == NetworkNode::TYPE_UE)
    {
	  double minimumCouplingLoss = 45; //[dB] - see 3GPP TSG RAN WG4 #42bis (R4-070456)
	  double floorPenetration = 0.0;

	  //Path Loss Model For Urban Environment (Indoor Case)
      /*
	  double distance = src->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (
			  dst->GetMobilityModel ()->GetAbsolutePosition ());

	  if( src->GetCell()->GetCellCenterPosition()->GetCoordinateZ() > 0
	 			  && ((UserEquipment*) dst)->IsIndoor()
	 			     && src->GetCell()->GetCellCenterPosition()->GetCoordinateZ() != dst->GetCell()->GetCellCenterPosition()->GetCoordinateZ())
	 	  {
	 		  int n = (int) abs( src->GetCell()->GetCellCenterPosition()->GetCoordinateZ() - dst->GetCell()->GetCellCenterPosition()->GetCoordinateZ() );
	 		  floorPenetration = 18.3 * pow( n, ((n+2)/(n+1)-0.46));
	 	  }

	  pathLoss = max( minimumCouplingLoss, 127 + ( 30 * log10 (distance * 0.001) ) + floorPenetration);
	  return pathLoss;
	  */


	  //Path Loss Model For WinnerII

	  double distance = src->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (
	  			  dst->GetMobilityModel ()->GetAbsolutePosition ());
	  int* nbWalls = GetWalls( (Femtocell*) (src->GetCell()), (UserEquipment*) dst);

	    double A, B, C;
	    double ExternalWallsAttenuation = 20.0;
	    double InternalWallsAttenuation = 10.0;

	    if (nbWalls[0] == 0 && nbWalls[1] == 0)
	      { //LOS
	        A = 18.7;
	        B = 46.8;
	        C = 20.0;
	      }
	    else
	      { //NLOS
	        A = 20.0;
	        B = 46.4;
	        C = 20.0;
	      }

	    pathLoss = A * log10( distance ) +
	                         B +
	                         C * log10(2. / 5.0) +
	                         InternalWallsAttenuation * nbWalls[1] +
	                         ExternalWallsAttenuation * nbWalls[0];

	    delete [] nbWalls;




	  return max(minimumCouplingLoss, pathLoss);

    }

  return pathLoss;

  //XXX: add other path loss models.
}



#endif /* COMPUTE_PATH_LOSS_H_ */
