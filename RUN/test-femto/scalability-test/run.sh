#!/bin/bash

#set -e
#set -x

RUN="../../../Debug/LTE-Sim"
SCENARIO="scalability-test-macro-with-femto"


_RADIUS=500

for _nbBuildings in $(seq 0 10)
do

echo $_nbBuildings

for _nbUE in $(seq 0 10 100)
do

        /usr/bin/time -f "%e" $RUN ${SCENARIO} ${_RADIUS} ${_nbBuildings} ${_nbUE} > trace
        grep total trace   | tail -1 | awk '{print $2}'

done
done


echo SIMULATION FINISHED!

