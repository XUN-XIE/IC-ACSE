function [varargout] = likGauss_new_noise(hyp, y, mu, s2, inf, i)

% likGauss - Gaussian likelihood function for regression. The expression for the 
% likelihood is 
%   likGauss(t) = exp(-(t-y)^2/2*sn^2) / sqrt(2*pi*sn^2),
% where y is the mean and sn is the standard deviation.
%
% The hyperparameters are:
%
% hyp = [  log(sn1)
%          log(sn2) ]
%
% Copyright (c) by Carl Edward Rasmussen and Hannes Nickisch, 2015-07-13.
%                 Dripta Sarkar, Michael Osborne, Thomas Adcock  01-12-16  

if nargin<3, varargout = {'2'}; return; end   % report number of hyperparameters

sn2a = exp(2*hyp(1));
sn2b = exp(2*hyp(2));

n=length(mu);
n2=n/2;

if nargin<5                              % prediction mode if inf is not present
  if isempty(y),  y = zeros(size(mu)); end
  s2zero = 1; if nargin>3&&numel(s2)>0&&norm(s2)>eps, s2zero = 0; end  % s2==0 ?
  if s2zero                                                    % log probability
      lp = vertcat(-(y(1:n2)-mu(1:n2)).^2./sn2a/2-log(2*pi*sn2a)/2 ,-(y((n2+1):2*n2)-mu((n2+1):2*n2)).^2./sn2b/2-log(2*pi*sn2b)/2)   ; s2 = 0;
  else
    lp = likGauss_new_noise(hyp, y, mu, s2, 'infEP');                         % prediction
  end
%  ymu = {}; ys2 = {};
  ymu = []; ys2 = [];
  if nargout>1
    ymu = mu;                                                   % first y moment
    if nargout>2
      if s2==0
          ys2(1:n2) = s2 + sn2a;                                          % second y moment
          ys2((n2+1):2*n2) = s2 + sn2b;
      else
          ys2(1:n2) = s2(1:n2) + sn2a;                                          % second y moment
          ys2((n2+1):2*n2) = s2((n2+1):2*n2) + sn2b;          
      end
    end
  end
  varargout = {lp,ymu,ys2};
else
  switch inf 
  case 'infLaplace'
    if nargin<6                                             % no derivative mode
      if isempty(y), y=0; end
      ymmu(1:n2,1) = y(1:n2)-mu(1:n2);  ymmu((n2+1):2*n2,1) = y((n2+1):2*n2,1)-mu((n2+1):2*n2);  dlp = []; d2lp = []; d3lp = []; %  dlp = {}; d2lp = {}; d3lp = {};
      lp(1:n2,1) = -ymmu(1:n2).^2/(2*sn2a) - log(2*pi*sn2a)/2; 
      lp((n2+1):2*n2,1) = -ymmu((n2+1):2*n2).^2/(2*sn2b) - log(2*pi*sn2b)/2;
      if nargout>1
        dlp(1:n2,1) = ymmu(1:n2)/sn2a;                      % dlp, derivative of log likelihood
        dlp((n2+1):2*n2,1) = ymmu((n2+1):2*n2)/sn2b;
        if nargout>2                    % d2lp, 2nd derivative of log likelihood
          d2lp(1:n2,1) = -ones(size(ymmu(1:n2)))/sn2a;
          d2lp((n2+1):2*n2,1) = -ones(size(ymmu((n2+1):2*n2)))/sn2b;
          if nargout>3                  % d3lp, 3rd derivative of log likelihood
            d3lp = zeros(size(ymmu));
          end
        end
      end
      varargout = {lp,dlp,d2lp,d3lp};
    else                                                       % derivative mode
      lp_dhyp(1:n2,1) = (y(1:n2)-mu(1:n2)).^2/sn2a - 1;  % derivative of log likelihood w.r.t. hypers
      lp_dhyp((n2+1):2*n2,1) = (y((n2+1):2*n2)-mu((n2+1):2*n2)).^2/sn2b - 1;
      dlp_dhyp(1:n2,1) = 2*(mu(1:n2)-y(1:n2))/sn2a;                               % first derivative,
      dlp_dhyp((n2+1):2*n2,1) = 2*(mu((n2+1):2*n2)-y((n2+1):2*n2))/sn2b;
      d2lp_dhyp(1:n2,1) = 2*ones(size(mu(1:n2)))/sn2a;   % and also of the second mu derivative
      d2lp_dhyp((n2+1):2*n2,1) = 2*ones(size(mu((n2+1):2*n2)))/sn2b;
      varargout = {lp_dhyp,dlp_dhyp,d2lp_dhyp};
    end

  case 'infEP'
   if length(s2)>1   
    if nargin<6                                             % no derivative mode
      lZ = vertcat(-(y(1:n2)-mu(1:n2)).^2./(sn2a+s2(1:n2))/2 - log(2*pi*(sn2a+s2(1:n2)))/2 , -(y((n2+1):2*n2)-mu((n2+1):2*n2)).^2./(sn2b+s2((n2+1):2*n2))/2 - log(2*pi*(sn2b+s2((n2+1):2*n2)))/2 );    % log part function
      dlZ = []; d2lZ = []; %dlZ = {}; d2lZ = {};
         if nargout>1
            dlZ(1:n2,1)  = (y(1:n2)-mu(1:n2))./(sn2a+s2(1:n2));                    % 1st derivative w.r.t. mean
            dlZ((n2+1):2*n2,1)  = (y((n2+1):2*n2)-mu((n2+1):2*n2))./(sn2b+s2((n2+1):2*n2));
            if nargout>2
               d2lZ(1:n2,1) = -1./(sn2a+s2(1:n2));                      % 2nd derivative w.r.t. mean
               d2lZ((n2+1):2*n2,1) = -1./(sn2b+s2((n2+1):2*n2));
            end
         end
         varargout = {lZ,dlZ,d2lZ};
    else                                                       % derivative mode
      dlZhyp = vertcat(((y(1:n2)-mu(1:n2)).^2./(sn2a+s2(1:n2))-1) ./ (1+s2(1:n2)./sn2a) , ((y((n2+1):2*n2)-mu((n2+1):2*n2)).^2./(sn2b+s2((n2+1):2*n2))-1) ./ (1+s2((n2+1):2*n2)./sn2b) );   % deriv. w.r.t. hyp.lik
      varargout = {dlZhyp};
    end
   else
     if nargin<6                                             % no derivative mode
      lZ = vertcat(-(y(1:n2)-mu(1:n2)).^2./(sn2a+s2)/2 - log(2*pi*(sn2a+s2))/2 , -(y((n2+1):2*n2)-mu((n2+1):2*n2)).^2./(sn2b+s2)/2 - log(2*pi*(sn2b+s2))/2 );    % log part function
      dlZ = []; d2lZ = []; %dlZ = {}; d2lZ = {};
         if nargout>1
            dlZ(1:n2,1)  = (y(1:n2)-mu(1:n2))./(sn2a+s2);                    % 1st derivative w.r.t. mean
            dlZ((n2+1):2*n2,1)  = (y((n2+1):2*n2)-mu((n2+1):2*n2))./(sn2b+s2);
            if nargout>2
               d2lZ(1:n2,1) = -1./(sn2a+s2);                      % 2nd derivative w.r.t. mean
               d2lZ((n2+1):2*n2,1) = -1./(sn2b+s2);
            end
         end
         varargout = {lZ,dlZ,d2lZ};
    else                                                       % derivative mode
      dlZhyp = vertcat(((y(1:n2)-mu(1:n2)).^2./(sn2a+s2)-1) ./ (1+s2./sn2a) , ((y((n2+1):2*n2)-mu((n2+1):2*n2)).^2./(sn2b+s2)-1) ./ (1+s2./sn2b) );   % deriv. w.r.t. hyp.lik
      varargout = {dlZhyp};
     end
   end
  case 'infVB'
    % variational lower site bound
    % t(s) = exp(-(y-s)^2/2sn2)/sqrt(2*pi*sn2)
    % the bound has the form: (b+z/ga)*f - f.^2/(2*ga) - h(ga)/2
    n = numel(s2); b = zeros(n,1); y = y.*ones(n,1); z = y;
    varargout = {b,z};
  end
end
