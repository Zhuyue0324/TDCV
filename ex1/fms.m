% Try using fminsearch to refine RT

%% Define function ft to minimize
cm = @(RT, A) cameraMatrix(A, rotationMatrix(RT(1:3)), RT(4:6)');
fold = @(h) h(1:2,:)./h(3,:);
ft = @(RT, A, C3, C2) sum(sum(max(...
    (fold(cm(RT,A)' * C3) - C2).^2)),...
    10000);

%% Get the initial pose and the other parameters
my_image = 9760;
poses_prefix = 'poses/DSC_';
coor_prefix = 'poses/2D3D_';
bestM = (importdata(strcat(poses_prefix,int2str(my_image),'.csv')));
data = (importdata(strcat(coor_prefix,int2str(my_image),'.csv')));
[RR,TT]= cameraPoseToExtrinsics(bestM(1:3,:),bestM(4,:)');
f = 2960.37845;
cx = 1841.68855;
cy = 1235.23369;

RTinput = [exponentialMap(RR),TT];
C2 = data(1:2,:);
C3 = data(3:5,:);
C3(4,:) = 1;
A = cameraIntrinsics([f,f],[cx,cy],[3680,2456]);

%% Test fms!
RTinput, ft(RTinput, A, C3, C2)
[refined, emin] = fminsearch(@(RT) ft(RT, A, C3, C2), RTinput)
    