#!/bin/bash


set -e 
set -x

# PARAMETERS
RUN="../../../Debug/LTE-Sim"
SCENARIO="test-sinr-femto"


for riuso in 1 2 4
do
for af in 1
do

        $RUN ${SCENARIO} ${riuso} ${af}> TRACE/riuso_${riuso}_af_${af}

	file_in=TRACE/riuso_${riuso}_af_${af}
	file_out_sinr=OUTPUT/SINR_riuso_${riuso}_af_${af}
	file_out_mcs=OUTPUT/MCS_riuso_${riuso}_af_${af}
	file_out_tbs=OUTPUT/TBS_riuso_${riuso}_af_${af}

	grep "SINR" ${file_in} | awk '{print $3,$4,$5}' > ${file_out_sinr}
	grep "SINR" ${file_in} | awk '{print $3,$4,$6}' > ${file_out_mcs}
	grep "SINR" ${file_in} | awk '{print $3,$4,$7}' > ${file_out_tbs}

done
done

echo SIMULATION FINISHED!
