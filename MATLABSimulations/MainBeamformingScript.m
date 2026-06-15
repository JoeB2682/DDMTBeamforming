%==========================================================================
%% Beamforming Main Script
%
% MSc Audio and Music Technology: Research Project
%
% Calls main beamforming functions and runs on kWave acoustic simulation.
%
% Created By: Joseph Adam Bozzo
% mhn547
% 303013659
%==========================================================================
%% System
clear all;
clc;

Fs = 44100;
t = (0:Fs-1)/Fs; % For possible other uses kwave uses own time handling
%==========================================================================
%% KWave Setup
% Create grid
N = 2^8; % for NxN
Nx = N;
Ny = N;

spacing = 5; % Grid spacing in meters
dx = spacing/Nx;
dy = spacing/Ny;

kgrid = kWaveGrid(Nx, dx, Ny, dy);

% Define medium
medium.sound_speed = 343; % sos in air
medium.density = 1.225 * ones(Nx, Ny);

% Time array (k-Wave)
t_end = 0.02; % Simulation Time
kgrid.makeTime(medium.sound_speed, [], t_end);

% Bright Point b (Constructive interfereance focal point), Sound at listener
% pos replace for X, Y of listener (Eventually motion capture) to measure
% effectiveness of beamforming to the motion captured listener.
b = [128; 128];

%==========================================================================
%% Construct Speaker Array (Source)

source.p_mask = zeros(Nx, Ny); % Source Mask

num_elements = 8;

x_pos = 10;

y_positions = round(linspace(round(Ny*0.3), round(Ny*0.7), num_elements));

for i = 1:num_elements
    source.p_mask(x_pos, y_positions(i)) = 1;
end

%==========================================================================
%% Source signal (What's Emmitted)

f0 = 1000;

source.p = zeros(num_elements, length(kgrid.t_array));

A = 1; % Scaling coeff for emitter signals

for i = 1:num_elements
    source.p(i, :) = A * sin(2*pi*f0*kgrid.t_array);
end

% Ensure correct mapping behaviour 
source.p_mode = 'additive';

%==========================================================================
%% Sensor
% Full field (records pressure at all grid points)
sensor.mask = ones(Nx, Ny);
sensor.record = {'p'};
%==========================================================================
%% Apply Beamformer
[beamformer_output] = zeros(num_elements, length(kgrid.t_array));

% DAS Narrowband
% DAS Wideband
% FAS Narrowband#
% FAS Wideband
% MVDR Narrowband
% MVDR Wideband

%source.p = beamformer_output;
%==========================================================================
%% Run Simulation
sensor_data = kspaceFirstOrder2D(kgrid, medium, source, sensor);
%==========================================================================
%% Plotting

% Setting animation name to "none" results in single image
%fieldAnimation(sensor_data, b, "none", Nx, Ny, num_elements, dx, dy, f0, num_elements); % Jack's Function
%fieldAnimation(sensor_data, b, "none", Nx, Ny, dx, dy);

%plotRMSPressureField(sensor_data.p, Nx, Ny, b, true);
plotPressureField(sensor_data.p, Nx, Ny, b, 'end', true);
%==========================================================================
