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

    % Modifies jet colourmap to have white gradient around 0 due to 0
    % usually being bright green limiting visibility of wave propagation.
    imagesc(p_frame);
    axis image;
    colorbar;
    max_p = max(abs(p_frame(:)));
    clim([-max_p max_p]);
    cmap = jet(256);

    % Make a smooth white region around zero    
    mid = 128;
    width = 30;   % size of white transition

    for i = (mid-width):(mid+width)
        alpha = abs(i-mid)/width;
        cmap(i,:) = (1-alpha)*[1 1 1] + alpha*cmap(i,:);
    end

    colormap(cmap);
    title('Pressure Field');
    hold on;

    % Plots the focal point 
    if show_focal
        plot(b(1), b(2), 'rx', 'MarkerSize', 10, 'LineWidth', 2);
    end
end
%==========================================================================