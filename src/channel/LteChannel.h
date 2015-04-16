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


#ifndef LTECHANNEL_H_
#define LTECHANNEL_H_

#include <vector>

class NetworkNode;
class PacketBurst;
class TransmittedSignal;
class PropagationLossModel;

class LteChannel {
public:
	LteChannel();
	virtual ~LteChannel();

    void StartTx (PacketBurst* p, TransmittedSignal* txSignal, NetworkNode* src);
    void StartRx (PacketBurst* p, TransmittedSignal* psd, NetworkNode* src);

    void AddDevice (NetworkNode* d);
    void DelDevice (NetworkNode* d);
    bool IsAttached (NetworkNode* d);

    std::vector<NetworkNode*>* GetDevices (void);

    void SetPropagationLossModel (PropagationLossModel* m);
    PropagationLossModel* GetPropagationLossModel (void);

    void SetChannelId (int id);
    int GetChannelId (void);

private:
    std::vector<NetworkNode*> *m_attachedDevices; //list of devices attached to the channel

    PropagationLossModel* m_propagationLossModel;

    int m_channelId;
};

#endif /* LTECHANNEL_H_ */
