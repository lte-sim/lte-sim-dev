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


#ifndef PROPAGATIONLOSSMODEL_H_
#define PROPAGATIONLOSSMODEL_H_

#include <map>

class TransmittedSignal;
class ChannelRealization;
class NetworkNode;

class PropagationLossModel {
public:
	PropagationLossModel();
	virtual ~PropagationLossModel();

	typedef std::pair<NetworkNode*, NetworkNode* > ChannelRealizationId_t;
	typedef std::map <PropagationLossModel::ChannelRealizationId_t, ChannelRealization* > ChannelRealizationMap;


	void AddChannelRealization (ChannelRealization* chRealization);
	void DelChannelRealization (NetworkNode* src, NetworkNode* dst);

	ChannelRealization* GetChannelRealization (NetworkNode* src, NetworkNode* dst);
	ChannelRealizationMap GetChannelRealizationMap (void);

	TransmittedSignal* AddLossModel (NetworkNode* src,
			                                 NetworkNode* dst,
			                                 TransmittedSignal* txSignal);
private:
	ChannelRealizationMap m_channelRealizationMap;
};

#endif /* PROPAGATIONLOSSMODEL_H_ */
