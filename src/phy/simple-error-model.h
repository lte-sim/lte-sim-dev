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

#ifndef SIMPLE_ERRORMODEL_H_
#define SIMPLE_ERRORMODEL_H_

#include "error-model.h"
#include <vector>

class SimpleErrorModel : public ErrorModel {
public:
	SimpleErrorModel();
	virtual ~SimpleErrorModel();

	virtual bool CheckForPhysicalError (std::vector<int> channels, std::vector<int> mcs, std::vector<double> m_sinr);
};

#endif /* ERRORMODEL_H_ */
