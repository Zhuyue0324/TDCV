function [output, bestR, bestT, bestModelNbInlier] = myransac(data,n,N,distBoundary)
  %data: a 5*l matrix, each colone is a feature point, with coodonate 2d
  %and 3d
  %n= n of PnP
  %N=number of couples of samples tried
  %output: a 5*n matrix, which is the n points which gives the best model
  %bestR, bestT: best pose model given by Ransac
  %bestReprojectionError: the E(p) given by model
  l = size(data,2);%number of features
  bestModelNbInlier = 0;
  nbInlier = 0;
  %bestReprojectionError = 10^10;
  fx = 2960.37845;
  fy = 2960.37845;
  cx = 1841.68855;
  cy = 1235.23369;
  IntrinsicMat=cameraIntrinsics([fx,fy],[cx,cy],[3680,2456]);
  A=[];
  A(1,1)=fx;
  A(2,2)=fy;
  A(3,3)=1;
  A(3,1)=cx;
  A(3,2)=cy;%compute A
  output=[];
  bestR=[];
  bestT=[];
  for i=1:N
    %randomly choose n samples
    idx = randsample(l,n); 
    sample = data(:,idx(1:n));
    %d2=2d coodinates of 2*n, d3=3d coordinates of 3*n
    d2=sample(1:2,:);
    d3=sample(3:5,:);
    [R,T] = estimateWorldCameraPose(transpose(d2),transpose(d3),IntrinsicMat,'MaxReprojectionError',1000);
    %reprojection with A(R|T)M
    reprojection=A*(R*data(3:5,:)+transpose(T));
    diff=(reprojection(1:2,:)-data(1:2,:));
    %reprojectionError=(diff(1,:).^2)+sum(diff(2,:).^2);
    reprojectionError=(diff(1,:).^2)+(diff(2,:).^2);
    nbInlier=sum(reprojectionError<=distBoundary^2);
    %if reprojectionError<=bestReprojectionError
    %  bestReprojectionError = reprojectionError;
    %  output=sample;
    %  bestR=R;
    %  bestT=T;
    %end
    if nbInlier>bestModelNbInlier
      bestModelNbInlier=nbInlier;
      output=sample;
      bestR=R;
      bestT=T;
    end
  end  
end
