% Compute the skew symmetric matrix of a,
%a should be imputed as a 3 by 1 vector
function [skew] = myskew(a)
    skew = [0 -a(3) a(2);...
        a(3) 0 -a(1);...
        -a(2) a(1) 0];

   
%%Compute derivative of 3D rotation in exponential coordinates    
function [dRr] =mydRr(R)
    dRr = zeros(3,1);
    w = exponential(R);
    I = eye(3,3);
    for i=1:3
        e = zeros(3,1);
        e(i)=1;
        dRr(i) = ((w(i) * myskew(w) + myskew(cross(w,(I-R)*e)))/norm(w,2))*R;
    end