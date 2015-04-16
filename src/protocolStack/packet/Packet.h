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

#ifndef PACKET_H_
#define PACKET_H_

#include <list>
#include "Header.h"
#include "PacketTAGs.h"

class Packet {
public:
	Packet();
	virtual ~Packet();

	/*
	 * The time stamp represents the time instant in which
	 * the packet is generated. It cannot be modified during the
	 * simulation. Moreover, it will be used to compute the
	 * transmission delay.
	 */
	void
	SetTimeStamp (double time);
	double
	GetTimeStamp (void) const;

	/*
	 * m_size represents the size of packet,
	 * including also the headers overhead.
	 * Is is updated every time an header is added or
	 * deleted
	 */
	void
	SetSize (int size);
	void
	AddHeaderSize (int size);
	int
	GetSize (void) const;
	void
	UpdatePacketSize (void);


	void
	AddUDPHeader (UDPHeader *header);
    UDPHeader*
    GetUDPHeader (void) const;

	void
	AddIPHeader (IPHeader *header);
    IPHeader*
    GetIPHeader (void) ;

	void
	AddPDCPHeader (PDCPHeader *header);
    PDCPHeader*
    GetPDCPHeader (void) const;

	void
	AddRLCHeader (RLCHeader *header);
    RLCHeader*
    GetRLCHeader (void) const;

	void
	AddMACHeader (MACHeader *header);
    MACHeader*
    GetMACHeader (void) const;


	int
	GetSourceID ();
	int
	GetDestinationID ();
	int
	GetSourcePort ();
	int
	GetDestinationPort ();
	int
	GetSourceMAC ();
	int
	GetDestinationMAC ();

	void
	SetID (int uid);
	int
	GetID (void);

	PacketTAGs*
	GetPacketTags (void) const;
	void
	SetPacketTags (PacketTAGs* tags);

	Packet*
	Copy (void);

	void
	Print ();

private:
	double m_timeStamp;
	int m_size;

	int m_id;

	UDPHeader *m_UDPHeader;
	IPHeader *m_IPHeader;
	PDCPHeader *m_PDCPHeader;
	RLCHeader *m_RLCHeader;
	MACHeader *m_MACHeader;

	PacketTAGs *m_tags;
};

#endif /* PACKET_H_ */
