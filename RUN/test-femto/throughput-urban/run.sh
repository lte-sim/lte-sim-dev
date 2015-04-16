#!/bin/bash


set -e 
set -x

# PARAMETERS
RUN="../../../Debug/LTE-Sim"
SCENARIO="test-throughput-urban"

_NB_SCREENSHOT=200
_COUNT=151
until [ $_COUNT -gt $_NB_SCREENSHOT ]; do

	for nbUE in 40 80 120
	do
	for activityFactor in 1
	do

        	#$RUN ${SCENARIO} 0 0 0 ${nbUE} ${activityFactor} > TRACE/only_macro_ue_${nbUE}_af_${activityFactor}_${_COUNT}

        	$RUN ${SCENARIO} 1 0 0 ${nbUE} ${activityFactor} > TRACE/macro_with_streets_ue_${nbUE}_af_${activityFactor}_${_COUNT}
		gzip TRACE/macro_with_streets_ue_${nbUE}_af_${activityFactor}_${_COUNT}

        	$RUN ${SCENARIO} 1 1 0 ${nbUE} ${activityFactor} > TRACE/macro_with_femto_ue_${nbUE}_af_${activityFactor}_${_COUNT}
		gzip TRACE/macro_with_femto_ue_${nbUE}_af_${activityFactor}_${_COUNT}

        	$RUN ${SCENARIO} 1 1 1 ${nbUE} ${activityFactor} > TRACE/macro_with_femto_reuse_ue_${nbUE}_af_${activityFactor}_${_COUNT}
		gzip TRACE/macro_with_femto_reuse_ue_${nbUE}_af_${activityFactor}_${_COUNT}

	done
	done

_COUNT=$(($_COUNT+1))
done

echo SIMULATION FINISHED!
