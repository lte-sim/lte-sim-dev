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
#include "../device/NetworkNode.h"
#include "../device/ENodeB.h"
#include "../core/eventScheduler/simulator.h"
#include "../load-parameters.h"
#include "../device/CqiManager/fullband-cqi-manager.h"
#include "../device/CqiManager/wideband-cqi-manager.h"
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
#include "../channel/propagation-model/macrocell-urban-area-channel-realization.h"
#include "../phy/wideband-cqi-eesm-error-model.h"
#include "../phy/simple-error-model.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <cstring>

/*
* ENABLE IN load-parameters.h:
* #define AMC_MAPPING
*/

static void TestAmcMapping (int nbCells, double radius, int speed, double bandwidth, int cluster)
{

  // define simulation times
  double speed_ = (speed * 1000.)/3600.;
  double distance = (radius * 1000) - 40;
  double duration = distance / speed_;

  NetworkManager* nm = NetworkManager::Init ();
  FrameManager* fm = FrameManager::Init ();


  std::vector <BandwidthManager*> spectrums = RunFrequencyReuseTechniques (nbCells, cluster, bandwidth);


  //create a set of 7 cell with radius equal to 1 km
  std::vector <Cell*> *cells = new std::vector <Cell*>;
  for (int i = 0; i < nbCells; i++)
    {
	  CartesianCoordinates center =
			  GetCartesianCoordinatesForCell(i, radius * 1000.);

	  Cell *c = new Cell (i, radius, 0.035, center.GetCoordinateX (), center.GetCoordinateY ());
	  cells->push_back (c);
	  nm->GetCellContainer ()->push_back (c);

	  std::cout << "Created Cell, id " << c->GetIdCell ()
			  <<", pos: " << c->GetCellCenterPosition ()->GetCoordinateX ()
			  << " " << c->GetCellCenterPosition ()->GetCoordinateY () << std::endl;
    }


  //Create a set of 7 couple of channels
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


  //create a set of 7 cell with radius equal to 1 km
  std::vector <ENodeB*> *eNBs = new std::vector <ENodeB*>;
  for (int i = 0; i < nbCells; i++)
    {
	  ENodeB* enb = new ENodeB (i, cells->at (i));
	  enb->GetPhy ()->SetDlChannel (dlChannels->at (i));
	  enb->GetPhy ()->SetUlChannel (ulChannels->at (i));

      enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);

	  enb->GetPhy ()->SetBandwidthManager (spectrums.at (i));

	  std::cout << "Created enb, id " << enb->GetIDNetworkNode()
			  << ", cell id " << enb->GetCell ()->GetIdCell ()
			  <<", pos: " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
			  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
			  << ", channels id " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
			  << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;

	  spectrums.at (i)->Print ();


	  ulChannels->at (i)->AddDevice((NetworkNode*) enb);


	  nm->GetENodeBContainer ()->push_back (enb);
	  eNBs->push_back (enb);
    }


  //Create UE
  int idUE = 7;
  double posX = 40;
  double posY = 0;
  double speedDirection = 0.;

  UserEquipment* ue = new UserEquipment (idUE,
		                                 posX, posY, speed, speedDirection,
		                                 cells->at (0),
		                                 eNBs->at (0),
		                                 1, //handover true!
		                                 Mobility::RANDOM_DIRECTION);

  ue->GetPhy ()->SetDlChannel (eNBs->at (0)->GetPhy ()->GetDlChannel ());
  ue->GetPhy ()->SetUlChannel (eNBs->at (0)->GetPhy ()->GetUlChannel ());
  ue->GetPhy ()->SetBandwidthManager (eNBs->at (0)->GetPhy ()->GetBandwidthManager ());

  FullbandCqiManager *cqiManager = new FullbandCqiManager ();
  cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
  cqiManager->SetReportingInterval (1);
  cqiManager->SetDevice (ue);
  ue->SetCqiManager (cqiManager);

  WidebandCqiEesmErrorModel *errorModel = new WidebandCqiEesmErrorModel ();
  ue->GetPhy ()->SetErrorModel (errorModel);

  nm->GetUserEquipmentContainer ()->push_back (ue);

  eNBs->at (0)->RegisterUserEquipment (ue);

  //attach device to the channel
  dlChannels->at (0)->AddDevice((NetworkNode*) ue);

  // define the channel realization
  MacroCellUrbanAreaChannelRealization* c = new MacroCellUrbanAreaChannelRealization (eNBs->at (0), ue);
  c->SetChannelType (ChannelRealization::CHANNEL_TYPE_PED_A);
  dlChannels->at (0)->GetPropagationLossModel ()->AddChannelRealization (c);


  Simulator::Init()->SetStop (duration);
  Simulator::Init()->Run ();

}
