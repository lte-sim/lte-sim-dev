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

static void TestUplinkMaximumThroughput ()
{

  srand (time(NULL));

  // CREATE COMPONENT MANAGERS
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  NetworkManager* networkManager = NetworkManager::Init();
  FlowsManager* flowsManager = FlowsManager::Init ();


  //Create devices
  Cell *cell = new Cell (0, 1, 0.35, 0, 0);
  LteChannel *dlCh = new LteChannel ();
  LteChannel *ulCh = new LteChannel ();
  BandwidthManager* spectrum = new BandwidthManager (5, 5, 0, 0);

  //Create ENodeB
  ENodeB* enb = new ENodeB (1, cell);
  enb->GetPhy ()->SetDlChannel (dlCh);
  enb->GetPhy ()->SetUlChannel (ulCh);
  enb->GetPhy ()->SetBandwidthManager (spectrum->Copy ());
  enb->SetULScheduler (ENodeB::ULScheduler_TYPE_MAXIMUM_THROUGHPUT);
  enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
  networkManager->GetENodeBContainer ()->push_back (enb);
  ulCh->AddDevice (enb);

  int nbUEs = 4;
  int idUe = 100;
  int applicationID = 0;
  int srcPort = 0;
  int dstPort = 100;

  for (int i = 0; i < nbUEs; i++)
    {
	  double posX = (double)(rand() %1000);
	  double posY = (double)(rand() %1000);
	  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);
	  double speed = 30;


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

	  double startTime = (double)(rand() %1); //s
	  double stopTime =  (double)(rand() %2);  //s
	  QoSParameters *qos = new QoSParameters ();
	  Application* be = flowsManager->CreateApplication (applicationID,
		                                   			     ue, enb,
									   				     srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
														 Application::APPLICATION_TYPE_INFINITE_BUFFER,
														 qos,
														 startTime, stopTime);
	  idUe++;
	  applicationID++;
	  dstPort++;
    }


  Simulator::Init ()->SetStop (5);
  Simulator::Init()->Run ();
}

