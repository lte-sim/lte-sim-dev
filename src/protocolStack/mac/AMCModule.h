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



#ifndef AMCModule_H_
#define AMCModule_H_

#include <vector>
/*
 *  Adaptive Modulation And Coding Scheme
 */

class AMCModule {
public:
	AMCModule();
	virtual ~AMCModule();

	int
	GetCQIFromEfficiency (double sinr);
	int
	GetMCSIndexFromEfficiency(double spectralEfficiency);
	int
	GetMCSFromCQI (int cqi);
	int
	GetCQIFromMCS (int mcs);
	int
	GetTBSizeFromMCS (int mcs);
	int
	GetTBSizeFromMCS (int mcs, int nbRBs);
	double
	GetEfficiencyFromCQI (int cqi);
	int
    GetCQIFromSinr (double sinr);
	double
	GetSinrFromCQI (int cqi);

    std::vector<int> CreateCqiFeedbacks (std::vector<double> sinr);
};

#endif /* AMCModule_H_ */
