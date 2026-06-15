%==========================================================================
%% PlotPressurefield
%
% Plots Pressure field (Abstracts main script)
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function plotPressureField (sensor_data, Nx, Ny, b, frame, show_focal)

    %======================================================================
    % sensor_data = defined sensor data (pressure) from kwave sim
    % Nx = grid points in x
    % Ny = grid points in y
    % b = 'bright' / focal point
    % frame = 'mid'||'end' show either middle or final frame of simulation
    % show_focal = boolean to show point b 
    %======================================================================
    
    p_final = reshape(sensor_data(:, end), Nx, Ny); % Final Frame
    p_mid = reshape(sensor_data(:, round(end/2)), Nx, Ny); % Mid Frame

    p_frame = p_final;

    if strcmp(frame, 'mid')
        p_frame = p_mid;
    else 
        p_frame = p_final;
    end

    imagesc(p_frame);
    axis image;
    colorbar;
    %clim([-0.1 0.1]);
    title('Pressure Field');

    hold on;

    if show_focal
        plot(b(2), b(1), 'rx', 'MarkerSize', 10, 'LineWidth', 2);
    end
end
%==========================================================================