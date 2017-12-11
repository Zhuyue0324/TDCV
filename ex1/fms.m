%% Try to use fminsearch to refine RT
my_image = 9760;
poses_prefix = 'poses/DSC_';
coor_prefix = 'poses/2D3D_';
bestM = (importdata(strcat(poses_prefix,int2str(my_image),'.csv')));
data = (importdata(strcat(coor_prefix,int2str(my_image),'.csv')));
[RR,TT]= cameraPoseToExtrinsics(bestM(1:3,:),bestM(4,:)');
RTinput=[exponentialMap(RR),TT];
h2d = data(1:2,:);
h3d = data(3:5,:);
h3d(4,:) = 1;
sizen=size(data);
n=sizen(2);
f = 2960.37845;
cx = 1841.68855;
cy = 1235.23369;
IntrinsicMat=cameraIntrinsics([f,f],[cx,cy],[3680,2456]);
RTinput;

fun = @(RT)eFromRT(RT, IntrinsicMat, h2d, h3d);
fun(RTinput);

[refined, emin] = fminsearch(fun, RTinput);

    