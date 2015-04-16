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


#ifndef QOSPARAMETERS_H_
#define QOSPARAMETERS_H_

/*
 * Bearer Level QoS (TS 23.401, Clause 4.7.3)
 *
 * A bearer uniquely identifies traffic flows that receive a
 * common QoS treatment.
 *
 * Each bearer (GBR and Non-GBR) is associated with
 * the following bearer level QoS parameters:
 *
 *  ** QoS Class Identifier (QCI)
 *  A QCI is a scalar that is used as a reference to access node-specific
 *  parameters that control bearer level packet forwarding treatment
 *  (e.g. scheduling weights, admission thresholds, queue management
 *  thresholds, link layer protocol configuration, etc.), and that have been
 *  pre-configured by the operator owning the access node (e.g. eNodeB).
 *  A one-to-one mapping of standardized QCI values to standardized
 *  characteristics is captured TS 23.203 [6].
 *
 *  ** Allocation and Retention Priority (ARP),
 *  The ARP shall contain information about the priority
 *  level (scalar), the pre-emption capability (flag) and the
 *  pre-emption vulnerability (flag).
 *
 *  ** Guaranteed Bit Rate (GBR) for UP and DW link
 *  ** Maximum Bit Rate (MBR) for UP and DW link
 *  The GBR denotes the bit rate that can be expected to
 *  be provided by a GBR bearer. The MBR limits the bit rate that
 *  can be expected to be provided by a GBR bearer
 *  (e.g. excess traffic may get discarded by a rate shaping function).
 *  See clause 4.7.4 for further details on GBR and MBR
 */

class QoSParameters {
public:
	QoSParameters();
	QoSParameters (int qci,
				   double gbr,
				   double mbr);
	QoSParameters (int qci,
				   bool arpPreEmptionCapability,
				   bool arpPreEmptionVulnerability,
				   double gbr,
				   double mbr);
	virtual ~QoSParameters();

	void
	SetQCI (int qci);
	int
	GetQCI (void) const;
	void
	SetArpPreEmptionCapability (bool flag);
	bool
	GetArpPreEmptionCapability (void) const;
	void
	SetArpPreEmptionVulnerability (bool flag);
	bool
	GetArpPreEmptionVulnerability (void) const;
	void
	SetGBR (double gbr);
	double
	GetGBR (void) const;
	void
	SetMBR (double mbr);
	double
	GetMBR (void) const;

	void
	SetMaxDelay (double targetDelay);
	double
	GetMaxDelay (void) const;

private:
	int m_qci;
	bool m_arpPreEmptionCapability;
	bool m_arpPreEmptionVulnerability;
	double m_gbr;
	double m_mbr;

	double m_maxDelay;
};

#endif /* QOSPARAMETERS_H_ */
