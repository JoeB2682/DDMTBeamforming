%==========================================================================
%% Fractional Delay FIR Filter
%
% Fractional Delay FIR Filter for use in FAS beamforming algorithm
%
% Based upon function by Neil Robertson
%
% Created By: Joseph Adam Bozzo
%==========================================================================
function [output,z] = fracDelFIR(x, ntaps, u, Fs, f, z)
    %======================================================================
    % x = Input sig (non-delayed driver sig)
    % ntaps = number of filter taps
    % u = fractional delay in samples (tau * Fs)
    % Fs = sample rate
    % z = Vector to allow for filter memory
    %======================================================================

    % Prevents division by 0
    if mod(u,1)==0
        u = u + eps;
    end

    % force correct number of samples
    n = 0:ntaps-1;

    % centre filter
    n = n - (ntaps-1)/2;

    % Bandwidth has to be Nyquist
    fc = Fs/2;
    wc = 2*pi*fc/Fs;

    % sinc fractional delay (the filter)
    h = sin(wc*(n-u))./(pi*(n-u));

    % window
    b = h .* chebwin(ntaps,70)';

    % normalise
    b = b/sum(b);

    % correct state size 
    if nargin < 6 || length(z) ~= length(b)-1
        z = zeros(length(b)-1,1);
    end

    [output,z] = filter(b,1,x,z);
end
%==========================================================================