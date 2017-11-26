function [] = compute_then_store_sift(path1, vis, path2)
    %init1=vl_impattern('../../data/images/init_texture/DSC_9743.jpg'); =>
    %fails because it's not a stock picture ... prefer:
    %disp(strcat(path1,'  > ctss >  sift/',path2))
    init = imread(path1);
    image(init);
    gray=single(rgb2gray(init));
    image(gray);
    [f,d] = vl_sift(gray);
    simple_save(strcat('sift/f',path2),f);
    simple_save(strcat('sift/d',path2),d);
    if vis
        %view(f1,d1);
        perm = randperm(size(f,2));
        sel = perm(1:50);
        h1 = vl_plotframe(f(:,sel));
        h2 = vl_plotframe(f(:,sel));
        set(h1,'color','k','linewidth',3);
        set(h2,'color','k','linewidth',2);
        h3 = vl_plotsiftdescriptor(d(:,sel),f(:,sel));
        set(h3,'color','g');
    end
    disp('done')
end