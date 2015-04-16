function pregen_H_trace = LTE_init_generate_FF_tracev2(system_bandwidth,channel_type,nTX,nRX,trace_length_s,UE_speed,correlated_fading,frequency)
% Generate fast fading coefficients for the simulation using the Rosa Zheng
% model. This function returns a struct containing the sampled channel model
% (c) Michal Simko, modified by Josep Colom Ikuno, INTHFT, 2009

%% Generation
[fs Nfft Ntot Tsubframe nRBs] = get_fs(system_bandwidth);
ChanMod   = ChanMod_params_generator(channel_type,nTX,nRX);

number_of_realizations_per_loop = 1000;
number_of_loops = ceil(trace_length_s/Tsubframe/number_of_realizations_per_loop); % We will calculate every second separately (try not to fill up all memory)

fading_config_params.nTX                             = nTX;
fading_config_params.nRX                             = nRX;
fading_config_params.corrRX                          = ChanMod.corrRX;
fading_config_params.corrTX                          = ChanMod.corrTX;
fading_config_params.number_of_realizations_per_loop = number_of_realizations_per_loop;
fading_config_params.tap_powers                      = sqrt(10.^(ChanMod.PDP_dB(1,:)./10)) / ChanMod.normH; % Power of taps in linear and normalized to 1
fading_config_params.tap_delays                      = round(ChanMod.PDP_dB(2,:)*fs);
fading_config_params.tap_position                    = [1, find(diff(fading_config_params.tap_delays)) + 1];

% Sum up all of the taps that merge (sum power!): nearest neighbor interpolation
fading_config_params.tap_powers_used = zeros(1,max(fading_config_params.tap_delays+1));
for tap_idx = unique(fading_config_params.tap_delays)
    taps_to_sum = fading_config_params.tap_delays==tap_idx;
    fading_config_params.tap_powers_used(tap_idx+1) = sqrt(sum(fading_config_params.tap_powers(taps_to_sum).^2));
end

% fading_config_params.tap_powers_used                 = fading_config_params.tap_powers(fading_config_params.tap_position);
fading_config_params.number_of_taps                  = length(fading_config_params.tap_position);
fading_config_params.f                               = frequency;  % Frequency at which our system operates [Hz]
fading_config_params.fs                              = fs;
fading_config_params.Tsubframe                       = Tsubframe;
fading_config_params.Nfft                            = Nfft;
fading_config_params.Ntot                            = Ntot;
fading_config_params.FFT_sampling_interval           = 6; % Get 2 samples per RB (1 RB=12 subcarriers: 6 SC=1/2 RB

rosa_zheng_params.M     = 15;
rosa_zheng_params.psi   = rand(nRX, nTX,fading_config_params.number_of_taps, rosa_zheng_params.M);
rosa_zheng_params.phi   = repmat(rand(nRX, nTX,fading_config_params.number_of_taps, 1),[1 1 1 rosa_zheng_params.M]);
rosa_zheng_params.theta = rand(nRX, nTX,fading_config_params.number_of_taps, rosa_zheng_params.M);
rosa_zheng_params.v     = UE_speed; % In m/s

%% Create output, where the pregenerated fast fading will be stored
% For now we will just use a struct version of the channel trace, as we don't need the functions related to it, just the data
% pregen_H_trace                    = fast_fading_generation.PregeneratedFastFading; 
pregen_H_trace.channel_model_name = channel_type;
pregen_H_trace.fs                 = fs;
pregen_H_trace.H_RB_samples       = zeros(nRX,nTX,number_of_realizations_per_loop*number_of_loops,fading_config_params.Ntot/6);
pregen_H_trace.fft_points         = Nfft;
pregen_H_trace.t_start            = 0;
pregen_H_trace.t_end              = (number_of_realizations_per_loop*number_of_loops-1)*Tsubframe;
pregen_H_trace.t_step             = Tsubframe;
pregen_H_trace.t_length           = number_of_realizations_per_loop*number_of_loops;
pregen_H_trace.h_length           = ChanMod.PDP_dB(2,end); % Length of the channel in seconds
pregen_H_trace.UE_speed           = UE_speed;

%% Debug output
size_H_RB = pregen_H_trace.t_length*nRBs*2*8;      % By default real 64-bit (double) precision and 2 samples (SCs per RB)
total_size = (size_H_RB)/(1024*1024); % Total size in MB

%% Generate channel coefficients
% Separated the channel coefficient generation in several loops so as not
% to eat up all of the memory.
%H_fft_sampled = zeros(nRX,nTX,number_of_realizations_per_loop,fading_config_params.Ntot/6);
print_i_ = unique(floor(linspace(1,number_of_loops,10)));
current_i = 1;
for loop_idx = 1:number_of_loops
    
    if loop_idx==print_i_(current_i)
        percentage = loop_idx/number_of_loops*100;
        fprintf([num2str(percentage,'%3.2f') '%% ']);
        current_i = current_i + 1;
    end
    
    samples = (1:number_of_realizations_per_loop)+(number_of_realizations_per_loop*(loop_idx-1));
    %H_fft_sampled(:,:,samples,:) = fading_generation(loop_idx,fading_config_params,rosa_zheng_params,false);
    pregen_H_trace.H_RB_samples(:,:,samples,:) = fading_generation(loop_idx,fading_config_params,rosa_zheng_params,false,correlated_fading);
end

fprintf('\n');

% if LTE_config.show_network > 2
     %figure
     %plot(abs(squeeze(pregen_H_trace.H_RB_samples(1,1,:,3))).^2);
     %xlabel('sample');
     %ylabel('power');
     %figure
     %surf(abs(squeeze(pregen_H_trace.H_RB_samples(1,1,:,:))),'LineStyle','none');
% end

function H_fft_to_return = fading_generation(loop_idx,fading_config_params,rosa_zheng_params,plot_graphs,correlated_fading)
% Generate fast fading coefficients for the simulation. Separated the
% function due to memory issues.
% (c) Michal Simko, INTHFT, 2009
number_of_realizations = fading_config_params.number_of_realizations_per_loop;
tap_powers             = fading_config_params.tap_powers;% Power of taps
tap_delays             = fading_config_params.tap_delays;
tap_position           = fading_config_params.tap_position;
tap_powers_used        = fading_config_params.tap_powers_used;
number_of_taps         = fading_config_params.number_of_taps;
f                      = fading_config_params.f;  % Frequency at which our system operates
loop_offset            = fading_config_params.number_of_realizations_per_loop*(loop_idx-1);
v                      = rosa_zheng_params.v;
nTX                    = fading_config_params.nTX;
nRX                    = fading_config_params.nRX;
M                      = rosa_zheng_params.M;
corrRX                 = fading_config_params.corrRX;
corrTX                 = fading_config_params.corrTX;
fs                     = fading_config_params.fs;
Tsubframe              = fading_config_params.Tsubframe;
Nfft                   = fading_config_params.Nfft;
Ntot                   = fading_config_params.Ntot;
FFT_sampling_interval  = fading_config_params.FFT_sampling_interval; % Specifiy out of the useful data subcarrier's FFT, 
                                                                     % which ones to take (all of them take up too much memory)
                                                                     
%% Preallocation of the Time matrix
time_i = zeros(nRX, nTX,number_of_realizations,number_of_taps,M);
time_i_help = tap_delays(tap_position) / fs;
% We sample once per TTI. for the LL simulator this happens depending on the sampling frequency (LTE_params.Fs)
time_i_help = kron(time_i_help, ones(number_of_realizations,1)) + ((0:(number_of_realizations-1))+loop_offset).'*ones(1,length(time_i_help))*Tsubframe;
%time_i_help = repmat(time_i_help,[1, 1, M]);
time_i(1,1,:,:,1) = time_i_help;
time_i = repmat(time_i(1,1,:,:,1),[nRX, nTX, 1, 1, M]);

if correlated_fading
%% Rosa-Zheng parameters
% Yahong Rosa Zheng; Chengshan Xiao, "Simulation models with correct statistical properties for Rayleigh fading channels," Communications, IEEE Transactions on , vol.51, no.6, pp. 920-928, June 2003
% URL:
% http://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=1209292&isnumber=27219
c = 299792458;
w_d   = 2*pi*v*f/c;      % Maximum radian Doppler frequency
X_c   = zeros(nRX, nTX,number_of_realizations,number_of_taps,M);
X_s   = zeros(nRX, nTX,number_of_realizations,number_of_taps,M);
psi_n = zeros(nRX, nTX,number_of_realizations,number_of_taps,M);
theta = zeros(nRX, nTX,number_of_realizations,number_of_taps,M);
phi   = zeros(nRX, nTX,number_of_realizations,number_of_taps,M);

%% For the generation of the rosa zheng weights
psi_n(:,:,1, :, :)   = rosa_zheng_params.psi;
phi(:,:,1, :, :)     = rosa_zheng_params.phi;
theta(:,:,1, :, :)   = rosa_zheng_params.theta;
for tap_i = 1:number_of_taps
    for M_i = 1:M
        psi_n(:,:,1, tap_i, M_i) = (sqrtm(squeeze(corrRX(tap_i,:,:)))*psi_n(:,:,1, tap_i, M_i)*(sqrtm(squeeze(corrTX(tap_i,:,:)))).');
        phi(:,:,1, tap_i, M_i)   = (sqrtm(squeeze(corrRX(tap_i,:,:)))*phi(:,:,1, tap_i, M_i)*(sqrtm(squeeze(corrTX(tap_i,:,:)))).');
        theta(:,:,1, tap_i, M_i) = (sqrtm(squeeze(corrRX(tap_i,:,:)))*theta(:,:,1, tap_i, M_i)*(sqrtm(squeeze(corrTX(tap_i,:,:)))).');
    end
end

%% Calculation of H
psi_n(:,:,1, :, :) = (psi_n(:,:,1, :, :)*2 - 1) * pi;
phi(:,:,1, :, :)   = (phi(:,:,1, :, :)*2 - 1) * pi;
theta(:,:,1, :, :) = (theta(:,:,1, :, :)*2 - 1) * pi;

psi_n = repmat(psi_n(:,:,1, :, :), [1,1,number_of_realizations, 1, 1]);
phi = repmat(phi(:,:,1, :, :), [1,1,number_of_realizations, 1, 1]);
theta = repmat(theta(:,:,1, :, :), [1,1,number_of_realizations, 1, 1]);

PI_mat = zeros(nRX, nTX,number_of_realizations,number_of_taps,M);
PI_mat(1,1,1,1,:) = (1:M)*2*pi;
PI_mat = repmat(PI_mat(1,1,1,1,:), [nRX, nTX, number_of_realizations, number_of_taps, 1]);
alpha_n = (PI_mat - pi + theta) / (4*M);

X_c = cos(psi_n).*cos(w_d.*time_i.*cos(alpha_n) + phi);
X_s = sin(psi_n).*cos(w_d.*time_i.*cos(alpha_n) + phi);
X = 2/sqrt(2*M) * sum(X_c + 1i*X_s,5);
else % uncorrelated fading
    X = (randn(nRX,nTX,number_of_realizations,number_of_taps)+1i*randn(nRX,nTX,number_of_realizations,number_of_taps))/sqrt(2);
end
weight_matrix = zeros(nRX, nTX,number_of_realizations,number_of_taps);
weight_matrix(1,1,:,:) = kron(tap_powers_used(unique(tap_delays+1)), ones(number_of_realizations,1));
weight_matrix = repmat(weight_matrix(1,1,:,:),[nRX, nTX, 1, 1]);
H = zeros(nRX, nTX, number_of_realizations, tap_delays(end)+1);
H(:,:,:, tap_delays(tap_position)+1) = X .* weight_matrix;

%% Trace in freq domain
H_fft_large = fft(H,Nfft,4);

% Eliminate guardband
H_fft = H_fft_large(:,:,:,[Nfft-Ntot/2+1:Nfft 2:Ntot/2+1]);

% Do not return the channel for all subcarriers, but just a subset of it (eg. 6)
H_fft_to_return = H_fft(:,:,:,1:FFT_sampling_interval:end);

%% Plot
if plot_graphs
    figure
    plot(abs(squeeze(H_fft(1,1,:,3))));
    figure
    surf(abs(squeeze(H_fft(1,1,:,:))),'LineStyle','none');
end

function ChanMod = ChanMod_params_generator(channel_type,nTX,nRX)
% Obtain the config parameter for the channel model (taken from the LTE LL
% simulator: LTE_load_parameters_dependant.m)
% (c) Josep Colom Ikuno, INTHFT, 2009

ChanMod.type = channel_type;
ChanMod.nTX  = nTX;
ChanMod.nRX  = nRX;

switch ChanMod.type
    case {'PedA'}
        ChanMod.PDP_dB = [0 -9.7 -19.2 -22.8;  % Average power [dB]
            [ 0 110 190 410 ]*10^-9 ]; % delay (s)
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
    case {'PedB', 'PedBcorr'}
        ChanMod.PDP_dB = [0   -0.9  -4.9  -8    -7.8  -23.9; % Average power [dB]
            [ 0 200 800 1200 2300 3700 ]*10^-9 ]; % delay (s)
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
    case {'extPedB'}
        % ITU-T extended PedestrianB channel model. From "Extension of the ITU Channel Models
        % for Wideband (OFDM) Systems", Troels B. Sørensen, Preben E.
        % Mogensen, Frank Frederiksen
        ChanMod.PDP_dB = [0 -0.1 -3.7 -3.0 -0.9 -2.5 -5.0 -4.8 -20.9; % Average power [dB]
            [ 0 30 120 200 260 800 1200 2300 3700 ]*1e-9 ]; % delay (s)
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
    case {'VehA'}
        ChanMod.PDP_dB = [0   -1  -9  -10    -15  -20; % Average power [dB]
            [ 0 310 710 1090 1730 2510 ]*10^-9 ]; % delay (s)
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
    case {'VehB'}
        ChanMod.PDP_dB = [-2.5   0  -12.8  -10    -25.2  -16; % Average power [dB]
            [ 0 300 8900 12900 17100 20000 ]*10^-9]; % delay (s)
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
    case {'TU'}
        ChanMod.PDP_dB = [-5.7000 -7.6000 -10.1000 -10.2000 -10.2000 -11.5000 -13.4000 -16.3000 -16.9000 -17.1000 -17.4000,...
            -19.0000 -19.0000 -19.8000 -21.5000 -21.6000 -22.1000 -22.6000 -23.5000 -24.3000; % Average power [dB]
            0 0.2170 0.5120 0.5140 0.5170 0.6740 0.8820 1.2300 1.2870 1.3110 1.3490 1.5330 1.5350,...
            1.6220 1.8180 1.8360 1.8840 1.9430 2.0480 2.1400];% delay (s)
        ChanMod.PDP_dB(2,:) = ChanMod.PDP_dB(2,:)*10^-6;
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
    case {'RA'}
        ChanMod.PDP_dB = [-5.2000 -6.4000 -8.4000 -9.3000 -10.0000 -13.1000 -15.3000 -18.5000 -20.4000 -22.4000; % Average power [dB]
            0 0.0420 0.1010 0.1290 0.1490 0.2450 0.3120 0.4100 0.4690 0.5280]; % delay (s)
        ChanMod.PDP_dB(2,:) = ChanMod.PDP_dB(2,:)*10^-6;
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
    case {'HT'}
        ChanMod.PDP_dB = [-3.6000 -8.9000 -10.2000 -11.5000 -11.8000 -12.7000 -13.0000 -16.2000 -17.3000 -17.700 -17.6000 -22.7000,...
            -24.1000 -25.8000 -25.8000 -26.2000 -29.0000 -29.9000 -30.0000 -30.7000; % Average power [dB]
            0 0.3560 0.4410 0.5280 0.5460 0.6090 0.6250 0.8420 0.9160 0.9410 15.0000 16.1720 16.4920 16.8760 16.8820,...
            16.9780 17.6150 17.827 17.8490 18.0160]; % delay (s)
        ChanMod.PDP_dB(2,:) = ChanMod.PDP_dB(2,:)*10^-6;
        ChanMod.normH = sqrt(sum(10.^(ChanMod.PDP_dB(1,:)/10)));
end

%% Channel parameters dependent - now only the same channel parameters for each user and BS are allowed
% load Correlation Matrices
if strcmp(ChanMod.type,'PedA') || strcmp(ChanMod.type,'PedB') || strcmp(ChanMod.type,'VehA') || strcmp(ChanMod.type,'VehB') ||strcmp(ChanMod.type,'TU') || strcmp(ChanMod.type,'RA') || strcmp(ChanMod.type,'HT') || strcmp(ChanMod.type,'extPedB')
    ChanMod.corrRX = ones(size(ChanMod.PDP_dB,2),nRX,nRX);
    ChanMod.corrTX = ones(size(ChanMod.PDP_dB,2),nTX,nTX);
    for kk = 1:size(ChanMod.PDP_dB,2)
        ChanMod.corrRX(kk,:,:) = eye(nRX);
        ChanMod.corrTX(kk,:,:) = eye(nTX);
    end
elseif strcmp(ChanMod.type,'PedBcorr')
    ChanMod.corrRX = ones(size(ChanMod.PDP_dB,2),nRX,nRX);
    ChanMod.corrTX = ones(size(ChanMod.PDP_dB,2),nTX,nTX);
    for kk = 1:size(ChanMod.PDP_dB,2)
        ChanMod.corrRX(kk,:,:) = eye(nRX) + ChanMod.corr_coefRX*ones(nRX) - ChanMod.corr_coefRX*eye(nRX);
        ChanMod.corrTX(kk,:,:) = eye(nTX) + ChanMod.corr_coefTX*ones(nTX) - ChanMod.corr_coefTX*eye(nTX);
    end
end

function [fs Nfft Ntot Tsubframe nRBs] = get_fs(LTE_bandwidth)
% Obtain Obtain samplign frequency and number of subcarriers (taken from the LTE LL
% simulator: LTE_load_parameters_dependant.m
% (c) Josep Colom Ikuno, INTHFT, 2009

% Subcarrier spacing
SubcarrierSpacing = 15e3;

% Subframe length
Tsubframe = 1e-3;

% Number of resource blocks
if(LTE_bandwidth == 1.4e6)
    Nrb = 6; % number of resource blocks
else
    Nrb = (LTE_bandwidth*0.9) / 180e3; % number of resource blocks, transmission BW is 90% of the total BW
end

% Total number of useful subcarriers
Ntot = 12*Nrb;
if(LTE_bandwidth == 15e6 && SubcarrierSpacing == 15e3)
    Nfft = 1536; % number of FFT points
elseif(LTE_bandwidth == 15e6 && SubcarrierSpacing == 7.5e3)
    Nfft = 1536*2; % number of FFT points
else
    Nfft =  2^ceil(log2(Ntot)); % number of FFT points
end

fs = SubcarrierSpacing*Nfft; % sampling frequency

switch(LTE_bandwidth)
    case 1.4e6
        nRBs = 6;
    case 3e6
        nRBs = 15;
    case 5e6
        nRBs = 25;
    case 10e6
        nRBs = 50;
    case 15e6
        nRBs = 75;
    case 20e6
        nRBs = 100;
    otherwise
        error('This number of RBs is not allowed');
end
