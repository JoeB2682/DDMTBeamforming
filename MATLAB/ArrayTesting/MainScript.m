

function [y] = Sine(F, A, dur, Fs)
    N = Fs * dur;                         
    Phase_Increment = 2*pi*(F/Fs);
    x = (0:N-1) * Phase_Increment;          
    y = A * sin(x);                        
end 

y = Sine(1000, 0.05, 3, 48000);

Fs = 48000;

deviceWriter = audioDeviceWriter( ...
    'SampleRate', Fs);

y = y(:);

block = 1024;

for n = 1:block:length(y)-block

    out = zeros(block,8);

    % speaker 5
    out(:,5) = y(n:n+block-1);

    deviceWriter(out);

end

release(deviceWriter);


