function [output, bestR, bestT, bestModelNbInlier] = myransac(data,n,N,distBoundary)
  %data: a 5*l matrix, each column is a feature point, with coodonate 2d
  %and 3d
  %n= n of PnP
  %N=number of couples of samples tried
  %output: a 5*n matrix, which is the n points which gives the best model
  %bestR, bestT: best pose model given by Ransac
  %bestReprojectionError: the E(p) given by model
  l = size(data,2);%number of features
  bestModelNbInlier = 0;
  %bestReprojectionError = 10^10;
  f = 2960.37845;
  cx = 1841.68855;
  cy = 1235.23369;
  IntrinsicMat=cameraIntrinsics([f,f],[cx,cy],[3680,2456]);
  output=[];
  bestR=[];
  bestT=[];
  for i=1:N
    %randomly choose n samples
    idx = randsample(l,n); 
    sample = data(:,idx(1:n));
    %d2=2d coodinates of 2*n, d3=3d coordinates of 3*n
    sd2=sample(1:2,:);
    sd3=sample(3:5,:);
    [R,T] = estimateWorldCameraPose(transpose(sd2),transpose(sd3),...
        IntrinsicMat,'MaxReprojectionError',1000);
    %reprojection with A(R|T)M
    [rm,tv] = cameraPoseToExtrinsics(R,T);
    camMatrix = cameraMatrix(IntrinsicMat,rm,tv);
    
    h = [data(3:5,:)]';
    h(:,4) = 1;
    test = h * camMatrix;
    reprojection=(test(:,1:2)./test(:,3))';
    diff=(reprojection(1:2,:)-data(1:2,:));
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
