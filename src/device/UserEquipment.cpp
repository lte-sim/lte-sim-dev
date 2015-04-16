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


#include "UserEquipment.h"
#include "NetworkNode.h"
#include "ENodeB.h"
#include "HeNodeB.h"
#include "Gateway.h"
#include "../phy/ue-lte-phy.h"
#include "CqiManager/cqi-manager.h"
#include "../core/eventScheduler/simulator.h"
#include "../componentManagers/NetworkManager.h"
#include "../protocolStack/rrc/ho/handover-entity.h"
#include "../protocolStack/rrc/ho/ho-manager.h"


UserEquipment::UserEquipment ()
{}

UserEquipment::UserEquipment (int idElement,
							  double posx,
							  double posy,
							  Cell *cell,
							  NetworkNode* target,
							  bool handover,
							  Mobility::MobilityModel model)
{
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_UE);
  SetCell(cell);

  m_targetNode = target;

  ProtocolStack *stack = new ProtocolStack (this);
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  classifier->SetDevice (this);
  SetClassifier (classifier);
  SetNodeState(STATE_IDLE);

  //Setup Mobility Model
  Mobility *m;
  if (model == Mobility::RANDOM_DIRECTION)
    {
	  m = new RandomDirection ();
    }
  else if (model == Mobility::RANDOM_WALK)
    {
	  m = new RandomWalk ();
    }
  else if (model == Mobility::RANDOM_WAYPOINT)
    {
	  m = new RandomWaypoint ();
    }
  else if (model == Mobility::CONSTANT_POSITION)
    {
	  m = new ConstantPosition ();
    }
  else if (model == Mobility::MANHATTAN)
    {
	  m = new Manhattan ();
    }
  else
    {
	  std::cout << "ERROR: incorrect Mobility Model"<< std::endl;
	  m = new RandomDirection ();
    }
  CartesianCoordinates *position = new CartesianCoordinates (posx, posy);
  m->SetHandover (handover);
  m->SetAbsolutePosition (position);
  m->SetNodeID (idElement);
  SetMobilityModel (m);

  m_timePositionUpdate = 0.001;
  Simulator::Init()->Schedule(m_timePositionUpdate,
		  &UserEquipment::UpdateUserPosition,
		  this,
		  Simulator::Init ()->Now());

  delete position;

  UeLtePhy *phy = new UeLtePhy ();
  phy->SetDevice(this);
  phy->SetBandwidthManager (target->GetPhy ()->GetBandwidthManager ());
  SetPhy(phy);

  m_cqiManager = NULL;
  m_isIndoor = false;
}

UserEquipment::UserEquipment (int idElement,
							  double posx,
							  double posy,
							  int speed,
							  double speedDirection,
							  Cell *cell,
							  NetworkNode* target,
							  bool handover,
							  Mobility::MobilityModel model)
{
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_UE);
  SetCell(cell);

  m_targetNode = target;

  ProtocolStack *stack = new ProtocolStack (this);
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  classifier->SetDevice (this);
  SetClassifier (classifier);
  SetNodeState (STATE_IDLE);
  //Setup Mobility Model
  Mobility *m;
  if (model == Mobility::RANDOM_DIRECTION)
    {
	  m = new RandomDirection ();
    }
  else if (model == Mobility::RANDOM_WALK)
    {
	  m = new RandomWalk ();
    }
  else if (model == Mobility::RANDOM_WAYPOINT)
    {
	  m = new RandomWaypoint ();
    }
  else if (model == Mobility::CONSTANT_POSITION)
    {
	  m = new ConstantPosition ();
    }
  else if (model == Mobility::MANHATTAN)
    {
	  m = new Manhattan ();
    }
  else
    {
	  std::cout << "ERROR: incorrect Mobility Model"<< std::endl;
	  m = new RandomDirection ();
    }
  CartesianCoordinates *position = new CartesianCoordinates(posx, posy);
  m->SetHandover(handover);
  m->SetAbsolutePosition(position);
  m->SetNodeID(idElement);
  m->SetSpeed(speed);
  m->SetSpeedDirection(speedDirection);
  SetMobilityModel (m);

  m_timePositionUpdate = 0.001;
  Simulator::Init()->Schedule(m_timePositionUpdate,
		  &UserEquipment::UpdateUserPosition,
		  this,
		  Simulator::Init ()->Now());

  delete position;


  UeLtePhy *phy = new UeLtePhy ();
  phy->SetDevice(this);
  phy->SetBandwidthManager (target->GetPhy ()->GetBandwidthManager ());
  SetPhy (phy);

  m_cqiManager = NULL;
  m_isIndoor = false;

}

UserEquipment::~UserEquipment()
{
  m_targetNode = NULL;
  delete m_cqiManager;
  Destroy ();
}

void
UserEquipment::SetTargetNode (NetworkNode* n)
{
  m_targetNode = n;
  SetCell (n->GetCell ());
}

NetworkNode*
UserEquipment::GetTargetNode (void)
{
  return m_targetNode;
}


void
UserEquipment::UpdateUserPosition (double time)
{
  GetMobilityModel ()->UpdatePosition (time);

    SetIndoorFlag(NetworkManager::Init()->CheckIndoorUsers(this));

    if (GetMobilityModel ()->GetHandover () == true)
      {
           NetworkNode* targetNode = GetTargetNode ();

        if (targetNode->GetProtocolStack ()->GetRrcEntity ()->
                   GetHandoverEntity ()->CheckHandoverNeed (this))
          {
           NetworkNode* newTagertNode = targetNode->GetProtocolStack ()
                           ->GetRrcEntity ()->GetHandoverEntity ()->GetHoManager ()->m_target;

           NetworkManager::Init()->HandoverProcedure(time, this, targetNode, newTagertNode);
          }
      }


    //schedule the new update after m_timePositionUpdate
    Simulator::Init()->Schedule(m_timePositionUpdate,
                                                           &UserEquipment::UpdateUserPosition,
                                                           this,
                                                           Simulator::Init ()->Now());
}


void
UserEquipment::SetCqiManager (CqiManager *cm)
{
  m_cqiManager = cm;
}

CqiManager*
UserEquipment::GetCqiManager (void)
{
  return m_cqiManager;
}

void
UserEquipment::SetIndoorFlag ( bool flag)
{
  m_isIndoor = flag;
}

bool
UserEquipment::IsIndoor (void)
{
  return m_isIndoor;
}

//Debug
void
UserEquipment::Print (void)
{
  std::cout << " UserEquipment object:"
      "\n\t m_idNetworkNode = " << GetIDNetworkNode () <<
	  "\n\t idCell = " << GetCell ()->GetIdCell () <<
	  "\n\t idtargetNode = " << GetTargetNode ()->GetIDNetworkNode () <<
	  "\n\t m_AbsolutePosition_X = " <<  GetMobilityModel ()->GetAbsolutePosition()->GetCoordinateX()<<
	  "\n\t m_AbsolutePosition_Y = " << GetMobilityModel ()->GetAbsolutePosition()->GetCoordinateY()<<
      "\n\t m_speed = " << GetMobilityModel ()->GetSpeed () <<
      "\n\t m_speedDirection = " << GetMobilityModel ()->GetSpeedDirection () <<
      std::endl;
}
