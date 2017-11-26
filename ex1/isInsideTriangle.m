function bool = isInsideTriangle(x,y,z,point)
  bool=(areaTriangle(x,y,point)+areaTriangle(y,z,point)+areaTriangle(z,x,point)-areaTriangle(x,y,z))<=0;
end