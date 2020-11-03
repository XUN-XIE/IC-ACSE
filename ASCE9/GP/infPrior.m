function [post nlZ dnlZ] = infPrior(inf, prior, hyp, varargin)

% Inference with hyperparameter prior. Any inference method can be used.
% The prior specification is done by means of the prior parameter that has
% the same structure as hyp. Examples:
%  2) >> prior.mean{3} = {@priorDelta} or 
%     >> prior.mean{3} =  @priorDelta  or
%     >> prior.mean{3} = 'priorClamped'
%     fix the third mean function hyperparameter to its current value and
%     exclude it from optimisation
%     i.e. hyp.mean(3) ~ delta(hyp.mean(3))
%
% Return a parametrization of the posterior, the negative log marginal
% likelihood and its derivatives w.r.t. the hyperparameters.
%
% Note that at most 1 prior is allowed per hyperparameter. We consider the
% specification of more than 1 prior for a hyperparameter as an error.
%
%
% Copyright (c) by Hannes Nickisch and Roman Garnett, 2014-12-04.


[post nlZ dnlZ] = inf(hyp, varargin{:});                     % perform inference
if ~isempty(prior)                % add hyperprior contributions to nlZ and dnlZ
  nam = fieldnames(prior);
  num = zeros(numel(unwrap(hyp)),1);       % number of priors per hyperparameter
  for i=1:numel(nam)     % iterate over kinds of hyperparameters cov/lik/mean/xu
    ni = nam{i};                                         % name of the ith field
    if strcmp(ni,'multi')                      % first catch multivariate priors
      p = prior.(ni);
      for j=1:numel(p)  % iterate over individual muti-hyperparameter components
        pj = p{j};
        if ~isempty(pj)          % only proceed if a nonempty prior is specified
          idx = pj{end}; pj = pj(1:end-1);            % grab index from cell end
          pjstr = pj{1}; if ~ischar(pjstr), pjstr = func2str(pjstr); end
          if numel(pjstr)<5 || ~strcmp(pjstr(end-4:end),'Multi')
            error('multivariate hyperpriors are called <Name>Multi')
          end
          if isstruct(idx)                           % massage idx into a vector
            idxs = rewrap(hyp,zeros(size(num)));              % structured index
            for nj = fieldnames(idx), idxs.(nj{1})( idx.(nj{1}) ) = 1; end
            idx = unwrap(idxs)>0;                   % linearise structured index
          else
            idxz = zeros(size(num)); idxz(idx) = 1; idx = idxz>0; % binary index
          end
          if sum(idx)<=1, error('multivariate priors need >1 hyperparam'), end
          num(idx) = num(idx)+1;                             % inc prior counter
          hypu = unwrap(hyp); dnlZ = unwrap(dnlZ);
          if strncmp(pjstr,'priorClamped',12) || ...  % clamp derivative to zero
             strncmp(pjstr,'priorDelta',10), dnlZ(idx) = 0;
          else
            [lp,dlp] = feval(pj{:}, hypu(idx));    % evaluate prior distribution
            nlZ = nlZ-lp; dnlZ(idx) = dnlZ(idx)-dlp(:);    % update nlZ and dnlZ
          end
          dnlZ = rewrap(hyp,dnlZ);
        else
          error('multivariate priors should be non empty')
        end
      end
      continue                                       % jump to univariate priors
    end
    if ~isfield(hyp,ni), error(['unknown prior field ',ni,' in hyp']), end
    p = prior.(ni);
    if numel(p)~=numel(hyp.(ni)), error(['bad hyp/prior field length ',ni]), end
    for j=1:numel(p)         % iterate over individual hyperparameter components
      pj = p{j}; if ~iscell(pj) && ~isempty(pj), pj = {pj}; end   % enforce cell
      if ~isempty(pj)            % only proceed if a nonempty prior is specified
        num = rewrap(hyp,num); num.(ni)(j) = num.(ni)(j)+1;  % inc prior counter
        num = unwrap(num);
        pj1str = pj{1}; if ~ischar(pj1str), pj1str = func2str(pj1str); end
        if strncmp(pj1str,'priorClamped',12) || strncmp(pj1str,'priorDelta',10)
          dnlZ.(ni)(j) = 0;                           % clamp derivative to zero
        else
          [lp,dlp] = feval(pj{:}, hyp.(ni)(j));    % evaluate prior distribution
          nlZ = nlZ-lp; dnlZ.(ni)(j) = dnlZ.(ni)(j)-dlp;   % update nlZ and dnlZ
        end
      end
    end
  end
  if any(num>1)                 % check for hypers with more than a single prior
    num = rewrap(hyp,num); nam = fieldnames(num);
    s = '';
    for i=1:numel(nam)
      idx = find(num.(nam{i})>1);
      for j=1:numel(idx)
        s = [s,sprintf('hyp.%s(%d) ',nam{i},idx(j))];
      end
    end
    error(['More than 1 prior specified for ',s(1:end-1),'.'])  
  end
end