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



#include "NetworkManager.h"
#include "../core/eventScheduler/simulator.h"
#include "../device/Gateway.h"
#include "../device/ENodeB.h"
#include "../device/HeNodeB.h"
#include "../device/UserEquipment.h"
#include "../networkTopology/Cell.h"
#include "../networkTopology/Femtocell.h"
#include "../networkTopology/Building.h"
#include "../phy/lte-phy.h"
#include "../protocolStack/rrc/ho/handover-entity.h"
#include "../protocolStack/rrc/ho/ho-manager.h"
#include "../channel/LteChannel.h"
#include "../channel/propagation-model/channel-realization-helper.h"
#include "../channel/propagation-model/propagation-loss-model.h"
#include "../flows/radio-bearer.h"
#include "../flows/radio-bearer-sink.h"
#include "../utility/frequency-reuse-helper.h"
#include "../device/CqiManager/cqi-manager.h"
#include "../device/CqiManager/fullband-cqi-manager.h"
#include "../device/CqiManager/wideband-cqi-manager.h"
#include "../load-parameters.h"
#include "../flows/application/Application.h"
#include "../utility/IndoorScenarios.h"
#include "../networkTopology/Street.h"

NetworkManager* NetworkManager::ptr=NULL;

NetworkManager::NetworkManager()
{
  m_cellContainer = new std::vector<Cell*>;
  m_femtoCellContainer = new std::vector<Femtocell*>;
  m_eNodeBContainer = new std::vector<ENodeB*>;
  m_homeENodeBContainer = new std::vector<HeNodeB*>;
  m_userEquipmentContainer = new std::vector<UserEquipment*>;
  m_gatewayContainer = new std::vector<Gateway*>;
  m_buildingContainer = new std::vector<Building*>;
}

NetworkManager::~NetworkManager()
{
  std::vector<Cell*>::iterator iter;
  for (iter = m_cellContainer->begin();
      iter != m_cellContainer->end(); iter++)
    {
    delete *iter;
    }
  delete m_cellContainer;

  std::vector<Gateway*>::iterator iter4;
  for (iter4 = m_gatewayContainer->begin();
      iter4 != m_gatewayContainer->end(); iter4++)
    {
    delete *iter4;
    }
  delete m_gatewayContainer;

  std::vector<ENodeB*>::iterator iter2;
  for (iter2 = m_eNodeBContainer->begin();
      iter2 != m_eNodeBContainer->end(); iter2++)
    {
    delete *iter2;
    }
  delete m_eNodeBContainer;

  std::vector<UserEquipment*>::iterator iter3;
  for (iter3 = m_userEquipmentContainer->begin();
      iter3 != m_userEquipmentContainer->end(); iter3++)
    {
    delete *iter3;
    }
  delete m_userEquipmentContainer;
}


Cell*
NetworkManager::CreateCell (int idCell, double radius, double minDistance, double X, double Y)
{
  Cell *cell = new Cell (idCell, radius, minDistance, X, Y);

  GetCellContainer()->push_back (cell);

  return cell;
}

ENodeB*
NetworkManager::CreateEnodeb (int id,
		                      Cell* cell,
		                      double pos_X, double pos_Y,
		                      LteChannel *dlCh, LteChannel *ulCh,
		                      BandwidthManager *bm)
{
  ENodeB* enb = new ENodeB (id, cell, pos_X, pos_Y); //ENodeB (1, cell);
  enb->GetPhy ()->SetDlChannel (dlCh);
  enb->GetPhy ()->SetUlChannel (ulCh);
  enb->GetPhy ()->SetBandwidthManager (bm);
  ulCh->AddDevice (enb);

  GetENodeBContainer ()->push_back (enb);

  return enb;
}

Gateway*
NetworkManager::CreateGateway (void)
{
  Gateway *gw = new Gateway ();

  GetGatewayContainer ()->push_back (gw);

  return gw;
}

UserEquipment*
NetworkManager::CreateUserEquipment (int id,
				 					 double pos_X, double pos_Y, double speed, double speedDirection,
									 Cell* cell, ENodeB* enb)
{
  UserEquipment* ue = new UserEquipment (id,
										 pos_X, pos_Y, speed, speedDirection,
										 cell, enb,
										 0,
										 Mobility::RANDOM_DIRECTION);

  ue->GetPhy ()->SetDlChannel (enb->GetPhy()->GetDlChannel ());
  ue->GetPhy ()->SetUlChannel (enb->GetPhy()->GetUlChannel ());

  FullbandCqiManager *cqiManager = new FullbandCqiManager ();
  cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
  cqiManager->SetReportingInterval (1);
  cqiManager->SetDevice (ue);
  ue->SetCqiManager (cqiManager);

  enb->RegisterUserEquipment (ue);

  MacroCellUrbanAreaChannelRealization* c_dl = new MacroCellUrbanAreaChannelRealization (enb, ue);
  enb->GetPhy()->GetDlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_dl);
  MacroCellUrbanAreaChannelRealization* c_ul = new MacroCellUrbanAreaChannelRealization (ue, enb);
  enb->GetPhy()->GetUlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_ul);

  GetUserEquipmentContainer ()->push_back (ue);

  return ue;
}

void
NetworkManager::CreateStreet (int idStreet, CartesianCoordinates* center, double theta,
    		                  int buildingtype, int nbBuildings, double street_width, double building_distance,
    		                  int nbFemtoInBuilding, double apartment_side, int firstFemtoIDInStreet)
{
	Street* street = new Street (idStreet, center, theta, nbBuildings, buildingtype, street_width, building_distance, apartment_side);

    vector<CartesianCoordinates*> *building_positions = street->GetBuildingDistributionInStreet();

    int idBuilding = 0;
    int firstFemtoinBuildingID = firstFemtoIDInStreet;

	while( idBuilding < 2*nbBuildings )
	    {
		  // Get Building Positions
		  double buildingCenter_X = building_positions->at (idBuilding)->GetCoordinateX ();
		  double buildingCenter_Y = building_positions->at (idBuilding)->GetCoordinateY ();

		  CreateBuildingForFemtocells( idBuilding, buildingtype, apartment_side, 1, buildingCenter_X, buildingCenter_Y, firstFemtoinBuildingID, nbFemtoInBuilding);

		  std::cout << "Created Building, id " << idBuilding
				  <<", position: " << buildingCenter_X
				  << " " << buildingCenter_Y
				  << " and " << nbFemtoInBuilding << " femtocells" << std::endl;

		  idBuilding++;
		  firstFemtoinBuildingID += nbFemtoInBuilding;
	    }

}

void
NetworkManager::CreateBuildingForFemtocells (int idBuilding,
											 int buildingType,
											 double apartmentSide,
											 int nbFloors,
											 double pos_X,
											 double pos_Y,
											 int firstFemtoCellID,
											 int nbOfFemtocells)
{

	double side_X;
	double side_Y;

	if ( buildingType == 0)
	{
		side_X = 5 * apartmentSide;
		side_Y = 5 * apartmentSide;
	}
	else
	{
		side_X = 10 * apartmentSide;
		side_Y = 5 * apartmentSide;
	}

  Building* building = new Building (idBuilding,
									   buildingType,
									   nbFloors,
									   side_X,
									   side_Y,
									   pos_X,
									   pos_Y);

	// CREATE FEMTO _CELLS
	int idCell;

	for (int floor = 0; floor < nbFloors; floor++ )
	{
		int femtocellIndex = 0;
		while ( femtocellIndex < nbOfFemtocells )
		{
			idCell = femtocellIndex + firstFemtoCellID;

			double* offset = GetFemtoCellOffsetInBuilding ( building->GetBuildingType(), femtocellIndex, apartmentSide);

			building->AddFemtoCell( CreateFemtoCell( idCell,
					apartmentSide,
					offset[0] + pos_X,
					offset[1] + pos_Y) );

			GetFemtoCellByID(idCell)->GetCellCenterPosition()->SetCoordinateZ( floor );

			delete [] offset;

			femtocellIndex++;
		}
	}

	GetBuildingContainer()->push_back(building);

}

Femtocell*
NetworkManager::CreateFemtoCell(int idCell, double side, double X, double Y)
{
  Femtocell *femtocell = new Femtocell (idCell, side, X, Y);

  GetFemtoCellContainer()->push_back (femtocell);

  return femtocell;
}


int
NetworkManager::GetNbCell (void)
{
  return m_cellContainer->size ();
}

std::vector<Cell*>*
NetworkManager::GetCellContainer (void)
{
  return m_cellContainer;
}

std::vector<Femtocell*>*
NetworkManager::GetFemtoCellContainer (void)
{
  return m_femtoCellContainer;
}

std::vector<ENodeB*>*
NetworkManager::GetENodeBContainer (void)
{
  return m_eNodeBContainer;
}

std::vector<HeNodeB*>*
NetworkManager::GetHomeENodeBContainer (void)
{
  return m_homeENodeBContainer;
}

std::vector<UserEquipment*>*
NetworkManager::GetUserEquipmentContainer (void)
{
  return m_userEquipmentContainer;
}

std::vector<Gateway*>*
NetworkManager::GetGatewayContainer (void)
{
  return m_gatewayContainer;
}

std::vector<Building*>*
NetworkManager::GetBuildingContainer (void)
{
  return m_buildingContainer;
}


Cell*
NetworkManager::GetCellByID (int idCell)
{
  std::vector<Cell*>* cellContainer = GetCellContainer ();
  std::vector<Cell*>::iterator iter;
  Cell *cell;
  for (iter = cellContainer->begin ();
      iter !=cellContainer->end (); iter++)
    {
    cell = *iter;
    if (cell->GetIdCell() == idCell)
      return cell;
    }
  return false;
}

Femtocell*
NetworkManager::GetFemtoCellByID (int idFemtoCell)
{
  std::vector<Femtocell*>* femtocellContainer = GetFemtoCellContainer();
  std::vector<Femtocell*>::iterator iter;
  Femtocell *cell;
  for (iter = femtocellContainer->begin ();
		  iter !=femtocellContainer->end (); iter++)
    {
	  cell = *iter;
	  if (cell->GetIdCell() == idFemtoCell)
		  return cell;
    }
  return false;
}

ENodeB*
NetworkManager::GetENodeBByID (int idENodeB)
{
  std::vector<ENodeB*>* eNodeBContainer = GetENodeBContainer ();
  std::vector<ENodeB*>::iterator iter2;
  ENodeB *eNodeB;
  for (iter2 = eNodeBContainer->begin ();
      iter2 !=eNodeBContainer->end (); iter2++)
    {
    eNodeB = *iter2;
    if (eNodeB->GetIDNetworkNode() == idENodeB)
      {
      return eNodeB;
      }
    }
  return false;
}

ENodeB*
NetworkManager::GetENodeBByCellID (int idCell)
{
  std::vector<ENodeB*>* eNodeBContainer = GetENodeBContainer ();
  std::vector<ENodeB*>::iterator iter2;
  ENodeB *eNodeB;
  for (iter2 = eNodeBContainer->begin ();
      iter2 !=eNodeBContainer->end (); iter2++)
    {
    eNodeB = *iter2;
    if (eNodeB->GetCell ()->GetIdCell ()== idCell)
      {
      return eNodeB;
      }
    }
  return false;
}

UserEquipment*
NetworkManager::GetUserEquipmentByID (int idUE)
{
  std::vector<UserEquipment*>* userEquipmentContainer = GetUserEquipmentContainer ();
  std::vector<UserEquipment*>::iterator iter3;
  UserEquipment *userEquipment;
  for (iter3 = userEquipmentContainer->begin ();
      iter3 !=userEquipmentContainer->end (); iter3++)
    {
    userEquipment = *iter3;
    if (userEquipment->GetIDNetworkNode() == idUE)
      {
      return userEquipment;
      }
    }
  return false;
}

Gateway*
NetworkManager::GetGatewayByID (int idGW)
{
  std::vector<Gateway*>* gatewayContainer = GetGatewayContainer ();
  std::vector<Gateway*>::iterator iter;
  Gateway *gateway;

  for (iter = gatewayContainer->begin ();
      iter !=gatewayContainer->end (); iter++)
    {
    gateway = *iter;
    if (gateway->GetIDNetworkNode() == idGW)
      {
      return gateway;
      }
    }
  return false;
}

Building*
NetworkManager::GetBuildingByID (int idBuilding)
{
  std::vector<Building*>* buildingContainer = GetBuildingContainer ();
  std::vector<Building*>::iterator iter;
  Building *building;

  for (iter = buildingContainer->begin ();
		  iter !=buildingContainer->end (); iter++)
    {
	  building = *iter;
	  if (building->GetIdBuilding() == idBuilding)
	    {
		  return building;
	    }
    }
  return false;
}

Building*
NetworkManager::GetBuildingByFemtoCellID (int idFemtoCell)
{
  std::vector<Building*>* buildingContainer = GetBuildingContainer ();
  std::vector<Building*>::iterator iter;
  Building *building;

  for (iter = buildingContainer->begin ();
		  iter !=buildingContainer->end (); iter++)
    {
	  building = *iter;
	  if (building->GetFemtoIdInBuilding(idFemtoCell) != -1)
	    {
		  return building;
	    }
    }
  return NULL;
}

NetworkNode*
NetworkManager::GetNetworkNodeByID (int id)
{
  std::vector<ENodeB*>* eNodeBContainer = GetENodeBContainer ();
  std::vector<ENodeB*>::iterator iter2;
  ENodeB *eNodeB;
  for (iter2 = eNodeBContainer->begin ();
      iter2 !=eNodeBContainer->end (); iter2++)
    {
    eNodeB = *iter2;
    if (eNodeB->GetIDNetworkNode() == id)
      {
      return eNodeB;
      }
    }

  std::vector<UserEquipment*>* userEquipmentContainer = GetUserEquipmentContainer ();
  std::vector<UserEquipment*>::iterator iter3;
  UserEquipment *userEquipment;
  for (iter3 = userEquipmentContainer->begin ();
      iter3 !=userEquipmentContainer->end (); iter3++)
    {
    userEquipment = *iter3;
    if (userEquipment->GetIDNetworkNode() == id)
      {
      return userEquipment;
      }
    }

  std::vector<Gateway*>* gatewayContainer = GetGatewayContainer ();
  std::vector<Gateway*>::iterator iter;
  Gateway *gateway;

  for (iter = gatewayContainer->begin ();
      iter !=gatewayContainer->end (); iter++)
    {
    gateway = *iter;
    if (gateway->GetIDNetworkNode() == id)
      {
      return gateway;
      }
    }
  return false;
}

std::vector<UserEquipment*>*
NetworkManager::GetRegisteredUEToENodeB (int idENB)
{
  std::vector<UserEquipment*>* UElist = new std::vector<UserEquipment*>;

  std::vector<UserEquipment*>* userEquipmentContainer = GetUserEquipmentContainer ();
  std::vector<UserEquipment*>::iterator iter;
  UserEquipment *userEquipment;
  for (iter = userEquipmentContainer->begin ();
      iter !=userEquipmentContainer->end (); iter++)
    {
    userEquipment = *iter;
    if (userEquipment->GetTargetNode ()->GetIDNetworkNode() == idENB)
      {
      UElist->push_back(userEquipment);
      }
    }
  return UElist;
}


std::vector<int>
NetworkManager::GetCellIDFromPosition (CartesianCoordinates *position)
{
  std::vector<int> CellsID;

  std::vector<Cell*>* cellContainer = GetCellContainer ();
  std::vector<Cell*>::iterator iter;
  Cell *cell;
  double distance;
  for (iter = cellContainer->begin ();
      iter !=cellContainer->end (); iter++)
    {
    cell = *iter;
    distance =  sqrt (pow ((cell->GetCellCenterPosition()->GetCoordinateX() - position->GetCoordinateX()),2)
                      + pow ((cell->GetCellCenterPosition()->GetCoordinateY() - position->GetCoordinateY()),2));

    if (distance <= cell->GetRadius())
      {
      CellsID.push_back(cell->GetIdCell());
      }
    }

  return CellsID;
}

int
NetworkManager::GetCellIDFromPosition (double posX,
                                       double posY)
{
  std::vector<Cell*>* cellContainer = GetCellContainer ();
  std::vector<Cell*>::iterator iter;
  Cell *cell;
  double target_distance = 10000.0;
  double distance;
  int cellID;

  for (iter = cellContainer->begin ();
      iter !=cellContainer->end (); iter++)
    {
    cell = *iter;
    distance = cell->GetCellCenterPosition()->GetDistance(posX, posY);

    if (distance < target_distance)
      {
        cellID = cell->GetIdCell ();
        target_distance = distance;
      }
    }
  return cellID;
}


void
NetworkManager::UpdateUserPosition (double time)
{
  std::vector<UserEquipment*> *records = GetUserEquipmentContainer ();
  std::vector<UserEquipment*>::iterator iter;
  UserEquipment *record;

#ifdef MOBILITY_DEBUG
  std::cout << "MOBILITY_DEBUG: UPDATE POSITION, "
      "number of UE = " << records->size () <<
      " time = " << time << std::endl;
#endif

  for (iter = records->begin(); iter != records->end(); iter++)
    {
      record = *iter;

#ifdef MOBILITY_DEBUG
    std::cout << "\t USER  " << record->GetIDNetworkNode ()
	              << std::endl;
#endif

      record->UpdateUserPosition (time);
      record->SetIndoorFlag( CheckIndoorUsers(record) );

#ifdef AMC_MAPPING
  std::cout << "time: " << time << "\n\t position: "
		  << record->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX () <<
		  " " << record->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
		  << std::endl;
#endif
#ifdef MOBILITY_DEBUG
  std::cout << "time: " << time << "\t position: "
		  << record->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX () <<
		  " " << record->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
		  << std::endl;
#endif


      if (record->GetMobilityModel ()->GetHandover () == true)
        {
    	  NetworkNode* targetNode = record->GetTargetNode ();

          if (targetNode->GetProtocolStack ()->GetRrcEntity ()->
        		  GetHandoverEntity ()->CheckHandoverNeed (record))
            {
        	  NetworkNode* newTagertNode = targetNode->GetProtocolStack ()
        			  ->GetRrcEntity ()->GetHandoverEntity ()->GetHoManager ()->m_target;

#ifdef HANDOVER_DEBUG
              std::cout << "** HO ** \t time: " << time << " user " <<  record->GetIDNetworkNode () <<
            		  " old eNB " << targetNode->GetIDNetworkNode () <<
            		  " new eNB " << newTagertNode->GetIDNetworkNode () << std::endl;
#endif
              HandoverProcedure(time, record, targetNode, newTagertNode);
            }
        }
    }

  //PrintUEsForEachCell();
}

bool
NetworkManager::CheckHandoverPermissions (NetworkNode* target, UserEquipment* user)
{
	if ( target->GetNodeType() == NetworkNode::TYPE_ENODEB )
	{
		return true;
	}
	else if (target->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION)
	{
		HenbMacEntity* henb_mac = (HenbMacEntity*) target->GetProtocolStack()->GetMacEntity();

		return henb_mac->CheckSubscribedUser(user);
	}
	else
	{
		std::cout<< " Wrong Node Type for handover. Exiting..."<<std::endl;
		exit(1);
	}
	return false;
}

bool
NetworkManager::CheckIndoorUsers (UserEquipment *ue)
{
	std::vector<Building*>* buildingContainer = GetBuildingContainer ();
	std::vector<Building*>::iterator iter;
	Building *building;

	if (buildingContainer->size()>0)
	{
		for (iter = buildingContainer->begin ();
				iter !=buildingContainer->end (); iter++)
		{
			building = *iter;

			if ( ( abs( building->GetCenterPosition()->GetCoordinateX()
					- ue->GetMobilityModel()->GetAbsolutePosition()->GetCoordinateX() ) < building->GetSide()[0]/2 ) &&
					( abs( building->GetCenterPosition()->GetCoordinateY()
							- ue->GetMobilityModel()->GetAbsolutePosition()->GetCoordinateY() ) < building->GetSide()[1]/2 ) )
			{
				return true;
			}
		}
	}
	return false;
}

int
NetworkManager::GetBuildingIDForUE (UserEquipment *ue)
{
	std::vector<Building*>* buildingContainer = GetBuildingContainer ();
	std::vector<Building*>::iterator iter;
	Building *building;

	if (buildingContainer->size()>0)
	{
		for (iter = buildingContainer->begin ();
				iter !=buildingContainer->end (); iter++)
		{
			building = *iter;

			if ( ( abs( building->GetCenterPosition()->GetCoordinateX()
					- ue->GetMobilityModel()->GetAbsolutePosition()->GetCoordinateX() ) < building->GetSide()[0]/2 ) &&
					( abs( building->GetCenterPosition()->GetCoordinateY()
							- ue->GetMobilityModel()->GetAbsolutePosition()->GetCoordinateY() ) < building->GetSide()[1]/2 ) )
			{
				return building->GetIdBuilding();
			}
		}
	}
	return -1;
}

Cell*
NetworkManager::GetBelongingCellFromPosition(UserEquipment* ue)
{
	CartesianCoordinates* ue_pos = ue->GetMobilityModel()->GetAbsolutePosition();
	int buildingID = GetBuildingIDForUE(ue);

	if ( buildingID != -1 )
	{
		//user is indoor and it will be attached to a femtocell
		std::vector<Femtocell*>* femtocellContainer = GetBuildingByID(buildingID)->GetFemtoCellsInBuilding();
		std::vector<Femtocell*>::iterator iter;
		Femtocell *cell;
		double distance = 9999999;
		for (iter = femtocellContainer->begin ();
				iter !=femtocellContainer->end (); iter++)
		{
			if ( ue_pos->GetDistance((*iter)->GetCellCenterPosition()) < distance )
			{
				distance = ue_pos->GetDistance((*iter)->GetCellCenterPosition());
				cell = (*iter);
			}
		}
		return (Cell*) cell ;
	}
	else
	{
		// ue is outdoor and it will be attached to a macrocell
		return GetCellByID( GetCellIDFromPosition( ue_pos->GetCoordinateX(), ue_pos->GetCoordinateY() ) );
	}

	return NULL;
}

void
NetworkManager::SelectTargetNode (UserEquipment* ue)
{
   NetworkNode* targetNode = ue->GetTargetNode ();

   if (targetNode->GetProtocolStack ()->GetRrcEntity ()->
		   GetHandoverEntity ()->CheckHandoverNeed (ue))
     {
	   NetworkNode* newTagertNode =
			   targetNode->GetProtocolStack ()->GetRrcEntity ()->
			   GetHandoverEntity ()->GetHoManager ()->m_target;

	   ue->SetTargetNode (newTagertNode);
     }
}

void
NetworkManager::HandoverProcedure(double time, UserEquipment* ue, NetworkNode* oldTarget, NetworkNode* newTarget)
{
#ifdef HANDOVER_DEBUG
              std::cout << "** HO ** \t HandoverProcedure for user "
            		  <<  ue->GetIDNetworkNode () << std::endl;
#endif

  // 1 - deactivate the UE for a time equal to 30 ms
  if (ue->GetNodeState() == UserEquipment::STATE_ACTIVE)
    {
    //XXX Admission Control is not implemented
	double detachTime = 0.030;
    ue->SetNodeState (UserEquipment::STATE_DETACHED);
    Simulator::Init()->Schedule(detachTime, &NetworkNode::MakeActive, ue);
    }

#ifdef MOBILITY_DEBUG
  std::cout << "user " <<  ue->GetIDNetworkNode () << " starts HO "
      "\n\t old eNB = " << ue->GetTargetNode ()->GetIDNetworkNode () <<
      "\n\t target eNB = " <<  newTarget->GetIDNetworkNode () << std::endl;
#endif

  // 2 - transfer all active radio bearer
  Simulator::Init()->Schedule(0.001,
                              &NetworkManager::TransferBearerInfo,
                              this,
                              ue,
                              newTarget);

  ue->GetMobilityModel ()->SetLastHandoverTime(time);
}


void
NetworkManager::TransferBearerInfo (UserEquipment* ue, NetworkNode* target)
{
#ifdef HANDOVER_DEBUG
              std::cout << "** HO ** \t TransferBearerInfo for user "
            		  <<  ue->GetIDNetworkNode () << std::endl;
#endif

  if ( (target->GetNodeType() == NetworkNode::TYPE_ENODEB && ue->GetTargetNode ()->GetNodeType() == NetworkNode::TYPE_ENODEB) ||
		 (target->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION && ue->GetTargetNode ()->GetNodeType() == NetworkNode::TYPE_ENODEB) ||
		 (target->GetNodeType() == NetworkNode::TYPE_ENODEB && ue->GetTargetNode ()->GetNodeType() == NetworkNode::TYPE_HOME_BASE_STATION))
    {
	   ENodeB *oldTargetNode = (ENodeB*) ue->GetTargetNode ();
	   ENodeB *newTargetNode = (ENodeB*) target;

	   // 1 - update spectrum, channels and propagation loss model
#ifdef HANDOVER_DEBUG
	   std::cout << "update spectrum, channels and propagation loss model"<< std::endl;
#endif
	   ue->GetPhy ()->SetBandwidthManager (newTargetNode->GetPhy ()->GetBandwidthManager ());

	   LteChannel *oldDl = oldTargetNode->GetPhy ()->GetDlChannel ();
	   LteChannel *oldUl = oldTargetNode->GetPhy ()->GetUlChannel ();
	   LteChannel *newDl = newTargetNode->GetPhy ()->GetDlChannel ();
	   LteChannel *newUl = newTargetNode->GetPhy ()->GetUlChannel ();

	   ue->GetPhy ()->SetDlChannel (newDl);
	   if (oldDl->IsAttached (ue))
	     {
		   newDl->AddDevice (ue);
           oldDl->DelDevice (ue);
	     }
	   if (newDl->GetPropagationLossModel () != NULL)
	     {
		   newDl->GetPropagationLossModel ()->
				   AddChannelRealization (CreateChannelRealization (newTargetNode, ue));

		   oldDl->GetPropagationLossModel ()->DelChannelRealization (oldTargetNode,ue);

	     }

	   ue->GetPhy ()->SetUlChannel (newUl);
	   if (oldUl->IsAttached (ue))
	     {
		   newUl->AddDevice (ue);
           oldUl->DelDevice (ue);
	     }
	   if (newUl->GetPropagationLossModel () != NULL)
	     {
		   newUl->GetPropagationLossModel ()->AddChannelRealization (
				   CreateChannelRealization (ue, newTargetNode));

		   oldUl->GetPropagationLossModel ()->DelChannelRealization (ue,oldTargetNode);;
	     }


	   // 2 - add ue record to the new Enb
#ifdef HANDOVER_DEBUG
	   std::cout << "add ue record to the new Enb"<< std::endl;
#endif
       newTargetNode->RegisterUserEquipment (ue);

	   // 3 - delete ue record form the old enb
#ifdef HANDOVER_DEBUG
       std::cout << "delete ue record form the old enb"<< std::endl;
#endif
       oldTargetNode->DeleteUserEquipment (ue);

	   // 4 - update cell and new target enb for the ue
#ifdef HANDOVER_DEBUG
       std::cout << "update cell and new target enb for the ue"<< std::endl;
#endif
	   ue->SetTargetNode (newTargetNode);

	   // MOVE RRC CONTEXT FOR THE OLD TARGET NODE TO THE NEWER ONE
#ifdef HANDOVER_DEBUG
	   std::cout << "MOVE RRC CONTEXT"<< std::endl;
#endif
	   RrcEntity * oldEnbRrc = oldTargetNode->GetProtocolStack ()->GetRrcEntity ();
	   RrcEntity * newEnbRrc = newTargetNode->GetProtocolStack ()->GetRrcEntity ();
	   RrcEntity * ueRrc = ue->GetProtocolStack  ()->GetRrcEntity ();


	   // --> move dl bearers
#ifdef HANDOVER_DEBUG
	   std::cout << "\t DL radio bearers " << oldEnbRrc->GetRadioBearerContainer ()->size () << std::endl;
#endif
	   std::vector<RadioBearer* > *dlBearerToDelete = new std::vector<RadioBearer* > ();
	   for (std::vector<RadioBearer* >::iterator it = oldEnbRrc->GetRadioBearerContainer ()->begin();
			   it != oldEnbRrc->GetRadioBearerContainer ()->end (); it++)
	     {
		   RadioBearer *bearer = (*it);
		   if (bearer->GetDestination ()->GetIDNetworkNode () == ue->GetIDNetworkNode ())
		     {
			   bearer->SetSource (newTargetNode);
			   bearer->GetRlcEntity ()->SetDevice (newTargetNode);
			   newEnbRrc->AddRadioBearer (bearer);
			   dlBearerToDelete->push_back (bearer);
		     }
	     }
	   for (std::vector<RadioBearer* >::iterator it = dlBearerToDelete->begin();
			   it != dlBearerToDelete->end (); it++)
	     {
		   RadioBearer *bearer = (*it);
		   oldEnbRrc->DelRadioBearer (bearer);
	     }
	   dlBearerToDelete->clear ();
	   delete dlBearerToDelete;



	   // --> move ul bearers
#ifdef HANDOVER_DEBUG
	   std::cout << "\t UL radio bearers"<< std::endl;
#endif
	   std::vector<RadioBearerSink* > *ulBearerToDelete = new std::vector<RadioBearerSink* > ();
	   for (std::vector<RadioBearerSink* >::iterator it = oldEnbRrc->GetRadioBearerSinkContainer ()->begin();
			   it != oldEnbRrc->GetRadioBearerSinkContainer ()->end (); it++)
	     {
		   RadioBearerSink *bearer = (*it);
		   if (bearer->GetSource ()->GetIDNetworkNode () == ue->GetIDNetworkNode ())
		     {
			   bearer->SetDestination (newTargetNode);
			   newEnbRrc->AddRadioBearerSink (bearer);
			   ulBearerToDelete->push_back (bearer);
		     }
	     }
	   for (std::vector<RadioBearerSink* >::iterator it = ulBearerToDelete->begin();
			   it != ulBearerToDelete->end (); it++)
	     {
		   RadioBearerSink *bearer = (*it);
		   oldEnbRrc->DelRadioBearerSink (bearer);
	     }
	   ulBearerToDelete->clear ();
	   delete ulBearerToDelete;


	   // UPDATE THE RRC CONTEXT FOR THE UE
#ifdef HANDOVER_DEBUG
	   std::cout << "\t UE updates DL radio bearers "<< std::endl;
#endif
	   for (std::vector<RadioBearerSink* >::iterator it = ueRrc->GetRadioBearerSinkContainer ()->begin();
			   it != ueRrc->GetRadioBearerSinkContainer ()->end (); it++)
	     {
		   RadioBearerSink *bearer = (*it);
		   bearer->SetSource (newTargetNode);
	     }
#ifdef HANDOVER_DEBUG
	   std::cout << "\t UE updates UL radio bearers "<< std::endl;
#endif
	   for (std::vector<RadioBearer* >::iterator it = ueRrc->GetRadioBearerContainer ()->begin();
			   it != ueRrc->GetRadioBearerContainer ()->end (); it++)
	     {
		   RadioBearer *bearer = (*it);
		   bearer->SetDestination (newTargetNode);
	     }
    }

}

std::vector <BandwidthManager*>
RunFrequencyReuse (int nodes, int cluster, double bandwidth)
{
  RunFrequencyReuseTechniques (nodes, cluster, bandwidth);
}

void
NetworkManager::PrintUEsForEachCell (void)
{
  std::vector<ENodeB*> *records = GetENodeBContainer();
  std::vector<ENodeB*>::iterator iter;

  std::cout << "UEsForCell: ";
  for (iter = records->begin(); iter !=records->end(); iter++)
    {
    std::cout << " " << (*iter)->GetNbOfUserEquipmentRecords();
    }
  std::cout << std::endl;
}


/*
 *  DEBUG
 */
void
NetworkManager::Print (void)
{
  cout << " TOTAL Network Manager Debug: " << endl;

  std::vector<Cell*>* cellContainer = GetCellContainer ();
  std::vector<Cell*>::iterator iter;
  Cell *cell;
  for (iter = cellContainer->begin ();
      iter !=cellContainer->end (); iter++)
    {
    cell = *iter;
    cell->Print();
    }

  std::vector<ENodeB*>* eNodeBContainer = GetENodeBContainer ();
  std::vector<ENodeB*>::iterator iter2;
  ENodeB *eNodeB;
  for (iter2 = eNodeBContainer->begin ();
      iter2 !=eNodeBContainer->end (); iter2++)
    {
    eNodeB = *iter2;
    eNodeB->Print();
    }

  std::vector<UserEquipment*>* userEquipmentContainer = GetUserEquipmentContainer ();
  std::vector<UserEquipment*>::iterator iter3;
  UserEquipment *userEquipment;
  for (iter3 = userEquipmentContainer->begin ();
      iter3 !=userEquipmentContainer->end (); iter3++)
    {
    userEquipment = *iter3;
    userEquipment->Print();
    }
}

void
NetworkManager::Print (NetworkManager::m_debugInfo info)
{
  cout << " Network Manager Debug: " << endl;

  switch (info)
  {
  case NetworkManager::ALL_NETWORK:
    {
      this->Print();
      break;
    }

  case NetworkManager::ONLY_CELL_DETAILS:
    {
      std::vector<Cell*>* cellContainer = GetCellContainer ();
      std::vector<Cell*>::iterator iter;
      Cell *cell;
      for (iter = cellContainer->begin ();
          iter !=cellContainer->end (); iter++)
        {
        cell = *iter;
        cell->Print();
        }
      break;
    }

  case NetworkManager::ONLY_ENODEB_DETAILS:
    {
      std::vector<ENodeB*>* eNodeBContainer = GetENodeBContainer ();
      std::vector<ENodeB*>::iterator iter2;
      ENodeB *eNodeB;
      for (iter2 = eNodeBContainer->begin ();
          iter2 !=eNodeBContainer->end (); iter2++)
        {
        eNodeB = *iter2;
        eNodeB->Print();
        }
      break;
    }
  case NetworkManager::ONLY_USER_EQUIPMENT_DETAILS:
    {
      std::vector<UserEquipment*>* userEquipmentContainer = GetUserEquipmentContainer ();
      std::vector<UserEquipment*>::iterator iter3;
      UserEquipment *userEquipment;
      for (iter3 = userEquipmentContainer->begin ();
          iter3 !=userEquipmentContainer->end (); iter3++)
        {
        userEquipment = *iter3;
        userEquipment->Print();
        }
      break;
    }

  default:
    {
      cout << " INVALID NETWORK DEBUG CODE " << endl;
      break;
    }

  }
}

void
NetworkManager::PrintFrequencyMask (void)
{
  std::vector<Cell*>* cellContainer = GetCellContainer ();
  std::vector<Cell*>::iterator iter;
  Cell *cell;
  for (iter = cellContainer->begin ();
      iter !=cellContainer->end (); iter++)
    {
    cell = *iter;
    }

  std::vector<ENodeB*>* eNodeBContainer = GetENodeBContainer ();
  std::vector<ENodeB*>::iterator iter2;
  ENodeB *eNodeB;
  for (iter2 = eNodeBContainer->begin ();
      iter2 !=eNodeBContainer->end (); iter2++)
    {
    eNodeB = *iter2;
    std::cout << "ENODEB " <<  eNodeB->GetIDNetworkNode() << ", frequencyMask:" << std::endl;
    }
}

void
NetworkManager::PrintUserPosition (void)
{
  std::vector<UserEquipment*>* users = GetUserEquipmentContainer();
  std::vector<UserEquipment*>::iterator iter;
  UserEquipment *user;

  std::cout << " UserPosition X [at " << Simulator::Init()->Now() << "] ";
  for (iter = users->begin(); iter != users->end(); iter++)
    {
    user = *iter;
    std::cout << user->GetMobilityModel ()->GetAbsolutePosition()->GetCoordinateX() << " ";
    }

  std::cout << "\n UserPosition Y [at " << Simulator::Init()->Now() << "] ";
  for (iter = users->begin(); iter != users->end(); iter++)
    {
    user = *iter;
    std::cout << user->GetMobilityModel ()->GetAbsolutePosition()->GetCoordinateY() << " ";
    }
  std::cout << std::endl;
}

