%% Pose refinement

%% Config
config
run(strcat(matlabpath,'vlfeat-0.9.20/toolbox/vl_setup'));


for my_image = 9751:9774
    %% Load the data
    poses_prefix = 'poses/DSC_';
    coor_prefix = 'poses/2D3D_';
    bestM = (importdata(strcat(poses_prefix,int2str(my_image),'.csv')));
      %   = [bestR ; bestT]
    data = (importdata(strcat(coor_prefix,int2str(my_image),'.csv')));
      %  = pairs, dim = 2+3 * N
    n_iters = 100;
    tau = 0.001;
    [RR,TT]= cameraPoseToExtrinsics(bestM(1:3,:),bestM(4,:)');
    RTinput=[exponentialMap(RR),TT];

    % Displays first and last E
    [refined, inliers] = lm_algorithm(data, RTinput, n_iters, tau);

    %% Use MATLAB's fminsearch (DEBUG)
    % fms(my_image);
    
    %% Save the refined poses
    simple_save(strcat('poses/refined_',int2str(my_image),'.csv'), bestM);
    disp(strcat('Saved refined model for image ',int2str(my_image),...
        ', nbInliers=',int2str(inliers)))
end
