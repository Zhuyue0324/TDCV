function [bestA,bestB,bestC] = myransac(data,N,distBoundary,nbInlierNeed)
  n = size(data,1);
  bestModelNbInlier = 0;
  for i=1:N
    idx = randsample(n,2); 
    sample = data(idx(1:2),:);   
    x1=sample(1,1);
    y1=sample(1,2);
    x2=sample(2,1);
    y2=sample(2,2);
    nbInlier=0;
    %if (A)line passes (x1,y1), (x2,y2) also passes (0,0) i.e.: Ax+By+C=0 with C=0
    if x1*y2==x2*y1
      if x1==x2
        A=1;
        B=0;
        C=-x1;
      elseif y1==y2
        A=0;
        B=1;
        C=-y1;
      elseif x1==0 %(case (x1,y1)=(0,0)) 
        A=y2;
        B=-x2;
        C=0;
      else
        A=y1;
        B=-x1;
        C=0;  
      end
    %else if (x1,y1), (x2,y2) pass Ax+By+1=0 then
    else
      A=(y1-y2)/(x1*y2-y1*x2);
      B=(x1-x2)/(y1*x2-y2*x1);
      C=1;
    end
    
    for j=1:n 
      %distance of (x0,y0) to Ax+By+C=0 is |Ax0+By0+C|/sqrt(A^2+B^2)
      distance=abs(A*data(j,1)+B*data(j,2)+C)/sqrt(A^2+B^2);
      if distance<=distBoundary
        nbInlier=nbInlier+1;  
      end
    end
    if nbInlier>=nbInlierNeed && nbInlier>bestModelNbInlier
      bestModelNbInlier = nbInlier;
      bestA=A;
      bestB=B;
      bestC=C;
    end
  end  
end