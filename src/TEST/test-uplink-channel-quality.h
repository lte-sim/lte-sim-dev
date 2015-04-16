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


#include "../channel/LteChannel.h"
#include "../phy/enb-lte-phy.h"
#include "../phy/ue-lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../networkTopology/Cell.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../protocolStack/packet/Packet.h"
#include "../core/eventScheduler/simulator.h"
#include "../load-parameters.h"
#include "../flows/application/CBR.h"
#include "../device/IPClassifier/ClassifierParameters.h"
#include "../flows/QoS/QoSParameters.h"
#include "../device/Gateway.h"
#include "../flows/radio-bearer.h"
#include "../channel/propagation-model/channel-realization-helper.h"
#include "../channel/propagation-model/propagation-loss-model.h"
#include "../protocolStack/mac/packet-scheduler/mt-uplink-packet-scheduler.h"
#include "../componentManagers/FlowsManager.h"

static void TestUplinkChannelQuality ()
{

  srand (time(NULL));

  // CREATE COMPONENT MANAGERS
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  NetworkManager* networkManager = NetworkManager::Init();
  FlowsManager* flowsManager = FlowsManager::Init ();



  //create cells
  int nbCells = 19;
  int radius = 1;
  int bandwidth = 5;
  int cluster = 1;

  std::vector <Cell*> *cells = new std::vector <Cell*>;
  for (int i = 0; i < nbCells; i++)
	{
	  CartesianCoordinates center =
			  GetCartesianCoordinatesForCell(i, radius * 1000.);

	  Cell *c = new Cell (i, radius, 0.035, center.GetCoordinateX (), center.GetCoordinateY ());
	  cells->push_back (c);
	  networkManager->GetCellContainer ()->push_back (c);

	  std::cout << "Created Cell, id " << c->GetIdCell ()
			  <<", position: " << c->GetCellCenterPosition ()->GetCoordinateX ()
			  << " " << c->GetCellCenterPosition ()->GetCoordinateY () << std::endl;
	}


  std::vector <BandwidthManager*> spectrums = RunFrequencyReuseTechniques (nbCells, cluster, bandwidth);

  //Create a set of a couple of channels
  std::vector <LteChannel*> *dlChannels = new std::vector <LteChannel*>;
  std::vector <LteChannel*> *ulChannels = new std::vector <LteChannel*>;
  for (int i= 0; i < nbCells; i++)
	{
	  LteChannel *dlCh = new LteChannel ();
	  dlCh->SetChannelId (i);
	  dlChannels->push_back (dlCh);

	  LteChannel *ulCh = new LteChannel ();
	  ulCh->SetChannelId (i);
	  ulChannels->push_back (ulCh);
	}


  //create eNBs
  std::vector <ENodeB*> *eNBs = new std::vector <ENodeB*>;
  for (int i = 0; i < nbCells; i++)
	{
	  ENodeB* enb = new ENodeB (i, cells->at (i));
	  enb->GetPhy ()->SetDlChannel (dlChannels->at (i));
	  enb->GetPhy ()->SetUlChannel (ulChannels->at (i));
	  enb->SetULScheduler (ENodeB::ULScheduler_TYPE_MAXIMUM_THROUGHPUT);
	  enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
	  enb->GetPhy ()->SetBandwidthManager (spectrums.at (i));
	  std::cout << "Created enb, id " << enb->GetIDNetworkNode()
			  << ", cell id " << enb->GetCell ()->GetIdCell ()
			  <<", position: " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
			  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
			  << ", channels id " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
			  << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;

	  ulChannels->at (i)->AddDevice((NetworkNode*) enb);
	  networkManager->GetENodeBContainer ()->push_back (enb);
	  eNBs->push_back (enb);
	}





  int nbUEs = 1;
  int idUe = 100;
  int applicationID = 0;
  int srcPort = 0;
  int dstPort = 100;

  for (int i = 0; i < nbUEs; i++)
    {
	  double posX = 0;
	  double posY = 0;
	  double speedDirection = 0;
	  double speed = 30;

	  ENodeB* enb = eNBs->at(0);
	  UserEquipment* ue = new UserEquipment (idUe,
			                                 posX, posY, speed, speedDirection,
											 enb->GetCell (),
											 enb,
			                                 0, //handover false!
			                                 Mobility::RANDOM_DIRECTION);


	  networkManager->GetUserEquipmentContainer ()->push_back (ue);
	  ue->SetTargetNode (enb);
	  enb->RegisterUserEquipment (ue);

	  std::cout << "Created UE - id " << idUe << " position " << posX << " " << posY << " direction " << speedDirection << std::endl;

	  ue->GetPhy ()->SetDlChannel (enb->GetPhy ()->GetDlChannel ());
	  ue->GetPhy ()->SetUlChannel (enb->GetPhy ()->GetUlChannel ());

      FullbandCqiManager *cqiManager = new FullbandCqiManager ();
      cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
      cqiManager->SetReportingInterval (1);
      ue->SetCqiManager (cqiManager);
      cqiManager->SetDevice (ue);

      WidebandCqiEesmErrorModel *errorModel = new WidebandCqiEesmErrorModel ();
      ue->GetPhy ()->SetErrorModel (errorModel);

	  MacroCellUrbanAreaChannelRealization* c_dl = new MacroCellUrbanAreaChannelRealization (enb, ue);
	  enb->GetPhy ()->GetDlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_dl);
	  MacroCellUrbanAreaChannelRealization* c_ul = new MacroCellUrbanAreaChannelRealization (ue, enb);
	  enb->GetPhy ()->GetUlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_ul);

	  ue->GetPhy ()->GetDlChannel ()->AddDevice (ue);

	  /*
	  double startTime = (double)(rand() %1); //s
	  double stopTime =  (double)(rand() %2);  //s
	  QoSParameters *qos = new QoSParameters ();
	  Application* be = flowsManager->CreateApplication (applicationID,
		                                   			     ue, enb,
									   				     srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
														 Application::APPLICATION_TYPE_INFINITE_BUFFER,
														 qos,
														 startTime, stopTime);
	  */
	  idUe++;
	  applicationID++;
	  dstPort++;
    }


  Simulator::Init ()->SetStop (60);
  Simulator::Init()->Run ();
}

