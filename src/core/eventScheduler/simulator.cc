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



#include "simulator.h"
#include "make-event.h"
#include "../../componentManagers/FrameManager.h"

#include <math.h>
#include <fstream>
#include <list>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

Simulator* Simulator::ptr=NULL;

Simulator::Simulator ()
{
  m_stop = false;
  m_currentUid = 0;
  m_currentTs = 0;
  m_unscheduledEvents = 0;
  m_calendar = new Calendar;
  m_uid = 0;
}

Simulator::~Simulator ()
{
  while (!m_calendar->IsEmpty ())
    {
	  m_calendar->RemoveEvent ();
    }
  delete m_calendar;
}

double
Simulator::Now (void)
{
  return m_currentTs;
}


void 
Simulator::Run (void)
{
  /*
   * This method start the whole simulation
   * The simulation will end when no events are into the
   * calendar list.
   */
  m_stop = false;
  while (!m_calendar->IsEmpty () && !m_stop)
    {
      ProcessOneEvent ();
    }
}

void
Simulator::ProcessOneEvent(void)
{
  Event *next = m_calendar->GetEvent();

  --m_unscheduledEvents;
  m_currentTs = next->GetTimeStamp();
  m_currentUid = next->GetUID();

  next->RunEvent();

  m_calendar->RemoveEvent();
}

int
Simulator::GetUID (void)
{
  m_uid++;
  return (m_uid-1);
}

void 
Simulator::Stop (void)
{
  std::cout << " SIMULATOR_DEBUG: Stop ()"
      << std::endl;
  m_stop = true;
}

void 
Simulator::SetStop (double time)
{
  DoSchedule (time,
			  MakeEvent (&Simulator::Stop, this));
}


void
Simulator::DoSchedule (double time,
					   Event *event)
{
  double timeStamp = time + Now();
  event->SetTimeStamp(timeStamp);

  ++m_unscheduledEvents;

  m_calendar->InsertEvent(event);
}

void
Simulator::PrintMemoryUsage (void)
{
  system("pmap `ps aux | grep LTE  | grep -v grep | awk '{print $2}'` | grep total");
}
