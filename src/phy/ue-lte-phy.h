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


#ifndef UE_LTE_PHY_H_
#define UE_LTE_PHY_H_

#include "lte-phy.h"
#include <vector>

class IdealControlMessage;

class UeLtePhy :public LtePhy {
public:
	UeLtePhy();
	virtual ~UeLtePhy();

	virtual void DoSetBandwidthManager (void);

	virtual void StartTx (PacketBurst* p);
	virtual void StartRx (PacketBurst* p, TransmittedSignal* txSignal);

	void CreateCqiFeedbacks (std::vector<double> sinr);

	virtual void SendIdealControlMessage (IdealControlMessage *msg);
	virtual void ReceiveIdealControlMessage (IdealControlMessage *msg);

	void SendReferenceSymbols (void);
	void SetTxSignalForReferenceSymbols (void);
	TransmittedSignal* GetTxSignalForReferenceSymbols (void);

private:
	std::vector<double> m_measuredSinr;
	TransmittedSignal* m_txSignalForRerferenceSymbols;

	std::vector<int> m_channelsForTx;
	std::vector<int> m_mcsIndexForTx;

	std::vector<int> m_channelsForRx;
	std::vector<int> m_mcsIndexForRx;
};

#endif /* UE_LTE_PHY_H_ */
