function [sf,sd,tri] = findInliersAndTrianglesAssociate(f,d,pathd2,pathd3,pathd3Tri)
  [n] = size(f);%4*N
  m2d = importdata(pathd2); %coordinates in 2d of vertices
  [n2] = size(m2d); %6*3 or 7*3
  m3d = importdata(pathd3); %coordinates in 3d of vertices
  t3dorigin = importdata(pathd3Tri); %each row is 3 indexes of one triangle in 3d
  nbTrianglesInImage2d=0; %counter for number of triangles existe in 2d image
  sf=[];
  sd=[];
  tri=[];
  t3d=[];
  %establish a table t3d, each row are 3 index of vertices, 6 coordinate values of them in
  %2d, 9 coordinate values of them in 3d, size = X*18
  for i = 1:12
    if (any(m2d(:,3)==t3dorigin(i,1)) && any(m2d(:,3)==t3dorigin(i,2)) && any(m2d(:,3)==t3dorigin(i,3)))
      nbTrianglesInImage2d=nbTrianglesInImage2d+1;
      for j = 1:3
        t3d(nbTrianglesInImage2d,j)=t3dorigin(i,j);
      end
      for j = 1:(n2(1))
        if m2d(j,3)==t3d(nbTrianglesInImage2d,1)
            t3d(nbTrianglesInImage2d,4)=m2d(j,1);
            t3d(nbTrianglesInImage2d,5)=m2d(j,2);
        end
        if m2d(j,3)==t3d(nbTrianglesInImage2d,2)
            t3d(nbTrianglesInImage2d,6)=m2d(j,1);
            t3d(nbTrianglesInImage2d,7)=m2d(j,2);
        end
        if m2d(j,3)==t3d(nbTrianglesInImage2d,3)
            t3d(nbTrianglesInImage2d,8)=m2d(j,1);
            t3d(nbTrianglesInImage2d,9)=m2d(j,2);
        end
      end
      t3d(nbTrianglesInImage2d,10)=m3d(t3d(nbTrianglesInImage2d,1),1);
      t3d(nbTrianglesInImage2d,11)=m3d(t3d(nbTrianglesInImage2d,1),2);
      t3d(nbTrianglesInImage2d,12)=m3d(t3d(nbTrianglesInImage2d,1),3);
      t3d(nbTrianglesInImage2d,13)=m3d(t3d(nbTrianglesInImage2d,2),1);
      t3d(nbTrianglesInImage2d,14)=m3d(t3d(nbTrianglesInImage2d,2),2);
      t3d(nbTrianglesInImage2d,15)=m3d(t3d(nbTrianglesInImage2d,2),3);
      t3d(nbTrianglesInImage2d,16)=m3d(t3d(nbTrianglesInImage2d,3),1);
      t3d(nbTrianglesInImage2d,17)=m3d(t3d(nbTrianglesInImage2d,3),2);
      t3d(nbTrianglesInImage2d,18)=m3d(t3d(nbTrianglesInImage2d,3),3);
    end
  end
  %output is a submatrix of f, submatrix of d, associated values in t3d for
  %each row
  outputcolomn=0;
  for i = 1:(n(2))
    for j = 1:nbTrianglesInImage2d
      if isInsideTriangle([t3d(j,4),t3d(j,5)],[t3d(j,6),t3d(j,7)],[t3d(j,8),t3d(j,9)],[f(1,i),f(2,i)])
        outputcolomn=outputcolomn+1;
        sf(:,outputcolomn)=f(:,i);
        sd(:,outputcolomn)=d(:,i);
        tri(:,outputcolomn)=transpose(t3d(j,:));
        break;
      end  
    end
  end
end