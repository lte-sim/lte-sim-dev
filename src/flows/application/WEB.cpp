/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)  2012
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
 * Author: Gbolly Aiyetoro <g.aiyetoro@gmail.com>
 */

#include "WEB.h"
#include <cstdlib>
//#include "../../componentManagers/NetworkManager.h"
#include "../radio-bearer.h"
#include <math.h>


int arrival;
Random Ran;
//-----------------------------------------------------------------------------------------------------------------
//	constructor and destructor for the random class
//-----------------------------------------------------------------------------------------------------------------
Random::Random() {		ReInit();	}

Random::~Random() {	}


//----------------------------------------------------------------------------------------------------------------
//	generating random variants in integer between interval (a, b)
//----------------------------------------------------------------------------------------------------------------
int Random::Uniform(int a, int b) {

	return a + (rand() % (b-a+1));

}// end of Uniform()


//---------------------------------------------------------------------------------------------------------------
// generates double random variants with Uniform distribution
//---------------------------------------------------------------------------------------------------------------
double Random::Uniform(double a, double b) {
	double f;

	f = (double ) rand()/RAND_MAX;

	return a + f *(b-a);

}// end of Uniform()


//--------------------------------------------------------------------------------------------------------------
//	change the seed of random number generation
//--------------------------------------------------------------------------------------------------------------
void Random::ReInit() {

	srand((unsigned int)time((time_t *)NULL));

}// end of ReInit()


//-------------------------------------------------------------------------------------------------------------
// generate random variants in binomial distribution
//-------------------------------------------------------------------------------------------------------------
int Random::Binomial(int n, double p) {
	double f;
	int b=0,i;

	for (i=0; i <= n-1; i++ ) {
		f = (double ) rand()/RAND_MAX;
		if (f < p) {
			b++;
		}
	}

	return b;

}// end of Binomial()


//-----------------------------------------------------------------------------------------------------------
// generate random variants with Exponential distribution
//-----------------------------------------------------------------------------------------------------------
double Random::Exponential(double lamda) {
	double f;

	f = (double ) rand()/RAND_MAX;

	return -log(1- f)/lamda;

}// end of Exponential()


//----------------------------------------------------------------------------------------------------------
// generate random variants with geometric distribution
//----------------------------------------------------------------------------------------------------------
int Random::Geometric(double p) {
	double f;

	f = (double ) rand()/RAND_MAX;

	return (int)ceil(log(f)/log(1-p));

}// end of Geometric()


//----------------------------------------------------------------------------------------------------------
// generate random variants with pareto distribution
//----------------------------------------------------------------------------------------------------------
double Random::Pareto(double a, double k) {
	double f;

	f=(double)rand()/RAND_MAX;
	f=pow( f, 1/a);
	f = k/f;

	return f;

}// end of Pareto()


//---------------------------------------------------------------------------------------------------------
// generate random variants with normal distribution
//---------------------------------------------------------------------------------------------------------
double Random::Normal(int n, double Mu, double Theta){
	double u, t;
	int i;

	u = (double)rand()/RAND_MAX;

	for (i=1; i<n; i++) {
		u = u + (double)rand()/RAND_MAX;
	}

	double rr=n/12;
	t = (u - n/2)/sqrt(rr);

	return (Mu + Theta * t);

}//end of Normal()


//-------------------------------------------------------------------------------------------------------
// generate random variants with poisson distribution
//-------------------------------------------------------------------------------------------------------
int Random::Poisson(double lamda){
	double R, n0, n1;
	int c;

	R = 1/exp(lamda);
	n1 = rand();

	n1 = n1 / RAND_MAX;
	c = 1;

	do {
		n0 = n1;
		n1 = n0 * rand()/RAND_MAX;

		if ((n1<=R)&&(R<n0)) {
			break;
		}
		else {
			c++;
		}
	} while (c>100);

	return c;

}// end of Poisson()
WEB::WEB()
{
  /*
   * G729 codec generates during the ON period a packet with
   * a fixed size (20 bytes). We must add the RTP header (12 bytes)
   */
  m_size = Ran.Pareto(1.1,81.5);
        if (m_size > 66666)
        {
         m_size = 66666;
        }
  m_stateON = false;
  SetApplicationType (Application::APPLICATION_TYPE_WEB);
}

WEB::~WEB()
{
  Destroy ();
}

void
WEB::DoStart (void)
{
  Simulator::Init()->Schedule(0.0, &WEB::Send, this);
}

void
WEB::DoStop (void)
{}

void
WEB::ScheduleTransmit (double time)
{
  if ( (Simulator::Init()->Now () + time) < GetStopTime () )
    {
      Simulator::Init()->Schedule(time, &WEB::Send, this);
    }
}

void
WEB::Send (void)
{
  if (!m_stateON)
	{
	  m_stateON = true;
	  //start state ON

          int n_packet = Ran.Geometric(300);
          arrival = Ran.Geometric(0.4);
	 // double random = rand() %10000;
	  m_stateDuration = n_packet * arrival;
	  m_endState = Simulator::Init()->Now () + m_stateDuration;
#ifdef APPLICATION_DEBUG
	  std::cout << " WEB_DEBUG - Start ON Period, "
	      "\n\t Time = " << Simulator::Init()->Now ()
		  << "\n\t state ON Duration = " << m_stateDuration
		  << "\n\t end ON state = " <<  m_endState << std::endl;
#endif
	}

  //CREATE A NEW PACKET (ADDING UDP, IP and PDCP HEADERS)
  Packet *packet = new Packet ();
  int uid = Simulator::Init()->GetUID ();

  packet->SetID(uid);
  packet->SetTimeStamp (Simulator::Init()->Now ());
  packet->SetSize (GetSize ());

  Trace (packet);

  PacketTAGs *tags = new PacketTAGs ();
  tags->SetApplicationType(PacketTAGs::APPLICATION_TYPE_WEB);
  tags->SetApplicationSize (packet->GetSize ());
  packet->SetPacketTags(tags);


  UDPHeader *udp = new UDPHeader (GetClassifierParameters ()->GetSourcePort (),
		                          GetClassifierParameters ()->GetDestinationPort ());
  packet->AddUDPHeader (udp);

  IPHeader *ip = new IPHeader (GetClassifierParameters ()->GetSourceID (),
                               GetClassifierParameters ()->GetDestinationID ());
  packet->AddIPHeader (ip);

  PDCPHeader *pdcp = new PDCPHeader ();
  packet->AddPDCPHeader (pdcp);

  GetRadioBearer()->Enqueue (packet);


  if (Simulator::Init()->Now () <= m_endState)
    {
	  ScheduleTransmit (arrival);
    }
  else
    {
	  //schedule OFF Period
         m_stateON = false;
	  //double random = rand() %10000;
	  m_stateDuration  = Ran.Geometric(2);;

#ifdef APPLICATION_DEBUG
	  std::cout << " WEB_DEBUG - Start OFF Period, "
 	      "\n\t Time = " << Simulator::Init()->Now ()
		  << "\n\t state OFF Duration = " << m_stateDuration << std::endl;
#endif
	  ScheduleTransmit (m_stateDuration);
    }
}


int
WEB::GetSize (void) const
{
  return m_size;
}
