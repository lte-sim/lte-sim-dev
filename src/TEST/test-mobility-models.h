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
* #define PLOT_USER_POSITION
*/

static void TestMobilityModels (double radius, int nbUE,
                                int mobility_model, int speed,
                                double duration)
{

  Mobility::MobilityModel model;
  if (mobility_model == 0)
	  model = Mobility::RANDOM_DIRECTION;
  else if (mobility_model == 1)
	  model = Mobility::RANDOM_WALK;

  srand (time(NULL));

  // CREATE COMPONENT MANAGER
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  NetworkManager* networkManager = NetworkManager::Init();

  frameManager->SetFrameStructure(FrameManager::FRAME_STRUCTURE_FDD);

  // CREATE CELL
  Cell *cell = new Cell (0, radius, 0.035, 0, 0);
  networkManager->GetCellContainer ()->push_back (cell);

  //Create ENodeB
  ENodeB* enb = new ENodeB (1, cell, 0, 0);
  networkManager->GetENodeBContainer ()->push_back (enb);

  //Create UEs
  int idUE = 2;
  for (int i = 0; i < nbUE; i++)
    {
	  //ue's random position
	  int maxXY = radius * 1000; // in metres
	  double posX = GetRandomVariable (maxXY); // rand () %maxXY;
	  double posY = GetRandomVariable (maxXY); // rand () %maxXY;
	  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);

	  UserEquipment* ue = new UserEquipment (idUE,
			                                 posX, posY, speed, speedDirection,
			                                 cell,
			                                 enb,
			                                 0,
			                                 model);

	  std::cout << "Created UE - id " << idUE << " position " << posX << " " << posY << std::endl;

      networkManager->GetUserEquipmentContainer ()->push_back (ue);

      idUE++;
    }

  simulator->SetStop(duration);
  simulator->Run ();
}
