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
 * along wittheh LTE-Sim; if not, see <http://www.gnu.org/licenses/>.
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

static void TestEnqueuePackets ()
{
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
  ulCh->AddDevice (enb);
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

  Gateway *gw = new Gateway ();



  CBR *app = new CBR ();
  app->SetSource (gw);
  app->SetDestination (ue);
  app->SetApplicationID (1);
  app->SetSize (500);
  app->SetInterval (0.01);

  ClassifierParameters *cp = new ClassifierParameters (gw->GetIDNetworkNode(),
                                                      ue->GetIDNetworkNode(),
                                                      0,
                                                      100,
                                                      TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);
  QoSParameters *qos = new QoSParameters ();

  app->SetClassifierParameters (cp);
  app->SetQoSParameters (qos);

  app->SetStartTime (0.01);
  app->SetStopTime (0.1);

  Simulator::Init()->Run ();
}
