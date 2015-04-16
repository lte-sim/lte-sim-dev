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




#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "event.h"
#include "calendar.h"
#include "make-event.h"

#include <stdint.h>
#include <string>
#include <iostream>


/*
 * Simulator
 * Control the scheduling of simulation events.
 */

class Simulator 
{
private:
	Simulator ();
	static Simulator *ptr;

	Calendar *m_calendar;
	bool m_stop;
	int m_currentUid;
	double m_currentTs;
	int m_unscheduledEvents;

	int m_uid;

	void ProcessOneEvent (void);

public:
	virtual ~Simulator ();

	static Simulator*
	Init (void)
	  {
		if (ptr==NULL)
	      {
		    ptr = new Simulator;
	   	  }
		return ptr;
	  }

	double Now (void);

	void Run (void);
	void Stop (void);
	void SetStop (double time);

	int GetUID (void);

	void
	DoSchedule (double time, Event *event);


	/*
	 * Schedule methods are called to insert a new method
	 * into the calendar scheduler
	 */
	template <typename MEM, typename OBJ>
	void
	Schedule (double time, MEM mem_ptr, OBJ obj);

	template <typename MEM, typename OBJ, typename T1>
	void
	Schedule (double time, MEM mem_ptr, OBJ obj, T1 a1);

	template <typename MEM, typename OBJ, typename T1, typename T2>
	void
	Schedule (double time, MEM mem_ptr, OBJ obj, T1 a1, T2 a2);

	template <typename MEM, typename OBJ, typename T1, typename T2, typename T3>
	void
	Schedule (double time, MEM mem_ptr, OBJ obj, T1 a1, T2 a2, T3 a3);

	void
	Schedule (double time, void (*f) (void));

	template <typename U1>
	void
	Schedule (double time, void (*f) (U1));

	template <typename U1, typename T1>
	void
	Schedule (double time, void (*f) (U1), T1 a1);

	void
	PrintMemoryUsage (void);
};


template <typename MEM, typename OBJ>
void
Simulator::Schedule (double time, MEM mem_ptr, OBJ obj)
{
  DoSchedule (time, MakeEvent (mem_ptr, obj));
}

template <typename MEM, typename OBJ, typename T1>
void
Simulator::Schedule (double time, MEM mem_ptr, OBJ obj, T1 a1)
{
  DoSchedule (time, MakeEvent(mem_ptr, obj, a1));
}

template <typename MEM, typename OBJ, typename T1, typename T2>
void
Simulator::Schedule (double time, MEM mem_ptr, OBJ obj, T1 a1, T2 a2)
{
  DoSchedule (time, MakeEvent(mem_ptr, obj, a1, a2));
}

template <typename MEM, typename OBJ, typename T1, typename T2, typename T3>
void
Simulator::Schedule (double time, MEM mem_ptr, OBJ obj, T1 a1, T2 a2, T3 a3)
{
  DoSchedule (time, MakeEvent(mem_ptr, obj, a1, a2, a3));
}

/*
void
Simulator::Schedule (double time, void (*f) (void))
{
	//NON FUNZIONA ANCORA !?!?!?!
  //DoSchedule (time, MakeEvent (f));
}
*/

template <typename U1, typename T1>
void
Simulator::Schedule (double time, void (*f) (U1), T1 a1)
{
  DoSchedule (time, MakeEvent(f, a1));
}




#endif /* SIMULATOR_H */


