function [E] = sqrtenergy(h1,h2,tTukey) 
    
    %% compute sqrtenergy
    diff=(h1(1:2,:)-h2(1:2,:));
    %reprojectionError=[(diff(1,:).^2),(diff(2,:).^2)];
    %maxxed = max(reprojectionError,tTukey^2);
    sizen=size(diff);
    n=sizen(2);
    E=[];
    for i=1:n
        if diff(1,i)>tTukey
            E(2*i-1) = tTukey;
        elseif diff(1,i)<-tTukey
            E(2*i-1) = -tTukey;
        else
            E(2*i-1) = diff(1,i);
        end
        if diff(2,i)>tTukey
            E(2*i) = tTukey;
        elseif diff(2,i)<-tTukey
            E(2*i) = -tTukey;
        else
            E(2*i) = diff(2,i);
        end
            
        %E(i) =(maxxed)/2; % unnecessary constant comes from the lecture
    end
end