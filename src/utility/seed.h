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


#ifndef SEED_H_
#define SEED_H_

static int commonSeed [9] =
{
	805290992,
	749913912,
	965326802,
	697084729,
	1518010490,
	56234558,
	1511265396,
	1412837728,
	947674421,
};

static int GetCommonSeed (int i)
{
  if (i >= 0 && i< 9 )
	  return commonSeed[i];
  else
	  return commonSeed[0];
}
#endif /* SEED_H_ */
