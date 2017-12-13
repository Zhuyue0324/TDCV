function [E] = sqrtenergy(h1,h2,c) 
    t = c/sqrt(6);
    %% compute sqrtenergy
    diff=(h1(1:2,:)-h2(1:2,:));
    sizen=size(diff);
    n=sizen(2);
    E=[];
    for i=1:n
        if diff(1,i)>t
            E(2*i-1) = t;
        elseif diff(1,i)<-t
            E(2*i-1) = -t;
        else
            E(2*i-1) = diff(1,i);
        end
        if diff(2,i)>t
            E(2*i) = t;
        elseif diff(2,i)<-t
            E(2*i) = -t;
        else
            E(2*i) = diff(2,i);
        end
    end
end