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

#ifndef CQIMANAGER_H_
#define CQIMANAGER_H_

#include <vector>

class NetworkNode;

class CqiManager {
public:

    enum CQIReportingMode
      {
        PERIODIC,
        APERIODIC
      };

	CqiManager();
	virtual ~CqiManager();

	void SetDevice (NetworkNode* d);
	NetworkNode* GetDevice (void);

	void SetCqiReportingMode (CQIReportingMode m);
	CQIReportingMode GetCqiReportingMode (void);

	void SetSendCqi (bool b);
	bool GetSendCqi (void);

	void SetReportingInterval (int i);
	int GetReportingInterval (void);

	void SetLastSent ();
	long int GetLastSent (void);

	virtual void CreateCqiFeedbacks (std::vector<double> sinr) = 0;

	bool NeedToSendFeedbacks (void);

private:
	CQIReportingMode m_reportingMode;

	bool m_sendCqi; //Used with aperiodic reporting. It is set to true by the eNB !

	int m_reportingInterval;
	long int m_lastSent;

	NetworkNode* m_device;

};

#endif /* CQIMANAGER_H_ */
