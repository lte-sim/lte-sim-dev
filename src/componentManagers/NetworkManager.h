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


#ifndef NetworkManager_H_
#define NetworkManager_H_

#include <stdint.h>
#include <vector>
#include "../networkTopology/Cell.h"

class NetworkNode;
class Cell;
class Femtocell;
class ENodeB;
class HeNodeB;
class UserEquipment;
class Gateway;
class Mobility;
class CartesianCoordinates;
class BandwidthManager;
class LteChannel;
class Building;
class Street;

class NetworkManager {
private:
	std::vector<Cell*> *m_cellContainer;
	std::vector<Femtocell*> *m_femtoCellContainer;
	std::vector<ENodeB*> *m_eNodeBContainer;
	std::vector<HeNodeB*> *m_homeENodeBContainer;
	std::vector<UserEquipment*> *m_userEquipmentContainer;
	std::vector<Gateway*> *m_gatewayContainer;
	std::vector<Building*> *m_buildingContainer;

	NetworkManager();
	static NetworkManager *ptr;


public:
	virtual ~NetworkManager();

	static NetworkManager*
	Init (void)
	  {
		if (ptr==NULL)
	      {
		    ptr = new NetworkManager;
	   	  }
		return ptr;
	  }

	/*
	 * Get a Network Element Container
	 */
	std::vector<Cell*>*
	GetCellContainer (void);
	std::vector<Femtocell*>*
	GetFemtoCellContainer (void);
	std::vector<ENodeB*>*
	GetENodeBContainer (void);
	std::vector<HeNodeB*>*
	GetHomeENodeBContainer (void);
	std::vector<UserEquipment*>*
	GetUserEquipmentContainer (void);
	std::vector<Gateway*>*
	GetGatewayContainer (void);
	std::vector<Building*>*
	GetBuildingContainer (void);

	int
	GetNbCell(void);

	/*
	 * Get Network Elements
	 */
	Cell*
	GetCellByID (int idCell);
	Femtocell*
	GetFemtoCellByID (int idFemtoCell);
	ENodeB*
	GetENodeBByID (int idENodeB);
	ENodeB*
	GetENodeBByCellID (int idCell);
	UserEquipment*
	GetUserEquipmentByID (int idUE);
	Gateway*
	GetGatewayByID (int idGW);
	Building*
	GetBuildingByID (int idBuilding);
	Building*
	GetBuildingByFemtoCellID (int idFemtoCell);

	std::vector<UserEquipment*>*
	GetRegisteredUEToENodeB (int idENB);

	NetworkNode*
	GetNetworkNodeByID (int id);




	/*
	 * Get Position-related Info for Network Elements
	 */
	std::vector<int>
	GetCellIDFromPosition (CartesianCoordinates *position);
	int
	GetCellIDFromPosition (double posX, double posY);




	void
	UpdateUserPosition (double time);
	bool
	CheckIndoorUsers (UserEquipment *ue);
	int
	GetBuildingIDForUE (UserEquipment *ue);

	Cell*
	GetBelongingCellFromPosition(UserEquipment* ue);
	void
	SelectTargetNode (UserEquipment* ue);

	bool
	CheckHandoverNeed (int idUserEquipment);
	bool
	CheckHandoverPermissions (NetworkNode* target, UserEquipment* user);
	void
	HandoverProcedure (double time, UserEquipment* ue, NetworkNode* oldTarget, NetworkNode* newTarget);
	void
	TransferBearerInfo (UserEquipment* ue, NetworkNode* target);

	void
	TransferPacketsThroughX2 (int bearerID,
							  int targetENB);

	std::vector <BandwidthManager*>
    RunFrequencyReuse (int nodes, int cluster, double bandwidth);

    void
    PrintUEsForEachCell (void);




    Cell* CreateCell (int idCell, double radius, double minDistance, double X, double Y);

    ENodeB* CreateEnodeb (int id,
    		              Cell* cell,
    		              double pos_X, double pos_Y,
    		              LteChannel *dlCh, LteChannel *ulCh,
    		              BandwidthManager *bm);

    Gateway* CreateGateway (void);

    UserEquipment* CreateUserEquipment (int id,
										double pos_X, double pos_Y, double speed, double speedDirection,
										Cell* cell, ENodeB* enb);

    void CreateStreet (int idStreet, CartesianCoordinates* center, double theta,
        		                  int buildingtype, int nbBuildings, double street_width, double building_distance,
        		                  int nbFemtoInBuilding, double apartment_side, int firstFemtoIDInStreet);
    void CreateBuildingForFemtocells (int idBuilding, int buildingType, double apartmentSize, int nbFloors, double pos_X, double pos_Y, int firsFemtoCellID, int nbOfFemtocells);
    Femtocell* CreateFemtoCell(int idCell, double side, double X, double Y);
    double* CheckBuildingPosition (int buildingType, double pos_X, double pos_Y, double side_X, double side_Y);

//Debug
    enum m_debugInfo
      {
	    ALL_NETWORK,
		ONLY_CELL_DETAILS,
		ONLY_ENODEB_DETAILS,
		ONLY_USER_EQUIPMENT_DETAILS
      };
	void
	Print (void);
	void
	Print (NetworkManager::m_debugInfo info);

	void
	PrintFrequencyMask (void);

	void
	PrintUserPosition (void);
};


#endif /* NetworkManager_H_ */



