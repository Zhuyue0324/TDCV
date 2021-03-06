% initialize lambda with a small value: lambda = 0.001
% Compute delta_i and E(p_i+delta_i)
%If E(p_i+delta_i)>E(p_i), lambda = 10lambda
%If E(p_i+delta_i)<E(p_i), lambda = 0.1lambda

%%Use Levenberg-Marquart algorithm to refine the pose(R,T), now the
%%following code is just about a single pose.
%this is the main function we want to use`
function[refinedRT, inliers] = lm_algorithm(data, RTinput, n_iters, tau)

    %% Load data, set parameters
    h2d = data(1:2,:);
    h3d = data(3:5,:);
    h3d(4,:) = 1;
    %h2d(3,:) = 1;
    %disp(size(data))
    sizen=size(data);
    n=sizen(2);
    f = 2960.37845;
    cx = 1841.68855;
    cy = 1235.23369;
    IntrinsicMat=cameraIntrinsics([f,f],[cx,cy],[3680,2456]);
    % t is a threshold with respect to delta
    u = tau + 1;
    lambda = 0.001;
    tukey = 100;
    
    %% Compute initial energy
    RT= RTinput;
    
    %disp(RT)
    R = rotationMatrix(RT(1:3));
    T = RT(4:6)';
            
    E = eye(3,3);
    I = eye(6,6);
    
    %[rm,tv] = cameraPoseToExtrinsics(R,T);
    camMatrix = cameraMatrix(IntrinsicMat,R,T);
    m_homo =  camMatrix' * h3d;
    m = (m_homo(1:2,:)./m_homo(3,:));
    [e,~] = energy(m,h2d,tukey,0);
    firstE = sum(e);
    %disp(sum(e))
    for t=1:n_iters 
        if u > tau
            %% compute J
            J =[];
            % derivative of R
            [dRr1,dRr2,dRr3] = mydRr(R);
            
            for i = 1:n
                %dRrM is (the derivative of R) * h3d
                dRrM1 = dRr1 * h3d(1:3,i);
                dRrM2 = dRr2 * h3d(1:3,i);
                dRrM3 = dRr3 * h3d(1:3,i);
                dMp=[dRrM1,dRrM2,dRrM3,E];
            
                dm = [1/m_homo(3,i), 0, -m_homo(1,i)/(m_homo(3,i)^2); ...
                      0, 1/m_homo(3,i), -m_homo(2,i)/(m_homo(3,i)^2)];
                J((2*i-1):(2*i),:) = dm * (IntrinsicMat.IntrinsicMatrix)' * dMp;
            end
            
            %% compute and apply change to the solution
            %sqrtE = sqrt(e');
            Delta = -inv(J' * J + lambda * I)*(J' * sqrtenergy(m,h2d,tukey)');
            RTnew = RT + Delta';
            Rnew = rotationMatrix(RTnew(1:3));
            Tnew = RTnew(4:6)';
            %J
            %JJ = J' * J
            % compute new energy
            %[rm,tv] = cameraPoseToExtrinsics(R,T);
            camMatrix = cameraMatrix(IntrinsicMat,Rnew,Tnew);
            m_homo =  camMatrix' * h3d;
            m = (m_homo(1:2,:)./m_homo(3,:));
            [enew,inliers] = energy(m,h2d,tukey,0);
             %since we don't reverse the change, e should be updated anyway!
            
            %% update lambda (toggle between SGD and GN)
            if norm(enew,1) > norm(e, 1)
                lambda = 10 * lambda;
                
                %disp(RTnew)
                %disp(sum(enew))
            else
                lambda = 0.1 * lambda;
                RT=RTnew;
                R=Rnew;
                T=Tnew;
                e=enew;
            end
            
            u = norm(Delta);
            %disp(u-tau)
            %disp(RT)
            %disp(sum(e))
            
        else
            break
        end
    end
    lastE = sum(e);
    [firstE,lastE]
    refinedRT = RT;

    %%Compute derivative of 3D rotation in exponential coordinates    
function [dRr1,dRr2,dRr3] = mydRr(R)
    w = exponentialMap(R);
    I = eye(3,3);
    %for i=1:3
    %    e = zeros(3,1);
    %    e(i)=1;
    %    %dRr(i) is a 3*3 matrix;
    %    dRr(i) = ((w(i) * myskew(w) + myskew(cross(w,(I-R)*e)))/norm(w,2))*R;
    %end
    dRr1 = ((w(1) * myskew(w) + myskew(cross(w,(I-R)*[1;0;0])))/norm(w,2))*R;
    dRr2 = ((w(2) * myskew(w) + myskew(cross(w,(I-R)*[0;1;0])))/norm(w,2))*R;
    dRr3 = ((w(3) * myskew(w) + myskew(cross(w,(I-R)*[0;0;1])))/norm(w,2))*R;
    
% Compute the skew symmetric matrix of a,
% a should be imputed as a 3 by 1 vector
function [skew] = myskew(a)
    skew = [0    -a(3) a(2);...
            a(3)  0   -a(1);...
           -a(2)  a(1) 0];

   


    
    