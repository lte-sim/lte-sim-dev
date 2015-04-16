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




#include "calendar.h"

#include <iostream>

Calendar::Calendar()
{
  m_events = new Events;
}

Calendar::~Calendar()
{
  delete m_events;
}

Calendar::Events*
Calendar::GetEvents (void)
{
  return m_events;
}

void
Calendar::InsertEvent (Event *newEvent)
{
  Events *events = GetEvents ();
  Event *event;
  Events::iterator iter;

  if (IsEmpty ())
    {
	  events->push_front(newEvent);
	  return;
    }

  for (iter = events->begin(); iter != events->end(); iter++)
	{
	  event = *iter;
	  if(newEvent->GetTimeStamp() < event->GetTimeStamp())
	    {
		  m_events->insert(iter, newEvent);
		  return;
	    }
	}

  m_events->push_back(newEvent);;
}

bool
Calendar::IsEmpty (void)
{
  return GetEvents ()->empty();
}

Event*
Calendar::GetEvent (void)
{
  if (IsEmpty ())
	return NULL;

  Event *event = GetEvents ()->front ();
  return event;
}

void
Calendar::RemoveEvent (void)
{
  if (!IsEmpty ())
    {
	  Event *event = GetEvents ()->front();
	  GetEvents ()->pop_front ();
	  delete event;
    }
}


