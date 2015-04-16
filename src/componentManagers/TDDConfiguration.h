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



#ifndef TDDCONFIGURATION_H_
#define TDDCONFIGURATION_H_

/*
 * Frame structure type 2 is applicable to TDD [TS 36.211].
 * Each radio frame consists of two half-frames.
 * Each half-frame consists of five subframes of length .
 */
enum SubFrameType
	  {
		SUBFRAME_FOR_DOWNLINK,	// code 0
		SUBFRAME_FOR_UPLINK,	// code 1
		SPECIAL_SUBFRAME		// code 2
	  };

static int TDDConfiguration[7][10] = {
 {
   0, 2, 1, 1, 1, 0, 2, 1, 1, 1
 },

 {
   0, 2, 1, 1, 0, 0, 2, 1, 1, 0
 },

 {
   0, 2, 1, 0, 0, 0, 2, 1, 0, 0
 },

 {
   0, 2, 1, 1, 1, 0, 0, 0, 0, 0
 },

 {
   0, 2, 1, 1, 0, 0, 0, 0, 0, 0
 },

 {
   0, 2, 1, 0, 0, 0, 0, 0, 0, 0
 },

 {
   0, 2, 1, 1, 1, 0, 2, 1, 1, 0
 },

};


#endif /* TDDCONFIGURATION_H_ */
