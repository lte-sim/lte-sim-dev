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


#include "Cell.h"

Cell::Cell()
{}

Cell::Cell(int idCell,
		   double radius,
		   double minDistance,
		   double X,
		   double Y)
{
  m_idCell = idCell;
  m_radius = radius;
  m_minDistance = minDistance;
  CartesianCoordinates *Position = new CartesianCoordinates(X,Y);
  SetCellCenterPosition(Position);
}

Cell::~Cell()
{
  delete m_CellCenterPosition;
}

void
Cell::SetIdCell (int idCell)
{
  m_idCell = idCell;
}

int
Cell::GetIdCell (void) const
{
  return m_idCell;
}

void
Cell::SetRadius (double radius)
{
  m_radius = radius;
}

double
Cell::GetRadius (void) const
{
  return m_radius;
}

void
Cell::SetMinDistance (double minDistance)
{
  m_minDistance = minDistance;
}

double
Cell::GetMinDistance (void) const
{
  return m_minDistance;
}

void
Cell::SetCellCenterPosition(CartesianCoordinates *position)
{
	m_CellCenterPosition = position;
}

CartesianCoordinates*
Cell::GetCellCenterPosition () const
{
	return m_CellCenterPosition;
}

//Debug
void
Cell::Print (void)
{
  std::cout << "Cell object:"
      "\n\t m_idCell = " << m_idCell <<
      "\n\t m_radius = " << m_radius <<
	  "\n\t m_minDistance = " <<  m_minDistance <<
	  std::endl;

  GetCellCenterPosition ()->Print();
}
