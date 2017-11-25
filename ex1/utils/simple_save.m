
function [] = simple_save(path, mat)
    % saves a 2D matrix in path as rows (separated by \n) of values
    % (separated by \t)
    fid = fopen(path,'w');
    assert(ismatrix(mat),'mat should be a matrix')
    for i=1:size(mat,1)
        for j=1:size(mat,2)
            fprintf(fid,'%g\t',mat(i,j));
        end
        fprintf(fid,'\r\n');
    end
    fclose(fid);
end
