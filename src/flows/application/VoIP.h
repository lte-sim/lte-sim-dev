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


#ifndef VOIP_H_
#define VOIP_H_

#include "Application.h"

class VoIP : public Application {
public:
	VoIP();
	virtual ~VoIP();

	void DoStart (void);
	void DoStop (void);

    void ScheduleTransmit (double time);
    void Send (void);

    int GetSize (void) const;


private:
	double m_interval;
	int m_size;
	bool m_stateON;
	double m_stateDuration;
	double m_endState;
};

#endif /* VOIP_H_ */
