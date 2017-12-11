function [E, nbInliers] = energy(h1,h2,tTukey, show) % 
    %% Computes the energy to be minimized in pose refinement
    % Based on reprojection error, weighted by a M function (cf II.39-45)
    
    % Inputs:
    %  hi are two 2*N vector whose columns represent a
    %   point, ie. its 2D coordinates in both models
    %  tTukey is the threshold at which we flatten the Tukey estimator, ie.
    %   c from slide II.41
    % Output:
    %  E. the lecture says we could use min(1,x²) too but this is the
    %   proper Tukey estimator, 1|1-(1-x²)^3|1.
    %  nbInliers is the number of points for which we took the non-flat
    %   value
    
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
        e = rho(diff(:,i),tTukey);
        E(2*i-1) = e;
        E(2*i) = e; % not quite sure separating both dimensions is legit ...
        if(e < tTukey^2 / 6.)
            nbInliers = nbInliers + 1;
        end
    end
    if show>0
        % disp(((diff(:,1:10))))
        % disp(E(:,1:10))
        inliers = nbInliers
    end

function [e] = rho(x, c)
    sc = c^2;
    sx = x(1)^2 + x(2)^2;
    r = 1;
    if sx <= sc
        r = 1. - (1. - (sx/sc))^3;
    end
    e = r * sc / 6.;
        

