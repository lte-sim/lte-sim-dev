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


#ifndef IDEAL_CONTROL_MESSAGES_H
#define IDEAL_CONTROL_MESSAGES_H

#include <list>

class NetworkNode;


/*
 * The IdealControlMessage provides a basic implementations for
 * control messages (such as PDCCH allocation map, CQI feedbacks)
 * that are exchanged among eNodeB and UEs.
 */
class IdealControlMessage
{
public:
  enum MessageType
  {
    CQI_FEEDBACKS, ALLOCATION_MAP,
    ARQ_RLC_ACK,
    SCHEDULING_REQUEST
  };

  IdealControlMessage (void);
  virtual ~IdealControlMessage (void);

  void SetSourceDevice (NetworkNode* src);
  void SetDestinationDevice (NetworkNode* dst);

  NetworkNode* GetSourceDevice (void);
  NetworkNode* GetDestinationDevice (void);

  void SetMessageType (MessageType type);
  MessageType GetMessageType (void);

private:
  NetworkNode* m_source;
  NetworkNode* m_destination;
  MessageType m_type;
};

#endif /* IDEAL_CONTROL_MESSAGES_H */

// ----------------------------------------------------------------------------------------------------------


#ifndef PDCCH_MAP_IDEAL_CONTROL_MESSAGES_H
#define PDCCH_MAP_IDEAL_CONTROL_MESSAGES_H

#include <list>

class NetworkNode;

/*
 * The PdcchMapIdealControlMessage defines an ideal allocation map
 * for both UL and DL sends by the eNodeB to all UE,
 * using an ideal PDCCH control channel.
 * IdealPdcchMessage is composed by a list of IdealPdcchRecord
 * where is indicated the UE that can use a particular sub channel
 * with a proper MCS scheme.
 *
 * This records are the same for both UL and DL, and are created by the
 * packet scheduler at the beginning of each sub frame.
 *
 * When the IdealPdcchMessage is sent under an ideal control channel,
 * all UE stores into a proper variables the informations about
 * the resource mapping.
 */
class PdcchMapIdealControlMessage : public IdealControlMessage
{
public:

  PdcchMapIdealControlMessage (void);
  virtual ~PdcchMapIdealControlMessage (void);

  enum Direction
  {
    DOWNLINK, UPLINK
  };

  struct IdealPdcchRecord
  {
    Direction m_direction;
    int m_idSubChannel;
    NetworkNode* m_ue;
    double m_mcsIndex;
  };

  typedef std::list<struct IdealPdcchRecord>  IdealPdcchMessage;

  void AddNewRecord (Direction direction,
                     int subChannel, NetworkNode* ue, double mcs);


  IdealPdcchMessage* GetMessage (void);

private:
  IdealPdcchMessage *m_idealPdcchMessage;
};

#endif /* PDCCH_MAP_IDEAL_CONTROL_MESSAGES_H */



// ----------------------------------------------------------------------------------------------------------



#ifndef CQI_IDEAL_CONTROL_MESSAGES_H
#define CQI_IDEAL_CONTROL_MESSAGES_H

#include <list>


class NetworkNode;

/*
 * The CqiIdealControlMessage defines an ideal list of feedback about
 * the channel quality sent by the UE to the eNodeB.
 */
class CqiIdealControlMessage : public IdealControlMessage
{
public:

  CqiIdealControlMessage (void);
  virtual ~CqiIdealControlMessage (void);


  struct CqiFeedback
  {
    int m_idSubChannel;
    double m_cqi;
  };

  typedef std::list<struct CqiFeedback>  CqiFeedbacks;

  void AddNewRecord (int subChannel, double cqi);
  CqiFeedbacks* GetMessage (void);


private:
  CqiFeedbacks *m_cqiFeedbacks;
};


#endif /* CQI_IDEAL_CONTROL_MESSAGES_H */


// ----------------------------------------------------------------------------------------------------------



#ifndef ARQ_IDEAL_CONTROL_MESSAGES_H
#define ARQ_IDEAL_CONTROL_MESSAGES_H

#include <list>

class NetworkNode;

/*
 *
 */
class ArqRlcIdealControlMessage : public IdealControlMessage
{
public:

  ArqRlcIdealControlMessage (void);
  virtual ~ArqRlcIdealControlMessage (void);

  void SetAck (int ack);
  int GetAck (void);

  void SetStartByte (int b);
  void SetEndByte (int b);
  int GetStartByte (void);
  int GetEndByte (void);

private:
  int m_ack;
  int m_startByte;
  int m_endByte;
};

#endif /* ARQ_IDEAL_CONTROL_MESSAGES_H */



// ----------------------------------------------------------------------------------------------------------



#ifndef SCHEDULING_REQUEST_IDEAL_CONTROL_MESSAGES_H
#define SCHEDULING_REQUEST_IDEAL_CONTROL_MESSAGES_H

#include <list>

class NetworkNode;

/*
 *
 */
class SchedulingRequestIdealControlMessage : public IdealControlMessage
{
public:

  SchedulingRequestIdealControlMessage (void);
  virtual ~SchedulingRequestIdealControlMessage (void);

  int GetBufferStatusReport (void);
  void SetBufferStatusReport (int b);

private:
  int m_bufferStatusReport;
};

#endif /* SCHEDULING_REQUEST_IDEAL_CONTROL_MESSAGES_H */



