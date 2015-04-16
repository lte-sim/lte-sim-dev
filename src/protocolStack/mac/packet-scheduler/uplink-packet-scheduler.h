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


#ifndef UPLINKPACKETSCHEDULER_H_
#define UPLINKPACKETSCHEDULER_H_

#include "packet-scheduler.h"

class UplinkPacketScheduler: public PacketScheduler {
public:
	UplinkPacketScheduler();
	virtual ~UplinkPacketScheduler();

	struct UserToSchedule
	  {
	    NetworkNode* m_userToSchedule;
	    int m_transmittedData;	//bytes
	    int m_dataToTransmit;		//bytes
	    double m_averageSchedulingGrant; // in bytes

	    std::vector<int> m_listOfAllocatedRBs;
	    int m_selectedMCS;
	    std::vector<int> m_channelContition; //similar to the CQI for the DL
	  };
	typedef std::vector<UserToSchedule*> UsersToSchedule;
	void CreateUsersToSchedule (void);
	void DeleteUsersToSchedule (void);
	void ClearUsersToSchedule ();
	UsersToSchedule* GetUsersToSchedule (void);

	void SelectUsersToSchedule (void);

	virtual void DoSchedule (void);
	virtual void DoStopSchedule (void);

	virtual void RBsAllocation ();
	virtual double ComputeSchedulingMetric (RadioBearer *bearer, double spectralEfficiency, int subChannel) = 0;
    virtual double ComputeSchedulingMetric (UserToSchedule* user, int subchannel) = 0;

private:
    UsersToSchedule* m_usersToSchedule;
};

#endif /* UPLINKPACKETSCHEDULER_H_ */
