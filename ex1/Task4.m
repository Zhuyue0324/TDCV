%% needs lm_algo to work better though

%% a) IRLS
% To estimate the current pose, run one iteration of weighted LM ...
% As a first approx. let's use basic LM

%% Find initial pose of 9775 using RANSAC and LM (copy-paste of task 2 :S)
Fi = [];
Di = [];
Ci = []; % coordinates in 3D
init_sift_prefix = 'sift/init';
for i = 1:8
    Fi = [Fi,importdata(strcat(init_sift_prefix,'_f_',int2str(i)))];
    Di = [Di,importdata(strcat(init_sift_prefix,'_d_',int2str(i)))];
    Ci = [Ci,importdata(strcat(init_sift_prefix,'_3dc_',int2str(i)))];
end
first_image = 9775;
image_prefix = strcat(datapath,'images/tracking/DSC_');
image_suffix = '.JPG';
image_path = strcat(image_prefix,int2str(first_image),image_suffix);
[f,d] = compute_sift(image_path, 0);
Di = cast(Di,'like',d); % cast type of Di to that of d
[matches,scores] = vl_ubcmatch(d,Di);
pairs = [];
for i = 1:size(matches,2)
    C2D = reshape(f(1:2,matches(1,i)),[2,1]);
    C3D = reshape(Ci(:,matches(2,i)),[3,1]);
    pairs = [pairs , [C2D;C3D]];
end
[output, bestR, bestT, bestNbInliers] = myransac(pairs,10,100,300);
bestM = [bestR;bestT];
%simple_save(strcat('poses/2D3D_',int2str(my_image),'.csv'), pairs);
%simple_save(strcat('poses/DSC_',int2str(my_image),'.csv'), bestM);
disp(strcat('RANSAC done for initial image, nbInliers=',int2str(bestNbInliers)))
%% LM on first image
n_iters = 100;
tau = 0.1;

RTinput=[exponentialMap(bestM(1:3,:)),bestM(4,:)];
[refined, inliers] = lm_algorithm(pairs, RTinput, n_iters, tau);
simple_save(strcat('tracking/DSC_',int2str(first_image),'.csv'), refined);
disp(strcat('LM done for initial, nbInliers=',int2str(inliers)))

%% (TODO still unweighted) LM on following images
previousSolution = refined;
lambda = 1e-3;
tau = 1 % ???
u = tau + 1
for image=9776:9821
    % find 2D-3D ...
    if u <= tau
        break;
    end
    %TODO compute e, sigma, w to input to WLM
    [nextSolution, inliers] = lm_algorithm(pairs, previousSolution, n_iters, tau);
    %TODO update u with |Delta|
    simple_save(strcat('tracking/DSC_',int2str(image),'.csv'), nextSolution);
    disp(strcat('LM done for image ',int2str(first_image),...
        ', nbInliers=',int2str(inliers)))

end
