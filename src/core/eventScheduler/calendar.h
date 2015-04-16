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



#ifndef CALENDAR_H_
#define CALENDAR_H_

#include "event.h"
#include <list>


class Calendar {
public:
	Calendar();
	virtual ~Calendar();

    typedef std::list<Event*> Events;

    Events*
    GetEvents (void);

	void
	InsertEvent (Event *newEvent);
	bool
	IsEmpty (void);
	Event*
	GetEvent (void);
	void
	RemoveEvent (void);


private:
    Events *m_events;
};

#endif /* CALENDAR_H_ */
