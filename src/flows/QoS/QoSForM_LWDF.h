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


#ifndef QOSFORM_LWDF_H_
#define QOSFORM_LWDF_H_

#include "QoSParameters.h"

class QoSForM_LWDF :public QoSParameters {

public:
	QoSForM_LWDF ();
	QoSForM_LWDF(double targetDelay);
	virtual ~QoSForM_LWDF();

	void
	SetDropPorbability (double probability);
	double
	GetDropProbability (void) const;

	//Debug
	void
	Print ();

private:
	double m_dropProbability;
};

#endif /* QOSFORM_LWDF_H_ */
