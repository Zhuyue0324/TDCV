%% Pose refinement

%% Config
config
run(strcat(matlabpath,'vlfeat-0.9.20/toolbox/vl_setup'));

%% Load the data
my_image = 9751;
poses_prefix = 'poses/DSC_';
coor_prefix = 'poses/2D3D_';
RT = (importdata(strcat(poses_prefix,int2str(my_image),'.csv')))';
data = (importdata(strcat(coor_prefix,int2str(my_image),'.csv')));
n_iters = 100;
tau = 0.1;
RTinput=[exponentialMap(RT(:,1:3)),RT(:,4)']
lm_algorithm(data, RTinput, n_iters, tau)