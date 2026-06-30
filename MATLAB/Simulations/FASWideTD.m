%==========================================================================
%% Wideband Filter and Sum Beamformer
%
% Time domain FAS implementation for wideband signals.
%
% Expands on FAS function adding near field compensation accepting wideband 
% signals.
%
% Created By: Joseph Adam Bozzo
%==========================================================================  
function [source_output] = FASWideTD(N, f, A, grid, b, d_x, d_y, dx, dy, ntaps, Fs)
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
    % ntaps = number of FIR filter taps
    %======================================================================
    
    c = 343; % sos in m/s
    Nt = length(grid.t_array);
    source_p = zeros(N,Nt);

    % Input signal
    %input_signal = sin(2*pi*f*grid.t_array);

    % Use Sample for wideband test
    [input_signal, fileFs] = audioread("Audio\GuitarSample.wav");

    % resample if not at the sample rate
    if fileFs ~= Fs
        input_signal = resample(input_signal, Fs, fileFs);
    end

    % Ensure mono
    input_signal = input_signal(:,1);

    % Gain Scale input
    input_signal = input_signal .* 0.4;

    % Block size
    block_size = 256;

    num_blocks = floor(Nt/block_size);

    % Initialise filter memory vector
    fir_state = zeros(ntaps-1,N);

    % Has to use block based processing for filter to work with moving
    % bright point implementation.
    for block = 1:num_blocks

        % Current block indices
        start_idx = (block-1)*block_size + 1;
        end_idx = block*block_size;

        % Get current audio block
        x_block = input_signal(start_idx:end_idx);

        % Bright point position at block centre
        t = start_idx + floor(block_size/2);

        b_y = b(1,t);
        b_x = b(2,t);

        % distances as vector for wideband (needs memory)
        distance = zeros(1,N); 

        % Calculate delays
        tau = zeros(1,N);

        for i = 1:N
            % Euclidean distance between sources and bright point (in meters)
            distance(i) = sqrt(((d_x(i)-b_x)*dx)^2 + ...
                              ((d_y(i)-b_y)*dy)^2);

            tau(i) = distance(i)/c;
        end

        % Max travel time
        tau_max = max(tau);

        % Filter each speaker
        for i = 1:N

            % Relative delay in samples
            u = (tau_max - tau(i))*Fs;

            % Apply FIR delay
            [y_block,fir_state(:,i)] = fracDelFIR(x_block,ntaps,u,Fs,f,fir_state(:,i));
            
            % Near field compensation weighting
            gamma = (4*pi*distance(i))^2 / N;

            % Assign current block to output
            source_p(i,start_idx:end_idx) = A * gamma * y_block;
        end
    end
    source_output = source_p;
    toc;
end     
%==========================================================================  
