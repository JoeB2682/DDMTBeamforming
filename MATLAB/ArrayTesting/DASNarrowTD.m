%==========================================================================
%% Narrowband Delay and Sum Beamformer
%
% Time domain DAS implementation for narrowband signals.
%
% Simulates motion tracking for a single moving listener through per-sample
% calculation.
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function [source_output] = DASNarrowTD(N, f, A, grid, b, d_x, d_y, dx, dy)
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

    for t = 1:Nt
        
        % Current focal point position
        b_y = b(1,t);
        b_x = b(2,t);

        %disp([b_x b_y])

        % arrival time array
        tau = zeros(1,N);

        % Steering Vector (essentially)
        for i = 1:N
            % Euclidean distance between sources and bright point (in meters)
            source_distances(i) = sqrt(((d_x(i) - b_x) * dx)^2 + ...
                                       ((d_y(i) - b_y) * dy)^2);

            tau(i) = source_distances(i) / c; 
        end

        % Max travel time
        tau_max = max(tau);

        % Generate current sample for every speaker
        for i = 1:N
            relative_delay = tau_max - tau(i);
            source_p(i,t) = A*sin(2*pi*f*(grid.t_array(t)-relative_delay));
        end
    end
    source_output = source_p;
    toc;
end
%==========================================================================