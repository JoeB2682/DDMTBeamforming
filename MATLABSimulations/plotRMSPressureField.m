%==========================================================================
%% PlotRMSPressurefield
%
% Plots RMS Pressure field (Abstracts main script)
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function plotRMSPressureField (sensor_data, Nx, Ny, b, show_focal)

    %======================================================================
    % sensor_data = defined sensor data (pressure) from kwave sim
    % Nx = grid points in x
    % Ny = grid points in y
    % b = 'bright' / focal point
    % show_focal = boolean to show point b 
    %======================================================================

    p = sensor_data;
    p_rms = sqrt(mean(p.^2, 2));
    p_rms = reshape(p_rms, Nx, Ny);

    imagesc(p_rms);
    axis image;
    colorbar;
    title('RMS Pressure Field');

    hold on;

    if show_focal
        plot(b(2), b(1), 'rx', 'MarkerSize', 10, 'LineWidth', 2);
    end
end
%==========================================================================