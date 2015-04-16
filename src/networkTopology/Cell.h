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

#ifndef CELL_H_
#define CELL_H_

#include <stdint.h>
#include <iostream>
#include "../core/cartesianCoodrdinates/CartesianCoordinates.h"

using namespace std;

class Cell {
private:
	int m_idCell;
	double m_radius;			// Km
	double m_minDistance;	// km
	CartesianCoordinates *m_CellCenterPosition;

public:
	Cell();
	Cell(int idCell,
		 double radius,
		 double minDistance,
		 double X,
		 double Y);
	virtual ~Cell();

	void
	SetIdCell (int idCell);
	int
	GetIdCell (void) const;
	void
	SetRadius (double radius);
	double
	GetRadius (void) const;
	void
	SetMinDistance (double minDistance);
	double
	GetMinDistance (void) const;
	void
	SetCellCenterPosition (CartesianCoordinates *position);
	CartesianCoordinates*
	GetCellCenterPosition (void) const;


	//Debug
	void
	Print (void);
};

#endif /* CELL_H_ */
