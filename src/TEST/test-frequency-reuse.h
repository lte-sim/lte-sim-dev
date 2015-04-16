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


#include "../device/ENodeB.h"
#include "../phy/enb-lte-phy.h"
#include "../phy/ue-lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../networkTopology/Cell.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../protocolStack/packet/Packet.h"
#include "../core/eventScheduler/simulator.h"
#include "../load-parameters.h"
#include "../utility/CellPosition.h"
#include "../utility/frequency-reuse-helper.h"


static void TestFrequencyReuse ()
{

  double bandwidth = 15;
  int cluster = 3;
  int nodes = 7;

  std::vector <BandwidthManager*> spectrums = RunFrequencyReuseTechniques (nodes, cluster, bandwidth);

  NetworkManager* nm = NetworkManager::Init ();
  FrameManager* fm = FrameManager::Init ();


  //create a set of 7 cell with radius equal to 1 km
  std::vector <Cell*> *cells = new std::vector <Cell*>;
  for (int i = 0; i < nodes; i++)
    {
	  CartesianCoordinates center =
			  GetCartesianCoordinatesForCell(i, 1000.);

	  Cell *c = new Cell (i, 1., 0.035, center.GetCoordinateX (), center.GetCoordinateY ());
	  cells->push_back (c);
	  nm->GetCellContainer ()->push_back (c);

	  std::cout << "Created Cell, id " << c->GetIdCell ()
			  <<", position: " << c->GetCellCenterPosition ()->GetCoordinateX ()
			  << " " << c->GetCellCenterPosition ()->GetCoordinateY () << std::endl;
    }


  //Create a set of 7 couple of channels
  std::vector <LteChannel*> *dlChannels = new std::vector <LteChannel*>;
  std::vector <LteChannel*> *ulChannels = new std::vector <LteChannel*>;
  for (int i= 0; i < nodes; i++)
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
  for (int i = 0; i < nodes; i++)
    {
	  ENodeB* enb = new ENodeB (i, cells->at (i));
	  enb->GetPhy ()->SetDlChannel (dlChannels->at (i));
	  enb->GetPhy ()->SetUlChannel (ulChannels->at (i));

	  enb->GetPhy ()->SetBandwidthManager (spectrums.at (i));

	  std::cout << "Created enb, id " << enb->GetIDNetworkNode()
			  << ", cell id " << enb->GetCell ()->GetIdCell ()
			  <<", position: " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
			  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
			  << ", channels id " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
			  << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;

	  spectrums.at (i)->Print ();

	  ulChannels->at (i)->AddDevice((NetworkNode*) enb);

	  nm->GetENodeBContainer ()->push_back (enb);
	  eNBs->push_back (enb);
    }


}
