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

#include "wideband-cqi-eesm-error-model.h"
#include "BLERTrace/BLERvsSINR_15CQI_AWGN.h"
#include "BLERTrace/BLERvsSINR_15CQI_TU.h"
#include "../utility/RandomVariable.h"
#include "../utility/eesm-effective-sinr.h"
#include "../load-parameters.h"

WidebandCqiEesmErrorModel::WidebandCqiEesmErrorModel()
{}

WidebandCqiEesmErrorModel::~WidebandCqiEesmErrorModel()
{}

bool
WidebandCqiEesmErrorModel::CheckForPhysicalError (std::vector<int> channels, std::vector<int> mcs, std::vector<double> sinr)
{

  bool error = false;

  //compute the sinr vector associated to assigned sub channels
  std::vector<double> new_sinr;
  for (int i = 0; i < channels.size (); i++)
	{
	  new_sinr.push_back (sinr.at (channels.at (i)));
	}

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
  for (int i = 0; i < new_sinr.size (); i++)
	{
	  std::cout << new_sinr.at (i) << " ";
	}
  std::cout << "\n"<< std::endl;
#endif


  double effective_sinr = GetEesmEffectiveSinr (new_sinr);
  double randomNumber = (rand () %100 ) / 100.;
  int mcs_ = mcs.at (0);
  double bler;

  if (_channel_AWGN_)
    {
	  bler = GetBLER_AWGN (effective_sinr, mcs_);
    }
  else if (_channel_TU_)
    {
	  bler = GetBLER_TU (effective_sinr, mcs_);
    }
  else
    {
	  bler = GetBLER_AWGN (effective_sinr, mcs_);
    }

#ifdef BLER_DEBUG
	  std::cout <<"CheckForPhysicalError: , effective SINR:" << effective_sinr
			  << ", selected CQI: " << mcs_
			  << ", random " << randomNumber
			  << ", BLER: " << bler << std::endl;
#endif

  if (randomNumber < bler)
	{
	  error = true;
	  if (_TEST_BLER_) std::cout << "BLER PDF " << effective_sinr << " 1" << std::endl;
	}
  else
	{
	  if (_TEST_BLER_) std::cout << "BLER PDF " << effective_sinr << " 0" << std::endl;
	}

  return error;
}
