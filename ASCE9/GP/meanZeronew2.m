function A = meanZeronew2(hyp, x, i)

% Zero mean function. The mean function does not have any parameters.
%
% m(x) = 0
%
% Copyright (c) by Carl Edward Rasmussen and Hannes Nickisch, 2010-01-10.
%                 Dripta Sarkar, Michael Osborne, Thomas Adcock  01-12-16  


if nargin<2, A = '0'; return; end             % report number of hyperparameters 
A = zeros(2*size(x,1),1);                                    % derivative and mean
