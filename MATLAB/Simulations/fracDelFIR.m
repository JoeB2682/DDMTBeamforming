%==========================================================================
%% Fractional Delay FIR Filter
%
% Fractional Delay FIR Filter for use in FAS beamforming algorithm
%
% Based upon function by Neil Robertson
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function [output] = fracDelFIR(x, ntaps, u, Fs)
    
    %======================================================================
    % x = Input sig (non-delayed driver sig)
    % ntaps = number of filter taps
    % u = fractional delay in samples (tau * Fs)
    % Fs = sample rate
    %======================================================================

    % Prevent Division by 0
    if mod(u,1) == 0
        u = u + eps;
    end
    
    % Make fc Nyquist to cover all signals (restrict for isolation)
    fc = Fs/2;
    wc = 2*pi*fc/Fs;
    
    % Specify sample range
    N = ntaps-1;
    n = -N/2:N/2;

    % Create filter 
    h = sin(wc*(n-u))./(pi*(n-u));

    % Create window (70dB sidelobe attenuation)
    win = chebwin(ntaps,70);

    % Filter coefficients (basic FIR equation h * w)
    b = h .* win'; 
   
    % Apply to input via convolution
    output = filter(b, 1, x);
end
%==========================================================================