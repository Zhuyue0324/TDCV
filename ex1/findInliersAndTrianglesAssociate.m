function [sf,sd,tri] = findInliersAndTrianglesAssociate(f,d,pathd2,pathd3,pathd3Tri)
  % Given feature vectors and descriptors F and D,
  % paths to files storing 2D, 3D coordinates of base points
  % and a triangulation on these points, returns
  % F',D', with 3D coordinates interpolated from
  % those of the base points for the features that
  % fit in the triangulation.
  
  [n] = size(f);            % (4, N)
  m2d = importdata(pathd2); % 2D coordinates of vertices
  [n2] = size(m2d); % (6|7, N)
  m3d = importdata(pathd3); % 3D coordinates of vertices
  t3dorigin = importdata(pathd3Tri); %each row is 3 indexes
    % of one triangle in 3D
  nbTrianglesInImage2d=0; %counter for number of triangles
    % that appear in 2d image
    
  sf=[];
  sd=[];

  %% Establish a table t3d,
  % each row corresponds to a triangle that appears in this image,
  % and consists in:
  % -  3 indices of vertices,
  % -  6 coordinate values of them in 2D,
  % -  9 coordinate values of them in 3d.
  % Total size = X*18
  % t3d(i) = [A B C XA YA XB YB XC YC xA yA zA xB yB zB xC yC zC]
  tri=[];
  t3d=[];
  for i = 1:12 % Consider each of the model's 12 triangles
    % If its 3 vertices are in this image ...
    if (any(m2d(:,3)==t3dorigin(i,1)) ...
            && any(m2d(:,3)==t3dorigin(i,2)) ...
            && any(m2d(:,3)==t3dorigin(i,3)))
      
      nbTrianglesInImage2d=nbTrianglesInImage2d+1;
      % ... add the 3 vertex indices ...
      for j = 1:3
        t3d(nbTrianglesInImage2d,j)=t3dorigin(i,j);
      end
      
      % ... add the 6 2D coordinates ...
      for j = 1:(n2(1))
        if m2d(j,3)==t3d(nbTrianglesInImage2d,1)
            t3d(nbTrianglesInImage2d,4)=m2d(j,1);
            t3d(nbTrianglesInImage2d,5)=m2d(j,2);
        elseif m2d(j,3)==t3d(nbTrianglesInImage2d,2)
            t3d(nbTrianglesInImage2d,6)=m2d(j,1);
            t3d(nbTrianglesInImage2d,7)=m2d(j,2);
        elseif m2d(j,3)==t3d(nbTrianglesInImage2d,3)
            t3d(nbTrianglesInImage2d,8)=m2d(j,1);
            t3d(nbTrianglesInImage2d,9)=m2d(j,2);
        end
      end
      
      % ... add the 9 3D coordinates.
      for j = 1:3
          for k = 1:3
              t3d(nbTrianglesInImage2d,6+3*j+k)=m3d(t3d(nbTrianglesInImage2d,j),k);
          end
      end
    end
  end
  
  %% Use t3d to modify [F,D]
  %output is a matrix combining f, d (both reshaped to have features as lines)
  % , and associated values in t3d for each row
  outputcolumn=0;
  for i = 1:(n(2))
    for j = 1:nbTrianglesInImage2d
      if isInsideTriangle([t3d(j,4),t3d(j,5)],...
              [t3d(j,6),t3d(j,7)],...
              [t3d(j,8),t3d(j,9)],...
              [f(1,i),f(2,i)])
        outputcolumn=outputcolumn+1;
        sf(:,outputcolumn)=f(:,i);
        %TODO add stuff here and change prev. line
        sd(:,outputcolumn)=d(:,i);
        tri(:,outputcolumn)=transpose(t3d(j,:));
        break;
      end  
    end
  end
end
