/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Communications Research Laboratory, Ilmenau University of Technology
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
 * Author: Florian Schlembach <florian.schlembach@tu-ilmenau.de>
 * Author2: Jianshu Zhang <jianshu.zhang@tu-ilmenau.de>
 * Author3: Yao Cheng <y.cheng@tu-ilmenau.de>
 */



#include "Manhattan.h"
#include "../componentManagers/NetworkManager.h"
#include "../device/Gateway.h"
#include "../device/ENodeB.h"
#include "../device/UserEquipment.h"
#include "../load-parameters.h"
#include <algorithm>
#include <time.h>
using namespace std;

Manhattan::Manhattan()
{
  SetMobilityModel(Mobility::MANHATTAN);
  SetSpeed (0);
  SetSpeedDirection (0.0);
  SetPositionLastUpdate (0.0);
  SetHandover (false);
  SetLastHandoverTime (0.0);
  SetAbsolutePosition (NULL);
}

Manhattan::~Manhattan()
{
  DeleteAbsolutePosition ();
}

void
Manhattan::UpdatePosition (double timestamp)
{
#ifdef MOBILITY_DEBUG
//	cout << "\t START MOBILITY MODEL for "<< GetNodeID () << endl;
#endif

  if (GetSpeed () == 0)
    {
#ifdef MOBILITY_DEBUG
	cout << "\t\t speed = 0 --> position has not been updated!"<< endl;
#endif
      return;
    }

  double timeInterval = timestamp - GetPositionLastUpdate ();
  double speedDirection;

  UserEquipment *thisNode = NetworkManager::Init ()->GetUserEquipmentByID (GetNodeID ());
  Cell *thisCell = thisNode->GetCell ();


  NetworkNode *targetNode = thisNode->GetTargetNode ();

#ifdef MOBILITY_DEBUG
	cout << "MOBILITY_DEBUG: User ID: " << GetNodeID ()
	    << "\n\t Cell ID " <<
					NetworkManager::Init()->GetCellIDFromPosition (GetAbsolutePosition()->GetCoordinateX(),
																   GetAbsolutePosition()->GetCoordinateY())
		<< "\n\t Initial Position (X): " << GetAbsolutePosition()->GetCoordinateX()
		<< "\n\t Initial Position (Y): " << GetAbsolutePosition()->GetCoordinateY()
		<< "\n\t Speed: " << GetSpeed()
		<< "\n\t Speed Direction: " << GetSpeedDirection()
		<< "\n\t Time Last Update: " << GetPositionLastUpdate()
		<< "\n\t Time Interval: " << timeInterval
		<< endl;
#endif
#ifdef MOBILITY_DEBUG_TAB
	cout <<  GetNodeID () << " "
		<< GetAbsolutePosition()->GetCoordinateX() << " "
		<< GetAbsolutePosition()->GetCoordinateY() << " "
		<< GetSpeed() << " "
		<< GetSpeedDirection() << " "
		<< GetPositionLastUpdate() << " "
		<< timeInterval << " "
		<< "-> ";
#endif
	const double pi = 3.14;

//  Init speedDirection if its not pointing into the direction of Manhattans Streets
	vector<double> directions;
	for(int i=0;i<=3;i++) {
	  directions.push_back(i * 1.57); // 1.57 = 90.0 * ((2.0*pi)/360.0))
	}

	double newdir;
	vector<double>::iterator it;
	it = find(directions.begin(), directions.end(), GetSpeedDirection());

	if(it == directions.end()) {
	  newdir = (double)(pi / 2) * round(GetSpeedDirection() * (double)(2 / pi));
	  if(newdir==6.28) newdir = 0;
	  SetSpeedDirection(newdir);
#ifdef MOBILITY_DEBUG_TAB
	  cout << "NEW SPEED-DIRECTION: " << newdir << " -> ";
#endif
	}

	double shift = timeInterval * (GetSpeed()*(1000.0/3600.0));

	CartesianCoordinates *newPosition = new CartesianCoordinates(GetAbsolutePosition()->GetCoordinateX(), GetAbsolutePosition()->GetCoordinateY());
	CartesianCoordinates *ENodeBPosition = targetNode->GetMobilityModel ()->GetAbsolutePosition ();

// Init Manhattan grid position
  if(fmod(GetAbsolutePosition()->GetCoordinateY(),100)!=0 && fmod(GetAbsolutePosition()->GetCoordinateX(),100)!=0){
	CartesianCoordinates *Correction = new CartesianCoordinates();
	double distfromEnB = newPosition->GetDistance (ENodeBPosition);
	double azim = newPosition->GetPolarAzimut (ENodeBPosition);

	//if it was randomly put outside the cell -> shift it inside
	if(distfromEnB > (thisCell->GetRadius()*1000)) {
	  Correction->SetCoordinates((newPosition->GetDistance (ENodeBPosition) - (thisCell->GetRadius()*1000)) * cos(azim),
			  (newPosition->GetDistance (ENodeBPosition) - (thisCell->GetRadius()*1000)) * sin(azim));
	  newPosition->SetCoordinates(newPosition->GetCoordinateX() - Correction->GetCoordinateX(),
			  newPosition->GetCoordinateY() - Correction->GetCoordinateY());
	}
	delete Correction;


	if(GetSpeedDirection()==0 || GetSpeedDirection()==3.14) {
			  if(newPosition->GetCoordinateY() < 0)
				  newPosition->SetCoordinateY( ceil( (double)(newPosition->GetCoordinateY() / 100) ) * 100);
			  else
				  newPosition->SetCoordinateY( floor( (double)(newPosition->GetCoordinateY() / 100) ) * 100);
		  }
	else {
			  if(newPosition->GetCoordinateX() < 0)
				  newPosition->SetCoordinateX( ceil( (double)(newPosition->GetCoordinateX() / 100)) * 100);
			  else
				  newPosition->SetCoordinateX( floor( (double)(newPosition->GetCoordinateX() / 100) ) * 100);
	}
  }

//Shift it
  double shift_x = shift * cos(GetSpeedDirection());
  double shift_y = shift * sin(GetSpeedDirection());
  if(GetSpeedDirection()==0 || GetSpeedDirection()==3.14) {
	  newPosition->SetCoordinateX(newPosition->GetCoordinateX()+shift_x);
  }
  else {
	  newPosition->SetCoordinateY(newPosition->GetCoordinateY()+shift_y);
  }

// if a node reaches a crossing, choose new speedDirection
  double old_x = abs( ((int)( GetAbsolutePosition()->GetCoordinateX() *1000))/1000.0 ); //cut after 3 decimal places
  double new_x = abs( ((int)( newPosition->GetCoordinateX() *1000))/1000.0 );
  double old_y = abs( ((int)( GetAbsolutePosition()->GetCoordinateY() *1000))/1000.0 );
  double new_y = abs( ((int)( newPosition->GetCoordinateY() *1000))/1000.0 );
  double rounded_x = abs( round(old_x/100)*100 );
  double rounded_y = abs( round(old_y/100)*100 );

  if( ((old_x<rounded_x && rounded_x<=new_x) || (old_x>rounded_x && rounded_x>=new_x)) ||
		  ((old_y<rounded_y && rounded_y<=new_y) || (old_y>rounded_y && rounded_y>=new_y)) ||
		  (rounded_x==0 && old_x<rounded_x && rounded_x<=new_x) || (rounded_x==0 && old_x>rounded_x && rounded_x>=new_x) ||
		  (rounded_y==0 && old_y<rounded_y && rounded_y<=new_y) || (rounded_y==0 && old_y>rounded_y && rounded_y>=new_y) )
  {
	  srand ( time(NULL) );
	  double prob_turn = (rand()%100)*0.01;
	  if(prob_turn<=0.25) {
		  speedDirection = GetSpeedDirection() + 1.57; //turn left;
		  newPosition->SetCoordinates(round(newPosition->GetCoordinateX()),round(newPosition->GetCoordinateY()));
#ifdef MOBILITY_DEBUG_TAB
		  cout << "TURN LEFT: " << speedDirection << " -> ";
#endif
	  }
	  if(prob_turn>0.25 && prob_turn<0.75) {
		  newPosition->SetCoordinates(round(newPosition->GetCoordinateX()),round(newPosition->GetCoordinateY()) );
		  speedDirection = GetSpeedDirection();
#ifdef MOBILITY_DEBUG_TAB
		  cout << "no TURN: straight -> ";
#endif
	  }
	  if(prob_turn>=0.75) {
		  newPosition->SetCoordinates(round(newPosition->GetCoordinateX()),round(newPosition->GetCoordinateY()) );
		  speedDirection = GetSpeedDirection() - 1.57;
#ifdef MOBILITY_DEBUG_TAB
		  cout << "TURN RIGHT: " << speedDirection << " -> ";
#endif
	  }

	  //Correction if speedDirection €! [0,2pi[
	  if(speedDirection>=2*pi) speedDirection -= 2*pi;
	  if(speedDirection<0) speedDirection += 2*pi;
	  SetSpeedDirection(speedDirection);
  }



//If node moves beyond the cell edge
  double azimut = newPosition->GetPolarAzimut (ENodeBPosition);
  double newDistanceFromTheENodeB = newPosition->GetDistance (ENodeBPosition);

  if (newDistanceFromTheENodeB >= (thisCell->GetRadius()*1000))
    {
	  if (GetHandover()== false)
		{
		  newPosition->SetCoordinateX(GetAbsolutePosition()->GetCoordinateX());
		  newPosition->SetCoordinateY(GetAbsolutePosition()->GetCoordinateY());

		  speedDirection = GetSpeedDirection() - pi;
#ifdef MOBILITY_DEBUG_TAB
		  cout << "Moved back to cell: SPEED DIRECTION CHANGE (HO): " << speedDirection << " -> ";
#endif
		  if(speedDirection<0) speedDirection = speedDirection + 2*pi;
		  SetSpeedDirection(speedDirection);
		}
	  else if (newPosition->GetDistance(0.0, 0.0) >= GetTopologyBorder ())
	      {
		  newPosition->SetCoordinateX(GetAbsolutePosition()->GetCoordinateX());
		  newPosition->SetCoordinateY(GetAbsolutePosition()->GetCoordinateY());

		  speedDirection = GetSpeedDirection() - 1.57;
#ifdef MOBILITY_DEBUG_TAB
		  cout << "Moved back to cell: SPEED DIRECTION CHANGE (TOPOLOGY-BORDER): " << speedDirection << " -> ";
#endif
		  if(speedDirection<0) speedDirection = speedDirection + 2*pi;
		  SetSpeedDirection(speedDirection);
	      }
    }

  SetAbsolutePosition(newPosition);
  SetPositionLastUpdate (timestamp);

#ifdef MOBILITY_DEBUG
  cout << "\n\t Final Position (X): " << GetAbsolutePosition()->GetCoordinateX()
			<< "\n\t Final Position (Y): " << GetAbsolutePosition()->GetCoordinateY()
			<< endl;
#endif
#ifdef MOBILITY_DEBUG_TAB
  cout << GetAbsolutePosition()->GetCoordinateX() << " "
			<< GetAbsolutePosition()->GetCoordinateY() << " " << GetSpeedDirection()
			<< endl;
#endif
  delete newPosition;
}

