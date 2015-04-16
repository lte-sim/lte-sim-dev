#!/bin/bash


#set -e 
set -x


# PARAMETERS
RUN="../../../Debug/LTE-Sim"
SCENARIO="test-throughput-building"
AVG="../../../TOOLS/make_avg"
TPUT="../../../TOOLS/make_goodput"

nbUE=1

_NB_SCREENSHOT=300
_COUNT=1

IDUE=3001;
NBFEMTO=25;
NBUEINFEMTO=1;
MAXIDUE=$((NBFEMTO*NBUEINFEMTO +IDUE-1));
TIME=30.0;
BW=5000000;

until [ $_COUNT -gt $_NB_SCREENSHOT ]; do
for riuso in 1
do

BWTOT=$(($BW * $riuso))

for activityFactor in 0.5 1
do

        IDUE=3001;
	
        echo "POST SIM FOR {SCENARIO} ${riuso} ${activityFactor} ${nbUE} ${_COUNT}"

	FILE_IN="TRACE/ue_${nbUE}_riuso_${riuso}_af_${activityFactor}_${_COUNT}"
	TPUT_BUILDING_AGGREGATE="OUTPUT/TPUT_BUILDING_AGGREGATE_ue_${nbUE}_riuso_${riuso}_af_${activityFactor}"
	TPUT_CELL_AGGREGATE="OUTPUT/TPUT_CELL_AGGREGATE_ue_${nbUE}_riuso_${riuso}_af_${activityFactor}"
	TPUT_USER="OUTPUT/TPUT_USER_ue_${nbUE}_riuso_${riuso}_af_${activityFactor}"
	EFF_BUILDING_AGGREGATE="OUTPUT/EFF_BUILDING_AGGREGATE_ue_${nbUE}_riuso_${riuso}_af_${activityFactor}"
        EFF_CELL_AGGREGATE="OUTPUT/EFF_CELL_AGGREGATE_ue_${nbUE}_riuso_${riuso}_af_${activityFactor}"
        EFF_USER="OUTPUT/EFF_USER_ue_${nbUE}_riuso_${riuso}_af_${activityFactor}"

	if [ -f ${FILE_IN}.gz ];then
		
		cd  TRACE; unp ue_${nbUE}_riuso_${riuso}_af_${activityFactor}_${_COUNT}.gz; cd ..
                n=$(grep -c "SIMULATOR_DEBUG" ${FILE_IN})
                if [ "$n" = "0" ]; then                                       
			echo "$FILE_IN" >> simulate_again
		else
		
	
			until [ $IDUE -gt $MAXIDUE ]; do
		
				rx=$(grep -c "^UE $IDUE position" ${FILE_IN})
				if [ "$rx" = "0" ]; then
                        		echo "USER NOT FOUND" 
					IDUE=$(($IDUE+$NBUEINFEMTO))
               	 	        else
	
					for i in $(seq ${IDUE} $(($IDUE+$NBUEINFEMTO-1)))
					do
			 			grep "^RX" $FILE_IN | grep " DST $i D" | awk '{print $8}' > tmp_rx_user	
			 			cat tmp_rx_user >> tmp_rx_cell
						cat tmp_rx_user >> tmp_rx_building	
					
						${TPUT} tmp_rx_user ${TIME} >> ${TPUT_USER}
						rm tmp_rx_user

					done
					IDUE=$(($IDUE+$NBUEINFEMTO))
					${TPUT} tmp_rx_cell ${TIME} >> ${TPUT_CELL_AGGREGATE}
					rm tmp_rx_cell

				fi		
			done

			${TPUT} tmp_rx_building ${TIME} >> ${TPUT_BUILDING_AGGREGATE}			
			rm tmp_rx_building
		fi

	rm ${FILE_IN}
	fi

done
done

_COUNT=$(($_COUNT+1))

done

echo SIMULATION FINISHED!
