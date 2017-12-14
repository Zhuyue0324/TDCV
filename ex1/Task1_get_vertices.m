%% Task 1 get vertices
initialAlignment('../../data/images/init_texture/DSC_9743.jpg', '../ex1/init1.txt', [1,2,3,4,7,8])
initialAlignment('../../data/images/init_texture/DSC_9744.jpg', '../ex1/init2.txt', [1,2,3,4,6,7,8])
initialAlignment('../../data/images/init_texture/DSC_9745.jpg', '../ex1/init3.txt', [1,2,3,4,6,7])
initialAlignment('../../data/images/init_texture/DSC_9746.jpg', '../ex1/init4.txt', [1,2,3,4,5,6,7])
initialAlignment('../../data/images/init_texture/DSC_9747.jpg', '../ex1/init5.txt', [1,2,3,4,5,6])
initialAlignment('../../data/images/init_texture/DSC_9748.jpg', '../ex1/init6.txt', [1,2,3,4,5,6,8])
initialAlignment('../../data/images/init_texture/DSC_9749.jpg', '../ex1/init7.txt', [1,2,3,4,5,8])
initialAlignment('../../data/images/init_texture/DSC_9750.jpg', '../ex1/init8.txt', [1,2,3,4,5,7,8])

%% Create 3D model and initialize camera intrinsics
d3path='position_vertices_3d.txt';
m3d=importdata(d3path);
f = 2960.37845;
cx = 1841.68855;
cy = 1235.23369;
image_width = 3680;
image_height = 2456;
IntrinsicMat=cameraIntrinsics([f,f],[cx,cy],[image_width,image_height]);
A=[   f, 0,cx;...
      0, f,cy;...
      0, 0, 1];
% run('../../../MATLAB/vlfeat-0.9.20/toolbox/vl_setup')

%% Config
config
run(strcat(matlabpath,'vlfeat-0.9.20/toolbox/vl_setup'));




%% Compute descriptors for initial images
first_idx = 9742;
for ii = 1:8
    disp(strcat('Getting SIFT for image ',int2str(ii)))
    % Compute 2D descriptors
    [f,d] = compute_sift(strcat(datapath,...
        'images/init_texture/DSC_',int2str(first_idx + ii),...
        '.jpg'), 0);
    disp('  Computing its 3D Coordinates')
    % Keep and convert to 3D those that lay in the teabox
    [sf,sd,nc] = find3D(f,d,strcat('init',int2str(ii),'.txt'),...
        'position_vertices_3d.txt','position_triangles_3d.txt');
    
    % % Check 10 first 3D-2D pairs
    % pair = [nc(:,1:10);0.001*sf(1:2,1:10)]
    % % Plot 3D cloud
    % figure;
    % pcshow(pointCloud(nc'));
    
    % % reproject 3D back to 2D
    % [R,T] = poseEstimator(strcat('init',int2str(ii),'.txt'),d3path,...
    %     IntrinsicMat);
    % [rm,tv] = cameraPoseToExtrinsics(R,T);
    % camMatrix = cameraMatrix(IntrinsicMat,rm,tv);
    % h = [nc]';
    % h(:,4) = 1;% m3d in homogeneous coordinates
    % test = h * camMatrix;
    % glad = [(test(:,1:2)./test(:,3))';sf(1:2,:)]
    % % first two lines are reprojected, last two original
    
    % % Plot the number of inliers
    % diff=(glad(1:2,:)-glad(3:4,:));
    % reprojectionError=(diff(1,:).^2)+(diff(2,:).^2);
    % fplot(@(x) sum(reprojectionError<=x),[0,1000]);
    
    disp('  not Saving the result')
    % Save the result
    %simple_save(strcat('sift/init_f_',int2str(ii))  , sf);
    %simple_save(strcat('sift/init_d_',int2str(ii))  , sd);
    %simple_save(strcat('sift/init_3dc_',int2str(ii)), nc);
end
disp('Done.')




