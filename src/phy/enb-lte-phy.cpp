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


#include "enb-lte-phy.h"
#include "../device/NetworkNode.h"
#include "../channel/LteChannel.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../core/spectrum/transmitted-signal.h"
#include "../core/idealMessages/ideal-control-messages.h"
#include "../device/ENodeB.h"
#include "../device/UserEquipment.h"
#include "interference.h"
#include "error-model.h"
#include "../channel/propagation-model/propagation-loss-model.h"
#include "../protocolStack/mac/AMCModule.h"
#include "../utility/eesm-effective-sinr.h"
#include "../componentManagers/FrameManager.h"


/*
 * Noise is computed as follows:
 *  - noise figure = 2.5
 *  - n0 = -174 dBm
 *  - sub channel bandwidth = 180 kHz
 *
 *  noise_db = noise figure + n0 + 10log10 (180000) - 30 = -148.95
 */
#define NOISE -148.95
#define UL_INTERFERENCE 4

EnbLtePhy::EnbLtePhy()
{
  SetDevice(NULL);
  SetDlChannel(NULL);
  SetUlChannel(NULL);
  SetBandwidthManager(NULL);
  SetTxSignal (NULL);
  SetErrorModel (NULL);
  SetInterference (NULL);
  SetTxPower(43); //dBm
}

EnbLtePhy::~EnbLtePhy()
{
  Destroy ();
}

void
EnbLtePhy::DoSetBandwidthManager (void)
{
  BandwidthManager* s = GetBandwidthManager ();
  std::vector<double> channels = s->GetDlSubChannels ();

  TransmittedSignal* txSignal = new TransmittedSignal ();

  std::vector<double> values;
  std::vector<double>::iterator it;

  double powerTx = pow (10., (GetTxPower () - 30) / 10); // in natural unit

  double txPower = 10 * log10 (powerTx / channels.size ()); //in dB

  for (it = channels.begin (); it != channels.end (); it++ )
    {
      values.push_back(txPower);
    }

  txSignal->SetValues (values);
  //txSignal->SetBandwidthManager (s->Copy());

  SetTxSignal (txSignal);
}

void
EnbLtePhy::StartTx (PacketBurst* p)
{
  //std::cout << "Node " << GetDevice()->GetIDNetworkNode () << " starts phy tx" << std::endl;
  GetDlChannel ()->StartTx (p, GetTxSignal (), GetDevice ());
}

void
EnbLtePhy::StartRx (PacketBurst* p, TransmittedSignal* txSignal)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "Node " << GetDevice()->GetIDNetworkNode () << " starts phy rx" << std::endl;
#endif

  //COMPUTE THE SINR
  std::vector<double> measuredSinr;
  std::vector<int> channelsForRx;
  std::vector<double> rxSignalValues;
  std::vector<double>::iterator it;
  rxSignalValues = txSignal->Getvalues();

  double interference = 0;
  double noise_interference = 10. * log10 (pow(10., NOISE/10) + interference); // dB

  int chId = 0;
  for (it = rxSignalValues.begin(); it != rxSignalValues.end(); it++)
    {
      double power; // power transmission for the current sub channel [dB]
      if ((*it) != 0.)
        {
          power = (*it);
          channelsForRx.push_back (chId);
        }
      else
        {
          power = 0.;
        }
      chId++;
      measuredSinr.push_back (power - noise_interference - UL_INTERFERENCE);
    }

  //CHECK FOR PHY ERROR
  bool phyError = false;
  /*
  if (GetErrorModel() != NULL)
    {
	  std::vector<int> cqi; //compute the CQI
	  phyError = GetErrorModel ()->CheckForPhysicalError (channelsForRx, cqi, measuredSinr);
	  if (_PHY_TRACING_)
	    {
	      if (phyError)
	        {
		      std::cout << "**** YES PHY ERROR (node " << GetDevice ()->GetIDNetworkNode () << ") ****" << std::endl;
	        }
	      else
	        {
		      std::cout << "**** NO PHY ERROR (node " << GetDevice ()->GetIDNetworkNode () << ") ****" << std::endl;
	        }
	    }
    }
    */

  if (!phyError && p->GetNPackets() > 0)
    {
	  //FORWARD RECEIVED PACKETS TO THE DEVICE
	  GetDevice()->ReceivePacketBurst(p);
    }

  delete txSignal;
  delete p;
}

void
EnbLtePhy::SendIdealControlMessage (IdealControlMessage *msg)
{
  if (msg->GetMessageType () == IdealControlMessage::ALLOCATION_MAP)
	{
	  ENodeB *enb = (ENodeB*) GetDevice ();
	  ENodeB::UserEquipmentRecords* registeredUe = enb->GetUserEquipmentRecords ();
	  ENodeB::UserEquipmentRecords::iterator it;

	  for (it = registeredUe->begin (); it != registeredUe->end (); it++)
	    {
		  //std::cout << "SendIdealControlMessage to " << (*it)->GetUE ()->GetIDNetworkNode() << std::endl;
		  (*it)->GetUE ()->GetPhy ()->ReceiveIdealControlMessage (msg);
	    }
	}
}

void
EnbLtePhy::ReceiveIdealControlMessage (IdealControlMessage *msg)
{
#ifdef TEST_CQI_FEEDBACKS
  std::cout << "ReceiveIdealControlMessage (PHY) from  " << msg->GetSourceDevice ()->GetIDNetworkNode ()
		  << " to " << msg->GetDestinationDevice ()->GetIDNetworkNode () << std::endl;
#endif

  //RECEIVE CQI FEEDBACKS
  if (msg->GetMessageType () == IdealControlMessage::CQI_FEEDBACKS)
    {
	  CqiIdealControlMessage *cqiMsg = (CqiIdealControlMessage*) msg;
	  EnbMacEntity *mac = (EnbMacEntity*) GetDevice ()->GetProtocolStack ()->GetMacEntity ();
      mac->ReceiveCqiIdealControlMessage (cqiMsg);
    }
  if (msg->GetMessageType () == IdealControlMessage::SCHEDULING_REQUEST)
    {
	  SchedulingRequestIdealControlMessage *srMsg = (SchedulingRequestIdealControlMessage*) msg;
	  EnbMacEntity *mac = (EnbMacEntity*) GetDevice ()->GetProtocolStack ()->GetMacEntity ();
	  mac->ReceiveSchedulingRequestIdealControlMessage (srMsg);
    }
}

void
EnbLtePhy::ReceiveReferenceSymbols (NetworkNode* n, TransmittedSignal* s)
{
  ENodeB::UserEquipmentRecord* user = ((ENodeB*) GetDevice ())->
		  GetUserEquipmentRecord (n->GetIDNetworkNode ());
  TransmittedSignal* rxSignal;
  if (GetUlChannel ()->GetPropagationLossModel () != NULL)
	{
	  rxSignal = GetUlChannel ()->GetPropagationLossModel ()->
			  AddLossModel (n, GetDevice (), s);
	}
  else
	{
	  rxSignal = s->Copy ();
	}
  AMCModule* amc = GetDevice ()->GetProtocolStack ()->GetMacEntity ()->GetAmcModule ();
  std::vector<double> ulQuality;
  std::vector<double> rxSignalValues = rxSignal->Getvalues ();
  double noise_interference = 10. * log10 (pow(10., NOISE/10)); // dB
  for (std::vector<double>::iterator it = rxSignalValues.begin(); it != rxSignalValues.end(); it++)
    {
      double power;
      if ((*it) != 0.)
        {
          power = (*it);
        }
      else
        {
          power = 0.;
        }
      ulQuality.push_back (power - noise_interference - UL_INTERFERENCE);
    }


#ifdef TEST_UL_SINR
  double effectiveSinr = GetEesmEffectiveSinr (ulQuality);
  if (effectiveSinr > 40) effectiveSinr = 40;
  int mcs = amc->GetMCSFromCQI (amc->GetCQIFromSinr(effectiveSinr));
  std::cout << "UL_SINR " << n->GetIDNetworkNode () << " "
		  << n->GetMobilityModel ()->GetAbsolutePosition()->GetCoordinateX () << " "
		  << n->GetMobilityModel ()->GetAbsolutePosition()->GetCoordinateY () << " "
		  << effectiveSinr << " " << mcs << std::endl;
#endif


  user->SetUplinkChannelStatusIndicator (ulQuality);
}

