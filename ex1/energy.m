function [E, nbInliers] = energy(h1,h2,tTukey, show) % 
    %% Computes the energy to be minimized in pose refinement
    % Based on reprojection error, weighted by a M function (cf II.39-45)
    
    % Inputs:
    %  hi are two 2*N vector whose columns represent a
    %   point, ie. its 2D coordinates in both models
    %  tTukey is the threshold at which we flatten the Tukey estimator, ie.
    %   c from slide II.41
    % Output:
    %  E = sum(M(d(2D,reproj(3D))²))
    
    %% compute energy
    diff=(h1(1:2,:)-h2(1:2,:));
    %reprojectionError=[(diff(1,:).^2),(diff(2,:).^2)];
    %maxxed = max(reprojectionError,tTukey^2);
    
    diff=diff.^2;
    sizen=size(diff);
    n=sizen(2);
    E=[];
    nbInliers = 0;
    for i=1:n
        E(2*i-1) = min(diff(1,i),.5*tTukey^2);
        E(2*i) = min(diff(2,i),.5*tTukey^2); % not quite sure separating both dimensions is legit ...
        if(diff(1,i)+diff(2,i) < tTukey^2)
            nbInliers = nbInliers + 1;
        end
    end
    if show>0
        % disp(((diff(:,1:10))))
        % disp(E(:,1:10))
        inliers = nbInliers
    end
end

