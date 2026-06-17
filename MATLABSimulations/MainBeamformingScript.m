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

%==========================================================================
%% Construct Speaker Array (Sources)

source_speakers.p_mask = zeros(Nx, Ny); % Speaker Array Mask
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
    source_speakers.p_mask(x_positions(i), y_positions(i)) = 1;
end

%==========================================================================
%% Fixed Sources (Aditional emitters in sound field) 

source_fixed.p_mask = zeros(Nx, Ny);

% Noise Source
noisesrc_x = 75;
noisesrc_y = round(Ny/2);

source_fixed.p_mask(noisesrc_x, noisesrc_y) = 1;

% Generate and normalise
noise_signal = randn(1, length(kgrid.t_array));
noise_signal = noise_signal / max(abs(noise_signal));

%==========================================================================
%% Bright Point (Focal Point)

% Bright Point b (Constructive interfereance focal point), Sound at listener
% pos replace for X, Y of listener (Eventually motion capture) to measure
% effectiveness of beamforming to the motion captured listener.
% Uses matrix of two points to move the point between 2 locations (simulates)
% motion tracking.

Nt = length(kgrid.t_array);

b = zeros(2,Nt); % BP Matrix

% Moves point from leftmost speaker to rightmost,
% Extremely bloody annoying as kgrid swaps rows and columns, be aware of
% this in plotting functions!!!!
offset = 108;
b(1,:) = linspace(min(y_positions), max(y_positions), Nt);
b(2,:) = x_positions(1) + offset;
%==========================================================================
%% Sensors (receivers)

% Needs masks for both full field and mic array, observing affect of mic
% array is visible on full field through beamforming algorithms.

sensor.mask = zeros(Nx, Ny);
num_mics = 25;
mic_spacing_m = 0.15;

% Calculate total microphone array length and start point
total_mic_length_m = (num_mics - 1) * mic_spacing_m;

grid_center_y_m = (Ny / 2) * dy;
start_y_m = grid_center_y_m - (total_mic_length_m / 2);

% Define horizontal microphone line position
mic_x_pos = 150;   % parallel to speaker array
mic_x_positions = mic_x_pos * ones(1, num_mics);

% Generate physical positions in metres, convert to grid indices
mic_y_positions_m = start_y_m + (0:num_mics-1) * mic_spacing_m;
mic_y_positions = round(mic_y_positions_m / dy);

% Create microphone array
for i = 1:num_mics
    sensor.mask(mic_x_positions(i), mic_y_positions(i)) = 1;
end

sensor.record = {'p'};

% Needs sensor field mask for plots to work
sensor_field.mask = ones(Nx, Ny);
sensor_field.record = {'p'};

%==========================================================================
%% Apply Transmit Beamformer

% Creates speaker signals
[beamformer_output] = DASNarrowTD(num_elements, 1000, 1, kgrid, b, x_positions, y_positions, dx, dy);

%==========================================================================
%% Combine Source Signals

% Combined source mask (Speaker + everything else)
source.p_mask = source_speakers.p_mask + source_fixed.p_mask;
source.p = zeros(num_elements + 1, length(kgrid.t_array));

% Speaker signals (assign beamformer output)
source.p(1:num_elements, :) = beamformer_output;

% Noise source signal (assign noise signal)
source.p(num_elements + 1, :) = noise_signal;

%==========================================================================
%% Run Simulation

plot_scale = [-0.25, 0.25]; 

% Microphone array data (for adaptive beamforming)
mic_data = kspaceFirstOrder2D(kgrid, medium, source, sensor, ...
                                 'PlotSim', true, ...
                                 'PlotScale', plot_scale, ...
                                 'PlotLayout', false, ...
                                 'DisplayMask', source.p_mask);


% Full field data (for plotting/animation)
field_data = kspaceFirstOrder2D(kgrid, medium, source, sensor_field, ...
                                 'PlotSim', false, ...
                                 'PlotLayout', false);
%========================================================================== 
%% Apply Receive Algorithm 

% Apply adaptive algorithm

% Extract microphone signals
mic_signals = mic_data.p;

[mvdr_output] = MVDRNarrowReceiver(num_mics, 1000, kgrid, b,dx, dy, mic_x_positions, ...
                                                   mic_y_positions, mic_signals);

% Assign back to mic output
mic_data.p = mvdr_output;
%==========================================================================
%% Plotting

%plotRMSPressureField(field_data.p, Nx, Ny, b, true, true, sensor.mask, ... 
%                                              x_positions, y_positions);

%plotPressureField(field_data.p, Nx, Ny, b, 'end', true, true, sensor.mask, ...
%                                              x_positions, y_positions);


MovingAnimation(20, field_data.p, Nx, Ny, x_positions, y_positions,...
                                                        b, plot_scale, 'pm', ...
                                                        sensor.mask);

%==========================================================================
