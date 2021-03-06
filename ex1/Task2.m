%% General overview
% Purpose: estimate and interpret camera pose for images in
% images/detection

% In that intent, we apply the following steps:
% - Extract SIFT features of those images
% - Match them to the features of init images to get their supposed 3D
%   model
% - Apply RANSAC to find a consistent enough triplet of feature coupling
%   (it tests the consistency of such a triplet by applying P3P to it and
%   verifying the sum of squared errors between the features' position and
%   that of the projection of their supposed 3D model)
% - RANSAC returns the world pose: we're done.


%% Config
% Set up local paths
config

% Set up VLFeat
run(strcat(matlabpath,'vlfeat-0.9.20/toolbox/vl_setup'));

%% Retrieve SIFT features from initial images

Fi = [];
Di = [];
Ci = []; % coordinates in 3D
init_sift_prefix = 'sift/init';
for i = 1:8
    Fi = [Fi,importdata(strcat(init_sift_prefix,'_f_',int2str(i)))];
    Di = [Di,importdata(strcat(init_sift_prefix,'_d_',int2str(i)))];
    Ci = [Ci,importdata(strcat(init_sift_prefix,'_3dc_',int2str(i)))];
end
% not necessary for Fi,Ci as their "floatness" is ok

%% Loop over all images
for my_image = 9751:9774
    %% Extract and match SIFT features
    image_prefix = strcat(datapath,'images/detection/DSC_');
    image_suffix = '.JPG';
    image_path = strcat(image_prefix,int2str(my_image),image_suffix);

    [f,d] = compute_sift(image_path, 0);
    Di = cast(Di,'like',d); % cast type of Di to that of d
    [matches,scores] = vl_ubcmatch(d,Di);
    % [size(matches),size(scores)]

    %% Create the 5 x N_matched input to RANSAC
    % its rows are of type [x2D,y2D, x3D,y3D,z3D]
    % with 2D and 3D the two paired points

    pairs = [];
    for i = 1:size(matches,2)
        C2D = reshape(f(1:2,matches(1,i)),[2,1]);
        C3D = reshape(Ci(:,matches(2,i)),[3,1]);
        pairs = [pairs , [C2D;C3D]];
    end
    % size(pairs)
    %% Use RANSAC and save best model

    
    [output, bestR, bestT, bestNbInliers] = myransac(pairs,10,100,300);
    % having N=1000 doesn't change much, and we'll do refinement anyway
    % ...N means the distance, threshold
    bestM = [bestR;bestT];
    
    %simple_save(strcat('poses/2D3D_',int2str(my_image),'.csv'), pairs);
    %simple_save(strcat('poses/DSC_',int2str(my_image),'.csv'), bestM);
    disp(strcat('Didn''t save pose for image ',int2str(my_image),...
        ', nbInliers=',int2str(bestNbInliers)))
end