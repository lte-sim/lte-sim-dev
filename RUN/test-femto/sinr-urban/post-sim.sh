#!/bin/bash


#set -e 
set -x

# PARAMETERS
RUN="../../../Debug/LTE-Sim"
SCENARIO="test-sinr-urban"

AVG=../../TOOLS/make_avg
PHY_GPUT=../../TOOLS/make_gput

_NB_SCREENSHOT=20
_COUNT=1
until [ $_COUNT -gt $_NB_SCREENSHOT ]; do

        for scenario in only_macro macro_with_streets macro_with_femto macro_with_femto_reuse
        do
    
        
		file_in=TRACE/${scenario}_${_COUNT}
		file_out=OUTPUT/${scenario}
		for ID_UE in $(seq 5001 1 5700); do

                        grep "CINR ${ID_UE}" ${file_in} | awk '{print $5}' > tmp_sinr
                        grep "CINR ${ID_UE} " ${file_in} | head -1 | awk '{print $3}' > tmp_x
                        grep "CINR ${ID_UE} " ${file_in} | head -1 | awk '{print $4}' > tmp_y
                        n=$(grep -c ".*" tmp_sinr)
                        if [ "$n" = "0" ]; then
                        echo "\t NODE ${ID_UE} not scheduled"
                        else
                        ${AVG} tmp_sinr > tmp_avg_sinr
                        paste tmp_x tmp_y tmp_avg_sinr > tmp_dati_sinr
                        cat tmp_dati_sinr >> ${file_out}
                        fi
                        rm tmp_*
        	done


	done

_COUNT=$(($_COUNT+1))
done


echo SIMULATION FINISHED!
