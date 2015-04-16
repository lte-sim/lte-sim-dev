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


#ifndef CHANNELREALIZATION_H_
#define CHANNELREALIZATION_H_

#include <vector>

class NetworkNode;

class ChannelRealization {
public:
	ChannelRealization();
	virtual ~ChannelRealization();
	void Destroy ();

	void SetSourceNode (NetworkNode* src);
	NetworkNode* GetSourceNode (void);
	void SetDestinationNode (NetworkNode* dst);
	NetworkNode* GetDestinationNode (void);


	void SetLastUpdate (void);
	double GetLastUpdate (void);
	void SetSamplingPeriod (double sp);
	double GetSamplingPeriod (void);

	bool NeedForUpdate (void);
	virtual void UpdateModels (void) = 0;

	virtual std::vector<double> GetLoss () = 0;

	enum ChannelType
	{
	  CHANNEL_TYPE_PED_A,
	  CHANNEL_TYPE_PED_B,
	  CHANNEL_TYPE_VEH_A,
	  CHANNEL_TYPE_VEH_B,
	  CHANNEL_TYPE_JAKES
	};

	void SetChannelType (ChannelType t);
	ChannelType GetChannelType (void);

	typedef std::vector<double> FastFadingForTimeDomain;
	typedef std::vector<FastFadingForTimeDomain> FastFading;

	void UpdateFastFading (void);
	void SetFastFading (FastFading* ff);
	FastFading* GetFastFading (void);

private:
	NetworkNode* m_src;
	NetworkNode* m_dst;

    double m_lastUpdate;
	double m_samplingPeriod;

	ChannelType m_channelType;

	FastFading* m_fastFading;
};

#endif /* CHANNELREALIZATION_H_ */
