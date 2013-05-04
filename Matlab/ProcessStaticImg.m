clc;
clear;

%Add paths for vlfeat
addpath(genpath('..\vlfeat\vlfeat-0.9.16'));
addpath(genpath('..\GML_RANSAC_Matlab_Toolbox_0.2'));

%Read image file
img = imread('post_office_30.jpg');
img = uint8(rgb2gray(img));
sizeImg = size(img);

imshow(img);
% (1) VL_MSER
    try
        % Compute region seeds and elliptical frames
            [reg fr] = vl_mser(img, 'MinDiversity', 0.7, 'MaxVariation', 0.4, 'Delta', 15, 'MaxArea', 0.1, 'MinArea', (50/(sizeImg(1)*sizeImg(2))) );
   
        % Calculate regions    
            MSERimg = zeros(sizeImg);
            % Sort region seeds, filter out reg < 0 (overlapping MSER regions)
            regSorted = sort(reg(reg > 0));  

            TessOutput = [];
            for x = regSorted'
                s = vl_erfill(img,x);  %Return pixels belonging to region seed x
                MSERimg(s) = MSERimg(s) + 1;            %Number of overlapping extremal regions                
            end
            figure(3), imshow(MSERimg);
%             imwrite(MSERimg,'C:\wamp\www\libSVM\MSERimg.jpg');

    % (2) Region Props to Segment out Lines        
        regLabels = bwlabel(MSERimg,8);  %label the regions
        figure(4), imagesc(regLabels);

        regProp   = regionprops(regLabels, 'BoundingBox', 'Centroid', 'Area', 'Solidity');

        %(2a) Find Centroid X and Centroid Y
            CentX = zeros(1,length(regProp));
            CentY = zeros(1,length(regProp));
            TopY  = zeros(1,length(regProp));
            BotY  = zeros(1,length(regProp));
            for ri = 1:length(regProp)
                Centroid  = regProp(ri).Centroid;
                CentX(ri) = Centroid(1);
                CentY(ri) = Centroid(2);
                BdBox     = regProp(ri).BoundingBox;
                TopY (ri) = BdBox(2);
                BotY (ri) = BdBox(2) + BdBox(4);     %Y for bottom of region            
            end

        %(2b) Figure out the number of lines using Histogram
            numBins = 20;
            bins = (sizeImg(1)/(numBins*2)):(sizeImg(1)/numBins):sizeImg(1);
            [imgHist binsHist] = hist(CentY, bins);
%             figure(5); hist(CentY, bins);

            line_ind = 1;      % line index
            lineY    = 0;
            peakHist = 0;
            for ii = 1:numBins
                if (imgHist(ii) > peakHist)          % New peak fond
                    lineY(line_ind) = binsHist(ii);  % Update line Centroid Y
                    peakHist        = imgHist(ii);   % Store current peak as new max
                end
                if (peakHist > 0) && (imgHist(ii) == 0)  % End of line
                    line_ind = line_ind + 1;             %   Increment line index
                    peakHist = 0;                        %   Reset max
                end
            end

        %(2c) Calculate boundary of lines based on mean of centroid Y in histogram       
            boundaryLineY      = zeros(1,length(lineY)+1);
            boundaryLineY(1)   = 1;          %Top of first line is row 1
            boundaryLineY(end) = sizeImg(1); %Bottom of last line is Y extent of image

            % Set boundary as mean of Centroid Y of neighbouring lines
            for ii = 2:length(lineY)
                boundaryLineY(ii) = round(mean([lineY(ii-1), lineY(ii)]));
            end
    catch e
        disp('ERROR: Bad image.  Please take another image!');
        disp(getReport(e,'extended'));
    end
    
    
    
%     %% (3) Segment out Lines, correct with RANSAC + Affine Transform
% %     system('del C:\wamp\www\upload\TessOutput.txt');
%     for li = 1:length(lineY)            
%         lineImg = MSERimg(boundaryLineY(li):boundaryLineY(li+1), :);
%         %imwrite(lineImg, 'lineImg.jpg');
%         %figure(10); imshow(lineImg); title('Original lineImg');
%             
%         % Find all regions with y-centroid within boundaryLineY(li) < CentY < boundaryLineY(li+1)
%         regAll = intersect(find(CentY > boundaryLineY(li)), find(CentY < boundaryLineY(li+1)));
%         lineCentX = CentX(regAll);
%         baselineY = BotY(regAll);
%         toplineY  = TopY(regAll);
%         
%                         
%         % (3a) Use RANSAC to find line running through inlier points
%             nSampLen = 2;     % Number of point for RANSAC Model Function
%             modelFcn = @TLS;  % Model function: Total least squared
%             nIter    = 50;    % Number of iterations for RANSAC algorithm
%             residFcn = @Dist; % Residuum function: Distance to line
%             dThreshold = 2;   % Threshold for residuum
%             
%             [RANSAC_maskBotY, RANSAC_BotY] = RANSAC([lineCentX; baselineY], modelFcn, nSampLen, residFcn, nIter, dThreshold);
%             figure(9), plot(lineCentX, baselineY, 'ob'); title('Bottom Y');
%             hold on; plot(lineCentX, -(RANSAC_BotY(1) * lineCentX + RANSAC_BotY(3))/RANSAC_BotY(2), 'r');
%             
%             [RANSAC_maskTopY, RANSAC_TopY] = RANSAC([lineCentX; toplineY], modelFcn, nSampLen, residFcn, nIter, dThreshold);
%             figure(10), plot(lineCentX, toplineY, 'ob'); title('Top Y');
%             hold on; plot(lineCentX, -(RANSAC_TopY(1) * lineCentX + RANSAC_TopY(3))/RANSAC_TopY(2), 'r');
%             
%             % X-centroid of leftmost and rightmost elements
%             minX = min(lineCentX(RANSAC_maskBotY));
%             maxX = max(lineCentX(RANSAC_maskBotY));
%             % Ends of baseline and topline as calculated from RANSAC models
%             baselineLeft  = -(RANSAC_BotY(1) * minX + RANSAC_BotY(3))/RANSAC_BotY(2);
%             baselineRight = -(RANSAC_BotY(1) * maxX + RANSAC_BotY(3))/RANSAC_BotY(2);
%             toplineLeft   = -(RANSAC_TopY(1) * minX + RANSAC_TopY(3))/RANSAC_TopY(2);
%             toplineRight  = -(RANSAC_TopY(1) * maxX + RANSAC_TopY(3))/RANSAC_TopY(2);
% 
%         % (3b) Use affine transformation to correct for skew
%             % Points in Skewed Image
%             inputpoints = [minX baselineLeft-boundaryLineY(li); minX toplineLeft-boundaryLineY(li); maxX baselineRight-boundaryLineY(li); maxX toplineRight-boundaryLineY(li)];
%             % Points in Correct Image
%             basepoints = [minX max(1,baselineLeft-boundaryLineY(li)); minX max(1,toplineLeft-boundaryLineY(li)); maxX max(1,baselineLeft-boundaryLineY(li)); maxX max(1,toplineLeft-boundaryLineY(li))];
%             % Calculate affine transform from projection
%             tform = cp2tform(inputpoints, basepoints, 'projective');
%             
%             sizeLine = size(lineImg);
%             lineImgCorr = imtransform(lineImg, tform, 'XData', [1 sizeLine(2)], 'YData', [1 sizeLine(1)]);            
%             %figure(11), imshow(lineImgCorr); title('Corrected lineImgCorr');
%                                        
%             % (*Optional*) OCR by Individual Characters
% %                 lineRegLabels = bwlabel(lineImg,8);  %label the regions
% %                 %figure(5), imagesc(lineRegLabels);             
% % 
% %                 % '=' will have two regions that have almost same x-centroid
% %                 minCentXSeparation = 20;  % Assume characters are separated by at least this much
% %                 % Use bitMask to eliminate double counting x-centroid of '='
% %                 regMask  = logical([ (CentX(regAll(2:end)) - CentX(regAll(1:end-1)) > minCentXSeparation) 1]);  %bitMask 
% %                 % Get x-centroid of all characters
% %                 charX = [CentX( regAll(regMask) ) sizeImg(2)];
% % 
% %                 % Set boundary as mean of x-centroid of neighbouring characters
% %                 boundaryCharX      = zeros(1,length(charX)+1);
% %                 boundaryCharX(1)   = 1;
% %                 boundaryCharX(end) = sizeImg(2);
% %                 for ii = 1:length(charX)-1
% %                     boundaryCharX(ii+1) = round(mean([charX(ii), charX(ii+1)]));
% % 
% %                     charImg = lineImg(:, boundaryCharX(ii):boundaryCharX(ii+1));
% %                     %figure, imshow(charImg);
% %                     imwrite(charImg, 'charImg.jpg');
% %                     [status,result] = system('tesseract C:\wamp\www\upload\charImg.jpg C:\wamp\www\upload\TessOutput -psm 10 alpha_number_math');
% %                     ocrLine = textread('TessOutput.txt', '%s', 'delimiter', ',')
% %                 end
%             lineImgCorr = lineImg;
%             figure(6), imshow(lineImgCorr);
% 
% %             imwrite(lineImgCorr, 'C:\wamp\www\upload\lineImgCorr.jpg');
% %             [status,result] = system('C:\wamp\www\Tesseract-OCR\tesseract C:\wamp\www\upload\lineImgCorr.jpg C:\wamp\www\upload\tessLineImg -psm 7 alpha_number_math');            
% % 
% %             Write output to File
% %             system('type C:\wamp\www\upload\tessLineImg.txt >> C:\wamp\www\upload\TessOutput.txt');
% %             system('echo. >> C:\wamp\www\upload\TessOutput.txt');
            
    end      