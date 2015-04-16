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
#include "../channel/propagation-model/macrocell-urban-area-channel-realization.h"
#include "../channel/propagation-model/propagation-loss-model.h"

static void TestPropagationLossModel ()
{
  //Create devices

  Cell *cell = new Cell (0, 1, 0.35, 0, 0);

  LteChannel *dlCh = new LteChannel ();
  LteChannel *ulCh = new LteChannel ();

  BandwidthManager* spectrum = new BandwidthManager (5, 5, 0, 0);

  //Create ENodeB
  ENodeB* enb = new ENodeB (1, cell);
  CartesianCoordinates *cc = new CartesianCoordinates (0, 0);
  enb->GetMobilityModel ()->SetAbsolutePosition (cc);
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


  //Create Channel realization and Propagation loss model
  MacroCellUrbanAreaChannelRealization* c = new MacroCellUrbanAreaChannelRealization (enb, ue);

  PropagationLossModel *model = new PropagationLossModel ();
  model->AddChannelRealization (c);



  // run the test
  TransmittedSignal* txSignal = enb->GetPhy ()->GetTxSignal ();

  model->AddLossModel(enb, ue, txSignal);
}
