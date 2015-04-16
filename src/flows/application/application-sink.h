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


#ifndef APPLICATIONSINK_H_
#define APPLICATIONSINK_H_

#include <iostream>
#include <stdint.h>
#include "Application.h"

class ClassifierParameters;
class RadioBearer;
class RadioBearerSink;
class Packet;


class ApplicationSink {
public:
	ApplicationSink();
	virtual ~ApplicationSink();

	void SetClassifierParameters (ClassifierParameters* cp);
	ClassifierParameters* GetClassifierParameters (void);

	void SetRadioBearerSink (RadioBearerSink* r);
	RadioBearerSink* GetRadioBearerSink (void);

	void SetSourceApplication (Application* a);
	Application* GetSourceApplication (void);

	void Receive (Packet* p);

private:
	ClassifierParameters* m_classifierParameters;
	RadioBearerSink* m_radioBearer;

	Application* m_sourceApplication;
};


#endif /* APPLICATIONSINK_H_ */
