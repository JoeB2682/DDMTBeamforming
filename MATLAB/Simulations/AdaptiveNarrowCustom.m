%==========================================================================
%% Adaptive Narrowband DAS-MVDR Beamformer
%
% Combines DAS transmit steering with MVDR adaptive spatial filtering.
%
% Created By: Joseph Adam Bozzo
%==========================================================================

function [source_output] = AdaptiveNarrowCustom(N, f, A, grid, b, ...
                                                d_x, d_y, dx, dy, ...
                                                micd_x, micd_y, mic_signals)

    tic;

    c = 343; % sos in m/s
    Nt = length(grid.t_array);

    % Allocate speaker output matrix
    source_p = zeros(N,Nt);

    % Spatial Covariance Matrix
    R = (mic_signals * mic_signals') / size(mic_signals,2);

    % Diagonal Loading
    R = R + 1e-3*trace(R)/N*eye(N);

    for t = 1:Nt

        % Current focal point position
        b_y = b(1,t);
        b_x = b(2,t);

        % MVDR time of arrival vector
        tau_tx = zeros(N,1);

        % DAS steering vector
        w_DAS = zeros(N,1);

        % Calculate DAS Weightings
        for i = 1:N
            % Euclidean distance between sources and bright point (in meters)
            source_distance = sqrt(((d_x(i)-b_x)*dx)^2 + ...
                                   ((d_y(i)-b_y)*dy)^2);

            tau_tx(i)=source_distance/c;
            w_DAS(i)=exp(-1i*2*pi*f*tau_tx(i));
        end
        
        % Initialise Steering Vector
        v = zeros(N,1);

        % Calculate Steering Vector using mic positions 
        for i = 1:N
            % Euclidean distance between mics and bright point (in meters)
            receiver_distance = sqrt(((micd_x(i)-b_x)*dx)^2 + ...
                                     ((micd_y(i)-b_y)*dy)^2);

            tau_rx = receiver_distance/c;
            v(i)=exp(-1i*2*pi*f*tau_rx);
        end

        % Force Column Steering Vector 
        v = v(:);

        % Invert SCM
        R_v = R\v;

        % MVDR weight 
        w_MVDR = R_v/(v'*R_v);

        % Combine DAS steering and MVDR Weights
        w_final = w_DAS .* w_MVDR;

        disp(norm(w_MVDR));
        disp(max(abs(w_MVDR)));

        % Generate narrowband signal
        signal = A*sin(2*pi*f*grid.t_array(t));

        % Add final weighting to signal
        for i = 1:N
            source_p(i,t)=real(w_final(i)*signal);
        end
    end
    source_output = source_p;
    toc;
end
%==========================================================================