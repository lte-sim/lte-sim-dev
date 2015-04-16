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

#include "henb-mac-entity.h"
#include "../../device/UserEquipment.h"

HenbMacEntity::HenbMacEntity()
{
  m_subscriberList = new std::vector<UserEquipment*>;
  m_restrictedAccessMode = false;
}

HenbMacEntity::HenbMacEntity(bool restrictedAccess)
{
  m_subscriberList = new std::vector<UserEquipment*>;
  m_restrictedAccessMode = restrictedAccess;
}

HenbMacEntity::~HenbMacEntity ()
{
  delete m_subscriberList;
}

void
HenbMacEntity::SetRestrictedAccessMode(bool restricted)
{
  m_restrictedAccessMode = restricted;
}

bool
HenbMacEntity::GetRestrictedAccessMode (void)
{
  return m_restrictedAccessMode;
}

void
HenbMacEntity::AddSubscribedUser (UserEquipment* user)
{
  m_subscriberList->push_back(user);
}

bool
HenbMacEntity::CheckSubscribedUser (UserEquipment* user)
{
	if (!m_restrictedAccessMode)
	{
		return true;
	}
	else
	{
		for (int i = 0; i < (int) m_subscriberList->size(); i++)
		{
			if ( m_subscriberList->at(i)->GetIDNetworkNode() == user->GetIDNetworkNode() )
			{
				return true;
			}
		}
	}
	return false;
}


