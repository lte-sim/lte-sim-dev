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

# params 1 MINUSERS, 2 MAXUSERS, 3 INTERVAL, 4 FILENAME, 5 FILE, 6 NUMSIM

FILE=$5   #OUTPUT FILE NAME
NUMSIM=$6  #SIMULATIONS NUMBER
FILENAME=$4   # SIMULATION TYPE NAME
COUNT=1
CELS=1       # NUMBER OF CELLS
TOTALNAME=""

NBUE=$1 #Number of UE's
# variables for values

bandwidth=10000000
time=150

until [ $NBUE -gt $2 ]; do
	echo FOR $NBUE USERS >> specEff.ods
	# bash until loop
	echo proportional fairnes >> specEff.ods
	
	until [ $COUNT -gt $NUMSIM ]; do
	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_PF_"$NBUE"U"$CELS"C"".sim"

	grep "^RX" $TOTALNAME  | awk '{print $8}'  > tmp
	/usr/local/home/miturral/LTE-Sim/src/Simulations/TOOLS/./make_cell_spectral_efficiency    tmp   ${time}   ${bandwidth} >> specEff.ods
	rm tmp
	 let COUNT=COUNT+1
	done
	COUNT=1
	echo AVERAGE >> specEff.ods
	echo  >> specEff.ods
	echo  >> specEff.ods

	echo MLWDF >> specEff.ods
	until [ $COUNT -gt $NUMSIM ]; do
	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_MLWDF_"$NBUE"U"$CELS"C"".sim"

	grep "^RX" $TOTALNAME  | awk '{print $8}'  > tmp
/usr/local/home/miturral/LTE-Sim/src/Simulations/TOOLS/./make_cell_spectral_efficiency   tmp   ${time}   ${bandwidth} >> specEff.ods
	rm tmp

	 let COUNT=COUNT+1
	done
	COUNT=1
	echo AVERAGE >> specEff.ods
	echo  >> specEff.ods
	echo  >> specEff.ods

	echo EXPPF >> specEff.ods
	until [ $COUNT -gt $NUMSIM ]; do
	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_EXPPF_"$NBUE"U"$CELS"C"".sim"

	grep "^RX" $TOTALNAME  | awk '{print $8}'  > tmp
/usr/local/home/miturral/LTE-Sim/src/Simulations/TOOLS/./make_cell_spectral_efficiency    tmp   ${time}   ${bandwidth} >> specEff.ods
	rm tmp
	 let COUNT=COUNT+1
	done
	COUNT=1
	echo AVERAGE >> specEff.ods
	echo  >> specEff.ods
	echo  >> specEff.ods
 let NBUE=NBUE+$3
done
echo  SPECTRAL EFFICIENCY REPORT FINISHED!! 








