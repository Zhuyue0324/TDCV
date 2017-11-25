function [] = initialAlignment(inputpath, outputpath, associateVertices)
  init1=imread(inputpath);
  imshow(init1);
  [n] = size(associateVertices);
  [x,y]=ginput(n(2));
  mysave(outputpath,x,y,associateVertices);
end
