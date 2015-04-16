set -x
set -e

_COUNT=1
_NB_SEEDS=40

until [ $_COUNT -gt $_NB_SEEDS ]; do
for sched in  1 4 5 6 		#scheduling algorithm
do
for ue in 10 20 30 40		#number of users
do
for del in 0.04 0.06 0.08 0.1 	#target delay
do	
for v in 3 120			#users speed
do
	../../LTE-Sim SingleCellWithI 19 0.5 $ue 1 1 1 0 $sched 1 $v $del 128 > TRACE/SCHED_${sched}_UE_${ue}_V_${v}_D_${del}_$_COUNT
	cd TRACE
	gzip SCHED_${sched}_UE_${ue}_V_${v}_D_${del}_$_COUNT
	cd ..
done
done
done
done
_COUNT=$(($_COUNT + 1))
done
