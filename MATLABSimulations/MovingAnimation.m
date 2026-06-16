%========================================================================== 
%% Animation of Moving Point 
%
% Shows moving motion tracked listener within sound field.
%
% Created By: Joseph Adam Bozzo
%==========================================================================

function MovingAnimation(AS, p, Nx, Ny, x_pos, y_pos, b, ps, mode)

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
    %======================================================================

    Nt = size(p,2);

    figure;

    for n = 1:AS:Nt

        clf; % clear current frame

        % Create single frame sensor data
        frame_data = p(:,1:n);

        % Utilise prior written plot functions
        if strcmp(mode, 'pm')
            plotPressureField(frame_data, Nx, Ny, b(:,n), 'end', true);
        elseif strcmp(mode, 'rms')        
            plotRMSPressureField(frame_data, Nx, Ny, b(:,n), true);
        else
            error("Mode must be 'pm' or 'rms'");
        end

        hold on;

        % Speaker locations as black circles
        plot(y_pos, x_pos, 'ko');

        clim(ps);
        title("Time Step: " + n);
        drawnow;
    end
end
%==========================================================================
