%==========================================================================
%% Wideband Delay and Sum Beamformer
%
% Frequency domain DAS implementation for wideband signals.
%
% Expands upon prior implemented DAS Beamformer allowing for wideband
% signal beamforming. Implemented using STFT. 
%
% This is essentially a frequency domain equivelant of the FASWideTD
% implementation.
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function [source_output] = DASWideFD(N, f, A, grid, b, d_x, d_y, dx, dy)
    tic;
    %======================================================================
    % N = size of speaker array
    % f = frequency of emitted narrowband signal 
    % A = Scaling constant
    % grid = kwave grid object (the defined soundfield area)
    % Nx = grid points in x 
    % Ny = grid points in y
    % b = bright/ focal point
    % d_x = source x positions
    % d_y = source y positions
    % dx = grid spacing in x
    % dy = grid spacing in y
    %======================================================================

    c = 343; % sos in m/s

    Nt = length(grid.t_array);

    % Allocate output matrix 
    source_p = zeros(N, Nt);

   
    source_output = source_p;
    toc;
end
%==========================================================================