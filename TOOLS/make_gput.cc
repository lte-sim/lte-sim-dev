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

int main(int argc, char* argv[])
{

int TransportBlockSize[32] = {
		0,
		18, 23, 28, 37, 45, 59, 72, 89, 105, 123, 141,
		159, 177, 203, 230, 259, 289, 288,
		308, 328, 363, 399, 433, 468, 506, 543, 578, 614, 640,
		667,
		0
};

	FILE *finput;


        long total_packets = 0;
	unsigned int tot_goodput = 0;
	int goodput;
 	int mcs;
 	int rbs;
 	double time = atof(argv[2]);

	finput=fopen(argv[1],"r");
	while (feof(finput)==0) 
          {
	    fscanf(finput,"%d %d\n",&mcs, &rbs);
	    goodput = TransportBlockSize[mcs]*rbs;
	    tot_goodput+=goodput;
	    total_packets+=1;
	  }
	fclose(finput);
	
        printf("%2.9lf\n",tot_goodput/time);

	return 0;
}
