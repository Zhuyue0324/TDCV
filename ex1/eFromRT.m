
function [e]=eFromRT(RT, IntrinsicMat, h2d, h3d)
% computes energy given RT, using global variables
    R = rotationMatrix(RT(1:3));
    T = RT(4:6)';
    camMatrix = cameraMatrix(IntrinsicMat,R,T);
    m_homo =  camMatrix' * h3d;
    m = (m_homo(1:2,:)./m_homo(3,:));
    [e,~] = energy(m,h2d,100,0);
end