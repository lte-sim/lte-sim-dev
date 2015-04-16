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

#include "test-attach-to-the-channel.h"
#include "test-start-application.h"
#include "test-enqueue-packet.h"
#include "test-dl-transmission.h"
#include "test-downlink-proportiona-fair.h"
#include "test-propagation-loss-model.h"
#include "test-cqi-feedbacks.h"
#include "test-multi-cell.h"
#include "test-frequency-reuse.h"
#include "test-mobility-models.h"
#include "test-amc-mapping.h"
#include "test-uplink-maximumthroughput.h"

#include <iostream>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <cstring>

static void Test ()
{

	/*
	TestSendPacketToTheChannel ();
	TestStartApplication ();
	TestEnqueuePackets ();
	TestDlTransmission ();
	TestPropagationLossModel ();
	TestCqiFeedbacks ();
	TestMultiCell ();
	TestFrequencyReuse ();
    */

	TestUplinkMaximumThroughput ();
}
