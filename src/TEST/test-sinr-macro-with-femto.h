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
 * Author: Francesco Capozzi <f.capozzi@poliba.it>
 */

#include "../channel/LteChannel.h"
#include "../phy/enb-lte-phy.h"
#include "../phy/ue-lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../networkTopology/Cell.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../protocolStack/packet/Packet.h"
#include "../core/eventScheduler/simulator.h"
#include "../flows/application/InfiniteBuffer.h"
#include "../flows/application/VoIP.h"
#include "../flows/application/CBR.h"
#include "../flows/application/TraceBased.h"
#include "../device/IPClassifier/ClassifierParameters.h"
#include "../flows/QoS/QoSParameters.h"
#include "../flows/QoS/QoSForEXP.h"
#include "../flows/QoS/QoSForFLS.h"
#include "../flows/QoS/QoSForM_LWDF.h"
#include "../componentManagers/FrameManager.h"
#include "../utility/seed.h"
#include "../utility/RandomVariable.h"
#include "../utility/UsersDistribution.h"
#include "../utility/IndoorScenarios.h"
#include "../channel/propagation-model/macrocell-urban-area-channel-realization.h"
#include "../channel/propagation-model/femtocell-urban-area-channel-realization.h"
#include "../load-parameters.h"
#include "../device/HeNodeB.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <math.h>


static void TestSinrMacroWithFemto (double radius,
								    int nbBuildings,
								    int nbUE_macro)
{

  srand (time(NULL));

  int nbCell = 19;
  double duration = 0.5;
  double flow_duration = duration - 0.2;
  int cluster = 1;
  double bandwidth = 5;


  // CREATE COMPONENT MANAGER
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  NetworkManager* nm = NetworkManager::Init();
  FlowsManager* flowsManager = FlowsManager::Init ();


  // CREATE MACRO CELLS with eNB
  for (int i = 0; i < nbCell; i++)
    {
	  CartesianCoordinates center = GetCartesianCoordinatesForCell(i, radius);
 	  Cell *c = new Cell (i, radius/1000., 0.035, center.GetCoordinateX (), center.GetCoordinateY ());
  	  nm->GetCellContainer ()->push_back (c);

  	  std::cout << "CELL " << c->GetIdCell ()
  			  <<" position " << c->GetCellCenterPosition ()->GetCoordinateX ()
  			  << " " << c->GetCellCenterPosition ()->GetCoordinateY () << std::endl;

 	  ENodeB* enb = new ENodeB (i, c);
 	  LteChannel* ch_dl = new LteChannel ();
 	  ch_dl->SetChannelId (i);
 	  LteChannel* ch_ul = new LteChannel ();
 	  ch_ul->SetChannelId (i);
 	  enb->GetPhy ()->SetDlChannel (ch_dl);
	  enb->GetPhy ()->SetUlChannel (ch_ul);
      enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
	  enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (5, 5, 0, 0));
	  nm->GetENodeBContainer ()->push_back (enb);

	  std::cout << "eNB " << enb->GetIDNetworkNode()
			  << " cell " << enb->GetCell ()->GetIdCell ()
			  <<" position " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
			  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
			  << " channels " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
			  << " " << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;
    }

  // CREATE FEMTOCELLS with HeNB
  int idFemto = 101;
  int idBuilding = 1101;
  int buildType = 0; // TYPE_3GPP_5x5_grid
  int nbFloors = 1;
  int apartmentSide = 10;
  int femtoCellsInBuilding = 25;
  int nbFemtoCells = nbBuildings * femtoCellsInBuilding;

  for (int i = 0; i < nbBuildings; i++)
    {
	   double buildingCenter_X = (double) sin (i * (M_PI/2)) * (radius * 0.5);
	   double buildingCenter_Y = (double) cos (i * (M_PI/2)) * (radius * 0.5);

	   nm->CreateBuildingForFemtocells(idBuilding, buildType, apartmentSide, nbFloors, buildingCenter_X, buildingCenter_Y, idFemto, femtoCellsInBuilding);

	   std::cout << "Building " << idBuilding
				  <<" position " << buildingCenter_X
				  << " " << buildingCenter_Y
				  << " with " << femtoCellsInBuilding << " femtocells" << std::endl;

	   //create HeNB
	   for (int j = 0; j < femtoCellsInBuilding; j++)
	     {
		   HeNodeB* enb = new HeNodeB (idFemto + j, nm->GetFemtoCellContainer ()->at (j + (i*femtoCellsInBuilding)));
		   LteChannel* ch_dl = new LteChannel ();
		   ch_dl->SetChannelId (idFemto + j);
		   LteChannel* ch_ul = new LteChannel ();
		   ch_ul->SetChannelId (idFemto + j);
		   enb->GetPhy ()->SetDlChannel (ch_dl);
		   enb->GetPhy ()->SetUlChannel (ch_ul);
		   enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
		   enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (5, 5, 50, 50));
		   nm->GetHomeENodeBContainer()->push_back (enb);

		   std::cout << "HeNB " << enb->GetIDNetworkNode()
				  << " cell " << enb->GetCell ()->GetIdCell ()
				  <<" position " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
				  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
				  << " channels " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
				  << " " << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;

	     }

	   idFemto = idFemto + femtoCellsInBuilding;
	   idBuilding++;
    }









  int idApp = 0;
  int dstPort = 0;
  int srcPort = 0;
  double startTime = 0.1;

  // CREATE UE into the MACRO CELL
  int idUE = 3001;
  int j = 0;
  //for (int j = 0; j < nbCell; j++)
  //  {
      for (int i = 0; i < nbUE_macro; i++)
        {
    	  double posX = (double)rand()/RAND_MAX; posX = 0.90 * (((2*radius)*posX) - radius);
  	  	  double posY = (double)rand()/RAND_MAX; posY = 0.90 * (((2*radius)*posY) - radius);
   	  	  double speed = 3;
   	  	  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);

    	  UserEquipment* ue = new UserEquipment (idUE,
    			  	  	  	  	  	  	  	  	 posX, posY, speed, speedDirection,
    	  			                             nm->GetCellByID (j),
    	  			                             nm->GetENodeBByCellID (j),
    	  			                             1, //handover false!
    	  			                             Mobility::CONSTANT_POSITION);
    	  ue->SetIndoorFlag(false);

    	  nm->SelectTargetNode (ue);
    	  ENodeB* target = (ENodeB*) ue->GetTargetNode ();
    	  if (target->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION)
    	    {
    		  ue->SetIndoorFlag (true);
    	    }

		  ue->GetPhy ()->SetDlChannel (target->GetPhy ()->GetDlChannel ());
		  ue->GetPhy ()->SetUlChannel (target->GetPhy ()->GetUlChannel ());

		  FullbandCqiManager *cqiManager = new FullbandCqiManager ();
		  cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
		  cqiManager->SetReportingInterval (1);
		  cqiManager->SetDevice (ue);
		  ue->SetCqiManager (cqiManager);

		  target->RegisterUserEquipment (ue);
		  MacroCellUrbanAreaChannelRealization* c_dl = new MacroCellUrbanAreaChannelRealization (target, ue);
		  target->GetPhy ()->GetDlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_dl);
		  MacroCellUrbanAreaChannelRealization* c_ul = new MacroCellUrbanAreaChannelRealization (ue, target);
		  target->GetPhy ()->GetUlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_ul);

		  nm->GetUserEquipmentContainer ()->push_back (ue);


		  std::cout << "UE " << idUE << " position " << posX << " " << posY
				  << " cell " <<  ue->GetCell ()->GetIdCell ()
				  << " enb " << ue->GetTargetNode ()->GetIDNetworkNode () << std::endl;


		  /*
		  //Create an Application
		  QoSParameters *qos = new QoSParameters ();
		  Application* be = flowsManager->CreateApplication (idApp,
				  	  	  	  	  	  	  	  	  	  	  	 target, ue,
										   				     srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
															 Application::APPLICATION_TYPE_INFINITE_BUFFER,
															 qos,
															 startTime, startTime + flow_duration);
          */

		  ue->GetPhy ()->GetDlChannel ()->AddDevice (ue);
		  idUE++;
		  idApp++;
		  dstPort++;
        }



  simulator->SetStop(duration);
  simulator->Run ();



  for (std::vector<UserEquipment*>::iterator it = nm->GetUserEquipmentContainer ()->begin (); it != nm->GetUserEquipmentContainer ()->end (); it++)
    {
	  UserEquipment* ue = (*it);
	  if (ue->GetTargetNode ()->GetNodeType () == NetworkNode::TYPE_ENODEB)
	    {
		  std::cout << "macro " << ue->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
				  << " " << ue->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()  << std::endl;
	    }
	  else if (ue->GetTargetNode ()->GetNodeType () == NetworkNode::TYPE_HOME_BASE_STATION)
	    {
		  std::cout << "femto " << ue->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
		  		  << " " << ue->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()  << std::endl;
	    }
	  else
	    {
          std::cout << "not available" << std::endl;
	    }

    }


}
