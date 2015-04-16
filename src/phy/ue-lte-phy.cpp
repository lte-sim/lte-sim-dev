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


#include "ue-lte-phy.h"
#include "../device/NetworkNode.h"
#include "../channel/LteChannel.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../core/spectrum/transmitted-signal.h"
#include "../core/idealMessages/ideal-control-messages.h"
#include "../protocolStack/mac/AMCModule.h"
#include "../device/UserEquipment.h"
#include "../device/ENodeB.h"
#include "../device/HeNodeB.h"
#include "interference.h"
#include "error-model.h"
#include "../device/CqiManager/cqi-manager.h"
#include "../load-parameters.h"
#include "../core/eventScheduler/simulator.h"
#include "../protocolStack/mac/ue-mac-entity.h"
#include "../utility/eesm-effective-sinr.h"
#include "enb-lte-phy.h"
#include "../utility/ComputePathLoss.h"

/*
 * Noise is computed as follows:
 *  - noise figure = 2.5
 *  - n0 = -174 dBm
 *  - sub channel bandwidth = 180 kHz
 *
 *  noise_db = noise figure + n0 + 10log10 (180000) - 30 = -148.95
 */
#define NOISE -148.95

UeLtePhy::UeLtePhy()
{
  m_channelsForRx.clear ();
  m_channelsForTx.clear ();
  m_mcsIndexForRx.clear ();
  m_mcsIndexForTx.clear ();
  SetDevice (NULL);
  SetDlChannel (NULL);
  SetUlChannel (NULL);
  SetBandwidthManager (NULL);
  SetTxSignal (NULL);
  SetErrorModel (NULL);
  Interference *interference = new Interference ();
  SetInterference (interference);
  SetTxPower (23); //dBm

  Simulator::Init()->Schedule(0.001, &UeLtePhy::SetTxSignalForReferenceSymbols, this);
}

UeLtePhy::~UeLtePhy()
{
  Destroy ();
}

void
UeLtePhy::DoSetBandwidthManager (void)
{
  BandwidthManager* s = GetBandwidthManager ();
  std::vector<double> channels = s->GetUlSubChannels ();

  TransmittedSignal* txSignal = new TransmittedSignal ();

  std::vector<double> values;
  std::vector<double>::iterator it;
  for (it = channels.begin (); it != channels.end (); it++ )
	{
	  values.push_back(0);
	}

  if (m_channelsForTx.size () > 0)
    {
      double totPower = pow (10., (GetTxPower () - 30) / 10); // in natural unit
      double txPower = 10 * log10 (totPower / m_channelsForTx.size ()); //in dB
      for (std::vector<int>::iterator it = m_channelsForTx.begin ();
    		  it != m_channelsForTx.end (); it++)
        {
    	  int channel = (*it);
    	  values.at (channel) = txPower;
        }
    }
  txSignal->SetValues (values);
  SetTxSignal (txSignal);
}

void
UeLtePhy::StartTx (PacketBurst* p)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "Node " << GetDevice()->GetIDNetworkNode () << " starts phy tx" << std::endl;
#endif

  GetUlChannel ()->StartTx (p, GetTxSignal (), GetDevice ());
}

void
UeLtePhy::StartRx (PacketBurst* p, TransmittedSignal* txSignal)
{
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "Node " << GetDevice()->GetIDNetworkNode () << " starts phy rx" << std::endl;
#endif

  m_measuredSinr.clear();

  //COMPUTE THE SINR
  std::vector<double> rxSignalValues;
  std::vector<double>::iterator it;

  rxSignalValues = txSignal->Getvalues();

  //compute noise + interference
  double interference;
  if (GetInterference () != NULL)
    {
      interference = GetInterference ()->ComputeInterference ((UserEquipment*) GetDevice ());
    }
  else
    {
	  interference = 0;
    }

  double noise_interference = 10. * log10 (pow(10., NOISE/10) + interference); // dB


  for (it = rxSignalValues.begin(); it != rxSignalValues.end(); it++)
    {
      double power; // power transmission for the current sub channel [dB]
      if ((*it) != 0.)
        {
          power = (*it);
        }
      else
        {
          power = 0.;
        }
      m_measuredSinr.push_back (power - noise_interference);
    }

  //CHECK FOR PHY ERROR
  bool phyError;

  if (GetErrorModel() != NULL && m_channelsForRx.size () > 0)
    {
	  std::vector<int> cqi_;
	  for (int i = 0; i < m_mcsIndexForRx.size (); i++)
	    {
		  AMCModule *amc = GetDevice ()->GetProtocolStack ()->GetMacEntity ()->GetAmcModule ();
		  int cqi =  amc->GetCQIFromMCS (m_mcsIndexForRx.at (i));
		  cqi_.push_back (cqi);
	    }
	  phyError = GetErrorModel ()->CheckForPhysicalError (m_channelsForRx, cqi_, m_measuredSinr);

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
  else
    {
	  phyError = false;
    }

#ifdef TEST_DL_SINR
  double effective_sinr = GetEesmEffectiveSinr (m_measuredSinr);
  if (effective_sinr > 40) effective_sinr = 40;
  int cqi = GetDevice ()->GetProtocolStack ()->GetMacEntity ()->GetAmcModule ()->GetCQIFromSinr (effective_sinr);
  int MCS_ = GetDevice ()->GetProtocolStack ()->GetMacEntity ()->GetAmcModule ()->GetMCSFromCQI (cqi);
  int TBS_ = GetDevice ()->GetProtocolStack ()->GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (MCS_, GetBandwidthManager ()->GetDlSubChannels ().size ());
  std::cout << "DL_SINR " << GetDevice ()->GetIDNetworkNode () << " " <<
		GetDevice ()->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX () << " " <<
		GetDevice ()->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY () << " " <<
		effective_sinr << " " <<
		MCS_ << " " << TBS_ << std::endl;
#endif

  if (!phyError && p->GetNPackets() > 0)
    {
	  //FORWARD RECEIVED PACKETS TO THE DEVICE
	  GetDevice()->ReceivePacketBurst(p);
    }

  //CQI report
  CreateCqiFeedbacks (m_measuredSinr);

  m_channelsForRx.clear ();
  m_channelsForTx.clear ();
  m_mcsIndexForRx.clear ();
  m_mcsIndexForTx.clear ();

  delete txSignal;
  delete p;
}

void
UeLtePhy::CreateCqiFeedbacks (std::vector<double> sinr)
{
  UserEquipment* thisNode = (UserEquipment*) GetDevice ();
  if (thisNode->GetCqiManager ()->NeedToSendFeedbacks ())
    {
      thisNode->GetCqiManager ()->CreateCqiFeedbacks (sinr);
    }
}


void
UeLtePhy::SendIdealControlMessage (IdealControlMessage *msg)
{
#ifdef TEST_CQI_FEEDBACKS
  std::cout << "SendIdealControlMessage (PHY) from  " << msg->GetSourceDevice ()->GetIDNetworkNode ()
		  << " to " << msg->GetDestinationDevice ()->GetIDNetworkNode () << std::endl;
#endif

  NetworkNode* dst = msg->GetDestinationDevice ();
  dst->GetPhy ()->ReceiveIdealControlMessage (msg);

  delete msg;
}

void
UeLtePhy::ReceiveIdealControlMessage (IdealControlMessage *msg)
{
  if (msg->GetMessageType () == IdealControlMessage::ALLOCATION_MAP)
	{
      //std::cout << "ReceiveIdealControlMessage, node " << GetDevice()->GetIDNetworkNode() << std::endl;

	  m_channelsForRx.clear ();
	  m_channelsForTx.clear ();
	  m_mcsIndexForRx.clear ();
	  m_mcsIndexForTx.clear ();

	  PdcchMapIdealControlMessage *map = (PdcchMapIdealControlMessage*) msg;
      PdcchMapIdealControlMessage::IdealPdcchMessage *map2 = map->GetMessage ();
      PdcchMapIdealControlMessage::IdealPdcchMessage::iterator it;

      int node = GetDevice ()->GetIDNetworkNode ();

      for (it = map2->begin(); it != map2->end (); it++)
        {
    	  if ((*it).m_ue->GetIDNetworkNode () == node)
    	    {
              if ((*it).m_direction == PdcchMapIdealControlMessage::DOWNLINK)
                {
            	  //std::cout << "\t channel " << (*it).m_idSubChannel
                	//		  << " mcs "<< (*it).m_mcsIndex << std::endl;

            	  m_channelsForRx.push_back ((*it).m_idSubChannel);
            	  m_mcsIndexForRx.push_back((*it).m_mcsIndex);
                }
              else if ((*it).m_direction == PdcchMapIdealControlMessage::UPLINK)
                {
            	  m_channelsForTx.push_back ((*it).m_idSubChannel);
            	  m_mcsIndexForTx.push_back((*it).m_mcsIndex);
                }
    	    }
	    }

      if (m_channelsForTx.size () > 0)
        {
    	  DoSetBandwidthManager ();
    	  UeMacEntity* mac = (UeMacEntity*) GetDevice ()->GetProtocolStack ()->GetMacEntity ();
    	  mac->ScheduleUplinkTransmission (m_channelsForTx.size (), m_mcsIndexForTx.at (0));
        }
	}
}

void
UeLtePhy::SetTxSignalForReferenceSymbols (void)
{
  BandwidthManager* s = GetBandwidthManager ();
  std::vector<double> channels = s->GetUlSubChannels ();

  double powerTx = pow (10., (GetTxPower () - 30.) / 10.); // in natural unit
  double txPower = 10 * log10 (powerTx / channels.size ()); //in dB

  TransmittedSignal* txSignal = new TransmittedSignal ();
  std::vector<double> values;
  std::vector<double>::iterator it;
  for (it = channels.begin (); it != channels.end (); it++ )
	{
	  values.push_back(txPower);
	}
  txSignal->SetValues (values);
  m_txSignalForRerferenceSymbols = txSignal;

  SendReferenceSymbols();
}

TransmittedSignal*
UeLtePhy::GetTxSignalForReferenceSymbols (void)
{
  return m_txSignalForRerferenceSymbols;
}

void
UeLtePhy::SendReferenceSymbols (void)
{
  UserEquipment* ue = (UserEquipment*) GetDevice ();
  ENodeB* target = (ENodeB*) ue->GetTargetNode ();
  EnbLtePhy* enbPhy = (EnbLtePhy*) target->GetPhy ();
  enbPhy->ReceiveReferenceSymbols (ue, GetTxSignalForReferenceSymbols ());
  Simulator::Init()->Schedule(0.001, &UeLtePhy::SendReferenceSymbols, this);
}


