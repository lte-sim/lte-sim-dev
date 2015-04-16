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


#ifndef CHANNEL_REALIZATION_HELPER_H_
#define CHANNEL_REALIZATION_HELPER_H_

#include "macrocell-urban-area-channel-realization.h"
#include "winner-downlink-channel-realization.h"
#include "femtocell-urban-area-channel-realization.h"

static ChannelRealization* CreateChannelRealization (NetworkNode* src, NetworkNode* dst)
{
  if (src->GetNodeType()==NetworkNode::TYPE_ENODEB ||
		  dst->GetNodeType()==NetworkNode::TYPE_ENODEB)
    {
	  MacroCellUrbanAreaChannelRealization* c =
			new MacroCellUrbanAreaChannelRealization (src, dst);
	  return c;
    }
  else if (src->GetNodeType()==NetworkNode::TYPE_HOME_BASE_STATION
		  || dst->GetNodeType()==NetworkNode::TYPE_HOME_BASE_STATION)
    {

	  //FemtoCellUrbanAreaChannelRealization* c =
	  //		  new FemtoCellUrbanAreaChannelRealization (src, dst);

	  WinnerDownlinkChannelRealization* c =
	   		  new WinnerDownlinkChannelRealization (src, dst);
	  return c;
	}
  else
	{
	  MacroCellUrbanAreaChannelRealization* c =
			  new MacroCellUrbanAreaChannelRealization (src, dst);
	  return c;
	}

}

#endif /* CHANNEL_REALIZATION_HELPER_H_ */
