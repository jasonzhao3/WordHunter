clc;
clear;

%Add paths for vlfeat
addpath(genpath('..\vlfeat\vlfeat-0.9.16'));
addpath(genpath('..\GML_RANSAC_Matlab_Toolbox_0.2'));

%Read image file
img = imread('.\img\no_skew_small.jpg');
img = uint8(rgb2gray(img));
sizeImg = size(img);
figure(1);imshow(img);

%img enhancement
img = img_enhance(img);

%img binarization
img = img_mser(img, sizeImg);


figure(2); imshow(img);