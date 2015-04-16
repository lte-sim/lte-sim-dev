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


#ifndef FREQUENCY_REUSE_HELPER_H_
#define FREQUENCY_REUSE_HELPER_H_

#include <stdint.h>
#include "stdlib.h"
#include <math.h>
#include "../core/spectrum/bandwidth-manager.h"
#include "../componentManagers/FrameManager.h"

/*

Number of supported non-overlapping channels in each frequency band and bandwidth.

operative band		bandwidth 			Channel bandwidth (MHz)
										1.4 3 	5 	10 	15 	20

1 						60 				— 	— 	12 	6 	4 	3
2 						60 				42 	20 	12 	6	[4] [3]
3 						75 				53 	23 	15 	7 	[5] [3]
...

XXX: now is supported only the 1-th operative sub-band
*/

static std::vector <BandwidthManager*>
RunFrequencyReuseTechniques(int nodes, int cluster, double bandwidth)
{
  std::vector <BandwidthManager*> spectrum;

  if (FrameManager::Init ()->GetFrameStructure () == FrameManager::FRAME_STRUCTURE_FDD)
    {

      if (bandwidth == 5)
        {
          if (cluster > 12)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 12;
            }

          int operatibeSubBands = 25;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

         	  BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset);
       	      spectrum.push_back (s);

         	  counter++;
    	      if (counter == cluster) counter = 0;
            }
        }

      else if (bandwidth == 10)
        {
          if (cluster > 6)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 6;
            }

          int operatibeSubBands = 50;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

    	      BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset);
    	      spectrum.push_back (s);

    	      counter++;
    	      if (counter == cluster) counter = 0;
            }
        }

      else if (bandwidth == 15)
        {
          if (cluster > 4)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 4;
            }

          int operatibeSubBands = 75;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

    	      BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset);
      	      spectrum.push_back (s);

    	      counter++;
    	      if (counter == cluster) counter = 0;
            }
        }

      else if (bandwidth == 20)
        {
          if (cluster > 3)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 3;
            }

          int operatibeSubBands = 100;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

       	      BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset);
    	      spectrum.push_back (s);

       	      counter++;
    	      if (counter == cluster) counter = 0;
            }
        }
    }
  else //case TDD
    {
      if (bandwidth == 5)
        {
          if (cluster > 12)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 12;
            }

          int operatibeSubBands = 25;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

         	  BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset, true);
       	      spectrum.push_back (s);

         	  counter++;
    	      if (counter == cluster) counter = 0;
            }
        }

      else if (bandwidth == 10)
        {
          if (cluster > 6)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 6;
            }

          int operatibeSubBands = 50;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

    	      BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset, true);
    	      spectrum.push_back (s);

    	      counter++;
    	      if (counter == cluster) counter = 0;
            }
        }

      else if (bandwidth == 15)
        {
          if (cluster > 4)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 4;
            }

          int operatibeSubBands = 75;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

    	      BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset, true);
      	      spectrum.push_back (s);

    	      counter++;
    	      if (counter == cluster) counter = 0;
            }
        }

      else if (bandwidth == 20)
        {
          if (cluster > 3)
            {
    	      std::cout << "ERROR: INVALID VALUES OF BANDWIDTH -> CLUSTER"<< std::endl;
    	      cluster = 3;
            }

          int operatibeSubBands = 100;
          int counter = 0;

          for (int i = 0; i < nodes; i++)
            {
    	      int offset = counter * operatibeSubBands;

       	      BandwidthManager *s = new BandwidthManager (bandwidth, bandwidth, offset, offset, true);
    	      spectrum.push_back (s);

       	      counter++;
    	      if (counter == cluster) counter = 0;
            }
        }
    }

  return spectrum;
}



#endif /* FREQUENCY_REUSE_HELPER_H_ */
