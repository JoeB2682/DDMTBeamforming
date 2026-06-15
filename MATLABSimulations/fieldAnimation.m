%==========================================================================
%% Field Animation
%
% NOT MY CODE! Taken from MSc thesis: Delivering Dynamic Personal Sound
% with Wideband Beamforming in
% Compact Arrays
%
% Created By: Jack Webb
%==========================================================================

function fieldAnimation(sensor_data, b, name, Nx, Ny, d, dx, dy, f, num_sources)
%% Creates animation of pressure field over time. Note very time consuming to create animation and leads to poor results
% p = sensor_data.p (collapsed matrix of pressure values over time)
% b = matrix of bright point coordinates over time
% name = filename "string + .mp4" for animation, "none" for still image
% Nx = number of grid points in x direction
% Ny = number of grid points in y direction
% dx = spacing of grid in x direction (m)
% dy = spacing of grid in y direction (m)
% f = source frequency (Hz)
% NB - animation is fairly defunct - too expensive. To get this to work for an
% image, the index at which p_final is recorded must be adjusted in
% kspaceFirstOrder2D, otherwise end frames of silence will be all that is
% recorded
    
% If animation specified
if name ~= "none"
    % Create axis
    x_vec = (0:Nx-1) * dx;
    y_vec = (-Ny/2:Ny/2 - 1) * dx;
    
    % Create figure
    figure();

    % Get data
    p = sensor_data.p;

    % Create video writer object
    video = VideoWriter(name, 'MPEG-4');
    
    % Set the frame rate
    video.FrameRate = 100;
    open(video);
    
    % Reshape pressure data
    reshaped_data = cell(1, width(p));
    for n = 1:width(p)
        line_data = sensor_data.p;
    end
    
    % Initialize the plot
    SPcolor = pcolor(y_vec, x_vec, reshaped_data{1});
    shading interp;
    axis image;
    colormap bone;
    xlabel('Lateral Distance [m]');
    ylabel('Axial Distance [m]');
    title(strcat(source_freq + "Hz: " + num_sources + " sources spaced " + d*dy + "m"));
    hold on;
    SScatter = scatter(b(2, 1).*dy - 128*dy, b(1, 1).*dx, 'k', 'filled');
    hold off;
    
    % Loop over frames and update plot
    for n = 1:width(p)
        % Update field data
        set(SPcolor, 'CData', reshaped_data{n});
        
        % Update scatter data
        set(SScatter, 'XData', b(2, n).*dy - 128*dy, 'YData', b(1, n).*dx);
        
        % Capture frame
        frame = getframe(gcf);
        writeVideo(video, frame);
    
        % Show progress
        if mod(n,100) == 0
            disp("Current percentage complete: " + (n/width(p) * 100))
        end
    end
    
    % Close video writer object
    close(video);

else
    % Create simple plot of end pressure value

    % Create axis
    x_vec = (0:Nx-1) * dx;
    y_vec = (-Ny/2:Ny/2 - 1) * dx;
    % Plot field
    figure();
    hold on;
    pcolor(y_vec, x_vec, sensor_data.p);
    
    % Add bright point marker NB k-wave convention has x and y axes swapped
    for i=1:1
        scatter(b(2, i).*dy - 128*dy, b(1, i).*dx, 'k', 'filled');
    end
    colormap(getColorMap);
    shading interp;
    axis image;
    xlabel('Lateral Distance [m]');
    ylabel('Axial Distance [m]');
    title(strcat(f + ", " + num_sources + " sources spaced " + d*dy + "m"));
    hold off;
end
end


%{
function fieldAnimation(sensor_data, b, name, Nx, Ny, dx, dy)

p = sensor_data.p;

figure();

for n = 1:size(p,2)

    imagesc(p(:, n));   % correct for line sensor
    colormap bone;
    axis tight;
    title("Time step " + n);

    hold on;
    scatter(b(2)*dy, b(1)*dx, 'r', 'filled');
    hold off;

    drawnow;
end

end
%}