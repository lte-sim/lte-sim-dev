# Copyright (c) 2010 
# 
# This file is part of LTE-Sim
# LTE-Sim is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation;
# 
# LTE-Sim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with LTE-Sim; if not, see <http://www.gnu.org/licenses/>.
#
# Author: Mauricio Iturralde <mauricio.iturralde@irit.fr, mauro@miturralde.com>


# Single Cell With Interference

FILE="Sim"   #OUTPUT FILE NAME
NUMSIM=5  #Number of simulations
FILENAME="Multi"   # SIMULATION TYPE NAME
COUNT=1
CELS=1       # NUMBER OF CELLS
TOTALNAME=""
MINUSERS=5 # Start users
INTERVAL=5 # Interval between users
MAXUSERS=20 #max of users 

# params of LTE-SIM MULTICEL

RADIUS=1 # Radius in Km
NBUE=0 #Number of UE's
NBVOIP=1  # Number of Voip Flows
NBVIDEO=1 #Number of Video
NBBE=1 # Number of Best Effort Flows
NBCBR=0 #Number of CBR flows
#Scheduler Type PF=1, MLWDF=2 EXP= 3
FRAME_STRUCT=1  # FDD or TDD
SPEED=3 #User speed 
MAXDELAY=0.1
VIDEOBITRATE=242

NBUE=$MINUSERS
until [ $NBUE -gt $MAXUSERS ]; do

	# bash until loop
	until [ $COUNT -gt $NUMSIM ]; do
	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_PF_"$NBUE"U"$CELS"C"".sim"
	../../LTE-Sim SingleCellWithI $CELS $RADIUS $NBUE $NBVOIP $NBVIDEO $NBBE $NBCBR 1 $FRAME_STRUCT $SPEED $MAXDELAY $VIDEOBITRATE $COUNT > $TOTALNAME
        	echo FILE $TOTALNAME CREATED!

 	let COUNT=COUNT+1
	done
	COUNT=1

	until [ $COUNT -gt $NUMSIM ]; do
       	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_MLWDF_"$NBUE"U"$CELS"C"".sim"
	../../LTE-Sim SingleCellWithI $CELS $RADIUS $NBUE $NBVOIP $NBVIDEO $NBBE $NBCBR 2 $FRAME_STRUCT $SPEED $MAXDELAY $VIDEOBITRATE $COUNT > $TOTALNAME
       	echo FILE $TOTALNAME CREATED!
	 let COUNT=COUNT+1
	done
	COUNT=1

	until [ $COUNT -gt $NUMSIM ]; do
  	      TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_EXPPF_"$NBUE"U"$CELS"C"".sim"
	../../LTE-Sim SingleCellWithI $CELS $RADIUS $NBUE $NBVOIP $NBVIDEO $NBBE $NBCBR 3 $FRAME_STRUCT $SPEED $MAXDELAY $VIDEOBITRATE $COUNT > $TOTALNAME
   	    echo FILE $TOTALNAME CREATED!
 	let COUNT=COUNT+1
	done
	COUNT=1
let NBUE=NBUE+INTERVAL
done
echo SIMULATION FINISHED!
echo CREATING RESULTS REPORT!

# params 1 MINUSERS, 2 MAXUSERS, 3 INTERVAL, 4 FILENAME, 5 FILE, 6 NUMSIM, 7 TypeFlow, Graphic_name

# result shells
./packet_loss_ratio.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM VIDEO Packet-Loss-Ratio
./packet_loss_ratio.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM VOIP Packet-Loss-Ratio
./packet_loss_ratio.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM INF_BUF Packet-Loss-Ratio
./thoughput_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM VIDEO Throughput
./thoughput_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM VOIP Throughput
./thoughput_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM INF_BUF Throughput
./delay_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM VIDEO Delay
./delay_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM VOIP Delay
./delay_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM INF_BUF Delay
./spectral_efficiency_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM Spectral-Efficiency Spec-Eff
./fairnessIndex_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM  VIDEO Fairness-Index
./fairnessIndex_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM  VOIP Fairness-Index
./fairnessIndex_comp.sh $MINUSERS $MAXUSERS $INTERVAL $FILENAME $FILE $NUMSIM  INF_BUF Fairness-Index


 

