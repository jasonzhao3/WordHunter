clc;
clear;

%Add paths for vlfeat
addpath(genpath('..\vlfeat\vlfeat-0.9.16'));
addpath(genpath('..\GML_RANSAC_Matlab_Toolbox_0.2'));

%Read image file
img = imread('.\img\word.jpg');
img = uint8(rgb2gray(img));
sizeImg = size(img);
figure(1);imshow(img);

%img enhancement
img = img_enhance(img);

%img binarization
img = img_mser(img, sizeImg);
figure(2); imshow(img);

% make word slimmer
se = strel('line', 1, 90);
img = imerode(img,se);
figure(3); imshow(img);

%img segementation into words
bbArray = word_segment(img);
for i = 1 : size(bbArray, 1)
    rectangle('Position',bbArray(i,:),'Linewidth',2,'EdgeColor','red');
end


