function angleRotate = getHoughAngle( img )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

% imgGray = rgb2gray(img);
% imgDouble = im2double(imgGray);
% % Binarize image
% [height, width] = size(img);
% threshold = graythresh(imgDouble);
% imgBinary = imgDouble > threshold;
% imgBinaryNot = 1 - imgBinary(1:1:end,1:1:end);
% 
% % Find Hough transform
% thetaVec = 0 : 0.5 : 179;
% [houghCounts, radiusVec] = radon(imgBinaryNot, thetaVec);
% [maxCount, maxCountIdx] = max(houghCounts(:));
% [radiusIdx, thetaIdx] = ind2sub(size(houghCounts), maxCountIdx);
% plot(thetaVec(thetaIdx), radiusVec(radiusIdx), 'ys');
% angleRotate = thetaVec(thetaIdx) - 90;


%Perform Hough Transform
Ig=img;
Iss=Ig(1:8:end,1:8:end);
level=graythresh(Iss);
BW=not(im2bw(Iss,level));
[H,Th,R]=hough(BW);
P=houghpeaks(H,1);
Theta=Th(P(:,2))-90;
if abs(Theta)>90
    Theta=-Theta+180;
end
angleRotate=Theta;
%I=imcomplement(imrotate(imcomplement(I),Theta,'crop'));
%End of Hough Transform

end

