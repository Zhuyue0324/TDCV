function [WO,WL] = poseEstimator(d2Input,d3Input,IntrinsicMat)
    m2d = importdata(d2Input);
    m3d=importdata(d3Input);
    [WO,WL] = estimateWorldCameraPose(m2d(:,1:2),m3d(m2d(:,3),:),...
        IntrinsicMat,'MaxReprojectionError',10);
end