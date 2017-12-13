function [E] = energy(h1,h2,tTukey) %TODO untested!
    %% Computes the energy to be minimized in pose refinement
    % Based on reprojection error, weighted by a M function (cf II.39-45)
    
    % Inputs:
    %  hi are two 2*N vector whose columns represent a
    %   point, ie. its 2D coordinates in both models
    %  tTukey is the threshold at which we flatten the Tukey estimator, ie.
    %   c from slide II.41
    % Output:
    %  E = sum(M(d(2D,reproj(3D))?))
    
    %% compute energy
    diff=(h1(1:2,:)-h2(1:2,:));
    reprojectionError=(diff(1,:).^2)+(diff(2,:).^2);
    maxxed = max(reprojectionError,tTukey^2);
    E = sum(maxxed)/2; % unnecessary constant comes from the lecture 
end

