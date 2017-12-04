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

% Currently I write the code for one particular image, it should be easy to
% change it into a for loop when all is done and good :) -- thomas

%% Set up VLFeat
matlabpath = '../../../MATLAB/';
run(strcat(matlabpath,'vlfeat-0.9.20/toolbox/vl_setup'));

%% Extract and match SIFT features

image_prefix = '../../data/images/detection/DSC_';
my_image = 9774;
image_suffix = '.JPG';
image_path = strcat(image_prefix,int2str(my_image),image_suffix);

[f,d] = compute_sift(image_path, 0);

% Concatenate all 8 init_[f,d,3dc]
Fi = [];
Di = [];
Ci = []; % coordinates in 3D
init_sift_prefix = 'sift/init';
for i = 1:8
    Fi = [Fi,importdata(strcat(init_sift_prefix,'_f_',int2str(i)))];
    Di = [Di,importdata(strcat(init_sift_prefix,'_d_',int2str(i)))];
    Ci = [Ci,importdata(strcat(init_sift_prefix,'_3dc_',int2str(i)))];
end
Di = cast(Di,'like',d); % cast type of Di to that of d
% not necessary for Fi,Ci as their "floatness" is ok

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