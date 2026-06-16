%==========================================================================
%% MVDR Narrowband Beamformer
%
% Minimum Variance Distortionless Response Beamformer. 
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function [source_output] = MVDRNarrow(N, f, A, grid, b, d_x, d_y, dx, dy, micd_x, micd_y)
    tic;
    %======================================================================

    %======================================================================

    c = 343; % sos in m/s

    Nt = length(grid.t_array);

    % Allocate output matrix 
    source_p = zeros(N, Nt);

    for t = 1:Nt
        
        % Current focal point position
        b_y = b(1,t);
        b_x = b(2,t);

        % arrival vector
        tau = zeros(1,N);

        % steering vecxtor
        v = zeros(1,N);

        % Calculate Steering Vector using mic positions
        for i = 1:N
            % Euclidean distance between mics and bright point (in meters)
            receiver_distances(i) = sqrt(((micd_x(i) - b_x) * dx)^2 + ...
                                        ((micd_y(i) - b_y) * dy)^2);

            tau(i) = receiver_distances(i) / c; 
            
            % Calculate steering vector by converting to polar form
            v(i) = exp(-1i*2*pi*f*tau(i));
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

    % Now do MVDR stuff.....
    


    toc;
end
%==========================================================================
