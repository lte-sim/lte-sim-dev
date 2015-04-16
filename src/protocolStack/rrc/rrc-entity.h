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


#ifndef RRC_ENTITY_H
#define RRC_ENTITY_H

#include <list>
#include <vector>

class ClassifierParameters;
class RadioBearer;
class RadioBearerSink;
class RadioBearerInstance;
class NetworkNode;
class HandoverEntity;

/*
 * This class implements the RRC entity
 */
class RrcEntity
{
public:

  RrcEntity (void);
  virtual ~RrcEntity (void);

  void SetDevice (NetworkNode* d);
  NetworkNode* GetDevice ();

  typedef std::vector<RadioBearer* > RadioBearersContainer;
  typedef std::vector<RadioBearerSink* > RadioBearersSinkContainer;


  RadioBearersContainer* GetRadioBearerContainer (void);
  RadioBearersSinkContainer* GetRadioBearerSinkContainer (void);


  void AddRadioBearer (RadioBearer* bearer);
  void DelRadioBearer (RadioBearer* bearer);


  void AddRadioBearerSink (RadioBearerSink* bearer);
  void DelRadioBearerSink (RadioBearerSink* bearer);


  RadioBearer* GetRadioBearer (ClassifierParameters* ipc);
  RadioBearerSink* GetRadioBearerSink (ClassifierParameters* ipc);
  RadioBearerInstance* GetRadioBearer (int rlcIndex);


  void SetHandoverEntity (HandoverEntity* h);
  HandoverEntity* GetHandoverEntity (void);

private:
  RadioBearersContainer* m_bearers;
  RadioBearersSinkContainer* m_sink;
  NetworkNode* m_device;
  HandoverEntity* m_handover;
};


#endif /* RRC_ENTITY_H */
