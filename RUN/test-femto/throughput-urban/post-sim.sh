#!/bin/bash


set -x

# PARAMETERS
RUN="../../../Debug/LTE-Sim"
SCENARIO="test-throughput-urban"
AVG="../../../TOOLS/make_avg"
TPUT="../../../TOOLS/make_goodput"
TIME=11


_NB_SCREENSHOT=200
_COUNT=1

MINIDUE=5001;

until [ $_COUNT -gt $_NB_SCREENSHOT ]; do

	for nbUE in 40 80 120
	do
	for activityFactor in 1
	do

	for scenario in macro_with_streets 
	do

        	FILE_IN="TRACE/${scenario}_ue_${nbUE}_af_${activityFactor}_${_COUNT}"
        	FILE_OUT="OUTPUT/${scenario}_ue_${nbUE}_af_${activityFactor}"

		if [ -f ${FILE_IN}.gz ];then

                cd  TRACE; unp ${scenario}_ue_${nbUE}_af_${activityFactor}_${_COUNT}.gz; cd ..
                n=$(grep -c "SIMULATOR_DEBUG" ${FILE_IN})
                if [ "$n" = "0" ]; then
                        echo "$FILE_IN" >> OUTPUT/simulate_again
                else

	
			IDUE=$MINIDUE
			MAXIDUE=$(($IDUE + $nbUE - 1))
			until [ $IDUE -gt $MAXIDUE ]; do
                                                
				npkt=$(grep "^RX" $FILE_IN | grep -c "DST $IDUE")
				if [ "$npkt" = "0" ]; then
                        		echo "ue $IDUE not scheduled " 
                		else 
					flag=$(grep "^RX" $FILE_IN | grep "DST $IDUE" | tail -1 | awk '{print $15}')
					target=$(grep "^UE $IDUE" $FILE_IN | awk '{print $9}')
					grep "^RX" $FILE_IN | grep "DST $IDUE" | awk '{print $8}'> tmp_rx
                        		tput=$(${TPUT} tmp_rx ${TIME})

					echo "$IDUE $npkt $tput $flag $target" >> $FILE_OUT
					rm tmp*
                                fi                
			IDUE=$(($IDUE+1))
			done


                fi

        	rm ${FILE_IN}
        	fi



	done

	done
	done

_COUNT=$(($_COUNT+1))
done

echo SIMULATION FINISHED!
