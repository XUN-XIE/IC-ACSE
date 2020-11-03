function mse_all = prediction(t_raw_node_reduced, u_raw_node_reduced, v_raw_node_reduced, hyp2, inf2, meanfunc, covfunc, likfunc, x1,y1, test_start, test_end, dlat, dlong, t_specific, u_specific, v_specific)
%  By    Dripta Sarkar, Michael Osborne, Thomas Adcock  01-12-16   

pred_loc=[1:5]';
num_prediction=size(pred_loc,1);

ydata1a=[]; ydata1b=[];  z1=[];
for i=1:num_prediction
    ydata1a=[ ydata1a; u_raw_node_reduced{pred_loc(i)}(test_start:test_end)' ] ;    
    ydata1b=[ ydata1b; v_raw_node_reduced{pred_loc(i)}(test_start:test_end)' ] ;
    z1 = [ z1; t_raw_node_reduced{pred_loc(i)}(test_start:test_end)' dlat(pred_loc(i))*ones(1,(test_end-test_start+1))' dlong(pred_loc(i))*ones(1,(test_end-test_start+1))'];
end
ydata1 =[ydata1a; ydata1b];
int=test_end-test_start+1;

tr_start=1; tr_end=2184;

y1a_initial=[]; y1b_initial=[];  x1_initial=[];
for i=1:num_prediction
    y1a_initial=[ y1a_initial; u_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' ] ;    
    y1b_initial=[ y1b_initial; v_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' ] ;
    x1_initial = [ x1_initial; t_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' dlat(pred_loc(i))*ones(1,(tr_end-tr_start+1))' dlong(pred_loc(i))*ones(1,(tr_end-tr_start+1))'];
end
y1_initial =[y1a_initial; y1b_initial];
int_initial=tr_end-tr_start+1;

[m_1 s2_1]  = gp(hyp2, inf2, meanfunc, covfunc, likfunc, x1,y1,z1);

u_gap=int*num_prediction;

figure(50)
for i=1:num_prediction
    subplot(num_prediction, 2, 2*(i-1)+1)
    f=[m_1((i-1)*int+1:i*int,1) + 2*sqrt(s2_1((i-1)*int+1:i*int,1));  flipdim( m_1((i-1)*int+1:i*int,1) - 2*sqrt(s2_1((i-1)*int+1:i*int,1)),1)];
    fill([z1((i-1)*int+1:i*int,1); flipdim( z1((i-1)*int+1:i*int,1), 1) ], f, [7 7 7]/8); hold on;
    plot(x1_initial((i-1)*int_initial+1:i*int_initial,1), y1a_initial((i-1)*int_initial+1:i*int_initial,1), 'g+'); 
    plot(z1((i-1)*int+1:i*int,1), ydata1a((i-1)*int+1:i*int,1), 'b-.');  plot(z1((i-1)*int+1:i*int,1), m_1((i-1)*int+1:i*int,1), 'r');
    plot(t_specific{pred_loc(i)}, u_specific{pred_loc(i)}, 'm+');
    ylabel('u-velocity (m/s)'); dateaxis('x',12);    ylim([-4.75 4.75]);  xlim([t_raw_node_reduced{pred_loc(i)}(tr_start) t_raw_node_reduced{pred_loc(i)}(test_end)]);

    subplot(num_prediction, 2, 2*i)
    f=[m_1(u_gap+(i-1)*int+1:u_gap+i*int,1) + 2*sqrt(s2_1(u_gap+(i-1)*int+1:u_gap+i*int,1));  flipdim( m_1(u_gap+(i-1)*int+1:u_gap+i*int,1) - 2*sqrt(s2_1(u_gap+(i-1)*int+1:u_gap+i*int,1)),1)];
    fill([z1((i-1)*int+1:i*int,1); flipdim( z1((i-1)*int+1:i*int,1), 1) ], f, [7 7 7]/8); hold on;
    plot(x1_initial((i-1)*int+1:i*int,1), y1b_initial((i-1)*int+1:i*int,1), 'g+'); 
    plot(z1((i-1)*int+1:i*int,1), ydata1b((i-1)*int+1:i*int,1), 'b-.'); 
    plot(z1((i-1)*int+1:i*int,1), m_1(u_gap+(i-1)*int+1:u_gap+i*int,1), 'r');
    plot(t_specific{pred_loc(i)}, v_specific{pred_loc(i)}, 'm+');
    ylabel('v-velocity (m/s)'); dateaxis('x',12);    ylim([-2.75 2.75]);  xlim([t_raw_node_reduced{pred_loc(i)}(tr_start) t_raw_node_reduced{pred_loc(i)}(test_end)]);
end
mse_u_1_5 = [] ; mse_v_1_5 = [];
for i=1:num_prediction
    mse_u     = sqrt(sum( (m_1((i-1)*int+1:i*int,1) - ydata1a((i-1)*int+1:i*int,1)).^2 )/int);
    mse_u_1_5 = [mse_u_1_5; mse_u];
    mse_v     = sqrt(sum( (m_1(u_gap+(i-1)*int+1:u_gap+i*int,1) - ydata1b((i-1)*int+1:i*int,1)).^2 )/int);    
    mse_v_1_5 = [mse_v_1_5; mse_v];
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

pred_loc=[6:10]';
num_prediction=size(pred_loc,1);

ydata1a=[]; ydata1b=[];  z1=[];
for i=1:num_prediction
    ydata1a=[ ydata1a; u_raw_node_reduced{pred_loc(i)}(test_start:test_end)' ] ;    
    ydata1b=[ ydata1b; v_raw_node_reduced{pred_loc(i)}(test_start:test_end)' ] ;
    z1 = [ z1; t_raw_node_reduced{pred_loc(i)}(test_start:test_end)' dlat(pred_loc(i))*ones(1,(test_end-test_start+1))' dlong(pred_loc(i))*ones(1,(test_end-test_start+1))'];
end
ydata1 =[ydata1a; ydata1b];
int=test_end-test_start+1;

tr_start=1; tr_end=2184;

y1a_initial=[]; y1b_initial=[];  x1_initial=[];
for i=1:num_prediction
    y1a_initial=[ y1a_initial; u_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' ] ;    
    y1b_initial=[ y1b_initial; v_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' ] ;
    x1_initial = [ x1_initial; t_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' dlat(pred_loc(i))*ones(1,(tr_end-tr_start+1))' dlong(pred_loc(i))*ones(1,(tr_end-tr_start+1))'];
end
y1_initial =[y1a_initial; y1b_initial];
int_initial=tr_end-tr_start+1;

[m_2 s2_2]  = gp(hyp2, inf2, meanfunc, covfunc, likfunc, x1,y1,z1);

u_gap=int*num_prediction;

figure(51)
for i=1:num_prediction
    subplot(num_prediction, 2, 2*(i-1)+1)
    f=[m_2((i-1)*int+1:i*int,1) + 2*sqrt(s2_2((i-1)*int+1:i*int,1));  flipdim( m_2((i-1)*int+1:i*int,1) - 2*sqrt(s2_2((i-1)*int+1:i*int,1)),1)];
    fill([z1((i-1)*int+1:i*int,1); flipdim( z1((i-1)*int+1:i*int,1), 1) ], f, [7 7 7]/8); hold on;
    plot(x1_initial((i-1)*int_initial+1:i*int_initial,1), y1a_initial((i-1)*int_initial+1:i*int_initial,1), 'g+'); 
    plot(z1((i-1)*int+1:i*int,1), ydata1a((i-1)*int+1:i*int,1), 'b-.');  plot(z1((i-1)*int+1:i*int,1), m_2((i-1)*int+1:i*int,1), 'r');
    plot(t_specific{pred_loc(i)}, u_specific{pred_loc(i)}, 'm+');    
    ylabel('u-velocity (m/s)'); dateaxis('x',12);    ylim([-4.75 4.75]);  xlim([t_raw_node_reduced{pred_loc(i)}(tr_start) t_raw_node_reduced{pred_loc(i)}(test_end)]);

    subplot(num_prediction, 2, 2*i)
    f=[m_2(u_gap+(i-1)*int+1:u_gap+i*int,1) + 2*sqrt(s2_2(u_gap+(i-1)*int+1:u_gap+i*int,1));  flipdim( m_2(u_gap+(i-1)*int+1:u_gap+i*int,1) - 2*sqrt(s2_2(u_gap+(i-1)*int+1:u_gap+i*int,1)),1)];
    fill([z1((i-1)*int+1:i*int,1); flipdim( z1((i-1)*int+1:i*int,1), 1) ], f, [7 7 7]/8); hold on;
    plot(x1_initial((i-1)*int+1:i*int,1), y1b_initial((i-1)*int+1:i*int,1), 'g+'); 
    plot(z1((i-1)*int+1:i*int,1), ydata1b((i-1)*int+1:i*int,1), 'b-.'); 
    plot(z1((i-1)*int+1:i*int,1), m_2(u_gap+(i-1)*int+1:u_gap+i*int,1), 'r');
    plot(t_specific{pred_loc(i)}, v_specific{pred_loc(i)}, 'm+');    
    ylabel('v-velocity (m/s)'); dateaxis('x',12);    ylim([-2.75 2.75]);  xlim([t_raw_node_reduced{pred_loc(i)}(tr_start) t_raw_node_reduced{pred_loc(i)}(test_end)]);
end
mse_u_5_10 = [] ; mse_v_5_10 = [];
for i=1:num_prediction
    mse_u = sqrt(sum( (m_2((i-1)*int+1:i*int,1) - ydata1a((i-1)*int+1:i*int,1)).^2 )/int);
    mse_u_5_10 = [mse_u_5_10; mse_u]; 
    mse_v = sqrt(sum( (m_2(u_gap+(i-1)*int+1:u_gap+i*int,1) - ydata1b((i-1)*int+1:i*int,1)).^2 )/int);    
    mse_v_5_10 = [mse_v_5_10; mse_v];
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

pred_loc=[11:15]';
num_prediction=size(pred_loc,1);

ydata1a=[]; ydata1b=[];  z1=[];
for i=1:num_prediction
    ydata1a=[ ydata1a; u_raw_node_reduced{pred_loc(i)}(test_start:test_end)' ] ;    
    ydata1b=[ ydata1b; v_raw_node_reduced{pred_loc(i)}(test_start:test_end)' ] ;
    z1 = [ z1; t_raw_node_reduced{pred_loc(i)}(test_start:test_end)' dlat(pred_loc(i))*ones(1,(test_end-test_start+1))' dlong(pred_loc(i))*ones(1,(test_end-test_start+1))'];
end
ydata1 =[ydata1a; ydata1b];
int=test_end-test_start+1;

tr_start=1; tr_end=2184;

y1a_initial=[]; y1b_initial=[];  x1_initial=[];
for i=1:num_prediction
    y1a_initial=[ y1a_initial; u_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' ] ;    
    y1b_initial=[ y1b_initial; v_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' ] ;
    x1_initial = [ x1_initial; t_raw_node_reduced{pred_loc(i)}(tr_start:tr_end)' dlat(pred_loc(i))*ones(1,(tr_end-tr_start+1))' dlong(pred_loc(i))*ones(1,(tr_end-tr_start+1))'];
end
y1_initial =[y1a_initial; y1b_initial];
int_initial=tr_end-tr_start+1;

[m_3 s2_3]  = gp(hyp2, inf2, meanfunc, covfunc, likfunc, x1,y1,z1);

u_gap=int*num_prediction;

figure(52)
for i=1:num_prediction
    subplot(num_prediction, 2, 2*(i-1)+1)
    f=[m_3((i-1)*int+1:i*int,1) + 2*sqrt(s2_3((i-1)*int+1:i*int,1));  flipdim( m_3((i-1)*int+1:i*int,1) - 2*sqrt(s2_3((i-1)*int+1:i*int,1)),1)];
    fill([z1((i-1)*int+1:i*int,1); flipdim( z1((i-1)*int+1:i*int,1), 1) ], f, [7 7 7]/8); hold on;
    plot(x1_initial((i-1)*int_initial+1:i*int_initial,1), y1a_initial((i-1)*int_initial+1:i*int_initial,1), 'g+'); 
    plot(z1((i-1)*int+1:i*int,1), ydata1a((i-1)*int+1:i*int,1), 'b-.');  plot(z1((i-1)*int+1:i*int,1), m_3((i-1)*int+1:i*int,1), 'r');
    plot(t_specific{pred_loc(i)}, u_specific{pred_loc(i)}, 'm+');    
    ylabel('u-velocity (m/s)'); dateaxis('x',12);    ylim([-4.75 4.75]);  xlim([t_raw_node_reduced{pred_loc(i)}(tr_start) t_raw_node_reduced{pred_loc(i)}(test_end)]);

    subplot(num_prediction, 2, 2*i)
    f=[m_3(u_gap+(i-1)*int+1:u_gap+i*int,1) + 2*sqrt(s2_3(u_gap+(i-1)*int+1:u_gap+i*int,1));  flipdim( m_3(u_gap+(i-1)*int+1:u_gap+i*int,1) - 2*sqrt(s2_3(u_gap+(i-1)*int+1:u_gap+i*int,1)),1)];
    fill([z1((i-1)*int+1:i*int,1); flipdim( z1((i-1)*int+1:i*int,1), 1) ], f, [7 7 7]/8); hold on;
    plot(x1_initial((i-1)*int+1:i*int,1), y1b_initial((i-1)*int+1:i*int,1), 'g+'); 
    plot(z1((i-1)*int+1:i*int,1), ydata1b((i-1)*int+1:i*int,1), 'b-.'); 
    plot(z1((i-1)*int+1:i*int,1), m_3(u_gap+(i-1)*int+1:u_gap+i*int,1), 'r');
    plot(t_specific{pred_loc(i)}, v_specific{pred_loc(i)}, 'm+');    
    ylabel('v-velocity (m/s)'); dateaxis('x',12);    ylim([-2.75 2.75]);  xlim([t_raw_node_reduced{pred_loc(i)}(tr_start) t_raw_node_reduced{pred_loc(i)}(test_end)]);
end
mse_u_10_15 = [] ; mse_v_10_15 = [];
for i=1:num_prediction
    mse_u     = sqrt(sum( (m_3((i-1)*int+1:i*int,1) - ydata1a((i-1)*int+1:i*int,1)).^2 )/int);
    mse_u_10_15 = [mse_u_10_15; mse_u];
    mse_v     = sqrt(sum( (m_3(u_gap+(i-1)*int+1:u_gap+i*int,1) - ydata1b((i-1)*int+1:i*int,1)).^2 )/int);    
    mse_v_10_15 = [mse_v_10_15; mse_v];
end

mse_u_all = [mse_u_1_5; mse_u_5_10; mse_u_10_15];            mse_v_all = [mse_v_1_5; mse_v_5_10; mse_v_10_15];
mse_all = [mse_u_all, mse_v_all]
