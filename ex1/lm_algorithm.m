% initialize lambda with a small value: lambda = 0.001
% Compute delta_i and E(p_i+delta_i)
%If E(p_i+delta_i)>E(p_i), lambda = 10lambda
%If E(p_i+delta_i)<E(p_i), lambda = 0.1lambda

%%Use Levenberg-Marquart algorithm to refine the pose(R,T), now the
%%following code is just about a single pose.
%this is the main function we want to use`
function[refinedR, refinedT] = lm_algorithm(data, RT, n_iters, tau)

    h2d = data(1:2,:);
    h3d = data(3:5,:);
    h3d(:,4) = 1;
    h2d(:,3) = 1;
    
    f = 2960.37845;
    cx = 1841.68855;
    cy = 1235.23369;
    A=cameraIntrinsics([f,f],[cx,cy],[3680,2456]);
    % t is a threshold with respect to delta
    u = tau + 1;
    lambda = 0.001;
    for t=1:n_iters 
        if u > tau
            
            %% compute J
            
            % derivative of R
            R = RT(:,1:3);
            T = RT(:,4);
            dRr = mydRr(R);
            for i = 1:3
                %dRrM is the derivative of R multiply h3d
                dRrM(i) = dRr(i) * h3d;
            end
            % p=[r1,r2,r3,t1,t2,t3]'; E is an 3*3 identity matrix 
            % Mp = R(p)M + T;Now I want to compute dMp;
            E = eye(3,3);
            dMp = [dRrM(1), dRrM(2), dRrM(3), E];
            %  compute dm and then J=dm*A*dMp

            [rm,tv] = cameraPoseToExtrinsics(R,T);
            camMatrix = cameraMatrix(IntrinsicMat,rm,tv);
            m_homo = h3d * camMatrix;
            m = (m_homo(:,1:2)./m_homo(:,3))';

            e = energy(m,data(1:2,:),300);
            dm = [1/m_homo(3), 0, -m_homo(1)/m_homo(3)^2;...
                0, 1/m_homo(3), -m_homo(2)/m_homo(3)^2];
            J = dm * A.IntrinsicMatrix * dMp;

            %the size of J and I is 2 by 6
            I = eye(size(J'*J));
            % e before updating M; 
       %%%%%question: the dimension of e is 3*1 or should be 2*1 

            % update M
            % but the dimension of delta is [6*2]/[2*2] not consistent with
            % RT(3,4)?? delta() ??wierd
            delta = -(J' * e) / (J' * J + lambda * I);
            RT = RT + delta;
            % the new e after updating M
            enew = m - h2d;
            if norm(enew,1) > norm(e, 1)
                lambda = 10 * lambda;
            else
                lambda = 0.1 * lambda;
            end
            u = delta;
        else
            break
        end
    end
    refinedR = RT(:,1:3);
    refinedT = RT(:,4);

    %%Compute derivative of 3D rotation in exponential coordinates    
function [dRr] = mydRr(R)
    w = exponentialMap(R);
    I = eye(3,3);
    for i=1:3
        e = zeros(3,1);
        e(i)=1;
        %dRr(i) is a 3*3 matrix;
        dRr(i) = ((w(i) * myskew(w) + myskew(cross(w,(I-R)*e)))/norm(w,2))*R;
    end
    
% Compute the skew symmetric matrix of a,
% a should be imputed as a 3 by 1 vector
function [skew] = myskew(a)
    skew = [0    -a(3) a(2);...
            a(3)  0   -a(1);...
           -a(2)  a(1) 0];

   


    
    