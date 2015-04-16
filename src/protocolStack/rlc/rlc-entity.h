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


#ifndef RLC_ENTITY_H
#define RLC_ENTITY_H

#include <list>
#include "../../load-parameters.h"

class NetworkNode;
class RadioBearerInstance;
class PacketBurst;
class Packet;

/*
 * This class implements the RLC entity (See 3GPP - TS 36.322)
 *
 * Functions of the RLC sub layer are performed by RLC entities.
 * For a RLC entity configured at the eNB, there is a peer RLC entity
 * configured at the UE and vice versa.
 */
class RlcEntity
{
public:

  RlcEntity (void);
  RlcEntity (NetworkNode* d);
  virtual ~RlcEntity (void);

  void Destroy (void);

  enum RlcMode
  {
    TM_RLC_MODE, UM_RLC_MODE, AM_RLC_MODE
  };

  void SetDevice (NetworkNode* d);
  NetworkNode* GetDevice (void);

  void SetRadioBearer (RadioBearerInstance *b);
  RadioBearerInstance* GetRadioBearerInstance (void);

  virtual PacketBurst* TransmissionProcedure (int availableBytes) = 0;
  virtual void ReceptionProcedure (Packet* p) = 0;

  void SetRlcEntityIndex (int i);
  int GetRlcEntityIndex (void);

  void SetRlcPduSequenceNumber (int sn);
  int GetRlcPduSequenceNumber (void);

  void SetRlcMode (RlcMode mode);
  RlcMode GetRlcModel (void);

private:
  RlcMode m_rlcMode;
  NetworkNode* m_device;
  RadioBearerInstance *m_bearer;

  int m_rlcEntityIndex;
  int m_rlcPduSequenceNumber;
};


#endif /* RLC_ENTITY_H */
