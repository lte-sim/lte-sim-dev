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


  echo "LTE-Sim"
  echo "--- Scalability Test ---"


  currentPath=$(pwd)
  cd $currentPath
  runSimulation="../../LTE-Sim"  
 
  rm -r PF MLWDF EXP
  mkdir PF MLWDF EXP


  sim="MultiCell"

  nbCells=19
  radius=1
  nbVoip=1 
  nbVideo=1 
  nbBE=1 
  nbCBR=0 
  frame_struct=1 
  speed=3 
  maxDelay=0.1 
  videoBitRate=128


  for scheduler in PF MLWDF EXP
  do
		echo "${scheduler}"

		for nbUE in $(seq 10 10 50) 
 	        do
			case $scheduler in
        		"PF")
              			sched_type=1;
              			;;        
			"MLWDF")
              			sched_type=2;
              			;;        
        	
			"EXP")
        	      		sched_type=3;
        	      		;;            
        		esac


		        echo "$runSimulation ${sim} ${nbCells} ${radius} ${nbUE} ${nbVoip} ${nbVideo} ${nbBE} ${nbCBR} ${sched_type} ${frame_struct} ${speed} ${maxDelay} ${videoBitRate}"
 		
		/usr/bin/time -f "%e %P" $runSimulation ${sim} ${nbCells} ${radius} ${nbUE} ${nbVoip} ${nbVideo} ${nbBE} ${nbCBR} ${sched_type} ${frame_struct} ${speed} ${maxDelay} ${videoBitRate}  > ${currentPath}/${scheduler}/${nbUE} 

			echo ""

		done
  done

  

