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


#ifndef LTE_PHY_H_
#define LTE_PHY_H_

class NetworkNode;
class LteChannel;
class BandwidthManager;
class PacketBurst;
class TransmittedSignal;
class IdealControlMessage;
class Interference;
class ErrorModel;

class LtePhy {
public:
	LtePhy();
	virtual ~LtePhy();

	void Destroy (void);

	virtual void StartTx (PacketBurst* p) = 0;
	virtual void StartRx (PacketBurst* p, TransmittedSignal* txSignal) = 0;

	void SetDevice (NetworkNode* d);
	NetworkNode* GetDevice (void);

	void SetDlChannel (LteChannel* c);
	LteChannel* GetDlChannel (void);
	void SetUlChannel (LteChannel* c);
	LteChannel* GetUlChannel (void);

	void SetBandwidthManager (BandwidthManager* s);
	BandwidthManager* GetBandwidthManager (void);
	virtual void DoSetBandwidthManager (void) = 0;

	void SetTxPower (double p);
	double GetTxPower (void);

	void SetTxSignal (TransmittedSignal* txSignal);
	TransmittedSignal* GetTxSignal (void);

	virtual void SendIdealControlMessage (IdealControlMessage *msg) = 0;
	virtual void ReceiveIdealControlMessage (IdealControlMessage *msg) = 0;

	void SetInterference (Interference* interference);
	void SetErrorModel (ErrorModel* e);

	Interference* GetInterference (void);
	ErrorModel* GetErrorModel (void);

private:
    NetworkNode* m_device;
    LteChannel* m_dlChannel;
    LteChannel* m_ulChannel;

    BandwidthManager* m_bandwidthManager; //Description of the UL and DL available BandwidthManager

    double m_txPower;
    TransmittedSignal* m_txSignal;

    Interference *m_interference;
    ErrorModel *m_errorModel;
};

#endif /* LTE_PHY_H_ */
