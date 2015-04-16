# Copyright (c) 2011 Communications Research Laboratory, Ilmenau University of Technology
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
# Author: Florian Schlembach <florian.schlembach@tu-ilmenau.de>
# Author2: Jianshu Zhang <jianshu.zhang@tu-ilmenau.de>
# Author3: Yao Cheng <y.cheng@tu-ilmenau.de>
#
#  plot the sinr of the channel varyying its position according a certain mobility model
#  (i) position of the UEs
#  (ii) AMC values (sinr, cqi, mcs...)
#  (iii) handover (if occuring)
# 

# ****************************************
# NOTE:
# 	BEFORE RUN SIMULATIONS WRITE INTO
#	LTE-Sim/CONFIG/global_config
#	the following code:
#	#define AMC_MAPPING
#	#define HANDOVER_DEBUG
#	#define PLOT_USER_POSITION
#	Then, compile the software again
# ****************************************

echo "LTE-Sim"
echo "Plot Sinr"

currentPath=$(pwd)
cd $currentPath
runSimulation="../../Debug/LTE-Sim"

sh clear.sh
mkdir output


file="mob_sinrplot"
echo "run ${file}"

nbcells=3
radius=1
nbUE=1
sched_type=1
frame_struct=1
speed=30
model=2


$runSimulation MultiCellSinrPlot $nbcells $radius $nbUE $sched_type $frame_struct $speed $model > output/${file}_manh
model=0
$runSimulation MultiCellSinrPlot $nbcells $radius $nbUE $sched_type $frame_struct $speed $model > output/${file}_randdir
