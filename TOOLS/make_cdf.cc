/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, Politecnico di Bari
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// in units of 0.1 milliseconds
#define MAX_DELAY 1000 

int main(int argc, char* argv[])
{
	unsigned int occurrence[MAX_DELAY];
	FILE *finput;
	FILE *foutput;

	double delay;
	double avg = 0.;
        long total_packets = 0;

	if (argc<3 && argc>4) return 1;

	for (int i=0; i<MAX_DELAY; i++) 
          {
  	    occurrence[i]=0;
          }

	finput=fopen(argv[1],"r");
	while (feof(finput)==0) 
          {
	    fscanf(finput,"%lf\n",&delay);
	    for (int i=0; i<MAX_DELAY; i++) 
	      {
		if ((int)ceil(delay*1000.0)<=i) 
		  {
                    occurrence[i]++;
                  }
	      }
	    avg+=delay;
	    total_packets+=1;
	  }

	fclose(finput);

	printf("%2.9lf\n",avg/total_packets);

	foutput=fopen(argv[2],"w");
	for (int i=0; i<MAX_DELAY; i++) {
		double delay=i;
		delay=delay/1000.0;
		double cdf=occurrence[i];
		cdf=cdf/total_packets;
		fprintf(foutput,"%lf %lf\n",delay,cdf);
	}
	fclose(foutput);
	return 0;
}
