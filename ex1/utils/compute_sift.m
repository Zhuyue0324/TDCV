function [f,d] = compute_sift(path1, vis, path2)
    %init1=vl_impattern('../../data/images/init_texture/DSC_9743.jpg'); =>
    %fails because it's not a stock picture ... prefer:
    disp(strcat(path1,'  > ctss >  sift/',path2))
    init = imread(path1);
    image(init);
    gray=single(rgb2gray(init));
    image(gray);
    [f,d] = vl_sift(gray);
end