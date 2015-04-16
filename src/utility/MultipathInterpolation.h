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
 * Author: Francesco Capozzi <f.capozzi@poliba.it>
 */


#ifndef MULTIPATHINTERPOLATION_H_
#define MULTIPATHINTERPOLATION_H_

double
LagrangeInterpolatingPolynomial (int degree, double *posVect, double *valVect, double desiredPos)
{
  double retVal = 0.0;

  for (int i = 0; i < degree; i++)
    {
	  double weight = 1;

	  for (int j = 0; j < degree; j++)
	    {
		  // The i-th term has to be skipped
		  if (j != i)
		    {
			  weight *= (desiredPos - posVect[j]) / (posVect[i] - posVect[j]);
		    }
	    }

	  retVal += weight * valVect[i];
    }

  return retVal;
}

#endif /* MULTIPATHINTERPOLATION_H_ */
