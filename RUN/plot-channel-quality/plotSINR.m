% Creating fast fading realization by using the Jakes model
%
% Copyright (c) 2010 TELEMATICS LAB, Politecnico di Bari
% 
% This file is part of LTE-Sim
%
% LTE-Sim is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License version 3 as
% published by the Free Software Foundation;
%
% LTE-Sim is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with LTE-Sim; if not, see <http://www.gnu.org/licenses/>.
%
% Author: Giuseppe Piro <g.piro@poliba.it>
%  

clc;
close all;


 

speed = 120; %[0, 3, 30, 120];
radius = [0.5, 1];
cells = [1, 7, 19];
bw = [5, 10];
cluster = [1, 2, 3, 4, 5, 6];

idFig = 1;

for cluster_ = 1:length(cluster)

    for radius_ = 1:length(radius)
        
        figure ('Name', strcat('radius ', num2str(radius(radius_)), ' cluster ', num2str(cluster(cluster_))) );
                
        
    
        i = 1;
        for cells_ = 2:length(cells)
           
            for bw_ = 1:length(bw)
            
                %cells_7_radius_1_speed_120_bw_5_cluster_4_TB
             
                pos = load (strcat('/home/peppe/workspace/LTE-Sim/src/Simulations/ChannelQuality/output/cells_', num2str(cells(cells_)), '_radius_', num2str(radius(radius_)), '_speed_120_bw_', num2str(bw(bw_)), '_cluster_', num2str(cluster(cluster_)), '_position' ) );
                sinr = load (strcat('/home/peppe/workspace/LTE-Sim/src/Simulations/ChannelQuality/output/cells_', num2str(cells(cells_)), '_radius_', num2str(radius(radius_)), '_speed_120_bw_', num2str(bw(bw_)), '_cluster_', num2str(cluster(cluster_)), '_sinr' ) );
                cqi = load (strcat('/home/peppe/workspace/LTE-Sim/src/Simulations/ChannelQuality/output/cells_', num2str(cells(cells_)), '_radius_', num2str(radius(radius_)), '_speed_120_bw_', num2str(bw(bw_)), '_cluster_', num2str(cluster(cluster_)), '_CQI' ) );
                mcs = load (strcat('/home/peppe/workspace/LTE-Sim/src/Simulations/ChannelQuality/output/cells_', num2str(cells(cells_)), '_radius_', num2str(radius(radius_)), '_speed_120_bw_', num2str(bw(bw_)), '_cluster_', num2str(cluster(cluster_)), '_MCS' ) );
                tb = load (strcat('/home/peppe/workspace/LTE-Sim/src/Simulations/ChannelQuality/output/cells_', num2str(cells(cells_)), '_radius_', num2str(radius(radius_)), '_speed_120_bw_', num2str(bw(bw_)), '_cluster_', num2str(cluster(cluster_)), '_TB' ) );
            
                subplot(4,4,i);
                plot (pos(1:end-1), sinr);
                axis([0 (radius(radius_)*1000) -10 60])
                title (strcat('SINR  nb cells ', num2str(cells(cells_)), ', bw ',num2str(bw(bw_)) ))
                grid
                i = i+1;
                
                subplot(4,4,i);
                plot (pos(1:end-1), cqi);
                axis([0 (radius(radius_)*1000) 1 15])
                title ('CQI')
                grid
                i=i+1;
                
                subplot(4,4,i);
                plot (pos(1:end-1), mcs);
                axis([0 (radius(radius_)*1000) 1 30])
                title ('MCS')
                grid
                i=i+1;
                
                subplot(4,4,i);
                plot (pos(1:end-1), tb);
                axis([0 (radius(radius_)*1000) 10 700])
                title ('TBS')
                grid
                i=i+1;
                                
            end
            
        end



        idFig = idFig+1;
 
    end    
end;
