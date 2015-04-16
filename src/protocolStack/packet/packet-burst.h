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


#ifndef PACKET_BURST_H
#define PACKET_BURST_H

#include <stdint.h>
#include <list>
#include "Packet.h"


class PacketBurst
{
  /*
   * This class implement a burst as a list of packets
   */

public:
  PacketBurst (void);
  virtual ~PacketBurst (void);

  PacketBurst* Copy (void);
  void AddPacket (Packet* packet);
  std::list<Packet* > GetPackets (void) const;
  uint32_t GetNPackets (void) const;
  uint32_t GetSize (void) const;

  std::list<Packet* >::const_iterator Begin (void) const;
  std::list<Packet* >::const_iterator End (void) const;

private:
  std::list<Packet* > m_packets;
};


#endif /* PACKET_BURST */
