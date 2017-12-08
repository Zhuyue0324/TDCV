function [R]=rotationMatrix(w)
  theta=norm(w);
  if (theta~=0)
    w0=w/(norm(w));
    omega=[0,-w0(3),w0(2);w0(3),0,-w0(1);-w0(2),w0(1),0];
    R=eye(3,3)+sin(theta)*omega+(1-cos(theta))*omega*omega;
  else
    R=[1,0,0;0,1,0;0,0,1]
  end
end