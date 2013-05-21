clc;
clear;

%Add paths for vlfeat
addpath(genpath('..\vlfeat\vlfeat-0.9.16'));
addpath(genpath('..\GML_RANSAC_Matlab_Toolbox_0.2'));

%Read image file
img = imread('.\img\no_skew.jpg');
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
% Disk I/O is too slow here 
% bbArray = word_segment(img);
% for i = 1 : size(bbArray, 1)
%     rectangle('Position',bbArray(i,:),'Linewidth',2,'EdgeColor','red');
%     imgPathStr = strcat('.\process_img\word_', num2str(i), '.png');
%     imwrite(img(bbArray(i,2):bbArray(i,2) + bbArray(i,4), ...
%             bbArray(i,1):bbArray(i,1) + bbArray(i,3)), imgPathStr);
% 
%     tessPath = strcat('tesseract  D:\_Stanford\Course_Information\EE368\Project\Matlab\process_img\word_', ...
%         num2str(i), '.png  D:\_Stanford\Course_Information\EE368\Project\Matlab\process_img\out -psm 8');
%     system(tessPath);
%    
% end


