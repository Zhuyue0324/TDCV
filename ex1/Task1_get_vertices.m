%Task 1 get vertices
init1=imread('E:/TDCVex1/data/images/init_texture/DSC_9743.jpg');
imshow(init1);
[x1,y1]=ginput(6);
indice1=[1,2,3,4,7,8];
mysave('E:/TDCVex1/init1.txt',x1,y1,indice1);

init2=imread('E:/TDCVex1/data/images/init_texture/DSC_9744.jpg');
imshow(init2);
[x2,y2]=ginput(7);
indice2=[1,2,3,4,6,7,8];
mysave('E:/TDCVex1/init2.txt',x2,y2,indice2);

init3=imread('E:/TDCVex1/data/images/init_texture/DSC_9745.jpg');
imshow(init3);
[x3,y3]=ginput(6);
indice3=[1,2,3,4,6,7];
mysave('E:/TDCVex1/init3.txt',x3,y3,indice3);

init4=imread('E:/TDCVex1/data/images/init_texture/DSC_9746.jpg');
imshow(init4);
[x4,y4]=ginput(7);
indice4=[1,2,3,4,5,6,7];
mysave('E:/TDCVex1/init4.txt',x4,y4,indice4);

init5=imread('E:/TDCVex1/data/images/init_texture/DSC_9747.jpg');
imshow(init5);
[x5,y5]=ginput(6);
indice5=[1,2,3,4,5,6];
mysave('E:/TDCVex1/init5.txt',x5,y5,indice5);

init6=imread('E:/TDCVex1/data/images/init_texture/DSC_9748.jpg');
imshow(init6);
[x6,y6]=ginput(7);
indice6=[1,2,3,4,5,6,8];
mysave('E:/TDCVex1/init6.txt',x6,y6,indice6);

init7=imread('E:/TDCVex1/data/images/init_texture/DSC_9749.jpg');
imshow(init7);
[x7,y7]=ginput(6);
indice7=[1,2,3,4,5,8];
mysave('E:/TDCVex1/init7.txt',x7,y7,indice7);

init8=imread('E:/TDCVex1/data/images/init_texture/DSC_9750.jpg');
imshow(init8);
[x8,y8]=ginput(7);
indice8=[1,2,3,4,5,7,8];
mysave('E:/TDCVex1/init8.txt',x8,y8,indice8);

%Task1 compute initial pose
m3d=importdata('E:/TDCVex1/position_vertices_3d.txt');
fx = 2960.37845;
fy = 2960.37845;
cx = 1841.68855;
cy = 1235.23369;
IntrinsicMat=cameraIntrinsics([fx,fy],[cx,cy],[3680,2456]);


m1=importdata('E:/TDCVex1/init1.txt');
d2=m1(:,1:2);
d3=m3d(m1(:,3),:);
[WO1,WL1] = estimateWorldCameraPose(d2,d3,IntrinsicMat,'MaxReprojectionError',10);
init1=vl_impattern('E:/TDCVex1/data/images/init_texture/DSC_9743.jpg');
image(init1);
grey1=single(rgb2gray(init1));
image(grey1);







pcshow(m3d,'VerticalAxis','Y','VerticalAxisDir','down', 'MarkerSize',30);
hold on
plotCamera('Size',10,'Orientation',WO1,'Location',WL1);
hold off


%save('E:/TDCVex1/init_coord.txt','x1','y1','-ascii');
[A,B,C]=myransac(d2,5,10,2)
