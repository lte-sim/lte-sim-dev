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


#ifndef MAC_ENTITY_H
#define MAC_ENTITY_H

#include <list>

class Packet;
class NetworkNode;
class AMCModule;
class HarqManager;

/*
 * This class provides a basic implementation of the MAC
 * layer
 */
class MacEntity
{
public:

  MacEntity (void);
  virtual ~MacEntity (void);

  void Destroy (void);

  void SetDevice (NetworkNode* d);
  NetworkNode* GetDevice ();

  void SetAmcModule (AMCModule* amcModule);
  AMCModule* GetAmcModule (void) const;

  HarqManager* GetHarqManager (void);

private:
  NetworkNode* m_device;
  AMCModule* m_amcModule;
  HarqManager* m_harqmanager;
};


#endif /* MAC_ENTITY_H */
