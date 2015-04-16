%Creating fast fading realization by using the Rosa Zheng model
%
% Copyright (c) 2010,2011,2012,2013 TELEMATICS LAB, Politecnico di Bari
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



%parameters

%speed = [0, 3, 30 120];
speed = [150, 200, 250, 300, 350];

bandwidth = 20e6;
nb_subChannels = 100;
nTX = 1;
nRX = 1;
freq = 2e9;
duration = 1; %seconds


%channel_type = ['PedA', 'PedB', 'VehA', 'VehB'];
channel_type = ['PedA', 'PedB','VehA', 'VehB'];

%%
%code
    for i_speed = 1:length (speed)
            
            if (speed (i_speed) == 0)
                speed_m_s = (0.1 * 1000)/3600;
            else
                speed_m_s = (speed (i_speed) * 1000)/3600;
            end

            for i_ch_type = 1:4:16
                
            
                if (i_ch_type == 1)
                    pregen_H_trace = LTE_init_generate_FF_tracev2(bandwidth,'PedA',nTX,nRX,duration,speed_m_s,'true',freq);
                end
                if (i_ch_type == 2)
                    pregen_H_trace = LTE_init_generate_FF_tracev2(bandwidth,'PedB',nTX,nRX,duration,speed_m_s,'true',freq);
                end
                if (i_ch_type == 3)
                    pregen_H_trace = LTE_init_generate_FF_tracev2(bandwidth,'VehA',nTX,nRX,duration,speed_m_s,'true',freq);
                end
                if (i_ch_type == 4)
                    pregen_H_trace = LTE_init_generate_FF_tracev2(bandwidth,'VehB',nTX,nRX,duration,speed_m_s,'true',freq);
                end
                
                %create .h file for LTE-Sim
                file = fopen(strcat('fast-fading_',channel_type(i_ch_type:i_ch_type+3),'_speed_', num2str(speed (i_speed)),'.h'),'at');

                %add copy rigth LTE-Sim
                fprintf(file, '/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */\n');
                fprintf(file, '/*\n');
                fprintf(file, ' * Copyright (c) 2010,2011,2012,2013 TELEMATICS LAB, Politecnico di Bari\n');
                fprintf(file, ' *\n');
                fprintf(file, ' * This file is part of LTE-Sim\n');
                fprintf(file, ' *\n');
                fprintf(file, ' * LTE-Sim is free software; you can redistribute it and/or modify\n');
                fprintf(file, ' * it under the terms of the GNU General Public License version 3 as\n');
                fprintf(file, ' * published by the Free Software Foundation;\n');
                fprintf(file, ' *\n');
                fprintf(file, ' * LTE-Sim is distributed in the hope that it will be useful,\n');
                fprintf(file, ' * but WITHOUT ANY WARRANTY; without even the implied warranty of\n');
                fprintf(file, ' * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n');
                fprintf(file, ' * GNU General Public License for more details.\n');
                fprintf(file, ' *\n');
                fprintf(file, ' * You should have received a copy of the GNU General Public License\n');
                fprintf(file, ' * along with LTE-Sim; if not, see <http://www.gnu.org/licenses/>.\n');
                fprintf(file, ' *\n');
                fprintf(file, ' * Author: Giuseppe Piro <g.piro@poliba.it>\n');
                fprintf(file, ' */  \n\n\n');

                fprintf(file, strcat('#ifndef FAST_FADING_',channel_type(i_ch_type:i_ch_type+3),'_speed_', num2str(speed(i_speed)),'_H_\n'));
                fprintf(file, strcat('#define FAST_FADING_',channel_type(i_ch_type:i_ch_type+3),'_speed_', num2str(speed(i_speed)),'_H_\n\n\n'));



                %print traces into the file
                fprintf(file, strcat('static float ff_',channel_type(i_ch_type:i_ch_type+3),'_speed_', num2str(speed(i_speed))));
                fprintf(file, '[%g][%g] = { \n ', nb_subChannels, duration*1000);

                for i_rb = 1:nb_subChannels
                    fprintf(file, '{ \n');
                    for i_time = 1:(duration*1000)
                        if(i_time == duration*1000)
                            fprintf(file, '%g ', abs(pregen_H_trace.H_RB_samples(1,1,i_time,i_rb)));
                        else
                            fprintf(file, '%g, ',abs(pregen_H_trace.H_RB_samples(1,1,i_time,i_rb)));
                        end
                    end
                    fprintf(file, '\n },\n');
                end
                fprintf(file, '\n }; \n\n');


                fprintf(file, strcat('#endif /* FAST_FADING_FAST_FADING_',channel_type(i_ch_type:i_ch_type+3),'_speed_', num2str(speed(i_speed)),'_H_ */ \n'));
                %close file created file
                fclose(file);
                

            end
    end

