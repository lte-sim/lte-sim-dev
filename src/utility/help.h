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


#ifndef HELP_H_
#define HELP_H_
#include <iostream>
#include <stdio.h>

static void Help (void)
{

	std::cout <<
            "\n\n"
			"\n*************************************\n"
			"               LTE-Sim "
			"\n is an open source framework to simulate "
			"\n            LTE networks."
			"\n*************************************\n"
			"\n\n"
            "run test suites:"
			"\n"
			"\t ./LTE-Sim test"
			"\n\n"
			"run examples:"
			"\n"
			"\t ./LTE-Sim SingleCell radius nbUE nbVoip nbVideo nbBE nbCBR sched_type frame_struct speed maxDelay videoBitRate seed(optional)"
            "\n\t\t --> ./LTE-Sim SingleCell 1 1 0 0 1 0 1 1 3 0.1 128"
			"\n"
			"\t ./LTE-Sim SingleCellWithI nbCells radius nbUE nbVoip nbVideo nbBE nbCBR sched_type frame_struct speed maxDelay videoBitRate seed(optional)"
			"\n\t\t --> ./LTE-Sim SingleCellWithI 7 1 1 0 0 1 0 1 1 3 0.1 128"
			"\n"
			"\t ./LTE-Sim MultiCell nbCells radius nbUE nbVoip nbVideo nbBE nbCBR sched_type frame_struct speed maxDelay videoBitRate seed(optional)"
			"\n\t\t --> ./LTE-Sim MultiCell 7 1 1 0 0 1 0 1 1 3 0.1 128"
			"\n"
			"\t ./LTE-Sim SingleCellWithFemto radius nbBuildings BuildingType activityRatio nbMacroUE nbFemtoUE nbVoip nbVideo nbBE nbCBR sched_type frame_struct speed accessPolicy maxDelay videoBitRate seed(optional)"
			"\n\t\t --> ./LTE-Sim SingleCellWithFemto 1 1 0 1 0 1 0 0 1 0 1 1 3 0 0.1 128"
			"\n\n\n"
			"\n\t legend:"
			"\n\t\t schd_type: 1-> PF, 2-> M-LWDF, 3-> EXP, 4-> FLS, 5 -> Optimize EXP Rule, 6 -> Optimized LOG Rule"
			"\n\t\t frame_struct: 1-> FDD, 2-> TDD"
			"\n\t\t available video bit rate: 128, 242, 440 kbps"
			"\n\t\t BuildingType: 0 -> 5x5 grid, 1 -> dualStripe"
			"\n\t\t accessPolicy : 0 -> Open Access, 1 -> close Access (requires subscriber list filling)"

//			"\n\n"
//			"other examples:"
//			"\n\t ./LTE-Sim test-amc-mapping nbCells radius speed bandwidth cluster"
			<< std::endl;



}

#endif /* HELP_H_ */
