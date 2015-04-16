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


static void ScalabilityTestMacroWithFemto (double radius,
								           int nbBuildings,
								           int nbUE_buildings)
{

  srand (time(NULL));

  int nbCell = 19;
  double duration = 30.;
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
	  double r = 2./3. * radius;
	  double angle = i * (2*3.14/nbBuildings);
	  double x = (r * cos (angle));
	  double y = (r * sin (angle));

	   nm->CreateBuildingForFemtocells(idBuilding, buildType, apartmentSide, nbFloors, x, y, idFemto, femtoCellsInBuilding);


	    std::cout << "Building " << idBuilding
				  <<" position " << x
				  << " " << y
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
		   enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (5, 5, 0, 0));
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
  int idUE = 3001;


  if (nbBuildings > 0)
    {
	  for (int i = 0; i < nbBuildings; i++)
	    {
	      int j = 0;
	      for (int k = 0; k < nbUE_buildings; k++)
	       {

	    	  HeNodeB* henb = nm->GetHomeENodeBContainer ()->at ((i*25)+j);
	    	  double posX = henb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ();
		      double posY = henb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ();
			  double speed = 3;
			  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);

			  UserEquipment* ue = new UserEquipment (idUE,
													 posX, posY, speed, speedDirection,
													 henb->GetCell (),
													 henb,
													 0,
													 Mobility::CONSTANT_POSITION);
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



			  //Create an Application
			  QoSParameters *qos = new QoSParameters ();
			  Application* be = flowsManager->CreateApplication (idApp,
																 target, ue,
																 srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
																 Application::APPLICATION_TYPE_INFINITE_BUFFER,
																 qos,
																 startTime, startTime + flow_duration);


			  idUE++;
			  idApp++;
			  dstPort++;



		    j++;
		    if (j == 25) j=0;

	      }
	    }

    }



  int macro_ue = 30;
  for (int i = 0; i < macro_ue; i++)
	{
	  double r = 1./4. * radius;
	  double angle = (double)(rand() %360) * ((2*3.14)/360);
	  double x = (r * cos (angle));
	  double y = (r * sin (angle));
	  double speed = 3;
	  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);

	  UserEquipment* ue = new UserEquipment (idUE,
											 x, y, speed, speedDirection,
											 nm->GetCellByID (0),
											 nm->GetENodeBByCellID (0),
											 0,
											 Mobility::CONSTANT_POSITION);
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


	  std::cout << "UE " << idUE << " position " << x << " " << y
			  << " cell " <<  ue->GetCell ()->GetIdCell ()
			  << " enb " << ue->GetTargetNode ()->GetIDNetworkNode () << std::endl;



	  //Create an Application
	  QoSParameters *qos = new QoSParameters ();
	  Application* be = flowsManager->CreateApplication (idApp,
														 target, ue,
														 srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
														 Application::APPLICATION_TYPE_INFINITE_BUFFER,
														 qos,
														 startTime, startTime + flow_duration);


	  idUE++;
	  idApp++;
	  dstPort++;
	}





  simulator->SetStop(duration);
  simulator->Schedule(duration-10, &Simulator::PrintMemoryUsage, simulator);
  simulator->Run ();

}
