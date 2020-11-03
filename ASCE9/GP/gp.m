function [varargout] = gp(hyp, inf, mean, cov, lik, x, y, xs, ys)
% Gaussian Process inference and prediction. The gp function provides a
% flexible framework for Bayesian inference and prediction with Gaussian
% processes for scalar targets, i.e. both regression and binary
% classification. The prior is Gaussian process, defined through specification
% of its mean and covariance function. The likelihood function is also
% specified. Both the prior and the likelihood may have hyperparameters
% associated with them.
%
%
% where:
%
%   hyp      struct of column vectors of mean/cov/lik hyperparameters
%   inf      function specifying the inference method 
%   mean     prior mean function
%   cov      prior covariance function
%   lik      likelihood function
%   x        n by D matrix of training inputs
%   y        column vector of length n of training targets
%   xs       ns by D matrix of test inputs
%   ys       column vector of length nn of test targets
%
%   nlZ      returned value of the negative log marginal likelihood
%   dnlZ     struct of column vectors of partial derivatives of the negative
%               log marginal likelihood w.r.t. mean/cov/lik hyperparameters
%   ymu      column vector (of length ns) of predictive output means
%   ys2      column vector (of length ns) of predictive output variances
%   fmu      column vector (of length ns) of predictive latent means
%   fs2      column vector (of length ns) of predictive latent variances
%   lp       column vector (of length ns) of log predictive probabilities
%
% Copyright (c) by Carl Edward Rasmussen and Hannes Nickisch, 2015-07-13.
%                  Dripta Sarkar, Michael Osborne, Thomas Adcock  01-12-16   

if nargin<7 || nargin>9
  disp('Usage: [nlZ dnlZ          ] = gp(hyp, inf, mean, cov, lik, x, y);')
  disp('   or: [ymu ys2 fmu fs2   ] = gp(hyp, inf, mean, cov, lik, x, y, xs);')
  disp('   or: [ymu ys2 fmu fs2 lp] = gp(hyp, inf, mean, cov, lik, x, y, xs, ys);')
  return
end

if isempty(mean), mean = {@meanZeronew2}; end                     % set default mean
if ischar(mean) || isa(mean, 'function_handle'), mean = {mean}; end  % make cell
if isempty(cov), error('Covariance function cannot be empty'); end  % no default
if ischar(cov) || isa(cov,'function_handle'), cov  = {cov};  end     % make cell
cstr = cov{1}; if isa(cstr,'function_handle'), cstr = func2str(cstr); end


if ischar(inf), inf = str2func(inf); end          % convert into function handle
if ischar(inf) || isa(inf,'function_handle'), inf = {inf};  end      % make cell
istr = inf{1}; if isa(istr,'function_handle'), istr = func2str(istr); end
if strcmp(istr,'infPrior')
  istr = inf{2}; if isa(istr,'function_handle'), istr = func2str(istr); end
end


if ischar(lik) || isa(lik,'function_handle'), lik = {lik};  end      % make cell
lstr = lik{1}; if isa(lstr,'function_handle'), lstr = func2str(lstr); end

D = size(x,2);


if ~isfield(hyp,'mean'), hyp.mean = []; end        % check the hyp specification
if eval(feval(mean{:})) ~= numel(hyp.mean)
  error('Number of mean function hyperparameters disagree with mean function')
end
if ~isfield(hyp,'cov'), hyp.cov = []; end
if eval(feval(cov{:})) ~= numel(hyp.cov)
  error('Number of cov function hyperparameters disagree with cov function')
end
if ~isfield(hyp,'lik'), hyp.lik = []; end
if eval(feval(lik{:})) ~= numel(hyp.lik)
  error('Number of lik function hyperparameters disagree with lik function')
end

try                                                  % call the inference method
  
  if nargin>7   % compute marginal likelihood and its derivatives only if needed
    if isstruct(y)
      post = y;            % reuse a previously computed posterior approximation
    else
      post = feval(inf{:}, hyp, mean, cov, lik, x, y);
    end
  else
    if nargout<=1
      [post nlZ] = feval(inf{:}, hyp, mean, cov, lik, x, y); dnlZ = {};
    else
      [post nlZ dnlZ] = feval(inf{:}, hyp, mean, cov, lik, x, y);
    end
  end
catch
  msgstr = lasterr;
  if nargin>7, error('Inference method failed [%s]', msgstr); else 
    warning('Inference method failed [%s] .. attempting to continue',msgstr)
    dnlZ = struct('cov',0*hyp.cov, 'mean',0*hyp.mean, 'lik',0*hyp.lik);
    varargout = {NaN, dnlZ}; return                    % continue with a warning
  end
end

if nargin==7                                     % if no test cases are provided
  varargout = {nlZ, dnlZ, post};    % report -log marg lik, derivatives and post
else
  alpha = post.alpha; L = post.L; sW = post.sW;
  if issparse(alpha)                  % handle things for sparse representations
    nz = alpha ~= 0;                                 % determine nonzero indices
    if issparse(L), L = full(L(nz,nz)); end      % convert L and sW if necessary
    if issparse(sW), sW = full(sW(nz)); end
   else nz = true(size(alpha,1),1);                % non-sparse representation
        nz1 = true(size(alpha,1)/2,1);                 % non-sparse representation                          
   end  
   if isempty(L)                       % in case L is not provided, we compute it
    K = feval(cov{:}, hyp.cov, x(nz1,:));
    L = chol(eye(sum(nz))+sW*sW'.*K);
  end
  %verify whether L contains valid Cholesky decomposition or something different
  Lchol = isnumeric(L) && all(all(tril(L,-1)==0)&diag(L)'>0&isreal(diag(L))');
  ns = size(xs,1);                                       % number of data points

  nperbatch = ns; %2000;                       % number of data points per mini batch
  nact = 0;                       % number of already processed test data points

   ymu = zeros(2*ns,1); ys2 = ymu; fmu = ymu; fs2 = ymu; lp = ymu;   % allocate mem                         
  while nact<ns               % process minibatches of test cases to save memory
    id = (nact+1):min(nact+nperbatch,ns);               % data points to process
    kss = feval(cov{:}, hyp.cov, xs(id,:), 'diag');              % self-variance

    Ks = feval(cov{:}, hyp.cov, x(nz1,:), xs(id,:));        % avoid computation

    ms = feval(mean{:}, hyp.mean, xs(id,:));
    N = size(alpha,2);  % number of alphas (usually 1; more in case of sampling)
    Fmu = repmat(ms,1,N) + Ks'*full(alpha(nz,:));        % conditional mean fs|f
    id1= id(1):(id(length(id)) + length(id) );                                                              
    fmu(id1) = sum(Fmu,2)/N;                                   % predictive means
    if Lchol    % L contains chol decomp => use Cholesky parameters (alpha,sW,L)
      V  = L'\(repmat(sW,1,length(id1)).*Ks);
      fs2(id1) = kss - sum(V.*V,1)';                       % predictive variances
    else                % L is not triangular => use alternative parametrisation
      if isnumeric(L), LKs = L*Ks; else LKs = L(Ks); end    % matrix or callback
      fs2(id1) = kss + sum(Ks.*LKs,1)';                    % predictive variances
    end
    fs2(id1) = max(fs2(id1),0);   % remove numerical noise i.e. negative variances
    Fs2 = repmat(fs2(id1),1,N);     % we have multiple values in case of sampling
    if nargin<9
      [Lp, Ymu, Ys2] = feval(lik{:},hyp.lik,[],   Fmu(:),Fs2(:));
    else
      Ys = repmat(ys(id1),1,N);
      [Lp, Ymu, Ys2] = feval(lik{:},hyp.lik,Ys(:),Fmu(:),Fs2(:));
    end
    lp(id1)  = sum(reshape(Lp, [],N),2)/N;    % log probability; sample averaging
    ymu(id1) = sum(reshape(Ymu,[],N),2)/N;          % predictive mean ys|y and ..
    ys2(id1) = sum(reshape(Ys2,[],N),2)/N;                          % .. variance
    nact = id(end);          % set counter to index of last processed data point
  end
  if nargin<9
    varargout = {ymu, ys2, fmu, fs2, [], post};        % assign output arguments
  else
    varargout = {ymu, ys2, fmu, fs2, lp, post};
  end
end
