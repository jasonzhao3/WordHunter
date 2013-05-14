% Author: Yang Zhao
% Function: img_enhance

function imgEnhanced = img_enhance(img)
%img enhancement
imgEnhanced = adapthisteq(img, 'NumTiles', [5 5], 'ClipLimit', 0.005);
end
