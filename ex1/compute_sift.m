function [f,d] = compute_sift(path1, vis)
    % Computes SIFT feature/detectors (F,D) of image in path1
    % eventually visualize the result (if vis>0)
    %disp(strcat(path1,'  > ctss >  sift/',path2))
    init = imread(path1);
    %image(init);
    gray=single(rgb2gray(init));
    
    [f,d] = vl_sift(gray);
    % simple_save(strcat('sift/f',path2),f);
    % simple_save(strcat('sift/d',path2),d);
    if vis>0 % view at most vis FD
        %view(f1,d1);
        image(gray);
        perm = randperm(size(f,2));
        sel = perm(1:max(vis, size(f,2)));
        h1 = vl_plotframe(f(:,sel));
        h2 = vl_plotframe(f(:,sel));
        set(h1,'color','k','linewidth',3);
        set(h2,'color','k','linewidth',2);
        h3 = vl_plotsiftdescriptor(d(:,sel),f(:,sel));
        set(h3,'color','g');
        disp('done')
    end
    
end