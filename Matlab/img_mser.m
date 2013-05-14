%
% Author: Yang Zhao
% Reference 2012 EquationSolver's Code
%

function imgMSER= img_mser(img, sizeImg)
try
    % Compute region seeds and elliptical frames
    [reg fr] = vl_mser(img, 'MinDiversity', 0.2, 'MaxVariation', 0.7, 'Delta', 15, 'MaxArea', 0.1, 'MinArea', (5/(sizeImg(1)*sizeImg(2))) );
    
    % Calculate regions
    imgMSER = zeros(sizeImg);
    % Sort region seeds, filter out reg < 0 (overlapping MSER regions)
    regSorted = sort(reg(reg > 0));
    
    for x = regSorted'
        s = vl_erfill(img,x);  %Return pixels belonging to region seed x
        imgMSER(s) = imgMSER(s) + 1;            %Number of overlapping extremal regions
    end
catch e
    disp('ERROR: Bad image.  Please take another image!');
    disp(getReport(e,'extended'));
end