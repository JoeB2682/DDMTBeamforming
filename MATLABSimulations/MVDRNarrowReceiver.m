%==========================================================================
%% MVDR Narrowband Receiver
%
% Minimum Variance Distortionless Response Receiver. 
%
% Add a noise source within the sound field then measure SNR at the
% receiver for array to evaluate effectiveness against other beamforming
% algorithms.
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function [output] = MVDRNarrowReceiver(N, f, grid, b, dx, dy, micd_x, micd_y, mic_signals)
    tic;
    %======================================================================
    % N = size of speaker array
    % f = frequency of emitted narrowband signal 
    % grid = kwave grid object (the defined soundfield area)
    % b = bright/ focal point
    % dx = grid spacing in x
    % micd_x = microphone x positions
    % micd_y = microphone y positions
    % mic_signals = received signals at microphones
    %======================================================================

    c = 343; % sos in m/s

    Nt = length(grid.t_array);

    % Allocate output matrix 
    output = zeros(N, Nt);

    % steering vecxtor
    v = zeros(1,N);

    for t = 1:Nt
        
        % Current focal point position
        b_y = b(1,t);
        b_x = b(2,t);

        % arrival vector
        tau = zeros(1,N);

        % Calculate Steering Vector using mic positions
        for i = 1:N
            % Euclidean distance between mics and bright point (in meters)
            receiver_distances(i) = sqrt(((micd_x(i) - b_x) * dx)^2 + ...
                                         ((micd_y(i) - b_y) * dy)^2);

            tau(i) = receiver_distances(i) / c; 
            
            % Calculate steering vector by converting to polar form
            v(i) = exp(-1i*2*pi*f*tau(i));
        end
    end

    % Force Column Vector 
    v = v(:);

    % Spatial Covariance Matrix (SCM) from received signals
    R = (mic_signals * mic_signals') / size(mic_signals,2);
    
    % Invert SCM
    R_v = R\v;

    % MVDR weight vector
    w = R_v / (v' * R_v);

    % Assign Output
    output = w' * mic_signals;

    toc;
end
%==========================================================================
