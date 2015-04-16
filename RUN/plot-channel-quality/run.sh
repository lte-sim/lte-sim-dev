# Copyright (c) 2010 TELEMATICS LAB, Politecnico di Bari
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
# Author: Giuseppe Piro <g.piro@poliba.it>

#  plot the channel quality varing
#  (i) the radius of the cell, 
#  (ii) the cluster 
#  (iii) and the bandwidth assigned to each cell
# 
 
# ****************************************
# NOTE:
# 	BEFORE RUN SIMULATIONS WRITE INTO
#	LTE-Sim/CONFIG/global_config
#	the following code:
#	#define AMC_MAPPING
#	Then, compile the software again
# ****************************************

  echo "LTE-Sim"
  echo "channel quality simulations"

  currentPath=$(pwd)
  cd $currentPath
  runSimulation="../../LTE-Sim"
 
  sh clear.sh
  mkdir output

  for cells in 1 7 19
  do
  	for speed in 120    
  	do
		for radius in 0.5 1  
    		do 

			for bandwidth in 5 10 
			do
				for cluster in 1 2 3 4 5 6	
				do
		
					file="cells_${cells}_radius_${radius}_speed_${speed}_bw_${bandwidth}_cluster_${cluster}"
					echo "run ${file}"
				
					$runSimulation test-amc-mapping  ${cells} ${radius} ${speed} ${bandwidth} ${cluster}> output/${file}
				done
			done
		done
	done
  done



 
 

  

