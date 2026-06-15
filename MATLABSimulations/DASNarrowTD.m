%==========================================================================
%% Narrowband Delay and Sum Beamformer
%
% Time domain DAS implementation for narrowband signals
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function [source_output] = DASNarrowTD(N, f, A, grid, Nx, Ny, b, d_x, ...
                                       d_y, dx, dy, Fs)
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
    % dx 
    % dy
    % Fs = sample rate 
    %======================================================================

    c = 343; % sos in m/s

    % As kwave flips 
    b_y = b(1);
    b_x = b(2);

    % distance array (to focal point)
    source_distances = zeros(1, N);

    % time array
    tau = zeros(1, N);

    for i = 1:N
        % Euclidean distance between sources and bright point (in meters)
        source_distances(i) = sqrt(((d_x(i) - b_x) * dx)^2 + ...
                                   ((d_y(i) - b_y) * dy)^2);

        tau(i) = source_distances(i) / c; 
    end

    % Max travel time
    tau_max = max(tau);

    % Allocate output matrix 
    source_p = zeros(N, length(grid.t_array));

    for i = 1:N
        relative_delay = tau_max - tau(i);
        source_p(i, :) = A * sin(2*pi*f*(grid.t_array - relative_delay));
    end

    source_output = source_p;
    toc;
end
%==========================================================================