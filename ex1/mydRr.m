function [dRr] = mydRr(R)
    w = exponentialMap(R);
    disp(w(1));
    I = eye(3,3);
    e = [1 0 0];
    %dRr(i) is a 3*3 matrix;
    %(I-R)*e is 3*1
    b_0 = (I - R) * e';
    disp(b_0);
    b = myskew(cross(w,(I - R) * e'));
    disp(b);
    dRr = ((w(1)*myskew(w) + myskew(cross(w,(I - R) * e)))/norm(w,2)) * R;

    
function [skew] = myskew(a)
    skew = [0 -a(3) a(2);...
        a(3) 0 -a(1);...
        -a(2) a(1) 0];

    
% function [dRr] = mydRr(R)
% w = exponentialMap(R);
% I = eye(3,3);
% for i=1:3
%     e = zeros(3,1);
%     e(i)=1;
%     %dRr(i) is a 3*3 matrix;
%     %(I-R)*e is 3*1
%     dRr(i) = ((w(i) * myskew(w) + myskew(cross(w,(I-R)*e)))/norm(w,2)) * R;