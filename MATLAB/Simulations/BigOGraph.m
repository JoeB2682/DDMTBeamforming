%==========================================================================
%% Big-O Time Complexity Comparison
%
% Graphs common BigO time complexities
%
% Created By: Joseph Adam Bozzo
%==========================================================================
%% System
clear all;
clc;
%==========================================================================
%% Input Size
% Define range of input sizes for complexity comparison
n = 1:100;
%==========================================================================
%% Time Complexities

% Calculate relative computational cost for each Big-O complexity
O_1       = ones(size(n));            % O(1)
O_sqrtn   = sqrt(n);                  % O(sqrt(n))
O_logn    = log2(n);                  % O(log n)
O_n       = n;                        % O(n)
O_nlogn   = n .* log2(n);             % O(n log n)
O_n2      = n.^2;                     % O(n^2)
O_n3      = n.^3;                     % O(n^3)
O_2n      = 2.^n;                     % O(2^n)
O_nfact   = factorial(n);             % O(n!)
%==========================================================================
%% Plot
figure;

semilogy(n, O_1, 'LineWidth', 2);
hold on;
semilogy(n, O_sqrtn, 'LineWidth', 2);
semilogy(n, O_logn, 'LineWidth', 2);
semilogy(n, O_n, 'LineWidth', 2);
semilogy(n, O_nlogn, 'LineWidth', 2);
semilogy(n, O_n2, 'LineWidth', 2);
semilogy(n, O_n3, 'LineWidth', 2);
semilogy(n, O_2n, 'LineWidth', 2);
semilogy(n, O_nfact, 'LineWidth', 2);
%==========================================================================
%% Graph Formatting
xlabel('Input Size, n');
ylabel('Relative Computational Cost');
title('Big-O Time Complexities');

legend( ...
    'O(1)', ...
    'O(\surd n)', ...
    'O(log n)', ...
    'O(n)', ...
    'O(n log n)', ...
    'O(n^2)', ...
    'O(n^3)', ...
    'O(2^n)', ...
    'O(n!)', ...
    'Location', 'northwest');

grid on;
xlim([1 100]);

hold off;
%==========================================================================