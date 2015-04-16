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

#ifndef HEADER_H_
#define HEADER_H_

#include "../protocols/TransportProtocol.h"

class Header {
public:
	enum ProtocolHeader
	  {
	    HEADER_UDP,
	    HEADER_TCP,
	    HEADER_IP,
	    HEADER_PDCP,
	    HEADER_RLC,
	    HEADER_MAC
	  };

	Header();
	virtual ~Header();

	void
	SetProtocolHeader (ProtocolHeader protocolHeader);
	ProtocolHeader
	GetProtocolHeader (void) const;

	void
	SetHeaderSize (int size);
	int
	GetHeaderSize (void) const;

private:
	int m_size;
	ProtocolHeader m_protocolHeader;
};


/*
 * TRANSPORT PROTOCOL HEADER
 */
class TransportHeader : public Header{
public:
	TransportHeader();
	virtual ~TransportHeader();

	void
	SetSourcePort (int port);
	int
	GetSourcePort (void) const;
	void
	SetDestinationPort (int port);
	int
	GetDestinationPort (void) const;

	void
	SetProtocolType (TransportProtocol::TransportProtocolType protocol);
	TransportProtocol::TransportProtocolType
	GetProtocolType (void) const;

private:
	int m_sourcePort;
	int m_destinationPort;
	TransportProtocol::TransportProtocolType m_protocolType;
};

/*
 * UDP HEADER
 */
class UDPHeader : public TransportHeader {
public:
	UDPHeader();
	UDPHeader(int sourcePort, int destinationPort);
	virtual ~UDPHeader();
};



/*
 * IP HEADER
 */
class IPHeader : public Header {
public:
	IPHeader();
	IPHeader(int sourceID, int destinationID);
	virtual ~IPHeader();

	void
	SetSourceID (int id);
	int
	GetSourceID (void) const;
	void
	SetDestinationID (int id);
	int
	GetDestinationID (void) const;

private:
	int m_sourceID;
	int m_destinationID;
};


/*
 * PDCP HEADER
 */
class PDCPHeader : public Header {
public:
	PDCPHeader();
	virtual ~PDCPHeader();
};



/*
 * RLC HEADER
 */
class RLCHeader : public Header {
public:
	RLCHeader();
	virtual ~RLCHeader();

	bool IsTheLatestFragment ();
	bool IsAFragment ();
	void SetTheLatestFragment (bool flag);
	void SetAFragment (bool flag);

	void SetFragmentNumber (int n);
	int GetFragmentNumber (void);

	void SetRlcEntityIndex (int i);
	int GetRlcEntityIndex (void);

	void SetRlcPduSequenceNumber (int i);
	int GetRlcPduSequenceNumber (void);

	void SetStartByte (int b);
	void SetEndByte (int b);
	int GetStartByte (void);
	int GetEndByte (void);

private:
	bool m_isAFragment;
	bool m_isTheLatestFragment; //Describes that there is a fragment of packet
    int m_fragmentNumber;
    int m_rlcEntityIndex;

    int m_rlcPduSequenceNumber;

    int m_startByte;
    int m_endByte;
};


/*
 * MAC HEADER
 */
class MACHeader : public Header {
public:
	MACHeader();
	MACHeader (int sourceMAC, int destinationMAC);
	virtual ~MACHeader();

	/*
	 * Actually the MAC address
	 * is the ID of the network element.
	 */
	void
	SetMACsourceID (int id);
	int
	GetMACsourceID (void) const;
	void
	SetMACdestinationID (int id);
	int
	GetMACdestinationID (void) const;

private:
	int m_macSourceID;
	int m_macDestinationID;
};



#endif /* HEADER_H_ */
