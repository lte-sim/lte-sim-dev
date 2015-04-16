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


#ifndef RADIO_BEARER_INSTANCE_H_
#define RADIO_BEARER_INSTANCE_H_

class NetworkNode;
class ClassifierParameters;
class Application;
class MacQueue;
class QoSParameters;
class Packet;
class RlcEntity;

class RadioBearerInstance {
public:
	RadioBearerInstance();
	virtual ~RadioBearerInstance();

	void Destory (void);

	void SetSource (NetworkNode* src);
	void SetDestination (NetworkNode* dst);
	NetworkNode* GetSource (void);
	NetworkNode* GetDestination (void);

	void SetRlcEntity (RlcEntity *rlc);
	RlcEntity* GetRlcEntity (void);

	void SetClassifierParameters (ClassifierParameters* cp);
	ClassifierParameters* GetClassifierParameters (void);

	void SetQoSParameters (QoSParameters *parameters);
	QoSParameters* GetQoSParameters (void) const;

	enum BearerQoSType
	  {
		BEARER_TYPE_GBR, BEARER_TYPE_NGBR
	  };

	void SetBearerQoSType (BearerQoSType bearerQoSType);
	BearerQoSType GetBearerQoSType (void) const;

private:
	NetworkNode* m_src;
	NetworkNode* m_dst;

	ClassifierParameters* m_classifierParameters;

	RlcEntity *m_rlc;

	//QoS
	QoSParameters *m_qosParameters;
	BearerQoSType m_type;

};

#endif /* RADIO_BEARER_INSTANCE_H_ */
