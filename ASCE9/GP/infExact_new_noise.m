function [post nlZ dnlZ] = infExact(hyp, mean, cov, lik, x, y)

% Exact inference for a GP with Gaussian likelihood. Compute a parametrization
% of the posterior, the negative log marginal likelihood and its derivatives
% w.r.t. the hyperparameters. See also "help infMethods".
%
% Copyright (c) by Carl Edward Rasmussen and Hannes Nickisch, 2015-07-13.
%                 Dripta Sarkar, Michael Osborne, Thomas Adcock  01-12-16  

if iscell(lik), likstr = lik{1}; else likstr = lik; end
if ~ischar(likstr), likstr = func2str(likstr); end
if ~strcmp(likstr,'likGauss_new_noise')               % NOTE: no explicit call to likGauss
  error('Exact inference only possible with Gaussian likelihood');
end
 
[n, D] = size(x);
n=2*n;
n2=n/2;

K = feval(cov{:}, hyp.cov, x);                      % evaluate covariance matrix
m = feval(mean{:}, hyp.mean, x);                          % evaluate mean vector

sn2a = exp(2*hyp.lik(1));                               % noise variance of likGauss
sn2b = exp(2*hyp.lik(2)); 

sn2_eye = vertcat( horzcat(sn2a*eye(n2), zeros(n2,n2)) ,  horzcat(zeros(n2,n2), sn2b*eye(n2))) ;
eye_2a = vertcat( horzcat(eye(n2), zeros(n2,n2)) ,  horzcat(zeros(n2,n2), (sn2b/sn2a)*eye(n2)));
eye_2b = vertcat( horzcat((sn2a/sn2b)*eye(n2), zeros(n2,n2)) ,  horzcat(zeros(n2,n2), eye(n2)));

if (sn2a<1e-6)||(sn2b<1e-6)                        % very tiny sn2 can lead to numerical trouble
% L = chol(K+sn2*eye(n)); sl =   1;   % Cholesky factor of covariance with noise
  L = chol(K+sn2_eye); sl =   1;
  pL = -solve_chol(L,eye(n));                            % L = -inv(K+inv(sW^2))
else
    if sn2a>=sn2b
          L = chol(K/sn2a+eye_2a); sl = sn2a;                       % Cholesky factor of B
          pL = L;                                              % L = chol(eye(n)+sW*sW'.*K)
    else
          L = chol(K/sn2b+eye_2b); sl = sn2b;                       % Cholesky factor of B
          pL = L;                                              % L = chol(eye(n)+sW*sW'.*K)
    end
%  L = chol(K/sn2+eye(n)); sl = sn2;                       % Cholesky factor of B
%  pL = L;                                           % L = chol(eye(n)+sW*sW'.*K)
end
alpha = solve_chol(L,y-m)/sl;

post.alpha = alpha;                            % return the posterior parameters
post.sW = vertcat(ones(n2,1)/sqrt(sn2a), ones(n2,1)/sqrt(sn2b));                  % sqrt of noise precision vector
post.L = pL;

if nargout>1                               % do we want the marginal likelihood?
  nlZ = (y-m)'*alpha/2 + sum(log(diag(L))) + n*log(2*pi*sl)/2;   % -log marg lik
  if nargout>2                                         % do we want derivatives?
    dnlZ = hyp;                                 % allocate space for derivatives
    Q = solve_chol(L,eye(n))/sl - alpha*alpha';     % precompute for convenience
    for i = 1:numel(hyp.cov)
      dnlZ.cov(i) = sum(sum(Q.*feval(cov{:}, hyp.cov, x, [], i)))/2;
    end
    Q0=diag(Q); Q1=Q0(1:n2);  Q2=Q0(n2+1:2*n2);  
    dnlZ.lik(1) = sn2a*sum(Q1);
    dnlZ.lik(2) = sn2b*sum(Q2);
%   dnlZ.lik = sn2*trace(Q);
    for i = 1:numel(hyp.mean)
      dnlZ.mean(i) = -feval(mean{:}, hyp.mean, x, i)'*alpha;
    end
  end
end