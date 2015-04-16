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


#ifndef BandwidthManager_H_
#define BandwidthManager_H_

#include <vector>

/*
 * This class models the bandwidth used for the transmission
 * in a particular cell.
 * See information on TR 36.101 - Table 5.5-1
 */
class BandwidthManager {
public:
	BandwidthManager();
	BandwidthManager(double ulBw, double dlBw, int ulOffset, int dlOffset);
	BandwidthManager(double ulBw, double dlBw, int ulOffset, int dlOffset, bool tddTrue);
	virtual ~BandwidthManager();

	void SetDlSubChannels (std::vector<double> s);
	std::vector<double> GetDlSubChannels (void);

	void SetUlSubChannels (std::vector<double> s);
	std::vector<double> GetUlSubChannels (void);


	void SetOperativeSubBand (int s);
	int GetOperativeSubBand (void);

	void SetUlBandwidth (double b);
	void SetDlBandwidth (double b);
	void SetUlOffsetBw (int o);
	void SetDlOffsetBw (int o);

	double GetUlBandwidth (void);
	double GetDlBandwidth (void);
	int GetUlOffsetBw (void);
	int GetDlOffsetBw (void);


	BandwidthManager* Copy ();

	void Print (void);

private:
	std::vector<double> m_dlSubChannels;
	std::vector<double> m_ulSubChannels;

	int m_operativeSubBand;

	double m_ulBandwidth;
	double m_dlBandwidth;

	int m_ulOffsetBw;
	int m_dlOffsetBw;
};

#endif /* BandwidthManager_H_ */
