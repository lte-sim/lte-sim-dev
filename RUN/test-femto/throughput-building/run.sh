#!/bin/bash


set -e 
set -x

# PARAMETERS
RUN="../../../Debug/LTE-Sim"
SCENARIO="test-throughput-building"

_NB_SCREENSHOT=100
nbUE=1
_COUNT=51
until [ $_COUNT -gt $_NB_SCREENSHOT ]; do


for riuso in 1 2 4
do
for activityFactor in 0.2 0.5 1
do


	trace="TRACE/ue_${nbUE}_riuso_${riuso}_af_${activityFactor}_${_COUNT}"
        $RUN ${SCENARIO} ${riuso} ${activityFactor} ${nbUE} > ${trace}
	gzip $trace

done
done


_COUNT=$(($_COUNT + 1))
done

echo SIMULATION FINISHED!
