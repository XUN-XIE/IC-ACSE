%By  Dripta Sarkar, Michael Osborne, Thomas Adcock  01-12-16   


%This is main file to run the example code


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Loads all the data

cfolder=pwd;    cd('./sampling');    load('combn_hour.mat', 'combn');

node_number(1)=4034;  node_number(2)=3832;  node_number(3)=3719;  node_number(4)=3721;  node_number(5)=3839;  node_number(6)=3947;  node_number(7)=4048;
node_number(8)=4045;  node_number(9)=3942;  node_number(10)=3939; node_number(11)=4038; node_number(12)=3833; node_number(13)=3836; node_number(14)=3944;
node_number(15)=3941; node_number(16)=3835; node_number(17)=3938; node_number(18)=4046; node_number(19)=3718; node_number(20)=3945;

% 调用node function to load data

% v is complex number, and real number represents u, imaginary number represents v

% total time is 8 months

% 读出来的t u v 都是一维数组
cd(cfolder); cd('./Data/1');  [t_raw_node{1} u_raw_node{1} v_raw_node{1} long_node(1) lat_node(1)] = node(node_number(1)); 
cd(cfolder); cd('./Data/2');  [t_raw_node{2} u_raw_node{2} v_raw_node{2} long_node(2) lat_node(2)] = node(node_number(2)); 
cd(cfolder); cd('./Data/3');  [t_raw_node{3} u_raw_node{3} v_raw_node{3} long_node(3) lat_node(3)] = node(node_number(3));
cd(cfolder); cd('./Data/4');  [t_raw_node{4} u_raw_node{4} v_raw_node{4} long_node(4) lat_node(4)] = node(node_number(4));
cd(cfolder); cd('./Data/5');  [t_raw_node{5} u_raw_node{5} v_raw_node{5} long_node(5) lat_node(5)] = node(node_number(5));
cd(cfolder); cd('./Data/6');  [t_raw_node{6} u_raw_node{6} v_raw_node{6} long_node(6) lat_node(6)] = node(node_number(6));
cd(cfolder); cd('./Data/7');  [t_raw_node{7} u_raw_node{7} v_raw_node{7} long_node(7) lat_node(7)] = node(node_number(7));
cd(cfolder); cd('./Data/8');  [t_raw_node{8} u_raw_node{8} v_raw_node{8} long_node(8) lat_node(8)] = node(node_number(8));
cd(cfolder); cd('./Data/9');  [t_raw_node{9} u_raw_node{9} v_raw_node{9} long_node(9) lat_node(9)] = node(node_number(9));
cd(cfolder); cd('./Data/10');  [t_raw_node{10} u_raw_node{10} v_raw_node{10} long_node(10) lat_node(10)] = node(node_number(10));
cd(cfolder); cd('./Data/11');  [t_raw_node{11} u_raw_node{11} v_raw_node{11} long_node(11) lat_node(11)] = node(node_number(11));
cd(cfolder); cd('./Data/12');  [t_raw_node{12} u_raw_node{12} v_raw_node{12} long_node(12) lat_node(12)] = node(node_number(12));
cd(cfolder); cd('./Data/13');  [t_raw_node{13} u_raw_node{13} v_raw_node{13} long_node(13) lat_node(13)] = node(node_number(13));
cd(cfolder); cd('./Data/14');  [t_raw_node{14} u_raw_node{14} v_raw_node{14} long_node(14) lat_node(14)] = node(node_number(14));
cd(cfolder); cd('./Data/15');  [t_raw_node{15} u_raw_node{15} v_raw_node{15} long_node(15) lat_node(15)] = node(node_number(15));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Computes the constituent frquencies for the time length of the data and sampling frequency 

cd(cfolder); cd('./UTide');
 
% get the number of data
total_no_data_points=length(t_raw_node{1});
factor=4;

% divide the number of data by 4 and to be integer
reduced_no_data_points=fix((total_no_data_points)/factor);

for i=1:15
% create the zeros matrix as shape 1 * reduced_no_data_points * 1
    t_raw_node_reduced{i}=zeros(1,reduced_no_data_points,1);  u_raw_node_reduced{i}=zeros(1,reduced_no_data_points,1);  v_raw_node_reduced{i}=zeros(1,reduced_no_data_points,1);
    for i1=1:reduced_no_data_points
% 每四个点取出一个值进行赋值
        t_raw_node_reduced{i}(1, i1)=t_raw_node{i}(1, factor*i1); u_raw_node_reduced{i}(1, i1)=u_raw_node{i}(1, factor*i1); v_raw_node_reduced{i}(1, i1)=v_raw_node{i}(1, factor*i1);
    end
end

tr_start=1; tr_end=2184;
test_start=2185; test_end=2184*2;

t_raw_utide=t_raw_node_reduced{4}(tr_start:tr_end); u_raw_utide=u_raw_node_reduced{4}(tr_start:tr_end); v_raw_utide=v_raw_node_reduced{4}(tr_start:tr_end);


lat=lat_node(1);  coef = ut_solv ( t_raw_utide, u_raw_utide, v_raw_utide, lat, 'auto' , 'OLS');

A1=cellstr(coef.name);

num_const=length(A1);                      % number of selected constituents
load('ut_constants.mat');
A=cellstr(const.name);
selected_const_index=zeros(num_const,1);   %location/index of the selected constituent in the list of all constituents
selected_const_freq=zeros(num_const,1);    % frequency of the selected constituents
for i=1:num_const
   
        selected_const_index(i,1)=find(strncmp(A,A1{i},4));
        selected_const_freq(i,1)=const.freq(find(strncmp(A,A1{i},4)));

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Convert  latitude and longitude of the locations to spatial distances
% from a mean location

mean_lat=mean(lat_node(1:15));
mean_long=mean(long_node(1:15));

lat0=min(lat_node(1:15));
long0=min(long_node(1:15));

num_location=15;
for i=1:num_location
    theta1=lat_node(i); theta2=lat_node(i); lambda1=mean_long; lambda2=long_node(i);   dlong(i)=6371*acos(sin(theta1/180*pi())*sin(theta2/180*pi()) + cos(theta1/180*pi())*cos(theta2/180*pi())*cos((lambda2-lambda1)/180*pi()));
    theta1=mean_lat; theta2=lat_node(i); lambda1=long_node(i); lambda2=long_node(i);   dlat(i) =6371*acos(sin(theta1/180*pi())*sin(theta2/180*pi()) + cos(theta1/180*pi())*cos(theta2/180*pi())*cos((lambda2-lambda1)/180*pi()));
end

for i=1:num_location
    if long_node(i)<mean_long
        dlong(i) = - dlong(i);
    end
    if lat_node(i)<mean_lat
        dlat(i) = - dlat(i);
    end    
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Prepare the inputs x and outputs y for training the Gaussian Process
x1=[];  y1a=[];  y1b=[];


for i=1:15
    t_specific{i}=[];   u_specific{i}=[];    v_specific{i}=[];
end

num_pts=168/(7*24);
for i=1:size(combn,1)

% input are t latitude longitude

% x1矩阵为先竖着排列t_raw_node_reduced{combn(i,1)}， 再排列t_raw_node_reduced{combn(i,2)}。 后面两个数据对应的按列向右平行排列
    x1 =[  x1;   t_raw_node_reduced{combn(i,1)}(num_pts*(i-1)+1:num_pts*i)'     dlat(combn(i,1))*ones(1,num_pts)'   dlong(combn(i,1))*ones(1,num_pts)' ; 
                 t_raw_node_reduced{combn(i,2)}(num_pts*(i-1)+1:num_pts*i)'     dlat(combn(i,2))*ones(1,num_pts)'   dlong(combn(i,2))*ones(1,num_pts)' ];
    y1a=[ y1a;   u_raw_node_reduced{combn(i,1)}(num_pts*(i-1)+1:num_pts*i)';
                 u_raw_node_reduced{combn(i,2)}(num_pts*(i-1)+1:num_pts*i)'  ]; 
    y1b=[ y1b;   v_raw_node_reduced{combn(i,1)}(num_pts*(i-1)+1:num_pts*i)'; 
                 v_raw_node_reduced{combn(i,2)}(num_pts*(i-1)+1:num_pts*i)'  ]; 
             
    t_sp_1 = t_raw_node_reduced{combn(i,1)}(num_pts*(i-1)+1:num_pts*i)';   t_sp_2 = t_raw_node_reduced{combn(i,2)}(num_pts*(i-1)+1:num_pts*i)';
    u_sp_1 = u_raw_node_reduced{combn(i,1)}(num_pts*(i-1)+1:num_pts*i)';   u_sp_2 = u_raw_node_reduced{combn(i,2)}(num_pts*(i-1)+1:num_pts*i)';
    v_sp_1 = v_raw_node_reduced{combn(i,1)}(num_pts*(i-1)+1:num_pts*i)';   v_sp_2 = v_raw_node_reduced{combn(i,2)}(num_pts*(i-1)+1:num_pts*i)';    
    
    t_specific{combn(i,1)} = [t_specific{combn(i,1)}; t_sp_1];   t_specific{combn(i,2)} = [t_specific{combn(i,2)}; t_sp_2]; 
    u_specific{combn(i,1)} = [u_specific{combn(i,1)}; u_sp_1];   u_specific{combn(i,2)} = [u_specific{combn(i,2)}; u_sp_2];
    v_specific{combn(i,1)} = [v_specific{combn(i,1)}; v_sp_1];   v_specific{combn(i,2)} = [v_specific{combn(i,2)}; v_sp_2];
end
y1=[y1a;y1b];


cd(cfolder); cd('./GP');  startup; 

per=zeros(num_const,1);
for i=1:num_const
    per(i,1)=log(1/(24*selected_const_freq(i,1)) );
end
   
asd = cell(1,num_const);
for i=1:num_const
        asd{i} = @covPeriodic_mahalanobis_multioutput_new;
end

covfunc = {@covSum ,asd};

likfunc={@likGauss_new_noise};

hyp2.cov=zeros(8*num_const,1);  


meanfunc=[];


u_height=max(u_raw_node_reduced{1})-min(u_raw_node_reduced{1});
u_var=sqrt(u_height)/num_const;

v_height=max(v_raw_node_reduced{1})-min(v_raw_node_reduced{1});
v_var=sqrt(v_height)/num_const;


hyp2.lik=[log(u_height/10); log(v_height/10)];

prior2.cov=cell(8*num_const,1);


min_diff_lat=50000;  min_diff_long=50000;
max_diff_lat=0;  max_diff_long=0;
for i=1:num_location
    for j=1:num_location
        if i==j
            
        else
            diff_lat=abs(dlat(i)-dlat(j));  diff_long=abs(dlong(i)-dlong(j));
            if diff_lat<min_diff_lat,    min_diff_lat  = diff_lat;    end
            if diff_long<min_diff_long,  min_diff_long = diff_long;   end
            if diff_lat>max_diff_lat,    max_diff_lat  = diff_lat;    end
            if diff_long>max_diff_long,  max_diff_long = diff_long;   end
        end
    end
end


for i=1:num_const

    hyp2.cov(8*i-7,1)=log(2);   
    prior2.cov{8*i-7}={@priorSmoothBox1, log(min_diff_lat), log(max_diff_lat), 1000};
    
    hyp2.cov(8*i-6,1)=log(2);   
    prior2.cov{8*i-6}={@priorSmoothBox1, log(min_diff_long), log(max_diff_long), 1000};
    
    hyp2.cov(8*i-5,1)=log(0.0000000000000000000001);   
    prior2.cov{8*i-5}={@priorSmoothBox1, log(0.0000000000000000000001), log(0.98), 1000};
    
    hyp2.cov(8*i-4,1)=per(i,1);
    prior2.cov{8*i-4}=@priorDelta;   
    
    hyp2.cov(8*i-3,1)=log(2);   
    prior2.cov{8*i-3}={@priorSmoothBox1, log(1.3), log(3.1), 1000};
    
    hyp2.cov(8*i-2,1)=log(u_var);
    
    hyp2.cov(8*i-1,1)=log(v_var); 
    
    hyp2.cov(8*i,1)=log(2.5*pi());   
    prior2.cov{8*i}={@priorSmoothBox1, log(2*pi()), log(3*pi()), 1000};    
end

inf2={@infPrior, @infExact_new_noise,prior2};    

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Determine the hyperparameters using an optimization scheme

%Note: For determining the hyperparameters, a default number of function evalutaions is mentioned as 100 in the code (end of file).
%A 100 evaluations can itself take around a day to run on descent CPU. 
hyp2 = minimize(hyp2, @gp, -100, inf2, meanfunc, covfunc, likfunc, x1, y1);


%cd(cfolder); cd('./sampling');   save('hyp2hour_Periodic.mat','hyp2');  
cd(cfolder); 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Plot the predictions  at the test location and compute the mean squared
%error (mse_all) of the u and v velocities

mse_all = prediction(t_raw_node_reduced, u_raw_node_reduced, v_raw_node_reduced, hyp2, inf2, meanfunc, covfunc, likfunc, x1,y1, test_start, test_end, dlat, dlong, t_specific, u_specific, v_specific);                                                         
