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


#include "position-based-ho-manager.h"
#include "../../../device/NetworkNode.h"
#include "../../../device/UserEquipment.h"
#include "../../../device/ENodeB.h"
#include "../../../device/HeNodeB.h"
#include "../../../componentManagers/NetworkManager.h"

PositionBasedHoManager::PositionBasedHoManager()
{
  m_target = NULL;
}

PositionBasedHoManager::~PositionBasedHoManager()
{
  m_target = NULL;
}

bool
PositionBasedHoManager::CheckHandoverNeed (UserEquipment* ue)
{
  NetworkNode *targetNode = ue->GetTargetNode ();

  CartesianCoordinates *uePosition = ue->GetMobilityModel ()->GetAbsolutePosition ();
  CartesianCoordinates *targetPosition;

  targetPosition = targetNode->GetMobilityModel ()->GetAbsolutePosition ();
  double targetDistance = uePosition->GetDistance (targetPosition);

  /*
  if (targetDistance <= (ue->GetCell ()->GetRadius () * 0.8))
    {
	  return false;
    }
  */

  std::vector<ENodeB*> *listOfNodes = NetworkManager::Init ()->GetENodeBContainer ();
  std::vector<ENodeB*>::iterator it;
  for (it = listOfNodes->begin (); it != listOfNodes->end (); it++)
    {
	  if ((*it)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it);


	      double distance = probableNewTargetNode->GetMobilityModel ()->
	    		  GetAbsolutePosition ()->GetDistance (uePosition);

	      if (distance < targetDistance)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
			      targetDistance = distance;
			      targetNode = probableNewTargetNode;
	    	  }
	        }
	    }
    }
  std::vector<HeNodeB*> *listOfNodes2 = NetworkManager::Init ()->GetHomeENodeBContainer();
  std::vector<HeNodeB*>::iterator it2;
  for (it2 = listOfNodes2->begin (); it2 != listOfNodes2->end (); it2++)
    {
	  if ((*it2)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it2);


	      double distance = probableNewTargetNode->GetMobilityModel ()->
	    		  GetAbsolutePosition ()->GetDistance (uePosition);

	      if (distance < targetDistance)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
			      targetDistance = distance;
			      targetNode = probableNewTargetNode;
	    	  }
	        }
	    }
    }

  if (ue->GetTargetNode ()->GetIDNetworkNode () != targetNode->GetIDNetworkNode ())
    {
	  m_target = targetNode;
	  return true;
    }
  else
    {
	  return false;
    }
}
