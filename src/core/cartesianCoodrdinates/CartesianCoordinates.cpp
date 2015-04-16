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



#include <stdint.h>
#include <iostream>
#include "CartesianCoordinates.h"

CartesianCoordinates::CartesianCoordinates()
{}

CartesianCoordinates::CartesianCoordinates(double x, double y)
{
  SetCoordinates(x,y);
}

CartesianCoordinates::CartesianCoordinates(double x, double y, double z)
{
  SetCoordinates(x,y,z);
}

CartesianCoordinates::~CartesianCoordinates()
{}

void
CartesianCoordinates::SetCoordinates (double x,
									  double y)
{
  m_posX = x;
  m_posY = y;
  m_posZ = 0.0;
}

void
CartesianCoordinates::SetCoordinates (double x,
									  double y,
									  double z)
{
  m_posX = x;
  m_posY = y;
  m_posZ = z;
}

void
CartesianCoordinates::SetCoordinatesFromPolar(CartesianCoordinates *center,
											  double radius,
											  double angle)
{
  m_posX = radius * sin (angle) + center->GetCoordinateX();
  m_posY = radius * cos (angle) + center->GetCoordinateY();
}

double
CartesianCoordinates::GetDistance(CartesianCoordinates *remoteObject)
{
  /*
   * Computes distance from this object and remoteObject
   */
  return
      sqrt (pow ((GetCoordinateX() - remoteObject->GetCoordinateX()),2) +
          pow ( (GetCoordinateY() - remoteObject->GetCoordinateY()) ,2));

}

double
CartesianCoordinates::GetDistance(double remoteObjectPosX,
								  double remoteObjectPosY)
{
  /*
   * Computes distance from this object and remoteObject
   */
  return
      sqrt (pow ((GetCoordinateX() - remoteObjectPosX),2) +
          pow ((GetCoordinateY() - remoteObjectPosY),2));
}

double
CartesianCoordinates::GetPolarAzimut(CartesianCoordinates *center)
{
	const double pi = 3.1415926;
	double X = GetCoordinateX() - center->GetCoordinateX();
	double Y = GetCoordinateY() - center->GetCoordinateY();
	double angle = atan2(Y, X);
	// azimut € [0,2*pi[
	if ( angle < 0 )
	  {
		angle = angle + (2*pi);
	  }
	return angle;
}

double
CartesianCoordinates::GetCoordinateX()
{
  return m_posX;
}

double
CartesianCoordinates::GetCoordinateY()
{
  return m_posY;
}

double
CartesianCoordinates::GetCoordinateZ()
{
  return m_posZ;
}

void
CartesianCoordinates::SetCoordinateX(double X)
{
  m_posX = (double) X;
}

void
CartesianCoordinates::SetCoordinateY(double Y)
{
  m_posY = (double) Y;
}

void
CartesianCoordinates::SetCoordinateZ(double Z)
{
  m_posZ = (double) Z;
}


CartesianCoordinates*
CartesianCoordinates::operator+(CartesianCoordinates *A)
{
  CartesianCoordinates *Result =
      new CartesianCoordinates ((GetCoordinateX() + A->GetCoordinateX()),
    		  					(GetCoordinateY() + A->GetCoordinateY()));
	return Result;
}

CartesianCoordinates*
CartesianCoordinates::operator-(CartesianCoordinates *B)
{
  CartesianCoordinates *Result =
      new CartesianCoordinates ((GetCoordinateX() - B->GetCoordinateX()),
								(GetCoordinateY() - B->GetCoordinateY()));
  return Result;
}

//Debug
void
CartesianCoordinates::Print()
{
  std::cout << "\t CenterCellPosition: "
		  "x = " << GetCoordinateX() <<
		  ", y = " << GetCoordinateY()
		  << std::endl;
}
