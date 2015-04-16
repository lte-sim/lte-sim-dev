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


#ifndef UMRLCENTITY_H_
#define UMRLCENTITY_H_

#include "rlc-entity.h"
#include <vector>

class PacketBurst;

class UmRlcEntity: public RlcEntity {
public:
	UmRlcEntity();
	virtual ~UmRlcEntity();

	virtual PacketBurst* TransmissionProcedure (int availableBytes);
	virtual void ReceptionProcedure (Packet* p);

	void ClearIncomingPackets (void);

private:
	std::vector <Packet*> m_incomingPacket;
};

#endif /* UMRLCENTITY_H_ */
