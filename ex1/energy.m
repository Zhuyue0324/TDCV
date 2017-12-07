function [E] = energy(cameraMatrix,correspondences,tTukey) %TODO untested!
    %% Computes the energy to be minimized in pose refinement
    % Based on reprojection error, weighted by a M function (cf II 39-45)
    % Inputs:
    %  cameraMatrix   the camera matrix taking R,T and A into account (cf
    %   myransac.m)
    %  correspondences   is a (2+3)*N vector whose columns represent a
    %   point, ie. its 2D coordinates and the 3D given by SIFT-matching
    %  tTukey is the threshold at which we flatten the Tukey estimator, ie.
    %   c from slide II.41
    % Output:
    %  E = sum(M(d(2D,reproj(3D))²))
    
    %% Reproject the 3D given camMatrix
    h = (correspondences(3:5,:))';
    h(:,4) = 1;
    h_cM = h * cameraMatrix;
    reprojection=(h_cM(:,1:2)./h_cM(:,3))';
    
    %% compute energy
    diff=(reprojection(1:2,:)-correspondences(1:2,:));
    reprojectionError=(diff(1,:).^2)+(diff(2,:).^2);
    maxxed = max(reprojectionError,tTukey^2);
    E = sum(maxxed)/2; % unnecessary constant comes from the lecture
end

