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


/*
 * LTE-Sim is the main program of the LTE-Sim simulator.
 * To run simulations you can
 * (i) select one of example scenarios developed in Scenario/ ("./LTE-Sim -h" for details)
 * (ii) create a new scenario and add its reference into the main program.
 *
 *  To create a new scenario, see documentation in DOC/ folder.
 *
 *  For any questions, please contact the author at
 *  g.piro@poliba.it
 */


#include "TEST/test.h"

#include "scenarios/simple.h"
#include "scenarios/single-cell-without-interference.h"
#include "scenarios/single-cell-with-interference.h"
#include "scenarios/single-cell-with-femto.h"
#include "scenarios/multi-cell.h"
#include "scenarios/single-cell-with-streets.h"
#include "scenarios/multi-cell-sinrplot.h"
#include "TEST/scalability-test-macro-with-femto.h"
#include "TEST/test-sinr-femto.h"
#include "TEST/test-throughput-macro-with-femto.h"
#include "TEST/test-sinr-urban.h"
#include "TEST/test-throughput-urban.h"
#include "TEST/test-throughput-building.h"
#include "TEST/test-uplink-fme.h"
#include "TEST/test-uplink-channel-quality.h"


#include "utility/help.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <cstring>

int
main (int argc, char *argv[])
{

  if (argc > 1)
    {

	  /* Help */
	  if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-H") || !strcmp(argv[1],
			  "--help") || !strcmp(argv[1], "--Help"))
	    {
		  Help ();
		  return 0;
	    }

	  /* run tests */
	  if(strcmp(argv[1], "test")==0)
	    {
		  /* To enable a test, please modify the file LTE-Sim/src/TESTS/test.h */
		  Test ();
	    }

      /* Run simple scenario */
	  if (strcmp(argv[1], "Simple")==0)
	    {
		  Simple ();
	    }









      /* Run more complex scenarios */
	  if (strcmp(argv[1], "SingleCell")==0)
	    {
	      double radius = atof(argv[2]);
	      int nbUE = atoi(argv[3]);
	      int nbVoIP = atoi(argv[4]);
	      int nbVideo = atoi(argv[5]);
	      int nbBE = atoi(argv[6]);
	      int nbCBR = atoi(argv[7]);
	      int sched_type = atoi(argv[8]);
	      int frame_struct = atoi(argv[9]);
	      int speed = atoi(argv[10]);
	      double maxDelay = atof(argv[11]);
		  int video_bit_rate = atoi(argv[12]);
	      int seed;
	      if (argc==14) seed = atoi(argv[13]);
	      else seed = -1;
	      SingleCellWithoutInterference (radius, nbUE, nbVoIP, nbVideo, nbBE, nbCBR, sched_type, frame_struct, speed, maxDelay, video_bit_rate, seed);
	    }
	  if (strcmp(argv[1], "SingleCellWithI")==0)
	    {
		  int nbCells = atoi(argv[2]);
		  double radius = atof(argv[3]);
	      int nbUE = atoi(argv[4]);
	      int nbVoIP = atoi(argv[5]);
	      int nbVideo = atoi(argv[6]);
	      int nbBE = atoi(argv[7]);
	      int nbCBR = atoi(argv[8]);
	      int sched_type = atoi(argv[9]);
	      int frame_struct = atoi(argv[10]);
	      int speed = atoi(argv[11]);
	      double maxDelay = atof(argv[12]);
		  int video_bit_rate = atoi(argv[13]);
	      int seed;
	      if (argc==15) seed = atoi(argv[14]);
	      else seed = -1;
	      SingleCellWithInterference (nbCells, radius, nbUE, nbVoIP, nbVideo, nbBE, nbCBR, sched_type, frame_struct, speed, maxDelay, video_bit_rate, seed);
	    }
	  if (strcmp(argv[1], "MultiCell")==0)
	    {
		  int nbCells = atoi(argv[2]);
		  double radius = atof(argv[3]);
	      int nbUE = atoi(argv[4]);
	      int nbVoIP = atoi(argv[5]);
	      int nbVideo = atoi(argv[6]);
	      int nbBE = atoi(argv[7]);
	      int nbCBR = atoi(argv[8]);
	      int sched_type = atoi(argv[9]);
	      int frame_struct = atoi(argv[10]);
	      int speed = atoi(argv[11]);
	      double maxDelay = atof(argv[12]);
		  int video_bit_rate = atoi(argv[13]);
	      int seed;
	      if (argc==15) seed = atoi(argv[14]);
	      else seed = -1;
	      MultiCell (nbCells, radius, nbUE, nbVoIP, nbVideo, nbBE, nbCBR, sched_type, frame_struct, speed, maxDelay, video_bit_rate, seed);
	    }

	  if (strcmp(argv[1], "SingleCellWithFemto")==0)
		{
		  double radius = atof(argv[2]);
		  	          int nbBuilding = atoi(argv[3]);
		  		  int buildingType = atoi(argv[4]);
		  		  double activityRatio = atof(argv[5]);
		  		  int nbUE = atoi(argv[6]);
		  		  int nbFemtoUE = atoi(argv[7]);
		  		  int nbVoIP = atoi(argv[8]);
		  		  int nbVideo = atoi(argv[9]);
		  		  int nbBE = atoi(argv[10]);
		  		  int nbCBR = atoi(argv[11]);
		  		  int sched_type = atoi(argv[12]);
		  		  int frame_struct = atoi(argv[13]);
		  		  int speed = atoi(argv[14]);
		  		  int accessPolicy = atoi(argv[15]);
		  		  double maxDelay = atof(argv[16]);
		  		  int video_bit_rate = atoi(argv[17]);
		  		  int seed;
		  		  if (argc==19) seed = atoi(argv[18]);
		  		  else seed = -1;
		  SingleCellWithFemto(radius, nbBuilding, buildingType, activityRatio, nbUE, nbFemtoUE, nbVoIP, nbVideo, nbBE, nbCBR, sched_type, frame_struct, speed, accessPolicy, maxDelay, video_bit_rate, seed);
		}
	  if (strcmp(argv[1], "SingleCellWithStreets")==0)
		{
		  int nbStreets = atoi(argv[2]);
		  double radius = atof(argv[3]);
		  int nbUE = atoi(argv[4]);
		  int nbFemtoUE = atoi(argv[5]);
		  int nbVoIP = atoi(argv[6]);
		  int nbVideo = atoi(argv[7]);
		  int nbBE = atoi(argv[8]);
		  int nbCBR = atoi(argv[9]);
		  int sched_type = atoi(argv[10]);
		  int frame_struct = atoi(argv[11]);
		  int speed = atoi(argv[12]);
		  double maxDelay = atof(argv[13]);
		  int video_bit_rate = atoi(argv[14]);
	      int seed;
	      if (argc==16) seed = atoi(argv[15]);
	      else seed = -1;
		  SingleCellWithStreets ( radius, nbStreets, nbUE, nbFemtoUE, nbVoIP, nbVideo, nbBE, nbCBR, sched_type, frame_struct, speed, maxDelay, video_bit_rate, seed);
		}








	  /* other dedicated simulations */
	  if (strcmp(argv[1], "test-amc-mapping")==0)
		{
		  int cells = atoi(argv[2]);
		  double radius = atof(argv[3]);
		  int speed = atoi(argv[4]);
		  int bandwidth = atoi(argv[5]);
		  int cluster = atoi(argv[6]);

		  TestAmcMapping (cells, radius, speed, bandwidth, cluster);
		}
	  if (strcmp(argv[1], "test-mobility-model")==0)
		{
		  double radius = atof(argv[2]);
		  int nbUE = atoi(argv[3]);
		  int model = atoi(argv[4]);
		  int speed = atoi(argv[5]);
		  double duration = atoi(argv[6]);
		  TestMobilityModels(radius, nbUE, model, speed, duration);
		}
	  if (strcmp(argv[1], "scalability-test-macro-with-femto")==0)
		{
		  double radius = atof(argv[2]);
		  int nbBuildings = atoi(argv[3]);
		  int nbUE = atoi(argv[4]);
		  ScalabilityTestMacroWithFemto (radius, nbBuildings, nbUE);
		}

	  if (strcmp(argv[1], "test-sinr-urban")==0)
		{
		  int streets = atoi(argv[2]);
		  int henb = atoi(argv[3]);
		  int reuse = atoi(argv[4]);
		  TestSinrUrban (streets, henb, reuse);
		}
	  if (strcmp(argv[1], "test-throughput-urban")==0)
		{
		  int streets = atoi(argv[2]);
		  int henb = atoi(argv[3]);
		  int reuse = atoi(argv[4]);
		  int nbUEs = atoi(argv[5]);
		  double activityFactor = atoi(argv[6]);
		  TestThroughputUrban (streets, henb, reuse, nbUEs, activityFactor);
		}
	  if (strcmp(argv[1], "test-throughput-macro-with-femto")==0)
		{
		  double radius = atof(argv[2]);
		  int nbBuildings = atoi(argv[3]);
		  int nbUE_macro = atoi(argv[4]);
		  TestThroughputMacroWithFemto (radius, nbBuildings, nbUE_macro);
		}
	  if (strcmp(argv[1], "test-sinr-femto")==0)
		{
		  double riuso = atof(argv[2]);
		  double activityFactor = atof(argv[3]);
		  TestSinrFemto (riuso, activityFactor);
		}

	  if (strcmp(argv[1], "test-throughput-building")==0)
		{
		  double riuso = atof(argv[2]);
		  double activityFactor = atof(argv[3]);
		  int nbUE_femto = atof(argv[4]);
		  TestThroughputBuilding (riuso, activityFactor, nbUE_femto);
		}
	  if (strcmp(argv[1], "MultiCellSinrPlot")==0)
	    {
		  int nbCells = atoi(argv[2]);
		  double radius = atof(argv[3]);
		  int nbUE = atoi(argv[4]);
		  int sched_type = atoi(argv[5]);
		  int frame_struct = atoi(argv[6]);
		  int speed = atoi(argv[7]);
		  int model = atoi(argv[8]);
		  int seed;
		  if (argc==10) seed = atoi(argv[9]);
		  else seed = -1;
		  MultiCellSinrPlot (nbCells, radius, nbUE, sched_type, frame_struct, speed, model, seed);
	    }
	  if (strcmp(argv[1], "testfme")==0)
	    {
		  TestUplinkFME ();
	    }
	  if (strcmp(argv[1], "test-ul-mt")==0)
	    {
		  TestUplinkMaximumThroughput ();
	    }
	  if (strcmp(argv[1], "test-ul-channel")==0)
		{
		  TestUplinkChannelQuality ();
		}
    }
}
