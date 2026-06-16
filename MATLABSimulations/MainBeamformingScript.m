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

num_elements = 25;

element_spacing_m = 0.15; 

% Calculate total array length in meters and the start point 
total_array_length_m = (num_elements - 1) * element_spacing_m;
grid_center_y_m = (Ny / 2) * dy; 
start_y_m = grid_center_y_m - (total_array_length_m / 2);

% Define horizontal line array position
x_pos = 20;
x_positions = x_pos * ones(1, num_elements);

% Generate physical positions in meters, then convert to closest grid index
y_positions_m = start_y_m + (0:num_elements-1) * element_spacing_m;
y_positions = round(y_positions_m / dy);

for i = 1:num_elements
    source.p_mask(x_positions(i), y_positions(i)) = 1;
end

%==========================================================================
%% Sensor
% Full field (records pressure at all grid points)
sensor.mask = ones(Nx, Ny);
sensor.record = {'p'};
%==========================================================================
%% Apply Beamformer

[beamformer_output] = DASNarrowTD(num_elements, 1000, 1, kgrid, Nx, ...
                              Ny, b, x_positions, y_positions, dx, dy, Fs);

source.p = beamformer_output;
%==========================================================================
%% Run Simulation

plot_scale = [-0.25, 0.25]; 

sensor_data = kspaceFirstOrder2D(kgrid, medium, source, sensor, ...
                                 'PlotSim', true, ...
                                 'PlotScale', plot_scale, ...
                                 'PlotLayout', false, ...
                                 'DisplayMask', source.p_mask);
%==========================================================================
%% Plotting
plotRMSPressureField(sensor_data.p, Nx, Ny, b, true);
%plotPressureField(sensor_data.p, Nx, Ny, b, 'end', true);
%==========================================================================
