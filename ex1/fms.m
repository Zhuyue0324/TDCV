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

% (following comment from fminsearch code, ls 48-52)
%     FUN can be a parameterized function. Use an anonymous function to
%     capture the problem-dependent parameters:
%        f = @(x,c) x(1).^2+c.*x(2).^2;  % The parameterized function.
%        c = 1.5;                        % The parameter.
%        X = fminsearch(@(x) f(x,c),[0.3;1])

% ff = @(x,cc) x(1).^2+cc.*x(2).^2;
% cc = 1.5;
% X = fminsearch(@(x) ff(x,cc),[0.3;1]) % Their example works ...

fun = @(RT, IM, C2, C3) eFromRT(RT, IM, C2, C3);
% boloss = @(RT, IM, C2, C3) ... % Doomed
%    sum(min(( (((cameraMatrix(IntrinsicMat,rotationMatrix(RT(1:3)),RT(4:6)'))' * C3)(1:2,:))./ ...
%    (((cameraMatrix(IntrinsicMat,rotationMatrix(RT(1:3)),RT(4:6)'))' * C3)(3,:)) ) .^2, 10000))

IM = IntrinsicMat;
C2 = h2d;
C3 = h3d;

[refined, emin] = fminsearch(@(RT) eFromRT(RT, IM, C2, C3), RTinput);

    