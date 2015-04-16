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
 
bandwidth=10000000 # setear en el archivo compute_spectral_efficiency
time=150 # setear en el archivocompute_spectral_efficiency

until [ $NBUE -gt $2 ]; do
	echo FOR $NBUE USERS >> Spectral_efficiency.ods
	# bash until loop
	echo proportional fairnes >> Spectral_efficiency.ods
	
	until [ $COUNT -gt $NUMSIM ]; do
	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_PF_"$NBUE"U"$CELS"C"".sim"

	grep -i "^RX" $TOTALNAME |grep -iv "AM_RLC"  | awk '{print $8+5}'  > tmp
	grep -i "^RX" $TOTALNAME |grep -i "AM_RLC"  | awk '{print $4+5}'  >> tmp
	./compute_spectral_efficiency.sh tmp  >> Spectral_efficiency.ods
	rm tmp
	 let COUNT=COUNT+1
	done
	COUNT=1
	echo AVERAGE >> Spectral_efficiency.ods
	echo  >> Spectral_efficiency.ods
	echo  >> Spectral_efficiency.ods

	echo MLWDF >> Spectral_efficiency.ods
	until [ $COUNT -gt $NUMSIM ]; do
	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_MLWDF_"$NBUE"U"$CELS"C"".sim"

	grep -i "^RX" $TOTALNAME |grep -iv "AM_RLC"  | awk '{print $8+5}'  > tmp
grep -i "^RX" $TOTALNAME |grep -i "AM_RLC"  | awk '{print $4+5}'  >> tmp
./compute_spectral_efficiency.sh tmp  >> Spectral_efficiency.ods
	rm tmp

	 let COUNT=COUNT+1
	done
	COUNT=1
	echo AVERAGE >> Spectral_efficiency.ods
	echo  >> Spectral_efficiency.ods
	echo  >> Spectral_efficiency.ods

	echo EXPPF >> Spectral_efficiency.ods
	until [ $COUNT -gt $NUMSIM ]; do
	TOTALNAME=$FILE"_"$COUNT"_"$FILENAME"_EXPPF_"$NBUE"U"$CELS"C"".sim"

	grep -i "^RX" $TOTALNAME |grep -iv "AM_RLC"  | awk '{print $8+5}'  > tmp
grep -i "^RX" $TOTALNAME |grep -i "AM_RLC"  | awk '{print $4+5}'  >> tmp
./compute_spectral_efficiency.sh tmp  >> Spectral_efficiency.ods
	rm tmp
	 let COUNT=COUNT+1
	done
	COUNT=1
	echo AVERAGE >> Spectral_efficiency.ods
	echo  >> Spectral_efficiency.ods
	echo  >> Spectral_efficiency.ods
 let NBUE=NBUE+$3
done
echo  SPECTRAL EFFICIENCY REPORT FINISHED!! 





