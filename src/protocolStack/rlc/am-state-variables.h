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

#ifndef AMSTATEVARIABLES_H_
#define AMSTATEVARIABLES_H_


/*
 * This class implements all variables, constants and timers for the AM RLC
 * see 3GPP TS 36.322 V9.3.0 (2010-09)
 */
class AmStateVariables {
public:
	AmStateVariables();
	virtual ~AmStateVariables();

	void Print (void);
	void PrintTxVariables (void);
	void PrintRxVariables (void);


    /*
	 * The transmitting side of each AM RLC entity
	 * shall maintain the following state variables:
	 */
	int m_vt_a;
	int m_vt_ms;
	int m_vt_s;
	int m_poll_sn;

	/*
	 * The transmitting side of each AM RLC entity
	 * shall maintain the following counters
	 */
	int m_pdu_without_poll_counter;
	int m_byte_without_poll_counter;
	int m_retx_counter;

	/*
	 * The receiving side of each AM RLC entity
	 * shall maintain the following state variables
	 */
	int m_vr_r;
	int m_vr_mr;
	int m_vr_x;
	int m_vr_ms;
	int m_vr_h;

	/*
	 * constants
	 */
	int m_am_window_size;

    /*
     * timers
     */
	double m_t_poll_retransmit;
	double m_t_reordering;
	double m_t_status_prohibit;


	/*
	 * Configurable parameters
	 */
	int m_max_retx_threshold;
	int m_poll_PDU;
	int m_poll_Byte;
};

#endif /* AMSTATEVARIABLES_H_ */
