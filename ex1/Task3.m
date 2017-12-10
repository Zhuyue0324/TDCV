%% Pose refinement

%% Config
config
run(strcat(matlabpath,'vlfeat-0.9.20/toolbox/vl_setup'));

%% testing w<->R => ok :D
R1 = [1.,0.,0.;...
     0.,0.,-1.;...
     0.,1.,0.];
R2 = [-0.216271,	-0.896039,	-0.387739;...
       0.223404,	 0.341184,	-0.913063;...
       0.95043 ,	-0.284091,	 0.12639 ]; % taken from poses
 
r = rotationMatrix(exponentialMap(R2));

%% Load the data
my_image = 9751;
poses_prefix = 'poses/DSC_';
coor_prefix = 'poses/2D3D_';
bestM = (importdata(strcat(poses_prefix,int2str(my_image),'.csv')));
  %   = [bestR ; bestT]
data = (importdata(strcat(coor_prefix,int2str(my_image),'.csv')));
  %  = pairs, dim = 2+3 * N
n_iters = 100;
tau = 0.1;

RTinput=[exponentialMap(bestM(1:3,:)),bestM(4,:)]
refined = lm_algorithm(data, RTinput, n_iters, tau);