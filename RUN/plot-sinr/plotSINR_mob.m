% Copyright (c) 2011 Communications Research Laboratory, Ilmenau University of Technology
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
% Author: Florian Schlembach <florian.schlembach@tu-ilmenau.de>
% Author2: Jianshu Zhang <jianshu.zhang@tu-ilmenau.de>
%  

clc;close all;clear all;

fontsize = 16;

% load trace files
file = 'mob_sinrplot'; 
eNBs = load (strcat('output/',file,'_manh_enbs'));
nbCells = size(eNBs,1);

sinr_manh = load (strcat('output/',file,'_manh_sinr'));
sinr_randdir = load (strcat('output/',file,'_randdir_sinr'));

hos_manh = load (strcat('output/',file,'_manh_hos'));
hos_randdir = load (strcat('output/',file,'_randdir_hos'));

figure (1);
%% Plot movement of the nodes
subplot(1,2,1);
r = 1000;
grey = [0.7 0.7 0.7];

% plot eNBs with cell 'circle'
for i=1:nbCells
    center_x = eNBs(i,1);
    center_y = eNBs(i,2);
    [x,y,z] = cylinder(r,200);
      
    node = plot (center_x,center_y,'kx','LineWidth',14);hold on;
    circle = plot(x(1,:)+eNBs(i,1),y(1,:)+eNBs(i,2),'Color',grey,'LineWidth',2);hold on;
    
    % create hexagon
    t=linspace(0,2*pi,7);
    ahx=center_x+r*cos(t);
    bhx=center_y+r*sin(t);
    plot(ahx,bhx,'-.','Color',grey,'LineWidth',2); hold on;
end

% read traces from files
pos_x_manh = load (strcat('output/',file,'_manh_X'));
pos_y_manh = load (strcat('output/',file,'_manh_Y'));
pos_x_randdir = load (strcat('output/',file,'_randdir_X'));
pos_y_randdir = load (strcat('output/',file,'_randdir_Y'));

%plot paths
manh = plot(pos_x_manh, pos_y_manh,'b-','LineWidth',2); hold on;
randdir = plot(pos_x_randdir, pos_y_randdir,'r-.','LineWidth',2);

starting = plot(pos_x_manh(1,1),pos_y_manh(1,1),'g>','LineWidth',2.5); % plot starting point
ending = plot(pos_x_manh(end,1),pos_y_manh(end,1),'rO','LineWidth',2.5); % plot end point
plot(pos_x_randdir(1,1),pos_y_randdir(1,1),'g>','LineWidth',2.5); % plot starting point
plot(pos_x_randdir(end,1),pos_y_randdir(end,1),'rO','LineWidth',2.5); % plot end point

axis([min(eNBs(:,1))-r max(eNBs(:,1))+r min(eNBs(:,2))-r max(eNBs(:,2))+r],'equal');
title(sprintf('Manhattan Mobility Model, nbCells = %d',nbCells),'FontSize',fontsize+2);
legend([node(1) circle(1) manh(1) randdir(1) starting(1) ending(1)], {'eNB position','cell edge','Manhattan Model','RandomDirection Model','starting point','end point'},4);
xlabel('X-coordinate','FontSize',fontsize);ylabel('Y-coordinate','FontSize',fontsize);set(gca,'FontSize',fontsize);
grid on;

%% Plot SINR
subplot(1,2,2);
t_manh = 0:0.001:size(sinr_manh,1)/1000-0.001;
t_randdir = 0:0.001:size(sinr_randdir,1)/1000-0.001;

h_manh = plot(t_manh,sinr_manh,'b-');hold on;
text(58.19,sinr_manh(58.19*1000),'\leftarrow Manhattan Model','FontSize',fontsize-2,'Color','Blue','HorizontalAlignment','left')
h_randdir = plot(t_randdir,sinr_randdir,'r-');
text(255.6,sinr_randdir(255.6*1000),'\leftarrow RandomDirection Model','FontSize',fontsize-2,'Color','Red','HorizontalAlignment','left')

% plot handovers
lim = get(gca,'YLim'); 
ho_manh = arrayfun(@(hos_manh) line([hos_manh hos_manh],lim,'LineWidth',2,'LineStyle','--'),hos_manh);
ho_randdir = arrayfun(@(hos_randdir) line([hos_randdir hos_randdir],lim,'LineWidth',2,'LineStyle','--','Color','red'),hos_randdir);

axis([0 t_manh(end) -10 60]);set(gca,'FontSize',fontsize);
legend([h_manh(1) h_randdir(1) ho_manh(1) ho_randdir(1) starting(1) ending(1)],{'Manhattan Model','RandomDirection Model','HO Manhattan','HO RandomDirection','starting point','end point'},4);
xlabel('time [sec]','FontSize',14);ylabel('SINR [dB]','FontSize',fontsize);set(gca,'FontSize',fontsize);
title('SINR','FontSize',fontsize+2);
grid on;

% remove margin around plot
set(gca, 'Position', get(gca, 'OuterPosition') - get(gca, 'TightInset') * [-1 0 1 0; 0 -1 0 1; 0 0 1 0; 0 0 0 1]);

% % print all this stuff to an eps figure
% set(gcf, 'renderer', 'painters');
% print(gcf, '-depsc2', 'mob.eps');