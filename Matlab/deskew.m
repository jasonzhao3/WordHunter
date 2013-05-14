%bad version of binarization
image = imread('./img/skewed_text.jpg');
% image = imread('./img/skew_30d.jpg');
% image = imread('./img/img_rotate.png');
if(size(image,3)==3)
    image = rgb2gray(image);
end;
image = double(image)/256;
thresh = graythresh(image);
bw = image < 0.5;
imshow(bw);

%step 1: close gaps between letters to form "strips"
%fixme: avoid hard coding
se = strel('disk', 10);
bw_closure = imerode(imdilate(bw, se), se);
%imshow(bw_closure);

%step 2: run linear regression on each strip to determine direction of each
%text line
L = bwlabel(bw_closure, 4);
%fixme: can I vectorize this
slope = zeros(1,max(max(L)));
for i=1:max(max(L))
    [x, y] = find(L==i);
    mean_x = mean(x);
    mean_y = mean(y);
    slope(i) = sum((x-mean_x).*(y-mean_y))/sum((x-mean_x).^2);
end
slope = slope(find(~isnan(slope)));

%step 3: select "good" lines/slopes
%fixme: maybe run this multiple times
sigma_slope = std(slope);
mean_slope = mean(slope);
good_line = (slope > mean_slope -sigma_slope) & (slope < mean_slope + sigma_slope);
final_slope = mean(slope(find(good_line)));
angle = atan(1/final_slope)/pi*180;
bw_deskew = imrotate(bw, angle, 'crop'); %maybe use 'loose' make sure we don't cut off anything
imshow(bw_deskew);
