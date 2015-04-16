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



#ifndef USEREQUIPMENT_H_
#define USEREQUIPMENT_H_

#include "NetworkNode.h"

class ENodeB;
class Gateway;
class CqiManager;


class UserEquipment : public NetworkNode {
public:
	UserEquipment ();
	UserEquipment (int idElement,
				   double posx, double posy,
				   Cell *cell,
				   NetworkNode* target,
				   bool handover, Mobility::MobilityModel model);
	UserEquipment (int idElement,
				   double posx, double posy, int speed, double speedDirection,
				   Cell *cell,
				   NetworkNode* target,
				   bool handover, Mobility::MobilityModel model);

	virtual ~UserEquipment();

	void SetTargetNode (NetworkNode *n);
	NetworkNode* GetTargetNode (void);

	void UpdateUserPosition (double time);

	void SetCqiManager (CqiManager *cm);
	CqiManager* GetCqiManager (void);

	void
	SetIndoorFlag ( bool flag );
	bool
	IsIndoor (void);

	//Debug
	void Print (void);

private:
	NetworkNode* m_targetNode;
	CqiManager *m_cqiManager;

	bool m_isIndoor;

	double m_timePositionUpdate;
};

#endif /* USEREQUIPMENT_H_ */
