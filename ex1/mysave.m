function [] = mysave(pat,xcoord,ycoord,indice)
  fid = fopen(pat,'w');
  n = size(xcoord);
  for i=1:1:n
    fprintf(fid,'%g\t',xcoord(i));
    fprintf(fid,'%g\t',ycoord(i));
    fprintf(fid,'%g\r\n',indice(i));
  end
  fclose(fid);
end