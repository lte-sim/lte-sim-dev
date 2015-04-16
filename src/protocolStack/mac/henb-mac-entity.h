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

#ifndef HENBMACENTITY_H_
#define HENBMACENTITY_H_

#include "enb-mac-entity.h"
#include <vector>
/*
 * This class implements the MAC layer of the Home eNodeB device extending the one of the eNodeB
 */
class UserEquipment;

class HenbMacEntity : public EnbMacEntity
{
public:
	HenbMacEntity (void);
	HenbMacEntity (bool restrictedAccess);
	virtual ~HenbMacEntity (void);

	void
	SetRestrictedAccessMode(bool restricted);
	bool
	GetRestrictedAccessMode (void);

	void
	AddSubscribedUser (UserEquipment* user);
	bool
	CheckSubscribedUser (UserEquipment* user);

private:

	bool m_restrictedAccessMode;
	std::vector<UserEquipment*>  *m_subscriberList;
};

#endif /* HENBMACENTITY_H_ */
