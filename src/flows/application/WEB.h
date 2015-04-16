/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)  2012
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
 * Author: Gbolly Aiyetoro <g.aiyetoro@gmail.com>
 */


#ifndef WEB_H_
#define WEB_H_

#include "Application.h"

class WEB : public Application {
public:
	WEB();
	virtual ~WEB();

	void DoStart (void);
	void DoStop (void);

    void ScheduleTransmit (double time);
    void Send (void);

    int GetSize (void) const;


private:
	double m_interval;
	int m_size;
	bool m_stateON;
	double m_stateDuration;
	double m_endState;
};

class Random {
	public:
	Random();
	~Random();

	int Uniform(int a, int b); 						// Generate random variants in Integer between interval (a, b)

	double Uniform(double a, double b);				// Generate double random variants with Uniform distribution

	void ReInit();									// Change the seed of random number generation

	int Binomial(int n, double p);					// Generate random variants in Binomial distribution

	double Exponential(double lamda);				// Generate random variants with Exponential distribution

	int Geometric(double p);						// Generate random variants with Geometric distribution

	double Pareto(double a, double k);				// Generate random variants with Pareto distribution

	double Normal(int n, double Mu, double Theta);	// Generate random variants with Normal distribution

	int Poisson(double lamda);	 					// Generate random variants with Poisson distribution

};


#endif /* WEB_H_ */
