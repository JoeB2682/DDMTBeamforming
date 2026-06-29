%==========================================================================
%% Narrowband Filter and Sum Beamformer
%
% Time domain FAS implementation for narrowband signals.
%
% Expands on DAS function implementing phase shifts using FIR filters.
%
% Created By: Joseph Adam Bozzo
%==========================================================================  
function [source_output] = FASNarrowTD(N, f, A, grid, b, d_x, d_y, dx, dy, Fs)
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
    % Fs = sample rate
    %======================================================================
    
    c = 343; % sos in m/s
    Nt = length(grid.t_array);
    source_p = zeros(N,Nt);

    % Input signal
    input_signal = A*sin(2*pi*f*grid.t_array);

    % Block size
    block_size = 256;

    num_blocks = floor(Nt/block_size);

    for block = 1:num_blocks

        % Current block indices
        start_idx = (block-1)*block_size + 1;
        end_idx = block*block_size;

        % Get current audio block
        x_block = input_signal(start_idx:end_idx);

        % Bright point position at block centre
        t = start_idx;

        b_y = b(1,t);
        b_x = b(2,t);

        % Calculate delays
        tau = zeros(1,N);

        for i = 1:N
            % Euclidean distance between sources and bright point (in meters)
            distance = sqrt(((d_x(i)-b_x)*dx)^2 + ...
                            ((d_y(i)-b_y)*dy)^2);

            tau(i) = distance/c;
        end

        % Max travel time
        tau_max = max(tau);

        % Filter each speaker
        for i = 1:N

            % Relative delay in samples
            u = (tau_max - tau(i))*Fs;

            % Apply FIR delay
            y_block = fracDelFIR(x_block,64,u,Fs);
            
            % Assign current block to output
            source_p(i,start_idx:end_idx) = y_block;
        end
    end
    source_output = source_p;
    toc;
end     
%==========================================================================  
