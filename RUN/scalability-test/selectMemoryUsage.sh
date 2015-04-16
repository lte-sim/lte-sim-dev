# Copyright (c) 2010 TELEMATICS LAB, Politecnico di Bari
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
# Author: Giuseppe Piro <g.piro@poliba.it>


  rm memoryUsage_MT memoryUsage_PF

  for nbUE in $(seq 10 10 50) # users/cell
  do
    for scheduler in PF MLWDF EXP
    do

        grep total ${scheduler}/${nbUE}   | tail -1 | awk '{print $2}' >> memoryUsage_${scheduler}

    done
  done

  

