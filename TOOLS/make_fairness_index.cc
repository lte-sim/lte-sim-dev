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

        int n = 0;
        double goodput;
	double sum_goodput = 0;
        double avg_goodput = 0;
	double sum_quad_goodput = 0;
        double quad_sum_goodput = 0;

	double fi = 0;

	finput=fopen(argv[1],"r");
	while (feof(finput)==0) 
          {
		fscanf(finput,"%lf\n",&goodput);
		sum_goodput += goodput;
            	sum_quad_goodput += pow (goodput, 2);
	    	n+=1;
	  }

	fclose(finput);

	quad_sum_goodput = pow (sum_goodput, 2);
        avg_goodput = sum_goodput / n;

        fi = quad_sum_goodput / (n * sum_quad_goodput);
		
 
       printf("tot %2.9lf\n",sum_goodput);
       printf("avg %2.9lf\n",avg_goodput);
       printf("FI %2.9lf\n",fi);

	return 0;
}
