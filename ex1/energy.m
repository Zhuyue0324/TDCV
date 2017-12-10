function [E] = energy(h1,h2,tTukey) % 
    %% Computes the energy to be minimized in pose refinement
    % Based on reprojection error, weighted by a M function (cf II.39-45)
    
    % Inputs:
    %  hi are two 2*N vector whose columns represent a
    %   point, ie. its 2D coordinates in both models
    %  tTukey is the threshold at which we flatten the Tukey estimator, ie.
    %   c from slide II.41
    % Output:
    %  E = sum(M(d(2D,reproj(3D))�))
    
    %% compute energy
    diff=(h1(1:2,:)-h2(1:2,:));
    %reprojectionError=[(diff(1,:).^2),(diff(2,:).^2)];
    %maxxed = max(reprojectionError,tTukey^2);
    disp(((diff(:,1:10))))
    diff=diff.^2;
    sizen=size(diff);
    n=sizen(2);
    E=[];
    for i=1:n
        E(2*i-1) = max(diff(1,i),tTukey^2);
        E(2*i) = max(diff(2,i),tTukey^2);
        %E(i) =(maxxed)/2; % unnecessary constant comes from the lecture
    end
end

