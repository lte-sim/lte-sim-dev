%Creating fast fading realization by using the Jakes model
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


%Questa funzione ricevuti in input il numero di cammini, la frequenza di
%lavoro, la durata della simulazione e la velocità dell'utente attraverso 
%il modello di Jakes fornisce in output un vettore di lunghezza pari al 
%numero di campioni della simulazione ed elementi pari al guadagno del 
%canale in db


T=0.5; %memoria del canale (s)

%M=[6,8,10,12];  %numero di sinusoidi
%v=[0, 3, 30];   %velocità utente (km/h)
    %v = v*(1000/(60*60));   %velocità in metri al secondo
M=6
v=3
%calcolo del vettore delle frequenze (sottoportanti):
B = 20;                      %banda
fc = 2e9;    %portante

switch B
    case 20
      f1=fc+90e3:180e3:(fc+9e3)+(49*180e3);
      f2=(fc-90e3)-(49*180e3):180e3:fc-90e3;
      f=[f2,f1];
    case 10
      f1=fc+90e3:180e3:(fc+90e3)+(24*180e3);
      f2=(fc-90e3)-(24*180e3):180e3:fc-90e3;
      f=[f2,f1];
    case 5
      f1=fc+180e3:180e3:fc+12*180e3;
      f2=fc-12*180e3:180e3:fc-180e3;
      f=[f2, fc, f1];
    case 3
      f1=fc+180e3:180e3:fc+7*180e3;
      f2=fc-7*180e3:180e3:fc-180e3;
      f=[f2, fc, f1];
    case 1.4
      f1=fc+90e3:180e3:(fc+90e3)+(2*180e3);
      f2=(fc-90e3)-(2*180e3):180e3:fc-90e3;
      f=[f2, f1];
end

file = fopen('multipathTrace.txt','at');

grid
hold on

for i=1:length(M)
    for j=1:length(v)
        
        fprintf(file, 'static float multipath_M%g_v_%g[25][500] = { \n ', M(i), v(j));
        
        for k=1:length(f)
            
            N=2*((2*M(i))+1);       %numero di cammini multipli
            wc=(2*pi)*f(k);         %pulsazione della portante 
            %wc=(2*pi)*fc;
            c=299792458;            %velocità della luce (in metri al secondo)
            wm=wc*( (v(j)*(1000/(60*60)) ) /c);         %pulsazione del max shift doppler
            t = 0.001:0.001:T;      %time range

            %inizializzaione parte reale e immaginaria risposta del canale
            hI=0;
            hQ=0;
            h=zeros(1,length(t)); %risposta in ampiezza del canale inizializzata
            multipath=zeros(1,length(t)); %risposta in ampiezza del canale in db

            for x=1:length(t)
                hI_sum=0;
                hQ_sum=0;
                for n=1:M(i);
                    Betan = (pi*n)/M(i); %variabile Beta_n
                    hI_sum = hI_sum + (cos(Betan)*cos(cos(2*pi*n/N)*wm*t(x)));
                end
                hI_sum = hI_sum*2;                    %parte di hI calcolata con sommatoria
                hI_nonsum = sqrt(2)*cos(wm*t(x));     %parte di hI calcolata senza sommatoria
                hI = hI_sum + hI_nonsum;
                hI = hI * (2/sqrt(N));
                for n=1:M(i)
                    Betan = (pi*n)/M(i);
                    hQ_sum = hQ_sum + sin(Betan)*cos(cos(2*pi*n/N)*wm*t(x));
                end
                hQ_sum = hQ_sum*2;                    %parte di hQ calcolata con sommatoria
                hQ_nonsum = sqrt(2)*cos(wm*t(x));     %parte di hQ calcolata senza sommatoria
                hQ = hQ_sum + hQ_nonsum;
                hQ = hQ * (2/sqrt(N));
                h(x)=abs(hI + j*hQ);
                multipath(x)=log10(h(x));
            end
           
            plot(t, multipath);
           
            fprintf(file, '{ \n');
            for s=1:length(multipath)
                if(s==length(multipath))
                    fprintf(file, '%g ',multipath(s));
                else
                    fprintf(file, '%g, ',multipath(s));
                end
  
            end
            fprintf(file, '\n }, \n\n');
        end
        
         
        
        fprintf(file, '\n }; \n\n');
    
        end

    end


fclose(file);
