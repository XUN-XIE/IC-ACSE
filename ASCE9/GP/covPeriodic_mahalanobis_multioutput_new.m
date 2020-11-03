function Knew = covPeriodic_mahalanobis_multioutput_new(hyp, x, z, i)

% The hyperparameters are:
%
% hyp = [ log(ellb)
%         log(ellc)
%         log(elld)
%         log(p)
%         log(ell_time)
%         log(sfa) 
%         log(sfb)
%         log(theta) ]
%
% Copyright (c) by Dripta Sarkar, Michael Osborne, Thomas Adcock, 2016-27-11.


if nargin<2, Knew = '8'; return; end                  
if nargin<3, z = []; end                                  
xeqz = isempty(z); dg = strcmp(z,'diag');                      

[n,D] = size(x);

ellb = exp(hyp(1));
ellc = exp(hyp(2));
elld = exp(hyp(3));
p  = exp(hyp(4));
ell_time = exp(hyp(5));
sfa2 = exp(2*hyp(6));
sfb2 = exp(2*hyp(7));
theta = exp(hyp(8));


if dg                                                              
  K1 = zeros(size(x,1),1);
  K2b = zeros(size(x,1),1);
  K2c = zeros(size(x,1),1); 
  K2bc = zeros(size(x,1),1);                                                                                 
else
  if xeqz                                             
    K1 = sqrt(sq_dist(x(:,1)'));
    K2b = sq_dist(x(:,2)');                                                                                                         
    K2c = sq_dist(x(:,3)');                                                                                                         
    K2bc = ( repmat(x(:,2),1,size(x,1))-repmat(x(:,2)',size(x,1),1) ).*( repmat(x(:,3),1,size(x,1))-repmat(x(:,3)',size(x,1),1) );  
  else                                               
    K1 = sqrt(sq_dist(x(:,1)',z(:,1)'));
    K2b = sq_dist(x(:,2)',z(:,2)');                                                                                                
    K2c = sq_dist(x(:,3)',z(:,3)');                                                                                                 
    K2bc = ( repmat(x(:,2),1,size(z,1))-repmat(z(:,2)',size(x,1),1) ).*( repmat(x(:,3),1,size(z,1))-repmat(z(:,3)',size(x,1),1) );  
  end
end



K1 = pi*K1/p;

if nargin<4                                                   

    
    K1 = sin(K1)/ell_time;    K1 = K1.*K1;    K1 = exp(-2*K1);  
    
    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));     
    if dg
      Ktop= sfa2*K1.*K2;
      Kdown= sfb2*K1.*K2;
      Knew= vertcat(Ktop,Kdown); return;
    end
    K0=K1.*K2;
    Ktop=horzcat( sfa2*K0, sqrt(sfa2*sfb2)* cos(theta)*K0 );
    Kdown= horzcat( sqrt(sfa2*sfb2)* cos(theta)*K0, sfb2*K0 );
    Knew=vertcat(Ktop, Kdown);
    
else                                                               
  if i==1
    K1 = sin(K1)/ell_time;   K1 = K1.*K1;    K1 = exp(-2*K1);  
    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                     
    deriv= ( ellc*K2b - elld*ellb*K2bc )/(ellb*ellb*ellc(1-elld*elld));                                                  
                                                          
    K0 = K1.*K2.*deriv;
    Ktop1 = sfa2*K0;            Ktop2 = sqrt(sfa2*sfb2)*cos(theta)*K0;
    Kdown1 = sqrt(sfa2*sfb2)*cos(theta)*K0;   Kdown2 = sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown);
        
  elseif i==2
    K1 = sin(K1)/ell_time;   K1 = K1.*K1;    K1 = exp(-2*K1);  

    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                           
    deriv= ( ellb*K2c - elld*ellc*K2bc )/(ellb*ellc*ellc(1-elld*elld));                                               
                                                              
    K0 = K1.*K2.*deriv;
    Ktop1 = sfa2*K0;            Ktop2 = sqrt(sfa2*sfb2)*cos(theta)*K0;
    Kdown1 = sqrt(sfa2*sfb2)*cos(theta)*K0;   Kdown2 = sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown);
    
  elseif i==3
    K1 = sin(K1)/ell_time;   K1 = K1.*K1;    K1 = exp(-2*K1);  

    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                              
    deriv= elld*( ellb*ellc*(1+elld*elld)*K2bc - elld*ellb*ellb*K2c - elld*ellc*ellc*K2b  )/(ellb*ellb*ellc*ellc*(1-elld*elld)^2); 
                                                                   
    K0 = K1.*K2.*deriv;
    Ktop1 = sfa2*K0;            Ktop2 = sqrt(sfa2*sfb2)*cos(theta)*K0;
    Kdown1 = sqrt(sfa2*sfb2)*cos(theta)*K0;   Kdown2 = sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown); 
    
  elseif i==4

    R1 = sin(K1)/ell_time;          K1 = 4/ell_time*exp(-2*R1.*R1).*R1.*cos(K1).*K1;
    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                            
    K0 = K1.*K2;
    Ktop1 = sfa2*K0;            Ktop2 = sqrt(sfa2*sfb2)*cos(theta)*K0;
    Kdown1 = sqrt(sfa2*sfb2)*cos(theta)*K0;   Kdown2 = sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown);
    

  elseif i==5

    K1 = sin(K1)/ell_time;          K1 = K1.*K1; K1 = 4*exp(-2*K1).*K1;
    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                          
    K0 = K1.*K2;
    Ktop1 = sfa2*K0;            Ktop2 = sqrt(sfa2*sfb2)*cos(theta)*K0;
    Kdown1 = sqrt(sfa2*sfb2)*cos(theta)*K0;   Kdown2 = sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown);    
  
 
  elseif i==6          
    K1 = sin(K1)/ell_time;   K1 = K1.*K1;    K1 = exp(-2*K1);   
    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                              
    K0 = K1.*K2;
    Ktop1 = 2*sfa2*K0;            Ktop2 = sqrt(sfa2*sfb2)*cos(theta)*K0;
    Kdown1 = sqrt(sfa2*sfb2)*cos(theta)*K0;   Kdown2 = 0*sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown); 
    
  elseif i==7         
    K1 = sin(K1)/ell_time;   K1 = K1.*K1;    K1 = exp(-2*K1);  
    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                           %New Line   
    K0 = K1.*K2;
    Ktop1 = 0*sfa2*K0;            Ktop2 = sqrt(sfa2*sfb2)*cos(theta)*K0;
    Kdown1 = sqrt(sfa2*sfb2)*cos(theta)*K0;   Kdown2 = 2*sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown);
    
  elseif i==8           
    K1 = sin(K1)/ell_time; K1 = K1.*K1; K1 =   exp(-2*K1);  
    K2 = exp(-0.5*(ellb*ellb*K2c - 2*elld*ellb*ellc*K2bc + ellc*ellc*K2b)/(ellb*ellb*ellc*ellc*(1-elld*elld)));                          %New Line   
    K0 = K1.*K2;
    Ktop1 = 0*sfa2*K0;                               Ktop2 = -theta*sin(theta)*sqrt(sfa2*sfb2)*K0;
    Kdown1 = -theta*sin(theta)*sqrt(sfa2*sfb2)*K0;   Kdown2 = 0*sfb2*K0; 
    Ktop=horzcat(Ktop1, Ktop2);
    Kdown=horzcat(Kdown1, Kdown2);
    Knew=vertcat(Ktop, Kdown);
    
  else
    error('Unknown hyperparameter')
  end
end