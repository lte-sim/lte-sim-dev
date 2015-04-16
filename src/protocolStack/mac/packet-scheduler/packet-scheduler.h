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


#ifndef PACKETSCHEDULER_H_
#define PACKETSCHEDULER_H_

#include <vector>
#include "../../../core/idealMessages/ideal-control-messages.h"

class MacEntity;
class PacketBurst;
class Packet;
class RadioBearer;

class PacketScheduler {
public:
	PacketScheduler();
	virtual ~PacketScheduler();

	void Destroy (void);

	void SetMacEntity (MacEntity* mac);
	MacEntity* GetMacEntity (void);

	void Schedule (void);
	virtual void DoSchedule (void);

	void StopSchedule ();
	virtual void DoStopSchedule ();

	struct FlowToSchedule
	  {
	    FlowToSchedule(RadioBearer* bearer,
		  		       int dataToTransmit);
	    virtual ~FlowToSchedule();
	    RadioBearer* m_bearer;
	    int m_allocatedBits;		//bits
	    int m_transmittedData;	//bytes
	    int m_dataToTransmit;		//bytes

	    std::vector<double> m_spectralEfficiency;
	    std::vector<int> m_listOfAllocatedRBs;
	    std::vector<int> m_listOfSelectedMCS;
	    std::vector<int> m_cqiFeedbacks;

		RadioBearer* GetBearer (void);

		void UpdateAllocatedBits (int allocatedBits);
		int GetAllocatedBits (void) const;
		int GetTransmittedData (void) const;
		void SetDataToTransmit (int dataToTransmit);
		int GetDataToTransmit (void) const;

		void SetSpectralEfficiency (std::vector<double> s);
		std::vector<double> GetSpectralEfficiency (void);

		std::vector<int>* GetListOfAllocatedRBs ();
		std::vector<int>* GetListOfSelectedMCS ();

		void SetCqiFeedbacks (std::vector<int> cqiFeedbacks);
		std::vector<int> GetCqiFeedbacks (void);
	  };

	typedef std::vector<FlowToSchedule*> FlowsToSchedule;

	void CreateFlowsToSchedule (void);
	void DeleteFlowsToSchedule (void);
	void ClearFlowsToSchedule ();

	FlowsToSchedule* GetFlowsToSchedule (void) const;

	void InsertFlowToSchedule (RadioBearer* bearer,
						       int dataToTransmit,
						       std::vector<double> specEff,
						       std::vector<int> cqiFeedbacks);

	void UpdateAllocatedBits (FlowToSchedule* scheduledFlow,
						      int allocatedBits,
						      int allocatedRB,
						      int selectedMCS);

	void CheckForDLDropPackets();


private:
	MacEntity *m_mac;
	FlowsToSchedule *m_flowsToSchedule;
};

#endif /* PACKETSCHEDULER_H_ */
