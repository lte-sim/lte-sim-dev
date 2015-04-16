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


static void TestSinrUrban (int streets, int henb, int reuse)
{


  srand (time(NULL));

  int nbCell = 1;
  int radius = 1000; //metres
  double duration = 0.2;

  double bandwidth = 20.;



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
      if (reuse == 0)
		{
		  enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (bandwidth, bandwidth, 0, 0));
		}
	  else
		{
		  enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (15, 15, 0, 0));
		}
	  nm->GetENodeBContainer ()->push_back (enb);

	  std::cout << "eNB " << enb->GetIDNetworkNode()
			  << " cell " << enb->GetCell ()->GetIdCell ()
			  <<" position " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
			  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
			  << " channels " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
			  << " " << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;
	  enb->GetPhy()->GetBandwidthManager ()->Print ();
    }


  int x,y;
  double theta = 0.0;
  int street_width = 15; //[m]
  int interBuilding_distance = 3; //[m]
  int nbOfBuildingInStreet;
  double apartment_side = 10;
  int buildingtype = 0; //5x5 grid
  int femtoCellsInBuilding = 25;
  int firstFemtoinBuildingID;





  if (streets > 0)
    {

	  int x_cross = 300;
	  int y_cross = 300;


      //create street 1
	  nbOfBuildingInStreet = 8;
	  x=x_cross;
	  y=y_cross + ((2*apartment_side*5 + interBuilding_distance) +(nbOfBuildingInStreet/2) * (apartment_side*5 + interBuilding_distance));
	  theta = 90.;
	  firstFemtoinBuildingID = 1001;

	  nm->CreateStreet(1001,
					   new CartesianCoordinates(x, y), theta,
					   buildingtype,
					   nbOfBuildingInStreet,
					   street_width, interBuilding_distance,
					   femtoCellsInBuilding, apartment_side, firstFemtoinBuildingID );


	  //create street 2
	  nbOfBuildingInStreet = 6;
	  x=x_cross+ ((2*apartment_side*5 + interBuilding_distance) +(nbOfBuildingInStreet/2) * (apartment_side*5 + interBuilding_distance));
	  y=y_cross ;
	  theta = 0.;
	  firstFemtoinBuildingID = 2001;
	  nm->CreateStreet(2001,
					   new CartesianCoordinates(x, y), theta,
					   buildingtype,
					   nbOfBuildingInStreet,
					   street_width, interBuilding_distance,
					   femtoCellsInBuilding, apartment_side, firstFemtoinBuildingID );

	  //create street 3
	  nbOfBuildingInStreet = 8;
	  x=x_cross;
	  y=y_cross - ((2*apartment_side*5 + interBuilding_distance) +(nbOfBuildingInStreet/2) * (apartment_side*5 + interBuilding_distance));
	  theta = 90.;
	  firstFemtoinBuildingID = 3001;
	  nm->CreateStreet(3001,
					   new CartesianCoordinates(x, y), theta,
					   buildingtype,
					   nbOfBuildingInStreet,
					   street_width, interBuilding_distance,
					   femtoCellsInBuilding, apartment_side, firstFemtoinBuildingID );


	  //create street 4
	  nbOfBuildingInStreet = 8;
	  x=x_cross - ((2*apartment_side*5 + interBuilding_distance) + (nbOfBuildingInStreet/2) * (apartment_side*5 + interBuilding_distance)) ;
	  y=y_cross ;
	  theta = 0.;
	  firstFemtoinBuildingID = 4001;
	  nm->CreateStreet(4001,
					   new CartesianCoordinates(x, y), theta,
					   buildingtype,
					   nbOfBuildingInStreet,
					   street_width, interBuilding_distance,
					   femtoCellsInBuilding, apartment_side, firstFemtoinBuildingID );



	  if (henb > 0)
	    {

	      //CREATE HENB
		  std::vector <Femtocell*> *femtocells = nm->GetFemtoCellContainer();
		  for (int i = 0; i < femtocells->size  (); i++)
			{
			  //double rnd = (double) rand()/RAND_MAX;
		      //if (rnd <= activityFactor)
		      //  {
				  int id = femtocells->at (i)->GetIdCell ();
				  HeNodeB* enb = new HeNodeB (id, femtocells->at (i));

				  LteChannel* ch_dl = new LteChannel ();
				  ch_dl->SetChannelId (id);
				  LteChannel* ch_ul = new LteChannel ();
				  ch_ul->SetChannelId (id);
				  enb->GetPhy ()->SetDlChannel (ch_dl);
				  enb->GetPhy ()->SetUlChannel (ch_ul);
				  enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
				  if (reuse == 0)
					{
					  enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (bandwidth, bandwidth, 0, 0));
					}
				  else
					{
					  enb->GetPhy ()->SetBandwidthManager (new BandwidthManager (5, 5, 75, 75));
					}

				  nm->GetHomeENodeBContainer()->push_back (enb);

				  std::cout << "HeNB " << enb->GetIDNetworkNode()
					  << " cell " << enb->GetCell ()->GetIdCell ()
					  <<" position " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
					  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
					  << " channels " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
					  << " " << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;
				  enb->GetPhy()->GetBandwidthManager ()->Print ();
		       // }
			}
	    }

    }





 int idUE = 5001;
 //for (double x = -(radius*0.8); x < radius*0.8; x=x+10)
 //   {
 //	  for (double y = -(radius*0.8); y < radius*0.8; y=y+10)
 //	    {

 for (int i = 0; i < 500; i++)
   {
          double posX = (double)rand()/RAND_MAX; posX = 0.90 * (((2*radius)*posX) - radius);
 	      double posY = (double)rand()/RAND_MAX; posY = 0.90 * (((2*radius)*posY) - radius);
		  double speed = 3;
		  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);

		  UserEquipment* ue = new UserEquipment (idUE,
												 posX, posY, speed, speedDirection,
												 nm->GetCellContainer ()->at (0),
												 nm->GetENodeBContainer ()->at (0),
												 1,
												 Mobility::CONSTANT_POSITION);


		  nm->SelectTargetNode (ue);
		  ENodeB* target = (ENodeB*) ue->GetTargetNode ();
		  ue->GetPhy ()->SetDlChannel (target->GetPhy ()->GetDlChannel ());
		  ue->GetPhy ()->SetUlChannel (target->GetPhy ()->GetUlChannel ());

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

		  std::cout << "UE " << idUE << " position " << posX << " " << posY
				  << " cell " <<  ue->GetCell ()->GetIdCell ()
				  << " target " << ue->GetTargetNode ()->GetIDNetworkNode () << std::endl;


		   ue->GetPhy ()->GetDlChannel ()->AddDevice (ue);
		   idUE++;

	//    }
    //}

   }

  simulator->SetStop(duration);
  simulator->Run ();



}
