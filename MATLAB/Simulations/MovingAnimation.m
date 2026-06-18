%========================================================================== 
%% Animation of Moving Point 
%
% Shows moving motion tracked listener within sound field.
%
% Created By: Joseph Adam Bozzo
%==========================================================================

function MovingAnimation(AS, p, Nx, Ny, x_pos, y_pos, b, ps, mode, sensor_mask)

    %======================================================================
    % AS = animation step size
    % p = sensor data
    % Nx = grid points x
    % Ny = grid points y
    % x_pos = speaker x positions
    % y_pos = speaker y positions
    % b = moving focal point matrix
    % ps = plot scale
    % mode = 'rms' or 'pm'
    % sensor_mask = mic array mask
    %======================================================================

    Nt = size(p,2);

    figure;

    for n = 1:AS:Nt

        clf; % clear current frame

        % Create single frame sensor data
        frame_data = p(:,1:n);

        % Utilise prior written plot functions
        if strcmp(mode, 'pm')
            plotPressureField(frame_data, Nx, Ny, b(:,n), 'end', true, ...
                                          false, sensor_mask, x_pos, y_pos);
        elseif strcmp(mode, 'rms')        
            plotRMSPressureField(frame_data, Nx, Ny, b(:,n), true, ... 
                                          false, sensor_mask, x_pos, y_pos);
        else
            error("Mode must be 'pm' or 'rms'");
        end

        hold on;

        % Speakers as black squares
        plot(y_pos, x_pos, 'ks', ...
                           'MarkerFaceColor', 'k', ...
                           'MarkerSize', 6);

        % microphones as black circles
        if nargin >= 7
                [mic_x, mic_y] = find(sensor_mask);

                plot(mic_y, mic_x, 'ko', ...
                                   'MarkerFaceColor', 'k', ...
                                   'MarkerSize', 6);
        end

        clim(ps);
        title("Time Step: " + n);
        drawnow;
    end
end
%==========================================================================
