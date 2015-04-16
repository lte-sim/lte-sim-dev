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
 * Author: Khaled Elsayed <khaled@ieee.org>
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

static void TestUplinkFME ()
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
  enb->SetULScheduler (ENodeB::ULScheduler_TYPE_FME);
  enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
  networkManager->GetENodeBContainer ()->push_back (enb);
  ulCh->AddDevice (enb);

 
  int nbUEs = 4;

  int idUe = 100;

  int applicationID = 0;
  int srcPort = 0;
  int dstPort = 100;
  int startTime = 0.01; //s
  int stopTime = 1.01;  //s

  for (int i = 0; i < nbUEs; i++)
    {
	  //ue's random position
	  int maxXY = cell->GetRadius () * 1000;
	  double posX = (double) (rand() % 1000); //200;
	  double posY = (double) (rand() % 1000); //200;
	  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);
	  double speed = 30;

	  printf("Creating UE %d at (%lf,%lf)\n", idUe, posX, posY);

	  UserEquipment* ue =
			  networkManager->CreateUserEquipment (idUe, posX, posY, speed, speedDirection, cell, enb);

	  MacroCellUrbanAreaChannelRealization* c_dl = new MacroCellUrbanAreaChannelRealization (enb, ue);
	  enb->GetPhy()->GetDlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_dl);
	  MacroCellUrbanAreaChannelRealization* c_ul = new MacroCellUrbanAreaChannelRealization (ue, enb);
	  enb->GetPhy()->GetUlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c_ul);

	  ue->GetPhy ()->SetDlChannel (dlCh);
	  ue->GetPhy ()->SetUlChannel (ulCh);

	  ue->GetPhy ()->GetDlChannel ()->AddDevice (ue);

	  enb->RegisterUserEquipment (ue);

	  FullbandCqiManager *cqiManager = new FullbandCqiManager ();
	  cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
	  cqiManager->SetReportingInterval (1);
	  cqiManager->SetDevice (ue);
	  ue->SetCqiManager (cqiManager);

	  //Create an Application
	  QoSParameters *qos = new QoSParameters ();
	  qos->SetMaxDelay(0.1);


	  CBR *cbrApp = new CBR;
	  // create application
	  cbrApp->SetApplicationID(applicationID);
	  cbrApp->SetSource(ue);
	  cbrApp->SetDestination(enb);
	  cbrApp->SetSourcePort(srcPort);
	  cbrApp->SetDestinationPort(dstPort);
	  cbrApp->SetStartTime(startTime);
	  cbrApp->SetStopTime(stopTime);
	  cbrApp->SetInterval (0.008);
	  cbrApp->SetSize (500);
	  cbrApp->SetQoSParameters(qos);

	  ClassifierParameters *cp = new ClassifierParameters (
			  ue->GetIDNetworkNode(),enb->GetIDNetworkNode(),srcPort,
			  dstPort,TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);
	  cbrApp->SetClassifierParameters (cp);

	  std::cout << "CREATED CBR APPLICATION, ID " << applicationID << std::endl;


	  idUe++;
	  applicationID++;
	  dstPort++;

    }






  //Simulator::Init ()->SetStop (0.01);
  Simulator::Init ()->SetStop (1.01);
  Simulator::Init()->Run ();
}

