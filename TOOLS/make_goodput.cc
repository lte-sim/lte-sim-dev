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
	FILE *finput;
	FILE *foutput;

        long total_packets = 0;
	unsigned int tot_goodput = 0;
	int goodput;
 	double time = atof(argv[2]);

	finput=fopen(argv[1],"r");
	while (feof(finput)==0) 
          {
	    fscanf(finput,"%d\n",&goodput);
	    tot_goodput+=(goodput*8);
	    total_packets+=1;
	  }
	fclose(finput);
	
        printf("%2.9lf\n",tot_goodput/time);

	return 0;
}
