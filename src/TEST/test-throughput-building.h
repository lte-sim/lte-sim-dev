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
#include "../channel/propagation-model/channel-realization-helper.h"
#include "../load-parameters.h"
#include "../device/HeNodeB.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <math.h>


static void TestThroughputBuilding (int riuso, double activityFactor, int nbUE_femto)
{

  srand (time(NULL));

  int nbBuildings = 1;
  double duration = 31;
  double flow_duration = 30;

  double bandwidth = 20./riuso;


  // CREATE COMPONENT MANAGER
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  NetworkManager* nm = NetworkManager::Init();
  FlowsManager* flowsManager = FlowsManager::Init ();




  // CREATE FEMTOCELLS with HeNB
  int idFemto = 101;
  int idBuilding = 1101;
  int buildType = 0; // TYPE_3GPP_5x5_grid
  int nbFloors = 1;
  int apartmentSide = 10;
  int femtoCellsInBuilding = 25;
  int nbFemtoCells = nbBuildings * femtoCellsInBuilding;


  /*
  std::vector<BandwidthManager*> bwVector;
  for  (int i = 0; i < 4; i++)
    {
	  BandwidthManager* bw = new BandwidthManager (5, 5, i*25, i*25);
	  bwVector.push_back (bw);
    }
  */


  for (int i = 0; i < nbBuildings; i++)
    {
	   double buildingCenter_X = 0;
	   double buildingCenter_Y = 0;

	   nm->CreateBuildingForFemtocells(idBuilding, buildType, apartmentSide, nbFloors, buildingCenter_X, buildingCenter_Y, idFemto, femtoCellsInBuilding);

	   std::cout << "Building " << idBuilding
				  <<" position " << buildingCenter_X
				  << " " << buildingCenter_Y
				  << " with " << femtoCellsInBuilding << " femtocells" << std::endl;

	   //create HeNB
	   for (int j = 0; j < femtoCellsInBuilding; j++)
	     {

		   double rnd = (double) rand()/RAND_MAX;
		   if (rnd <= activityFactor)
		     {
		       HeNodeB* enb = new HeNodeB (idFemto + j, nm->GetFemtoCellContainer ()->at (j + (i*femtoCellsInBuilding)));

		       //position
		       /*
		       double x = (double)rand()/RAND_MAX;
			   x = (apartmentSide*x) + enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ();
			   double y = (double)rand()/RAND_MAX;
			   y = (apartmentSide*y) + enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ();
			   CartesianCoordinates *position = new CartesianCoordinates(x, y);
			   enb->GetMobilityModel ()->SetAbsolutePosition (position);
               */

		       LteChannel* ch_dl = new LteChannel ();
			   ch_dl->SetChannelId (idFemto + j);
			   LteChannel* ch_ul = new LteChannel ();
			   ch_ul->SetChannelId (idFemto + j);
			   enb->GetPhy ()->SetDlChannel (ch_dl);
			   enb->GetPhy ()->SetUlChannel (ch_ul);
			   enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);

			   if (riuso == 1)
				 {
				   enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (bandwidth, bandwidth, 0, 0));
				 }
			   else if (riuso == 2)
				 {
				   enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (bandwidth, bandwidth, 50*(j%2), 50*(j%2)));
				 }
			   else if (riuso == 4)
				 {
				   std::vector<BandwidthManager*> bwVector;
				   for  (int i = 0; i < 4; i++)
				     {
				 	  BandwidthManager* bw = new BandwidthManager (5, 5, i*25, i*25);
				 	  bwVector.push_back (bw);
				     }

				   if ((j >= 0 && j <=4) || (j >= 10 && j <=14) || (j >= 20 && j <=24))
					 {
					   int id = j%2;
					   enb->GetPhy ()->SetBandwidthManager (bwVector.at (id));
					 }
				   else
					 {
					   int id = 2 + (j%2);
					   enb->GetPhy ()->SetBandwidthManager (bwVector.at (id));
					 }

				 }
			   else
				 {
				   std::cout << "Reuse not available !"<< std::endl;
				   return;
				 }
			   nm->GetHomeENodeBContainer()->push_back (enb);

			   std::cout << "HeNB " << enb->GetIDNetworkNode()
					  << " cell " << enb->GetCell ()->GetIdCell ()
					  <<" position " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
					  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
					  << " channels " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
					  << " " << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;
			   enb->GetPhy ()->GetBandwidthManager ()->Print ();
		     }

		   idFemto = idFemto + femtoCellsInBuilding;
		   idBuilding++;
	     }
    }





  int idUE = 3001;

  int idApp = 0;
  int dstPort = 0;
  int srcPort = 0;
  double startTime = 0.1;

  for (std::vector<HeNodeB*>::iterator it = nm->GetHomeENodeBContainer ()->begin ();
		  it != nm->GetHomeENodeBContainer ()->end (); it++)
    {

      HeNodeB* henb = (*it);

	  for (int k = 0; k < nbUE_femto; k++)
	    {

	      CartesianCoordinates* henb_position = henb->GetMobilityModel ()->GetAbsolutePosition ();

	      double x = (double)rand()/RAND_MAX;
	      x = (apartmentSide*x) + henb_position->GetCoordinateX ();
	      double y = (double)rand()/RAND_MAX;
	      y = (apartmentSide*y) + henb_position->GetCoordinateY ();

	      /*
	      double r = (double) rand()/RAND_MAX; r = r * 5.;
	      double angle = (double)(rand() %360) * ((2*3.14)/360);
	      double x = (r * cos (angle) + henb_position->GetCoordinateX ());
	      double y = (r * sin (angle) + henb_position->GetCoordinateY ());
		  */

		  double speed = 3;
		  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);

		  UserEquipment* ue = new UserEquipment (idUE,
												 x, y, speed, speedDirection,
												 henb->GetCell (),
												 henb,
												 0,
												 Mobility::CONSTANT_POSITION);


		  //nm->SelectTargetNode (ue);
		  ENodeB* target = (ENodeB*) ue->GetTargetNode ();
		  ue->GetPhy ()->SetDlChannel (target->GetPhy ()->GetDlChannel ());
		  ue->GetPhy ()->SetUlChannel (target->GetPhy ()->GetUlChannel ());
		  ue->SetIndoorFlag(true);

		  FullbandCqiManager *cqiManager = new FullbandCqiManager ();
		  cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
		  cqiManager->SetReportingInterval (1);
		  cqiManager->SetDevice (ue);
		  ue->SetCqiManager (cqiManager);

		  target->RegisterUserEquipment (ue);
		  ChannelRealization* c_dl = CreateChannelRealization (target, ue);
		  target->GetPhy ()->GetDlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_dl);
		  ChannelRealization* c_ul = CreateChannelRealization (ue, target);
		  target->GetPhy ()->GetUlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_ul);

		  nm->GetUserEquipmentContainer ()->push_back (ue);

		  std::cout << "UE " << idUE << " position " << x << " " << y
				  << " cell " <<  ue->GetCell ()->GetIdCell ()
				  << " henb " << ue->GetTargetNode ()->GetIDNetworkNode () << std::endl;



		  //Create an Application
		  QoSParameters *qos = new QoSParameters ();
		  Application* be = flowsManager->CreateApplication (idApp,
															 target, ue,
															 srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
															 Application::APPLICATION_TYPE_INFINITE_BUFFER,
															 qos,
															 startTime, startTime + flow_duration);

		  ue->GetPhy ()->GetDlChannel ()->AddDevice (ue);
		  idUE++;
		  idApp++;
		  dstPort++;

	    }

    }



  simulator->SetStop(duration);
  simulator->Run ();



}
