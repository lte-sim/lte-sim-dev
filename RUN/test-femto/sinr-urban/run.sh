#!/bin/bash


set -e 
set -x

# PARAMETERS
RUN="../../../Debug/LTE-Sim"
SCENARIO="test-sinr-urban"

_NB_SCREENSHOT=30
_COUNT=1
until [ $_COUNT -gt $_NB_SCREENSHOT ]; do

        $RUN ${SCENARIO} 0 0 0  > TRACE/only_macro_${_COUNT}
	file_in=TRACE/only_macro
	file_out=OUTPUT/only_macro
	grep "SINR" ${file_in} | awk '{print $3,$4,$5}' > ${file_out}


        $RUN ${SCENARIO} 1 0 0  > TRACE/macro_with_streets_${_COUNT}
        file_in=TRACE/macro_with_streets
        file_out=OUTPUT/macro_with_streets
        grep "SINR" ${file_in} | awk '{print $3,$4,$5}' > ${file_out}


        $RUN ${SCENARIO} 1 1 0  > TRACE/macro_with_femto_${_COUNT}
        file_in=TRACE/macro_with_femto
        file_out=OUTPUT/macro_with_femto
        grep "SINR" ${file_in} | awk '{print $3,$4,$5}' > ${file_out}


        $RUN ${SCENARIO} 1 1 1  > TRACE/macro_with_femto_reuse_${_COUNT}
        file_in=TRACE/macro_with_femto_reuse
        file_out=OUTPUT/macro_with_femto_reuse
        grep "SINR" ${file_in} | awk '{print $3,$4,$5}' > ${file_out}



_COUNT=$(($_COUNT+1))
done


echo SIMULATION FINISHED!
