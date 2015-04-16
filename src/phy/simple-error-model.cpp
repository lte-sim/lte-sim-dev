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
 * Author: Francesco Capozzi <f.capozzi@poliba.it>
 */

#include "simple-error-model.h"
#include "BLERTrace/BLERvsSINR_15CQI_AWGN.h"
#include "BLERTrace/BLERvsSINR_15CQI_TU.h"
#include "../utility/RandomVariable.h"

SimpleErrorModel::SimpleErrorModel()
{}

SimpleErrorModel::~SimpleErrorModel()
{}

bool
SimpleErrorModel::CheckForPhysicalError (std::vector<int> channels, std::vector<int> mcs, std::vector<double> sinr)
{
  /*
   * The device determines if a packet has been received correctly.
   * To this aim, for each sub-channel, used to transmit that packet,
   * the Block Error Rate (BLER) is estimated.
   *
   * The BLER is obtained considering both MCS used for the transmission
   * and SINR that the device has estimated for the considered sub-channel.
   * In particular, the BLER value is drawn using stored BLER-SINR curves
   * stored into trace files, generated using an ad hoc OFDMA tool written in matlab.
   * According to the proper BLER-SINR curve (depending on the used MCS),
   * the device establishes if the packet has been correctly received or not.
   * In the latter case, the packet is considered erroneous and ignored.
   */

  bool error = false;

#ifdef  BLER_DEBUG
  std::cout << "\n--> CheckForPhysicalError \n\t\t Channels: ";
  for (int i = 0; i < channels.size (); i++)
	{
	  std::cout << channels.at (i) << " ";
	}
  std::cout << "\n\t\t MCS: ";
    for (int i = 0; i < mcs.size (); i++)
  	{
  	  std::cout << mcs.at (i) << " ";
  	}
    std::cout << "\n\t\t SINR: ";
  for (int i = 0; i < sinr.size (); i++)
	{
	  std::cout << sinr.at (i) << " ";
	}
  std::cout << "\n"<< std::endl;
#endif


  double randomNumber = (rand () %100 ) / 100.;

  for (int i = 0; i < channels.size (); i++)
    {
	  int mcs_ = mcs.at (i);
	  double sinr_ = sinr.at (channels.at (i));


	  double bler;
	  if (_channel_TU_)
	    {
		  bler = GetBLER_TU (sinr_, mcs_);
	    }
	  else
	    {
		  bler = GetBLER_AWGN (sinr_, mcs_);
	    }

#ifdef BLER_DEBUG
	  std::cout <<"Get BLER for ch " << channels.at(i)<<
			  " cqi " << mcs_ << " sinr "  << sinr_
			  << " BLER = " << bler << std::endl;
#endif

      if (randomNumber < bler)
	    {
#ifdef BLER_DEBUG
	    	std::cout << "ERROR RX ---- "
	    			  << "effective SINR:" << sinr_
	    			  << ", selected CQI: " << mcs_
	    			  << ", random " << randomNumber
	    			  << ", BLER: " << bler << std::endl;
#endif
		  error = true;
		  if (_TEST_BLER_) std::cout << "BLER PDF " << sinr_ << " 1" << std::endl;
		}
	  else
		{
		  if (_TEST_BLER_) std::cout << "BLER PDF " << sinr_ << " 0" << std::endl;
		}
    }

  return error;
}
