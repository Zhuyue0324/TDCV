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
A=[  fx, 0,cx;...
      0,fy,cy;...
      0, 0, 1];
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

%% Config
config
run(strcat(matlabpath,'vlfeat-0.9.20/toolbox/vl_setup'));




%% Compute descriptors for initial images
first_idx = 9742;
for ii = 1:1
    disp(strcat('Getting SIFT for image ',int2str(ii)))
    % Compute 2D descriptors
    [f,d] = compute_sift(strcat(datapath,...
        'images/init_texture/DSC_',int2str(first_idx + ii),...
        '.jpg'), 0);
    disp('  Computing its 3D Coordinates')
    % Keep and convert to 3D those that lay in the teabox
    [sf,sd,nc] = find3D(f,d,strcat('init',int2str(ii),'.txt'),...
        'position_vertices_3d.txt','position_triangles_3d.txt');
    nc(:,1:10)
    sf(:,1:10)
    % Check 3D coordinates by attempting to reproject them
    [R,T] = poseEstimator(strcat('init',int2str(ii),'.txt'),d3path,...
        IntrinsicMat);
    reprojection=A*(R*nc+transpose(T));
    
    % [size(reprojection),size(sf)]
    repro = reprojection(:,1:10)
    fff = sf(1:2,1:10)
    RT = [R;T]'
    diff=(reprojection(1:2,:)./reprojection(3,:) - sf(1:2,:));
    reprojectionError=(diff(1,:).^2)+(diff(2,:).^2);
    nbInlier=sum(reprojectionError<=100)
    
    disp('  Saving the result')
    % Save the result
    simple_save(strcat('sift/init_f_',int2str(ii))  , sf);
    simple_save(strcat('sift/init_d_',int2str(ii))  , sd);
    simple_save(strcat('sift/init_3dc_',int2str(ii)), nc);
end
disp('Done.')

%% Function

%function [WO,WL] = poseEstimator(txtInput)
%    m = importdata(txtInput)
%    [WO,WL] = estimateWorldCameraPose(m(:,1:2),m3d(m(:,3),:),...
%        IntrinsicMat,'MaxReprojectionError',10)
%end


