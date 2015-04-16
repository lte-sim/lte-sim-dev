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



#ifndef MOBILITY_H_
#define MOBILITY_H_

#include "../core/cartesianCoodrdinates/CartesianCoordinates.h"
#include <stdio.h>
#include <stdlib.h>

class Mobility {
public:
	Mobility();
	virtual ~Mobility();

	enum MobilityModel {
		CONSTANT_POSITION,
		RANDOM_DIRECTION,
		RANDOM_WALK,
		RANDOM_WAYPOINT,
		MANHATTAN
	};

	void
	SetNodeID (int id);
	int
	GetNodeID (void) const;

	void
	SetMobilityModel(MobilityModel model);
	Mobility::MobilityModel
	GetMobilityModel(void) const;

	void
	SetAbsolutePosition (CartesianCoordinates *position);
	CartesianCoordinates*
	GetAbsolutePosition (void) const;
	void
	DeleteAbsolutePosition (void);

	virtual void UpdatePosition (double time) = 0;

	void
	SetSpeed (int speed);
	int
	GetSpeed (void) const;
	void
	SetSpeedDirection (double speedDirection);
	double
	GetSpeedDirection (void) const;

	void
	SetPositionLastUpdate (double time);
	double
	GetPositionLastUpdate (void) const;

	void
	SetHandover (bool handover);
	bool
	GetHandover (void) const;
	void
	SetLastHandoverTime (double lastHOtime);
	double
	GetLastHandoverTime (void) const;

	double
	GetTopologyBorder (void);


private:
	int m_nodeID;

	MobilityModel m_mobilityModel;

	CartesianCoordinates *m_AbsolutePosition;
	int m_speed;			 		// Km/h
	double m_speedDirection;	 	// radianti
	double m_positionLastUpdate; 	// s
	bool m_handover;				// true to enable hand over
	double m_handoverLastRun;		// s
};

#endif /* MOBILITY_H_ */
