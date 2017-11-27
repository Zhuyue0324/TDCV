%% Task 1 get vertices

initialAlignment('../data/images/init_texture/DSC_9743.jpg', '../ex1/init1.txt', [1,2,3,4,7,8])
initialAlignment('../data/images/init_texture/DSC_9744.jpg', '../ex1/init2.txt', [1,2,3,4,6,7,8])
initialAlignment('../data/images/init_texture/DSC_9745.jpg', '../ex1/init3.txt', [1,2,3,4,6,7])
initialAlignment('../data/images/init_texture/DSC_9746.jpg', '../ex1/init4.txt', [1,2,3,4,5,6,7])
initialAlignment('../data/images/init_texture/DSC_9747.jpg', '../ex1/init5.txt', [1,2,3,4,5,6])
initialAlignment('../data/images/init_texture/DSC_9748.jpg', '../ex1/init6.txt', [1,2,3,4,5,6,8])
initialAlignment('../data/images/init_texture/DSC_9749.jpg', '../ex1/init7.txt', [1,2,3,4,5,8])
initialAlignment('../data/images/init_texture/DSC_9750.jpg', '../ex1/init8.txt', [1,2,3,4,5,7,8])

%% Create 3D model and initialize camera intrinsics
d3path='position_vertices_3d.txt';
m3d=importdata(d3path);
fx = 2960.37845;
fy = 2960.37845;
cx = 1841.68855;
cy = 1235.23369;
IntrinsicMat=cameraIntrinsics([fx,fy],[cx,cy],[3680,2456]);

% run('../../../MATLAB/vlfeat-0.9.20/toolbox/vl_setup')

%% Estimate all poses
[WO1,WL1] = poseEstimator('init1.txt',d3path,IntrinsicMat); % this function is defined down under
[WO2,WL2] = poseEstimator('init2.txt',d3path,IntrinsicMat);
[WO3,WL3] = poseEstimator('init3.txt',d3path,IntrinsicMat);
[WO4,WL4] = poseEstimator('init4.txt',d3path,IntrinsicMat);
[WO5,WL5] = poseEstimator('init5.txt',d3path,IntrinsicMat);
[WO6,WL6] = poseEstimator('init6.txt',d3path,IntrinsicMat);
[WO7,WL7] = poseEstimator('init7.txt',d3path,IntrinsicMat);
[WO8,WL8] = poseEstimator('init8.txt',d3path,IntrinsicMat);

%% Plot the world points
pcshow(m3d,'VerticalAxis','Y','VerticalAxisDir','down', 'MarkerSize',30);
hold on
plotCamera('Size',10,'Orientation',WO1,'Location',WL1);
hold off

%% Compute (2D) descriptors for initial images
datapath = '../../data/';
first_idx = 9742;
for ii = 1:8
    compute_then_store_sift(strcat(datapath,...
        'images/init_texture/DSC_',int2str(first_idx + ii),...
        '.jpg'), 0, strcat('_init_',int2str(ii)));
end

%% Compute triangles for each sift feature inside it
ff=importdata('sift/f_init_1');
%ff=ff(:,1:100);
dd=importdata('sift/d_init_1');
%dd=dd(:,1:100);

[sf,sd,tri] = findInliersAndTrianglesAssociate(ff,dd,'init1.txt','position_vertices_3d.txt','position_triangles_3d.txt');


%save('../init_coord.txt','x1','y1','-ascii');
%[A,B,C]=myransac(d2,5,10,2)

%% Function

%function [WO,WL] = poseEstimator(txtInput)
%    m = importdata(txtInput)
%    [WO,WL] = estimateWorldCameraPose(m(:,1:2),m3d(m(:,3),:),...
%        IntrinsicMat,'MaxReprojectionError',10)
%end


