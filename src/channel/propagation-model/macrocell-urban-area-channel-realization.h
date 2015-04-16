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


#ifndef MacroCellUrbanAreaChannelRealization_H_
#define MacroCellUrbanAreaChannelRealization_H_

#include "channel-realization.h"
#include "vector"

class NetworkNode;

class MacroCellUrbanAreaChannelRealization: public ChannelRealization {
public:
	MacroCellUrbanAreaChannelRealization (NetworkNode* src, NetworkNode* dst);
	virtual ~MacroCellUrbanAreaChannelRealization ();
	void SetPenetrationLoss (double pnl);
	double GetPenetrationLoss (void);
	double GetPathLoss (void);
	void SetShadowing (double sh);
	double GetShadowing (void);
	virtual void UpdateModels (void);

	virtual std::vector<double> GetLoss ();


private:
	double m_penetrationLoss;
	double m_pathLoss;
	double m_shadowing;
};

#endif /* MacroCellUrbanAreaChannelRealization_H_ */

