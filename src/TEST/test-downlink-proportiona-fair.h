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
#include "../componentManagers/FrameManager.h"


static void TestDownlinkProportionalFair ()
{

  FrameManager *frameManager = FrameManager::Init();
  frameManager->SetFrameStructure(FrameManager::FRAME_STRUCTURE_FDD);
  NetworkManager* networkManager = NetworkManager::Init();

  //Create devices

  Cell *cell = new Cell (0, 1, 0.35, 0, 0);
  networkManager->GetCellContainer ()->push_back (cell);

  LteChannel *dlCh = new LteChannel ();
  LteChannel *ulCh = new LteChannel ();

  BandwidthManager* spectrum = new BandwidthManager (10, 10, 0, 0);

  //Create ENodeB
  ENodeB* enb = new ENodeB (1, cell);
  enb->GetPhy ()->SetDlChannel (dlCh);
  enb->GetPhy ()->SetUlChannel (ulCh);
  enb->GetPhy ()->SetBandwidthManager (spectrum->Copy ());
  ulCh->AddDevice (enb);
  networkManager->GetENodeBContainer ()->push_back (enb);

  //Create UE
  UserEquipment* ue = new UserEquipment (2, 50, 50, 0, 0, cell, enb, 0, Mobility::RANDOM_DIRECTION);
  ue->GetPhy ()->SetDlChannel (dlCh);
  ue->GetPhy ()->SetUlChannel (ulCh);
  ue->GetPhy ()->SetBandwidthManager (spectrum->Copy ());

  FullbandCqiManager *cqiManager = new FullbandCqiManager ();
  cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
  cqiManager->SetReportingInterval (0.002);
  cqiManager->SetDevice (ue);
  ue->SetCqiManager (cqiManager);

  networkManager->GetUserEquipmentContainer ()->push_back (ue);

  //Create GW
  Gateway *gw = new Gateway ();
  networkManager->GetGatewayContainer ()->push_back (gw);


  enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
  enb->RegisterUserEquipment (ue);

  CBR *app = new CBR ();
  app->SetSource (gw);
  app->SetDestination (ue);
  app->SetApplicationID (1);
  app->SetSize (100);
  app->SetInterval (0.01);

  ClassifierParameters *cp = new ClassifierParameters (gw->GetIDNetworkNode(),
                                                      ue->GetIDNetworkNode(),
                                                      0,
                                                      100,
                                                      TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);
  QoSParameters *qos = new QoSParameters ();
  qos->SetMaxDelay (0.1);

  app->SetClassifierParameters (cp);
  app->SetQoSParameters (qos);


  app->SetStartTime (0.01);
  app->SetStopTime (0.1);

  Simulator::Init()->SetStop(0.101);
  Simulator::Init()->Run ();
}
